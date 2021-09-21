
// tlib.c

#include <kernel.h>

/*
 *************************************************
 * kgws_send_to_tid:
 *
 */

// Send a message to the thread associated with the
// window with focus.

// Called by KEYBOARD_SEND_MESSAGE() in ps2kbd.c.

// #todo:
// We need to associate the current thread and the current tty.
// tty->control
// window->control

// #bugbug
// Only keyboard messages,
// long1 and long2 will mask to single byte.
// IN: tid, window, message code, ascii code, raw byte.

// Post message.
// Asynchronous.
int
post_message_to_tid ( 
    int tid, 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

// Target thread.
    struct thread_d *t;
    int target_tid = (int) (tid & 0xFFFF);

    unsigned long tmp_msg=0;


    //#debug
    debug_print("kgws_send_to_tid:\n");

    if ( target_tid < 0 || target_tid >= THREAD_COUNT_MAX )
    {
        debug_print("kgws_send_to_tid: target_tid\n");
        goto fail;
    }

//
// Pega a thread alvo. 
//

    t = (struct thread_d *) threadList[target_tid];

    if ( (void *) t == NULL ){
        panic ("kgws_send_to_tid: t \n");
    }

    if ( t->used != 1 || t->magic != 1234 ){
        panic ("kgws_send_to_tid: t validation \n");
    } 


//
// Wake up the target thread ?
//

    // wakeup_thread(t->tid);

// #todo
// Podemos melhorar a prioridade ou o quantum da thread alvo.

//
// The message
//

    tmp_msg = (unsigned long) (msg & 0xFFFF);

    //Send system message to the thread.
    t->window_list[ t->tail_pos ]  = (unsigned long) window;
    t->msg_list[ t->tail_pos ]     = (unsigned long) (tmp_msg & 0xFFFF);
    t->long1_list[ t->tail_pos ]   = (unsigned long) long1;
    t->long2_list[ t->tail_pos ]   = (unsigned long) long2;

    t->tail_pos++;
    if ( t->tail_pos >= 31 )
        t->tail_pos = 0;


//
// #test
//

// lets end this round putting a given thread at the end
// of this round.
// This is gonna be the next, 
// and the last of this round.
    
    // cut_round( (struct thread_d *) t );

    //ok
    return 0;

fail:
    // fail
    return -1;
}


// Post message to the foreground thread.
// Asynchronous.
int
post_message_to_foreground_thread ( 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    if( foreground_thread < 0 )
        return -1;

    return (int) post_message_to_tid( 
                     foreground_thread,
                     window,
                     msg,
                     long1,
                     long2 );
}

// service 112
// Post message to tid.
// Asynchronous.
unsigned long
sys_post_message_to_tid( 
    int tid, 
    unsigned long message_buffer )
{

    if( tid < 0 || tid >= THREAD_COUNT_MAX )
        return 0;

    if( message_buffer == 0 )
        return 0;

    unsigned long *event = (unsigned long *) message_buffer;

// Post message.
// Asynchronous.

    post_message_to_tid(
        tid,         //tid
        event[0],    //window
        event[1],    //msg code
        event[2],    //long1
        event[3] );  //long2

    return 0;
}



/*
 * show_slot:
 *     Show info about a thread.
 */

void show_slot (int tid){

    struct thread_d  *t;


// tid
    if ( tid < 0 || tid >= THREAD_COUNT_MAX )
    {
        printf ("show_slot: tid\n");
        goto fail;
    }

// Thread
    t = (void *) threadList[tid];

    if ( (void *) t == NULL ){
        printf ("show_slot: t\n");
        goto fail;
    }

// Show one slot

    printf ("\n");
    printf ("TID   PID   pdPA  Prio  State Quan *Jiffies initial_rip rflags   tName \n");
    printf ("====  ====  ====  ====  ===== ==== ====    ==========  ======  ===== \n");
    printf ("%d    %d    %x   %d    %d    %d    %d      %x          %x      %s \n", 
        t->tid, 
        t->ownerPID,
        t->pml4_PA,
        t->priority, 
        t->state,
        t->quantum,    // Quantum
        t->step,       // >>> Jiffies
        t->initial_rip,
        t->rflags,
        t->name_address );

    goto done;

fail:
    printf ("Fail\n");
done:
    return; 
}

