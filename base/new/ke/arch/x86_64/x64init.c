
// x64init.c

#include <kernel.h>


extern unsigned long InitializationPhase;
int InitialProcessInitialized = FALSE;


// Task switching support.
extern void turn_task_switch_on (void);

extern void x64_clear_nt_flag (void);


//Onde ficam os códigos e arquivos de configuração usados na inicialização.
//A ideia é que se a inicialização precisar de algum arquivo, deve procurá-lo
//nos diretórios previamente combinados.
//melhor que sejam poucos e que sempre sejam os mesmos.

#define INIT_ROOT_PATH  "/"
#define INIT_BIN_PATH   "/BIN"
//...






/*
// #deprecated
// local
// Call a mm routine for that.
void x64init_load_pml4_table(unsigned long phy_addr)
{
    asm volatile ("movq %0,%%cr3"::"r"(phy_addr));
}
*/


// Create a process for the first ring3 client, called GWS.BIN
// + Carrega a imagem do primeiro processo que vai rodar em user mode.
// + Configura sua estrutura de processo.
// + Configura sua estrutura de thraed.
// Não passa o comando para o processo.
// See: gramado/core/client

int I_x64CreateInitialProcess (void)
{
    int fileret = -1;

// #debug
    debug_print ("I_x64CreateInitialProcess: \n");
    //printf      ("I_x64CreateInitialProcess:\n");
    //refresh_screen();

    if ( system_state != SYSTEM_BOOTING ){
        printf ("I_x64CreateInitialProcess: system_state\n");    
        return FALSE;
    }

//
// Load imag SM.BIN.
//

    // Session manager

    // This is the first user mode process running
    // after the kernel base.

    // #importante
    // Carregado do diretório raiz
    // loading image.
    // #bugbug
    // Loading from root dir. 512 entries limit.
    
    // #todo
    // O propósito é termos a possibilidade de selecionar qual será
    // a imagem desse processo incial
    // e configurarmos isso no modulo init/ do kernel base.

    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (512 * 4096);

    fileret = (unsigned long) fsLoadFile ( 
                                  VOLUME1_FAT_ADDRESS, 
                                  VOLUME1_ROOTDIR_ADDRESS, 
                                  FAT16_ROOT_ENTRIES,    //#bugbug: number of entries.
                                  "GWS     BIN", 
                                  (unsigned long) CONTROLTHREAD_BASE, //0x00200000
                                  BUGBUG_IMAGE_SIZE_LIMIT );

    if ( fileret != 0 ){
        printf ("I_x64CreateInitialProcess: Coldn't load GWS.BIN \n");
        return FALSE;
    }

	// Creating init process.
	// > Cria um diretório que é clone do diretório do kernel base 
	// Retornaremos o endereço virtual, para que a função create_process possa usar 
	// tanto o endereço virtual quanto o físico.
	// > UPROCESS_IMAGE_BASE;
	// #todo
	// temos que checar a validade do endereço do dir criado
	// antes de passarmos..

    void *init_pml4_va = (void *) CloneKernelPML4();

    if ( init_pml4_va == 0 ){
        printf ("I_x64CreateInitialProcess: init_pml4_va\n");
        return FALSE;
    }

    init_mm_data.pml4_va = init_pml4_va;
    init_mm_data.pml4_pa = (unsigned long) virtual_to_physical ( 
                                               init_pml4_va, 
                                               gKernelPML4Address );

    if ( init_mm_data.pml4_pa == 0 ){
        printf ("I_x64CreateInitialProcess: init_mm_data.pml4_pa\n");
        return FALSE;
    }

    // ...

    init_mm_data.used  = TRUE;
    init_mm_data.magic = 1234;

// ===========================================

    InitProcess = (void *) create_process ( 
                               NULL, NULL, NULL, 
                               (unsigned long) CONTROLTHREAD_BASE, //0x00200000 
                               PRIORITY_HIGH, 
                               (int) KernelProcess->pid, 
                               "SM-PROCESS", 
                               RING3, 
                               (unsigned long ) init_pml4_va,
                               (unsigned long ) kernel_mm_data.pdpt0_va,
                               (unsigned long ) kernel_mm_data.pd0_va );


// struct
    if ( (void *) InitProcess == NULL ){
        printf ("I_x64CreateInitialProcess: InitProcess\n");
        return FALSE;
    }

// validation
    if ( InitProcess->used != TRUE || 
         InitProcess->magic != 1234 )
    {
        printf ("I_x64CreateInitialProcess: InitProcess validation\n");
        return FALSE;
    }

// struct
    if ( InitProcess->pid != GRAMADO_PID_INIT )
    {
        printf ("I_x64CreateInitialProcess: pid\n");
        return FALSE;
    }

// The init process is a system application
// GWS.BIN
    InitProcess->type = SYSTEM_APPLICATION;

    if ( init_mm_data.used != TRUE || 
         init_mm_data.magic != 1234 )
    {
        printf ("I_x64CreateInitialProcess: init_mm_data validation\n");
        return FALSE;
    }

    // Esse foi configurado agora.
    InitProcess->pml4_VA = init_mm_data.pml4_va;
    InitProcess->pml4_PA = init_mm_data.pml4_pa; 

    // Herdado do kernel
    InitProcess->pdpt0_VA = kernel_mm_data.pdpt0_va;
    InitProcess->pdpt0_PA = kernel_mm_data.pdpt0_pa; 

    // Herdado do kernel
    InitProcess->pd0_VA = kernel_mm_data.pd0_va;
    InitProcess->pd0_PA = kernel_mm_data.pd0_pa; 

    InitProcess->position = SPECIAL_GUEST;
    fs_initialize_process_cwd ( InitProcess->pid, "/" );


//====================================================
// Create thread

// #
// Criamos um thread em ring3.
// O valor de eflags é 0x3200.
// The control thread of the first ws's client.
// See: gws.bin

    InitThread = (void *) create_tid3();

// struct
    if ( (void *) InitThread == NULL ){
        printf ("I_x64CreateInitialProcess: InitThread\n");
        return FALSE;
    }

// validation
    if ( InitThread->used != TRUE || 
         InitThread->magic != 1234 )
    {
        printf ("I_x64CreateInitialProcess: InitThread validation\n");
        return FALSE;
    }

// tid
    if ( InitThread->tid != INIT_TID ){
        printf ("I_x64CreateInitialProcess: INIT_TID\n");
        return FALSE;
    }

// owner pid
    if ( InitThread->ownerPID != GRAMADO_PID_INIT ){
        printf ("I_x64CreateInitialProcess: GRAMADO_PID_INIT\n");
        return FALSE;
    }

//
// Paging
//

    // Herdando do processo configurado logo antes.
    InitThread->pml4_VA  = InitProcess->pml4_VA;
    InitThread->pml4_PA  = InitProcess->pml4_PA;
    InitThread->pdpt0_VA = InitProcess->pdpt0_VA;
    InitThread->pdpt0_PA = InitProcess->pdpt0_PA;
    InitThread->pd0_VA   = InitProcess->pd0_VA;
    InitThread->pd0_PA   = InitProcess->pd0_PA;


    InitThread->position = SPECIAL_GUEST;

    //IdleThread->ownerPID = (int) InitProcess->pid;

    // #todo #bugbug
    //InitThread->tss = current_tss;

    // [Processing time]
    current_process = InitProcess->pid;
    current_thread  = InitThread->tid;

    // [Focus]
    active_process = current_process;
    active_thread  = current_thread;
        
    // foreground thread ?

    // [Scheduler stuff]
    next_thread = InitThread->tid;


// ===========================


    // #importante
    // A thread de controle do processo init2.bin.
    InitProcess->control = InitThread;


    // #todo #bugbug
    //registra um dos servidores do gramado core.
    //server_index, process, thread

    //ipccore_register ( 
        //(int) 0, 
        //(struct process_d *) InitProcess, 
        //(struct thread_d *) InitThread ); 
    
    
    // Agora ja temos um processo em user mode devidamente 
    // configurado. Então a rotina de inicialização em init/
    // poderá pasasr o comando para ele quando quizer.

    InitialProcessInitialized = TRUE;

    return TRUE;
}