/*
 *****************************************
 * show_slots:
 *     Show info about all threads.
 */

// Loop
// Mostra as tarefas válidas, mesmo que estejam com problemas.

void show_slots(void)
{
    struct process_d  *p;
    struct thread_d   *t;
    int i=0;

    printf("\n");
    //printf("Thread info: jiffies{%d} sec{%d} min{%d}\n", 
        //jiffies, seconds, (seconds/60));

    printf("Thread info: jiffies{%d} \n", jiffies );
    printf("Focus tid {%d}\n",foreground_thread);

    for ( i=0; i<THREAD_COUNT_MAX; i++ )
    {
        t = (void *) threadList[i];

        if ( (void *) t != NULL && 
             t->used  == 1 && 
             t->magic == 1234 )
        {
            show_slot(t->tid);
        }
    };
}

/*
 *************************************************
 * show_reg:
 *     Show the content of the registers.
 * 
 *    rflags
 *    cs:rip
 *    ss:rsp
 *    ds,es,fs,gs
 *    a,b,c,d
 */

void show_reg (int tid){

    struct thread_d *t; 


    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        printf ("show_reg: fail\n");
        return;
    }

    // Structure.
    t = (void *) threadList[tid];

    if ( (void *) t == NULL ){
        printf ("show_reg: fail\n");
        return;

    } else {

        // Show registers.

        printf ("\n rflags=[%x]", t->rflags);
        printf ("\n cs:rip=[%x:%x] ss:rsp=[%x:%x]", 
            t->cs, t->rip, t->ss, t->rsp );
        printf ("\n ds=[%x] es=[%x] fs=[%x] gs=[%x]",
            t->ds, t->es, t->fs, t->gs );
        printf ("\n a=[%x] b=[%x] c=[%x] d=[%x]\n",
            t->rax, t->rbx, t->rcx, t->rdx );
        
        // r8~r12
        // ...
    };
}

/* 
 *********************************
 * set_thread_priority: 
 *
 */

// Muda a prioridade e o quantum de acordo com a prioridade.

void 
set_thread_priority ( 
    struct thread_d *t, 
    unsigned long priority )
{

    unsigned long OldPriority  = 0;
    unsigned long BasePriority = 0;


    // Limits
    if ( priority > PRIORITY_MAX )
        return;

    if ( (void *) t == NULL )
    {
        //
        return;

    }else{

        // Validation
        if ( t->used != 1 || t->magic != 1234 ){ return; }

        OldPriority  = t->priority;
        BasePriority = t->base_priority;

        // Se aprioridade solicitada for igual da prioridade atual.
        if ( priority == OldPriority ){ return; }
        
        // Se a prioridade solicitada for menor que a prioridade basica.
        if ( priority < BasePriority ){ return; }

        // Se a prioridade basica pertencer a classe de tempo real
        // nao podemos mudar a prioridade.
        if ( BasePriority > PRIORITY_NORMAL ){ return; }
        
        // limits again
        if ( priority > PRIORITY_MAX )
        {
            t->priority = PRIORITY_MAX;
        }

        // Change!
        // Se aprioridade solicitada for diferente da prioridade atual.
        // Tambem nao pode ser menor que a base.
        if ( priority != OldPriority )
        {
            // Muda a prioridade.
            t->priority = priority;
        
            t->quantum = ( priority * TIMESLICE_MULTIPLIER );

            if ( t->quantum < QUANTUM_MIN ){  t->quantum = QUANTUM_MIN; }
            if ( t->quantum > QUANTUM_MAX ){  t->quantum = QUANTUM_MAX; }
        };    
        // ...
    };
    // ??
}