// Passa o comando para o primeiro processo em user mode.
// Esse processo ja foi previamente configurado.
// Called by kernel_main() in init.c

void I_x64ExecuteInitialProcess (void)
{
    struct thread_d  *Thread;
    int i=0;

    //#todo
    debug_print ("I_x64ExecuteInitialProcess: [TODO]\n");
    //printf      ("I_x64ExecuteInitialProcess: [TODO]\n");
    //refresh_screen();
   
    if ( system_state != SYSTEM_BOOTING ){
        panic ("I_x64ExecuteInitialProcess: system_state\n");    
    }

    if ( InitialProcessInitialized != TRUE ){
        debug_print ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
        panic       ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
    }

// Se essa rotina foi chamada antes mesmo
// do processo ter sido devidamente configurado.

    if ( InitialProcessInitialized != TRUE ){
        debug_print ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
        panic       ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
    }

// The first thread to run will the control thread 
// of the init process. It is called InitThread.

    Thread = (struct thread_d *) InitThread; 

    if ( (void *) Thread == NULL ){
        debug_print ("I_x64ExecuteInitialProcess: Thread\n");
        panic       ("I_x64ExecuteInitialProcess: Thread\n");
    }

    if ( Thread->used != TRUE || Thread->magic != 1234 )
    {
        debug_print ("I_x64ExecuteInitialProcess: Thread validation\n");
        //printf ("I_x64ExecuteInitialProcess: tid={%d} magic \n", 
        //    Thread->tid);
        printf ("I_x64ExecuteInitialProcess: Thread validation\n");
        die();
    }

// It its context is already saved, so this is not the fist time.
    
    if ( Thread->saved != FALSE ){
        panic("I_x64ExecuteInitialProcess: saved\n");
    }

    if ( Thread->tid < 0 ){
        panic("I_x64ExecuteInitialProcess: tid\n");
    }

// Set the current thread.
    set_current_thread(Thread->tid);

// State
// The thread needs to be in Standby state.

    if ( Thread->state != STANDBY )
    {
        printf ("I_x64ExecuteInitialProcess: state tid={%d}\n", 
            Thread->tid);
        die();
    }

// :: MOVEMENT 2 ( Standby --> Running )

    if ( Thread->state == STANDBY ){
        debug_print("I_x64ExecuteInitialProcess: Thread->state = RUNNING\n");
        Thread->state = RUNNING;
    }

// Current process.

    current_process = (pid_t) Thread->process->pid;

// List
// Dispatcher ready list.
// Maybe it is not used.

    for ( i=0; i < PRIORITY_MAX; i++ ){
        dispatcherReadyList[i] = (unsigned long) Thread;
    };

// Counting the type of the dispatching criteria.
    IncrementDispatcherCount(SELECT_IDLE_COUNT);


// #todo 
// check this
// turn_task_switch_on:
//  + Creates a vector for timer irq, IRQ0.
//  + Enable taskswitch. 

    turn_task_switch_on();

// #todo
// Isso deve ser liberado pelo processo init
// depois que ele habilitar as interrupções.
    
    taskswitch_lock();
    scheduler_lock();

// timer.

    // timerInit8253 ( HZ );
    // timerInit8253 ( 800 );
    // timerInit8253 ( 900 );

// Setup cr3.
// This is a Physical address.
// See: hal/
    x64_load_pml4_table( Thread->pml4_PA );

// Reload tlb.
// See: https://en.wikipedia.org/wiki/Translation_lookaside_buffer
// #bugbug: rever isso.

    asm ("movq %cr3, %rax");
    asm ("movq %rax, %cr3");

// #todo 
// check this
// See: headlib.asm
// Se nao limpar, um iret causa taskswitch
// em sistemas x86 de 32bit. Mas estamos em 64bit.

    x64_clear_nt_flag();   

// #maybe
// Vamos iniciar antes para que
// possamos usar a current_tss quando criarmos as threads

    //x64_init_gdt();

// ??
    asm ("clts \n");

// #importante
// Mudamos para a última fase da inicialização.
// Com isso alguns recursos somente para as fases anteriores
// deverão ficar indisponíveis.

// End of phase.
// Starting phase 4.

    InitializationPhase = 4;

// =============
// # go!
// Nos configuramos a idle thread em user mode e agora vamos saltar 
// para ela via iret.
// #todo:
// #importante:
// Podemos usr os endereços que estão salvos na estrutura.
// #bugbug:
// temos a questão da tss:
// será que a tss está configurada apenas para a thread idle do INIT ??
// temos que conferir isso.
// base dos arquivos.
// #todo
// Rever se estamos usando a base certa.
// sm.bin (ELF)
// See: fs.c

    int elfStatus = -1;
    elfStatus = (int) fsCheckELFFile ( (unsigned long) CONTROLTHREAD_BASE );
    if ( elfStatus < 0 ){
        debug_print ("I_x64ExecuteInitialProcess: .ELF signature\n");
        panic       ("I_x64ExecuteInitialProcess: .ELF signature");
    }

// ==============
// #debug

    debug_print("I_x64ExecuteInitialProcess: [x64] Go to user mode! IRETQ\n");
    //printf     ("I_x64ExecuteInitialProcess: [x64] Go to user mode! IRETQ\n");
    //refresh_screen();

    // Here is where the boot routine ends.

    system_state = SYSTEM_RUNNING;

// =============
// Fly!
// #important:
// This is an special scenario,
// Where we're gonna fly with the eflags = 0x3000,
// it means that the interrupts are disabled,
// and the init process will make a software interrupt
// to reenable the interrupts. 
// Softwre interrupts are not affecte by this flag, I guess.
// #bugbug
// This routine is very ugly and very gcc dependent.
// We deserve a better thing.
// We need to have the same stack in the TSS.
// ss, rsp, rflags, cs, rip;
// See:
// gva.h

    if (Thread->iopl != RING3 ){
        panic ("I_x64ExecuteInitialProcess: iopl");
    }

    PROGRESS("-- Fly ----------------\n");

    unsigned long entry = (unsigned long) 0x0000000000201000;
    unsigned long rsp3  = (unsigned long) 0x00000000002FFFF0;

    asm volatile ( 
        " movq $0, %%rax  \n" 
        " mov %%ax, %%ds  \n" 
        " mov %%ax, %%es  \n" 
        " mov %%ax, %%fs  \n" 
        " mov %%ax, %%gs  \n" 
        " movq %0, %%rax  \n" 
        " movq %1, %%rsp  \n" 
        " movq $0, %%rbp  \n" 
        " pushq $0x23     \n"  
        " pushq %%rsp     \n" 
        " pushq $0x3002   \n"  // Interrupts disabled for the first thread.
        " pushq $0x1B     \n" 
        " pushq %%rax     \n" 
        " iretq           \n" :: "D"(entry), "S"(rsp3) );

// Paranoia
    PROGRESS("I_x64ExecuteInitialProcess: Unexpeted error\n");
    panic   ("I_x64ExecuteInitialProcess: Unexpeted error\n");
}


// Create the kernel process.
// It will create a process for two images:
// KERNEL.BIN and GWSSRV.BIN.
// See: gramado/core/server.

int I_x64CreateKernelProcess(void)
{
    int Status=FALSE;
    unsigned long fileret=1;
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (512 * 4096);


    debug_print ("I_x64CreateKernelProcess:\n");

//
// Window server image.
//

// WS_IMAGE_VA

    fileret = (unsigned long) fsLoadFile ( 
                                  VOLUME1_FAT_ADDRESS, 
                                  VOLUME1_ROOTDIR_ADDRESS, 
                                  FAT16_ROOT_ENTRIES,    //#bugbug: number of entries.
                                  "GWSSRV  BIN", 
                                  (unsigned long) 0x30A00000,  //0x30E00000
                                  BUGBUG_IMAGE_SIZE_LIMIT ); 

    if ( fileret != 0 ){
        printf ("I_x64CreateInitialProcess: GWSSRV.BIN \n");
        return FALSE;
    }

//
// Kernel image.
//

// IN: 
// Room, Desktop, Window
// base address, priority, ppid, name, iopl, page directory address.
// See: ps/action/process.c
// KERNELIMAGE_VA
 
    KernelProcess = (void *) create_process ( 
                                 NULL, NULL, NULL, 
                                 (unsigned long) 0x30000000, 
                                 PRIORITY_HIGH, 
                                 (int) 0,        //ppid
                                 "KERNEL-PROCESS", 
                                 RING0,   
                                 (unsigned long ) gKernelPML4Address,
                                 (unsigned long ) kernel_mm_data.pdpt0_va,
                                 (unsigned long ) kernel_mm_data.pd0_va );


// struct
    if ( (void *) KernelProcess == NULL ){
        printf ("I_x64CreateKernelProcess: KernelProcess\n");
        return FALSE;
    }

// validation
    if ( KernelProcess->used != TRUE || 
         KernelProcess->magic != 1234 )
    {
        printf ("I_x64CreateKernelProcess: KernelProcess validation\n");
        return FALSE;
    }

// pid
    if ( KernelProcess->pid != GRAMADO_PID_KERNEL )
    {
        printf ("I_x64CreateKernelProcess: pid\n");
        return FALSE;
    }


// The kernel process is a system program.
// KERNEL.BIN and GWSSRV.BIN
    KernelProcess->type = KERNEL_PROCESS;


    if ( kernel_mm_data.used != TRUE || 
         kernel_mm_data.magic != 1234 )
    {
        printf ("I_x64CreateKernelProcess: kernel_mm_data validation\n");
        return FALSE;
    }

    KernelProcess->pml4_VA = kernel_mm_data.pml4_va;
    KernelProcess->pml4_PA = kernel_mm_data.pml4_pa; 

    KernelProcess->pdpt0_VA = kernel_mm_data.pdpt0_va;
    KernelProcess->pdpt0_PA = kernel_mm_data.pdpt0_pa; 

    KernelProcess->pd0_VA = kernel_mm_data.pd0_va;
    KernelProcess->pd0_PA = kernel_mm_data.pd0_pa; 

    KernelProcess->position = KING;
    
    fs_initialize_process_cwd ( KernelProcess->pid, "/" ); 
    
// ==================
// The control thread.

    Status = I_x64CreateWSThread();

    if ( Status != TRUE ){
        printf("Couldn't Create the WS thread\n");
        return FALSE;
    }


// ====================

// Initialize the kernel module list
    int i=0;
    for (i=0; i<KMODULE_MAX; i++){
        kmList[i] = 0;
    };

// ====================
// Setup the first kernel module.
// It is not a dynlinked module.
// This is just a loadable ring0 thread with shared symbols.

    struct kernel_module_d *m;
    
    m = (struct kernel_module_d *) kmalloc( sizeof(struct kernel_module_d) );
    if( (void*) m == NULL ){
        printf ("I_x64CreateKernelProcess: m\n");
        return FALSE;
    }


// name

    char *name = "km::WindowServer";
    size_t s=0;

    memset(m->name,0,64);
    mysprintf(m->name,name);
    s = strlen(name);
    m->name_size = s;
    m->name[s] = 0;   // finalize

//#debug
    //printf ("String: {%s}\n",m->name);
    //refresh_screen();
    //while(1){}

// See:
// I_x64CreateWSControlThread()

    m->thread = (struct thread_d *) ws_thread;

// info
    m->info.entry_point = (unsigned long) ws_thread->initial_rip;
    m->info.dialog_address = 0;
    m->info.function_table_address = 0;

// Put it into the list.
    m->id = KMODULE_WS;
    kmList[KMODULE_WS] = (unsigned long) m;

//done:
    m->initialized = FALSE;
    m->used  = TRUE;
    m->magic = 1234;
    
    return TRUE;
}