// muda a prioridade para alem dos limites ... teste.
void threadi_power(
    struct thread_d *t, 
    unsigned long priority )
{

    if ( (void *) t == NULL ){ return; }
   
    if ( t->used != 1 || t->magic != 1234 ){ return; }
    
    t->priority = priority;
    t->quantum = ( priority * TIMESLICE_MULTIPLIER );

    if ( t->quantum < QUANTUM_MIN ){  t->quantum = QUANTUM_MIN; }
    if ( t->quantum > QUANTUM_MAX ){  t->quantum = QUANTUM_MAX; }
}


/*
 ****************************************
 * release:
 * #importante
 * Isso deve liberar uma thread que estava esperando 
 * ou bloqueada por algum motivo.
 * Obs: Aqui não devemos julgar se ela pode ou não ser
 * liberada, apenas alteramos se estado.
 */
 
void release ( int tid ){

    struct thread_d *Thread;


    if ( tid < 0 || tid >= THREAD_COUNT_MAX )
    {
        //  
		return; 
    }
	
    Thread = (void *) threadList[tid];

    if ( (void *) Thread == NULL ){
		//
		return; 

    }else{

        if ( Thread->magic != THREAD_MAGIC )
        {
            return; 
        }

		//#importante:
		//Não estamos selecionando ela para execução
		//Apenas estamos dizendo que ela está pronta para
		//executar.

        Thread->state = READY; 
    };
}


void 
SetThread_PML4PA ( 
    struct thread_d *thread, 
    unsigned long pa )
{

    if ( (void *) thread == NULL ){
        //
        return;

    }else{
		
		//@todo:
		//Aqui podemos checar a validade da estrutura,
		//antes de operarmos nela.

        thread->pml4_PA = (unsigned long) pa;
    };
}

void check_for_dead_thread_collector (void)
{
    // #importante
    // Essa flag é acionada quando uma thread 
    // entra em estado zombie.

    switch (dead_thread_collector_flag)
    {
        // waik up
        // Liberamos a thread.
        // O próprio dead thread collector vai sinalizar que 
        // quer dormir, dai o case default faz isso.

        case 1:
            // #suspenso
            // release ( RING0IDLEThread->tid );
            break;

        // sleep
        default:
            // #suspenso
            //block_for_a_reason ( 
            //    RING0IDLEThread->tid, 
            //    WAIT_REASON_BLOCKED );
            
            dead_thread_collector_flag = FALSE;
            break;
    };
}


/*
 * dead_thread_collector:
 *     Procura por uma thread no estado zombie mata ela.
 *     #todo: Alertar o processo que a thread morreu.
 */

void dead_thread_collector (void){

    register int i=0;  //loop
    
    struct process_d  *p; 
    struct thread_d   *Thread; 


	//
	// Check idle
	//

    if ( (void *) ____IDLE == NULL ){
        panic ("dead_thread_collector: ____IDLE fail");

    }else{

        if ( ____IDLE->used != 1 || ____IDLE->magic != 1234 )
        {
            panic ("dead_thread_collector: ____IDLE validation");
        }
        // ...
    };

    // loop
    // Scan
    for ( i=0; i < THREAD_COUNT_MAX; i++ )
    {
	    Thread = (void *) threadList[i];
		
		if ( (void *) Thread != NULL )
		{
		    if ( Thread->state == ZOMBIE && 
			     Thread->used == 1 && 
				 Thread->magic == 1234 )
			{

                // Se queremos deletar a idle.
                if ( Thread->tid == ____IDLE->tid ){
                    panic ("dead_thread_collector: Sorry, we can't kill the idle thread!");
                }

				//kill_thread(Thread->tid);
				Thread->used = 0;
				Thread->magic = 0;
				Thread->state = DEAD; // Por enquanto apenas fecha.
				//...
			    
				// #importante:
				// Nessa hora precisamos notificar o 
				// a thread que estava esperando essa thread  
				// terminar.
				// Se essa for a thread primária então o processo 
				// irá terminar também, então o processo que esperava 
				// também deverá ser notificado.
				
                //Thread = NULL;
	            //threadList[i] = NULL;   //@todo: Liberar o espaço na lista.


				//ProcessorBlock.threads_counter--;
				//if ( ProcessorBlock.threads_counter < 1 ){
					//panic("dead_thread_collector: threads_counter");
				//}

				UPProcessorBlock.threads_counter--;
				if ( UPProcessorBlock.threads_counter < 1 ){
					panic ("dead_thread_collector: threads_counter");
				}

			};
			//Nothing.
		};
		//Nothing.
	};
	
	//@todo:
	// * MOVEMENT 10 (zombie --> Dead)
	// * MOVEMENT 11 (zombie --> Initialized) .. reinicializar.	
}