// Create a ring0 thread for the window server image.
// It belongs to the kernel process.
int I_x64CreateWSThread(void)
{
    debug_print ("I_x64CreateWSThread:\n");

//
// Thread
//

// This is the control thread of the window server module.
// See: create.c, thread.h.

    ws_thread = (void *) create_tid0();

    if ( (void *) ws_thread == NULL ){
        printf ("I_x64CreateWSThread: ws_thread\n");
        return FALSE;
    }

    if ( ws_thread->used  != TRUE || 
         ws_thread->magic != 1234 )
    {
        printf ("I_x64CreateWSThread: ws_thread validation\n");
        return FALSE;
    }

// tid
    if ( ws_thread->tid != WS_TID ){
        printf ("I_x64CreateWSThread: WS_TID");
        return FALSE;
    }

// owner pid
    if ( ws_thread->ownerPID != GRAMADO_PID_KERNEL ){
        printf ("I_x64CreateWSThread: GRAMADO_PID_KERNEL");
        return FALSE;
    }

//
// Memory
//

    ws_thread->pml4_VA  = KernelProcess->pml4_VA;
    ws_thread->pml4_PA  = KernelProcess->pml4_PA;

    ws_thread->pdpt0_VA = KernelProcess->pdpt0_VA;
    ws_thread->pdpt0_PA = KernelProcess->pdpt0_PA;

    ws_thread->pd0_VA   = KernelProcess->pd0_VA;
    ws_thread->pd0_PA   = KernelProcess->pd0_PA;


// ?
// Set position.
    ws_thread->position = KING;


//
// tss
//

// #bugbug 
// #todo
    
    // ws_thread->tss = current_tss;


// Priority

    set_thread_priority ( 
        (struct thread_d *) ws_thread,
        PRIORITY_MAX );


// Qauntum

    ws_thread->quantum = QUANTUM_MAX;

// #importante
// Sinalizando que ainda não podemos usar as rotinas que dependam
// de que o dead thread collector esteja funcionando.
// Esse status só muda quando a thread rodar.

    dead_thread_collector_status = FALSE;


//
// Idle thread
//

// For now,
// the control thread of the window server will be our idle thread.
// But it is not actually a idle routine, 
// it is a standard server code.

    ____IDLE = (struct thread_d *) ws_thread;

// ??
// This is the control thread of the kernel process.
// OK, the loadable tharead that belongs to the ws is
// the kernel process's control thread. :)

    if ((void*)KernelProcess != NULL){
        KernelProcess->control = (struct thread_d *) ws_thread;
    }

    return TRUE;
}