/*
 * kill_thread:
 *     Destrói uma thread.
 *     Destroi a estrutura e libera o espaço na lista.
 */

void kill_thread (int tid){

    struct thread_d *__Thread;


	//Limits.
    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        debug_print ("kill_thread: tid");
        return;
    }


    // #bugbug
    // The IDLE thread.
    
    if ( (void *) ____IDLE == NULL ){
        panic ("kill_thread: ____IDLE fail");
    
    }else{

        if ( ____IDLE->used != 1 || ____IDLE->magic != 1234 )
        {
            panic ("kill_thread: ____IDLE validation");
        }

	    // Se queremos deletar a idle.
        if ( tid == ____IDLE->tid ){
            panic ("kill_thread: Sorry, we can't kill the idle thread!");
	    }
	    // ...
    };

	
	//
	// @todo: 
	//    Deve acordar o pai que está esperando o filho fechar.
	//    Mas no caso estamos apenas fechando uma thread.
    //    Caso essa não seja a thread primária, isso não deve 
    // causar o fechamento do processo.	
    //

    __Thread = (void *) threadList[tid];

	// A thread alvo nem existe,
	// vamos apenas continuar.

    if ( (void *) __Thread == NULL ){
        printf ("kill_thread: This thread doesn't exist!\n");
        refresh_screen();
        return;
    }else{


        /*
        // This process can't be killed by another process.
        if ( __Thread->_protected == 1 )
        {
            debug_print("exit_process: [FAIL] This thread can't be killed\n");
            debug_print("This is a protected thread!\n");
            return;  
        }
        */


        // #todo 
        // Pegar o id do pai e enviar um sinal e acorda-lo
        //se ele estiver esperando por filho.		
        __Thread->used  = 0;
        __Thread->magic = 0; 
        __Thread->state = DEAD; 
        // ...


		//ProcessorBlock.threads_counter--;
		//if ( ProcessorBlock.threads_counter < 1 ){
			//#bugbug
			//panic("kill_thread: threads_counter");
		//}

        UPProcessorBlock.threads_counter--;
        if ( UPProcessorBlock.threads_counter < 1 ){
            panic ("kill_thread: threads_counter");
        }


        threadList[tid] = (unsigned long) 0;
        __Thread = NULL;		
        
         
        // se matamos a thread atual.         
        if ( tid == current_thread )
        {
            //#todo:
            panic("kill_thread: #todo call scheduler()\n");
            //scheduler ();
        }

        
        // Se falhou o escalonamento.
        if ( current_thread < 0 || 
             current_thread >= THREAD_COUNT_MAX )
        {
            current_thread = ____IDLE->tid;
        }
    };

done:
    debug_print ("kill_thread: done\n");
    return;
}


void kill_all_threads (void)
{
    register int i=0;
    for ( i=0; i < THREAD_COUNT_MAX; ++i ){ kill_thread(i); };
}