void init_globals (void)
{
    int Status=FALSE;
    int i=0;


//#ifdef EXECVE_VERBOSE
    debug_print("init_globals:\n");
    //printf     ("init_globals:\n");
//#endif

// Inicializa as estruturas do fluxo padrão.
// Isso vai usar a file table.

    Status = (int) kstdio_initialize();
    if(Status != TRUE){
        panic("init_globals: kstdio_initialize fail\n");
    }

// Screen
// Now we can print strings in the screen.
// Reinitializing ... we already printed the banner.

    screenInit();

    debug_print("init_globals: [printf] WE HAVE MESSAGES NOW!\n");
    //printf     ("init_globals: [printf] WE HAVE MESSAGES NOW!\n");

// ===================
// Vamos atrasar configuração de janela 
// em favor de configuração de mensagem

// ??
// What is this?
// Atalho para a próxima mensagem de teclado..(test) debug
    gNextKeyboardMessage = (int) 0;

// Essa flag bloqueia e impede que uma janela obtenha o foco.
    gFocusBlocked = (int) 0;

// Profiler
// See: 
// include/kernel/profiler/pints.h

// Legacy hardware interrupts (irqs) (legacy pic)
    g_profiler_ints_irq0  = 0;
    g_profiler_ints_irq1  = 0;
    g_profiler_ints_irq2  = 0;
    g_profiler_ints_irq3  = 0;
    g_profiler_ints_irq4  = 0;
    g_profiler_ints_irq5  = 0;
    g_profiler_ints_irq6  = 0;
    g_profiler_ints_irq7  = 0;
    g_profiler_ints_irq8  = 0;
    g_profiler_ints_irq9  = 0;
    g_profiler_ints_irq10 = 0;
    g_profiler_ints_irq11 = 0;
    g_profiler_ints_irq12 = 0;
    g_profiler_ints_irq13 = 0;
    g_profiler_ints_irq14 = 0;
    g_profiler_ints_irq15 = 0;
    // ...

// Interrupção para serviços do sistema.
    g_profiler_ints_gde_services = 0;

// User and group.
    current_user  = 0;
    current_group = 0;

// Security layers.
// user session, room(window station), desktop.
    current_usersession = (int) 0;
    current_room        = (int) 0;
    current_desktop     = (int) 0;

// Process, Thread.
// See: kernel.h
    
    foreground_process = (pid_t) 0;
    foreground_thread  = (int) 0;
    current_process    = (pid_t) 0;
    current_thread     = (int) 0;

// Network
// Initialize the ports table used by the socket infrastruture.
    socket_initialize_gramado_ports();

// The kernel request
// See: request.c
    clear_request();


//
// == kgws ===========================================
//

    // =========================================
    //if ( g_useGUI != TRUE ){
    //    panic("init_globals: No GUI\n");
    //}

    // ===============================================
    // Window support.
 
    current_window    = (int) 0;  // Current Window.
    windows_count     = (int) 0;  // Window count.
    window_with_focus = (int) 0;  // Window with focus.
    current_menu      = (int) 0;  // Current Menu.
    // Continua ...

    // #todo

    // windows
    //init_windows();
        
    // #todo: Delete?
    //init_menus();

    // Continua...

    // ??
    // Messages.
    g_new_message = 0;

//
// == fs ==============================
//

// ==============================
// fs support
// FS type.
// type 1, fat16.
// #todo

    g_currentvolume_filesystem_type = FS_TYPE_FAT16;
    g_currentvolume_fatbits = (int) 16;
}






// Phases 1 and 2.
// Called in the initialization phase 0.
// Called by x64main in x64init.c
// OUT: TRUE if it is ok.

int I_init (void)
{
    int Status = FALSE;
    unsigned char ProcessorType=0;

// #debug
    debug_print ("I_init:\n");
    //printf      ("I_init:\n");

// ==================
// Check kernel phase.

    if ( InitializationPhase != 0 ){
        printf ("I_init: InitializationPhase fail. Not 0\n");
        return FALSE;
    }

// ===============================
// Globals

    PROGRESS("Kernel:2:1\n"); 
    debug_print ("I_init: Globals\n");
    init_globals();


// ===============================
// Create the progress bar.
// No bar for now.
// #importante
// À partir daqui podemos exibir strings na tela.

    PROGRESS("Kernel:2:2\n"); 

// ===============================
// io manager
// #bugbug:
// Depois de iniciar as globais, provavelmente o cursor mude 
// para o início da tela.
// então após iniciarmos as globais temos que repintar o background e 
// fazer um refresh da tela se a flag de verbose estiver ligada.

    PROGRESS("Kernel:2:3\n"); 
    debug_print ("I_init: Object manager\n");
    //init_object_manager ();
    debug_print ("I_init: io manager\n");
    //ioInit ();

// ===============================
// device manager.
// Inicializa o gerenciamento de dispositivos.
// Inicializa a lista de dispositivos.

    PROGRESS("Kernel:2:4\n"); 
    debug_print ("I_init: device manager\n");
    init_device_manager();

// ===============================
// Storage manager
// Ordem: storage, disk, volume, file system.
// #importante 
// A estrutura 'storage' vai ser o nível mais baixo.
// É nela que as outras partes devem se basear.

    PROGRESS("Kernel:2:5\n"); 
    debug_print ("I_init: storage structure\n");
    storage = (void *) kmalloc ( sizeof(struct storage_d) );
    if ( (void *) storage == NULL )
    {
       printf ("I_init: storage\n");
       return FALSE;
    }

// Storage:
// disk, volume, vfs and fs.

    disk_init();
    volume_init();
    //vfsInit();
    fsInit();
    // ...

// ==========================
// network
// Maybe we have a new plain for that in the 64bit version of Gramado.
// See: network.c

    PROGRESS("Kernel:2:6\n"); 
    debug_print ("I_init: network [FIXME]\n");
    networkInit();

// ==========================
// Initialize Platform structure.
// #todo

    PROGRESS("Kernel:2:7\n"); 
    debug_print ("I_init: Platform struct\n");

//++
// ====================================================================
    /*
    Platform = (void *) kmalloc ( sizeof(struct platform_d) );

    if ( (void *) Platform ==  NULL ){
        panic ("core-init: Platform\n");
    }else{

        // UP or MP.
        Platform->system_type = SYSTEM_TYPE_NULL;

        // Hardware
        Hardware = (void *) kmalloc ( sizeof(struct hardware_d) );

        if ( (void *) Hardware ==  NULL ){
            panic ("core-init: Hardware\n");
        }else{
            Platform->Hardware = (void *) Hardware;
            //printf(".");
        };

		//Firmware
        Firmware = (void *) kmalloc ( sizeof(struct firmware_d) );

        if ((void *) Firmware ==  NULL ){
            panic ("core-init: Firmware\n");
        }else{
            Platform->Firmware = (void *) Firmware;
            //printf("."); 
        };


		//System (software)

		// #IMPORTATE: 
		// Aqui estamos inicializando a estrutura do systema.

        System = (void *) kmalloc ( sizeof(struct system_d) );

        if ( (void *) System ==  NULL ){
            panic ("core-init: System\n");
        }else{
            System->used  = TRUE;  // Sinaliza que a estrutura esta em uso.
            System->magic = 1234;  // Sinaliza que a estrutura não esta corrompida.
            
            Platform->System = (void *) System;
            //printf(".");
        };

		//printf(" Done!\n");	
		//...
    };
    */
// ====================================================================
//--


// ===============================
// #important
// We need to be in the phase 0.

    if (InitializationPhase != 0){
        printf ("I_init: InitializationPhase\n");
        return FALSE;
    }


// ==========================
// hal

    PROGRESS("Kernel:2:8\n"); 
    debug_print ("I_init: hal\n");
    Status = init_hal();
    if (Status != TRUE){
        printf ("I_init: init_hal fail\n");
        return FALSE;
    }

// ==========================
// microkernel components:
// mm, ipc, ps ...

    PROGRESS("Kernel:2:9\n"); 
    Status = init_microkernel();
    if (Status != TRUE){
        printf ("I_init: init_microkernel fail\n");
        return FALSE;
    }

// =========================================
// Executive components

    PROGRESS("Kernel:2:10\n"); 
    Status = init_executive();
    if (Status != TRUE){
        printf ("I_init: init_executive fail\n"); 
        return FALSE;
    }

// =========================================
// Some gui components.
// #todo: rever 

    PROGRESS("Kernel:2:11\n"); 
    Status = init_gramado();
    if (Status != TRUE){
        printf ("I_init: init_gramado fail\n"); 
        return FALSE;
    }

// =========================================
// In-kernel window manager.
// #todo: maybe

    PROGRESS("Kernel:2:12\n"); 
    //init_window_manager();


// End of phase.
// Starting phase 1.

    InitializationPhase = 1;


// ========================
// We need to be in the phase 1.

    if ( InitializationPhase != 1 ){
        printf ("I_init: InitializationPhase. Not 1\n");
        return FALSE;
    }

// =========================================
// 'processor' structure.
// The 'processor' structure.
// ?? Is it 'up' or 'smp' ?

    PROGRESS("Kernel:2:13\n"); 
    processor = (void *) kmalloc ( sizeof( struct processor_d ) ); 
    if ( (void *) processor == NULL )
    {
        printf("I_init: processor\n");
        return FALSE;
    }

    // #todo
    // Check if cpuid instruction is available.
    // See: _x86_test_cpuid_support on bootmx/headlib.asm
    // #todo: extern int x86_test_cpuid_support(void);


    // Sonda pra ver qual é a marca do processador.
    // #todo: 
    // É a segunda vez que fazemos a sondagem ?!
    // See: hal/detect.c
    // This routine is valid for intel and amd processors.
    // Ok.
    // Let's make some initialization and 
    // get more information about the processor
    // using the cpuid instruction.
    // See: 
    // detect.c
    // 1pump/arch/x86/x86.c
    // 1pump/arch/amd/cpuamd.c

    ProcessorType = (int) hal_probe_processor_type();

    // Error
    // Vamos suspender isso por enquanto ...
    // precisamos apurar essa rotina de identificação da cpu.

    //if (ProcessorType <= 0){
    //    x_panic("init: [ERROR] ProcessorType\n");
    //}

    processor->Type = (int) ProcessorType;
    
    switch (ProcessorType){
    case Processor_INTEL:  
    case Processor_AMD:
        x64_init_intel();   
        //init_amd(); 
        break;
    // ...
    default:
        printf ("I_init: [ERROR] default Type");
        return FALSE;
        break;
    };


//
// qemu
//
    // Check if we are running on qemu.
    // and set flag.

    int isQEMU=FALSE;
    isQEMU = detect_IsQEMU();
    if( isQEMU == TRUE ){
        debug_print("I_init: Running on QEMU\n");
        //printf ("Running on QEMU\n");
    }


// =========================================
// process manager.
// #obs: O contexto é depedente.
// Inicializando o Process manager.

    PROGRESS("Kernel:2:14\n"); 
    // init_process_manager();

// =========================================
// Load rootdir.

    PROGRESS("Kernel:2:15\n"); 

    // #todo: 
    // We can move this above to the 'storage' section.

    // #importante
    // Só podemos carregar o diretório raiz depois que 
    // inicializamos o controlador de IDE e as estruturas de 
    // sistema de arquivos.
    // #todo: Precisamos inicializar a estrutura de diretorio
    // usada pelo diretorio raiz, para colocarmos o ponteiro
    // na estrutura de processos, juntamente com o ponteiro
    // para o cwd. 
    // Talvez essa seja a hora de criarmos a estrutura para
    // o diretorio raiz, mas ainda nao temos estrutura de processo.

    debug_print ("I_init: load rootdir.\n");
	// Carregando o diretório raiz.
    //fs_load_rootdir( VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32 );


    // Disable interrupts, lock task switch and scheduler.

    // # isso ja foi feito no começo da rotina de inicialização do kernel.
    // Tem que pensar nisso antes de tudo.

    //asm ("cli");
    //set_task_status(LOCKED); 
    //scheduler_lock();


// End of phase.
// Starting phase 2.

    InitializationPhase = 2;

// =========================================
// keyboard stuff.
// Nothing for now.

    PROGRESS("Kernel:2:16\n"); 
    debug_print ("I_init: done\n");
    //printf      ("I_init: done\n");

// ok
// Return to the main initialization routine
// in x64init.c

    return TRUE;

// ====================
// fail
// Return to the main initialization routine
// in x64init.c

//fail1:
    // If we already have printf verbose.
fail0:
    debug_print ("I_init: fail\n");
    return FALSE;
}





/*
 *************************************************
 * I_x64main: 
 *
 * Function history:
 *     2015 - Created by Fred Nora.
 */

// Initialization phases:
// 0 - I_x64main()
// 1 - See: I_init()
// 2 - See: I_init()
// 3 - See: I_x64main()
// 4 - See: I_x64main()

// Called by kernel_main in init/init.c

int I_x64main (void)
{
    int Status = FALSE;


// ============================
// Phase counter

// Starting phase 0.

    InitializationPhase = 0;


// ============================
// The first ring3 process.
// Ainda não configuramos qual será o primeiro processo
// a rodar em user mode.

    InitialProcessInitialized = FALSE;

// Obs: 
// O video já foi inicializado em main.c.
// Isso atualiza a estrutura de console do console atual.
// #bugbug: 
// A inicialização não funciona, pois os elementos das estruturas
// não guardam os valores corretamente.
// Talvez está escrevendo em lugar inapropriado.
// #test: 
// Mudamos isso para o momento em que inicializamos os consoles.
 
    // #debug
    debug_print ("I_x64main: [TODO]\n");
    //printf      ("I_x64main: [TODO]\n");
    //refresh_screen();

// System State

    if (system_state != SYSTEM_BOOTING)
    {
        debug_print ("[x64] I_x64main: system_state\n");
        //x_panic     ("[x64] I_x64main: system_state\n");
        return FALSE;
    }

// System Arch

    if (current_arch != CURRENT_ARCH_X86_64)
    {
        debug_print ("[x64] I_x64main: current_arch fail\n");
        //x_panic     ("[x64] I_x64main: current_arch fail\n");
        return FALSE;
    }

// Threads counter

    UPProcessorBlock.threads_counter = 0;

// ================================
// sse support.

    PROGRESS("Kernel:1:1\n"); 
    debug_print ("[x64] I_x64main: [TODO] SSE support\n");
    // x86_sse_init();


// ================================
// Status Flag

    gSystemStatus = 1;

// ================================
// Edition flag ?

    gSystemEdition = 0;

// =========================
// Hypervisor
// Initializing the variable.
// We will check the hv and change this flag.
// Not qemu for now.

    g_is_qemu = FALSE;


// ===================================
// I_init
// Calling the main initialization routine.
// Antes de tudo: 
// CLI, Video, runtime.
// ## BUGBUG ##
// As mensagens do abort podem não funcionarem nesse caso.
// AINDA NÃO INICIALIZAMOS O RECURSO DE MENSAGENS.
// Essa rotina só pode ser chamada 
// durante essa fase da inicialização.
// See: sysinit.c

    PROGRESS("Kernel:1:2\n"); 
    debug_print ("I_x64main: Calling I_init()\n");

    if ( InitializationPhase != 0 )
    {
        debug_print ("I_x64main: InitializationPhase\n");
        //x_panic ("I_x64main: InitializationPhase\n");
        //KiAbort();
        return FALSE;
    }

// Phases 1 and 2.

    Status = (int) I_init(); 

    if ( Status != TRUE ){
        printf ("I_x64main: I_init fail\n");
        return FALSE;
    }


// End of phase.
// Starting phase 3.

    InitializationPhase = 3;

//================================
// Initialize all the kernel graphics support.
    PROGRESS("Kernel:1:3\n"); 

// Initialize all the kernel graphics support.
// some extra things like virtual terminal and tty.
// #todo: rever essa inicializaçao.
// See: user/graphics.c

    KGWS_initialize();

    // debug
    //printf("~kgws\n");
    //refresh_screen();
    //while(1){}


// ================================
// user/ ?
// Initialize window server manager.
// ws.c
// #debug:  
// Esperamos alcaçarmos esse alvo.
// Isso funcionou gigabyte/intel
// Vamos avançar
// Quem chamou essa funçao foi o começo da inicializaçao do kernel.
// Retornamos para x86main.c para arch x86.

    PROGRESS("Kernel:1:4\n"); 
    ws_init();

// ================================
// GDT
// Setup GDT again.
// We already made this at kernel startup.
// # Caution.
// Lets create a TSS and setup a GDT.
// This way we can use 'current_tss' when we create threads.
// This function creates a TSS and sets up a GDT.
// See: hal/arch/x86/x86.c
// #todo
// Depois de renovarmos a GDT precisamos
// recarregar os registradores de segmento?

//
// DANGER !!!
//

    PROGRESS("Kernel:1:5\n"); 
    debug_print ("[x64] I_x64main: [DANGER] Initializing GDT\n");
    //printf      ("[x86] I_x64main: Initializing GDT\n");      
    x64_init_gdt();

// ================================
// Creating kernel process.
// Local
// It loads the window server's image and create
// a process structure to handle the kernel base and the
// window server's control thread.

    PROGRESS("Kernel:1:6\n"); 
    Status = I_x64CreateKernelProcess();

    if(Status != TRUE){
        debug_print ("Couldn't create the Kernel process\n");
        return FALSE;
    }

// ================================
// Creating a ring 0 thread for the kernel.
// Local
// It creates the thread used by the window server.
// It is the ring0 thread that belongs to the kernel process.
// It is also the control thread of the kernel process.
// And it is also the initial idle thread.
// #todo: The kernel needs a standar sti/hlt idle thread.

// #
// Agora a rotina que cria o processo também cria
// a thread.

    //PROGRESS("Kernel:1:7\n"); 
    //I_x64CreateWSControlThread();


// ================================
// Session Manager.
// Cria e inicializa apenas o SM.BIN
// Local routine.
// + Carrega a imagem do primeiro processo que vai rodar em user mode.
// + Configura sua estrutura de processo.
// + Configura sua estrutura de thraed.
// Não passa o comando para o processo.

    PROGRESS("Kernel:1:8\n"); 
    Status = I_x64CreateInitialProcess();

    if(Status != TRUE){
        debug_print ("Couldn't create the Initial process\n");
        return FALSE;
    }

//================================
// Check some initialization flags.
    PROGRESS("Kernel:1:9\n"); 


/*
#ifdef  ENTRY_DEBUG_CHECK_VALIDATIONS

    Status = (int) debug();

    if ( Status != 0 ){
        printf ("[x86] x86main: debug\n");
        system_state = KERNEL_ABORTED;
        goto fail;
    }

#endif
*/


	// ======== # TESTS # ========
	// begin - We can make some tests here.

    //Inicializando as variáveis do cursor piscante do terminal.
    //isso é um teste.
    
    //timer_cursor_used   = 0;   //desabilitado.
    //timer_cursor_status = 0;

// ================================
// Early ps/2 initialization.
// Initializing ps/2 controller.
// #todo: 
// Essa inicialização deve ser adiada.
// deixando para o processo init fazer isso.
// Chamaremos essa inicialização básica nesse momento.
// A inicialização completa será chamada pelo processo init.
// See: i8042.c

    PROGRESS("Kernel:1:10\n"); 
    debug_print ("[x64] I_x64main: ps2\n"); 

// ================
// Early initialization
// Only the keyboard.
// It is working
    PS2_early_initialization();


// ================
// This is the full initialization.
// #bugbug This is a test yet.
// It fails in the real machine.
    //PS2_initialization();


// ================================
// Loading some system files.
// icons, bmps, etc ...
// Loading file tests.
// #test:
// Background support.
// Used to test load_path()
// See: ws/bg.c
// #Aviso:
// Isso funcionou, não mudar de lugar.
// Mas isso faz parte da interface gráfica,
// Só que somente nesse momento temos recursos 
// suficientes para essa rotina funcionar.
// See: kgwm.c

    PROGRESS("Kernel:1:11\n"); 

    //bg_load_image ();
    windowLoadGramadoIcons();


// ================================
// Font support.
// #bugbug
// Font is independent from the x86 archtechture.
// We can do this in some other place. Maybe :)
// See: config/config.h

    PROGRESS("Kernel:1:12\n"); 
    //gfontSize = DEFAULT_FONT_SIZE;


// ================================
// Testing some rectangle support.

    PROGRESS("Kernel:1:13\n"); 

// ================================
// Done
// The routine is over.
// The caller will start the first thread create by us.
// The expected return value is 1234.

    PROGRESS("Kernel:1:14\n"); 
    debug_print ("[x64] I_x64main: done\n");
    debug_print ("==============\n");

    return TRUE;

// ================================
// The routine fails.

fail:
    // Nothing
    PROGRESS("Kernel:1:00\n"); 
fail0:
    debug_print ("[x64] I_x64main: fail\n");
    refresh_screen(); 
    return FALSE;
}


