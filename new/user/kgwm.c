

#include <kernel.h> 


// 
// Imports
//

// from swlib.asm
// Used to handle callbacks used by the window server.
extern unsigned long wmData_RDI;
extern unsigned long wmData_RSI;
extern unsigned long wmData_RDX;
extern unsigned long wmData_RCX;
extern unsigned long wmWindowMananer_SendMessage(void);


// from swlib.asm
// Used to handle callbacks used by the window server.
extern unsigned long wmData_Callback0;
extern unsigned long wmData_Callback1;
extern unsigned long wmData_Callback2;


// ============================
static unsigned long presence_level=32;
static unsigned long flush_fps=30;

// ============================





// Register callbacks sent by gwssrv.bin
void 
wmRegisterWSCallbacks(
    unsigned long callback0,
    unsigned long callback1,
    unsigned long callback2 )
{

    printf("wmRegisterWSCallbacks:\n");


    gUseWMCallbacks = TRUE;

    // Save callbacks
    // See: swlib.asm
    wmData_Callback0 = (unsigned long) callback0;
    wmData_Callback1 = (unsigned long) callback1;
    wmData_Callback2 = (unsigned long) callback2;
    
    printf("wmData_Callback0 = %x \n", wmData_Callback0);
    printf("wmData_Callback0 = %x \n", wmData_Callback1);
    printf("wmData_Callback0 = %x \n", wmData_Callback2);

    // No while.
    // scroll will not work
    //while(1){
        //asm("call *%0" : : "r"(wsCallbacks[0]));
    //}
    
    printf("done\n");
    refresh_screen();
    
// #importante
// Nesse momento podemos criar
// interrupçoes para esses endereços.

    //while(1){}
}


// Send input to the window manager
// inside the window server ( gwssrv.bin )
unsigned long wmSendInputToWindowManager(
    unsigned long wid,
    unsigned long msg,
    unsigned long long1,
    unsigned long long2)
{

    
    if( gUseWMCallbacks != TRUE)
        return 0;

    //asm("call *%0" : : "r"(wsCallbacks[0]));

// Setup parameters
// See: swlib.asm

    wmData_RDI = (unsigned long) (wid & 0xFFFF); 
    wmData_RSI = (unsigned long) (msg & 0xFFFF);
    wmData_RDX = (unsigned long) long1;
    wmData_RCX = (unsigned long) long2;


// Trampoline
// See: swlib.asm

    return (unsigned long) wmWindowMananer_SendMessage();
}






/*
 ***************************************** 
 * wmProcedure:
 * 
 *       Some combinations with control + F1~F12
 */

// Local function.

// Called by kgws_event_dialog in kgws.c
// Called by si_send_longmessage_to_ws and si_send_message_to_ws
// in siws.c 
// #bugbug: We don't wanna call the window server. Not now.

// #important:
// Isso garante que o usuário sempre podera alterar o foco
// entre as janelas do kgws usando o teclado, pois essa rotina
// é independente da thread que está em foreground.

// #todo
// Talvez a gente possa usar algo semelhando quando o window
// server estiver ativo. Mas possivelmente precisaremos 
// usar outra rotina e não essa. Pois lidaremos com uma estrutura
// de janela diferente, que esta localizada em ring3.

// From Windows:
// Because the mouse, like the keyboard, 
// must be shared among all the different threads, the OS 
// must not allow a single thread to monopolize the mouse cursor 
// by altering its shape or confining it to a small area of the screen.

// #todo
// This functions is the moment to check the current input model,
// and take a decision. It will help us to compose the event message.
// It is because each environment uses its own event format.

unsigned long 
wmProcedure ( 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int Status = -1;
    int UseSTDIN=TRUE;      // Input model
    char ch_buffer[2];
    char buffer[128];


    unsigned long tmp_value=0;

    debug_print("wmProcedure:\n");
    
    sprintf (buffer,"My \x1b[8C string!!\n"); 


// ===================================
// Control:
//     'Control + ?' belongs to the kernel.

// ===================================
// Shift:
//     'Shift + ?' belongs to the window server.


//
// Dispatcher
//

    if (msg<0){
        debug_print("wmProcedure: Invalid msg\n");
        return 0;
    }

    switch (msg){


        case MSG_KEYDOWN:

            // Para todas as teclas quando o console não está ativo.
            if ( ShellFlag!=TRUE ){
                wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
            }

            switch (long1){

            case VK_RETURN:
                //if(ShellFlag!=TRUE){
                    //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
                    //return 0;
                //}
                if(ShellFlag==TRUE)
                {
                    input('\0');               // finalize
                    consoleCompareStrings();   // compare
                    invalidate_screen();
                    //refresh_screen();
                    return 0;
                }
                break; 

            //case VK_TAB: 
                //printf("TAB\n"); 
                //invalidate_screen();
                //refresh_screen(); 
                //break;

            default:

                // Not console.
                // Pois não queremos que algum aplicativo imprima na tela
                // enquanto o console virtual está imprimindo.
                if ( ShellFlag!=TRUE )
                {
                    // Send it to the window server.
                    //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);

                    // #test
                    // Write into stdin
                    if ( UseSTDIN == TRUE )
                    {
                        //debug_print ("wmProcedure: Writing into stdin ...\n");
                        stdin->sync.can_write = TRUE;
                        ch_buffer[0] = (char) (long1 & 0xFF);
                        sys_write(0,ch_buffer,1);
                        return 0;
                    }
                }

                // Console!
                // O teclado vai colocar o char no prompt[]
                // e exibir o char na tela somente se o prompt
                // estiver acionado.
                if (ShellFlag==TRUE)
                {
                    consoleInputChar(long1);
                    console_putchar ( (int) long1, fg_console );
                    return 0;
                }

                //debug_print ("wmProcedure: done\n");
                return 0;
                break;
            };
            break;


        // Pressionadas: teclas de funçao
        case MSG_SYSKEYDOWN:
             
            // Send it to the window server.
            wmSendInputToWindowManager(0,MSG_SYSKEYDOWN,long1,long2); 

            switch (long1){

                // Exibir a surface do console.
                case VK_F1:
                    if (ctrl_status == TRUE){
                        
                        // dado em char size, ou linhas
                        //drawDataRectangle( 
                         //   (CONSOLE_TTYS[fg_console].cursor_left * 8), 
                         //   (CONSOLE_TTYS[fg_console].cursor_top * 8), 
                         //   (CONSOLE_TTYS[fg_console].cursor_right * 8), 
                         //   (CONSOLE_TTYS[fg_console].cursor_bottom * 8), 
                         //   COLOR_GRAY );
                         //refresh_screen(); //usar refresh rectangle
                         
                         return 0;
                        
                    }
                    if (alt_status == 1){
                        printf ("wmProcedure: alt + f1\n");
                        refresh_screen();
                    }
                    if (shift_status == 1){
                        //jobcontrol_switch_console(0);
                    }
                    return 0;
                    break;

                case VK_F2:
                    if (ctrl_status == TRUE){
                         //powertrio_select_client(1);
                    }
                    if (alt_status == 1){
                        printf ("wmProcedure: alt + f2\n");
                        refresh_screen();
                    }
                    if (shift_status == 1){
                        //jobcontrol_switch_console(1);
                    }
                    return 0;
                    break;

                case VK_F3:
                    if (ctrl_status == TRUE){
                        //powertrio_select_client(2);
                    }
                    if (alt_status == 1){
                        printf ("wmProcedure: alt + f3\n");
                        refresh_screen();
                    }
                    if (shift_status == 1){
                        //jobcontrol_switch_console(2);
                    }
                    return 0;
                    break;

                case VK_F4:
                    if (ctrl_status == TRUE){
                        //powertrio_next();
                    }
                    if (alt_status == 1){
                        printf ("wmProcedure: alt + f4\n");
                        refresh_screen();
                    }
                    if (shift_status == 1){
                        //jobcontrol_switch_console(3);
                    }
                    return 0;
                    break;


                // Reboot
                case VK_F5:
                    if (ctrl_status == TRUE){
                        //powertrio_select_client(0);
                        //reboot();
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f5\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        //kgwm_next();
                    }
                    return 0;
                    break;

                // Send a message to the Init process.
                // 9216 - Launch the redpill application
                case VK_F6:
                    if (ctrl_status == TRUE){
                        //powertrio_select_client(1);
                        // #todo: 
                        // shutdown. Only the ring3 applications
                        // can shutdown via qemu for now. 
                        //__kgwm_SendMessageToInitProcess(9216); 
                        return 0; 
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f6\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        //kgwm_next();
                    }
                    return 0;
                    break;

                // Test 1.
                case VK_F7:
                    if (ctrl_status == TRUE){
                        //powertrio_select_client(2);
                       // Send message to init process to launch gdeshell.
                        //__kgwm_SendMessageToInitProcess(9217);
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f7\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        //kgwm_next();
                    }
                    return 0;
                    break;

                // Test 2.
                case VK_F8:
                    if (ctrl_status == TRUE){

                        // #test: IN: (wid,msg,long1,long2)
                        //wmSendInputToWindowManager(0,1,0,0);
                        //wmSendInputToWindowManager(0,2,0,0);
                        //wmSendInputToWindowManager(0,3,0,0);
                        //wmSendInputToWindowManager(0,4,0,0);
                        //wmSendInputToWindowManager(0,5,0,0);
                        //wmSendInputToWindowManager(0,6,0,0);
                        //wmSendInputToWindowManager(0,7,0,0);  //close
                        wmSendInputToWindowManager(0,MSG_PAINT,0,0);
                        //wmSendInputToWindowManager(0,9,0,0);
                        //wmSendInputToWindowManager(0,10,0,0);
                        //wmSendInputToWindowManager(0,18,0,0);   //set focus
                        //wmSendInputToWindowManager(0,19,0,0);   //get focus

                        //#oldtest
                        //powertrio_next();
                        // Send message to init process to launch the launcher.
                        //__kgwm_SendMessageToInitProcess(9216); 
                        //__kgwm_SendMessageToInitProcess(9218);  // launch sysmon
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f8\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        //kgwm_next();
                    }
                    return 0;
                    break;


                case VK_F9:
                // #todo:
                // [Window Manager]: Switcher
                    if (ctrl_status == TRUE){
                        printf("\n");
                        printf ("Prompt ON: Type something\n");
                        //printf("\n");
                        consolePrompt();
                        ShellFlag = TRUE;
                        refresh_screen();
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f9\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        // #goal
                        // This is a emergency keycombination.
                        // We can't call another process and 
                        // we want to reboot the machine.
                        sys_reboot();
                        //__kgwm_SendMessageToInitProcess(9216);  //reboot
                    }
                    return 0;
                    break;

                case VK_F10:
                // #todo:
                // [Window Manager]: Minimize
                    if (ctrl_status == TRUE){
                        printf("\n");
                        printf ("Prompt OFF: Bye\n");
                        printf("\n");
                        ShellFlag = FALSE;
                        refresh_screen();
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f10\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        Background_initialize();
                        show_slots(); //See: tlib.c
                        //pages_calc_mem();
                        refresh_screen();
                        //__kgwm_SendMessageToInitProcess(9217);  //gdeshell
                    }
                    return 0;
                    break;

                case VK_F11:
                // #todo:
                // [Window Manager]: Restore
                    if (ctrl_status == TRUE){
                        tmp_value = get_update_screen_frequency();
                        tmp_value--;
                        set_update_screen_frequency(tmp_value);
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f11\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                       hal_reboot();
                       // #bugbug: Something is wrong with this routine.
                       //__kgwm_SendMessageToInitProcess(9218);  // redpill application
                    }
                    return 0;
                    break;

                case VK_F12:
                // #todo:
                // [Window Manager]: Close
                    if (ctrl_status == TRUE){
                        tmp_value = get_update_screen_frequency();
                        tmp_value++;
                        set_update_screen_frequency(tmp_value);
                    }
                    if (alt_status == TRUE){
                        printf ("wmProcedure: alt + f12\n");
                        refresh_screen();
                    }
                    if (shift_status == TRUE){
                        // Switch focus
                        wmSendInputToWindowManager(0,9090,0,0);
                        //__kgwm_SendMessageToInitProcess(9219);  // sysmon
                    }
                    return 0;
                    break;

                default:
                    // nothing
                    return 0;
            
            }

        default:
            return 0;
            break;
    };


//unexpected_fail:
    debug_print("wmProcedure: unexpected fail\n");
    return 0;

fail:
    debug_print("wmProcedure: fail\n");
    refresh_screen();
    return 0;
}


// called by init()
int init_gramado (void)
{
    debug_print ("init_gramado: [?? fixme]\n");
    return 0;
}


/*
 ***********************************
 * windowLoadGramadoIcons:
 * 
 *     Carrega alguns ícones do sistema.
 *     It's a part of the window system's initialization.
 */

int windowLoadGramadoIcons (void)
{
    unsigned long fRet=0;

	//#debug
	//printf("windowLoadGramadoIcons:\n");

	//
	//  ## Icon support ##
	//

//iconSupport:

	// Carregando alguns ícones básicos usados pelo sistema.

	// ## size ##
	// Vamos carregar ícones pequenos.
	//@todo checar a validade dos ponteiros.


    // #bugbug
    // Size determinado, mas não sabemos o tamanho dos ícones.

    // 4 pages.
    // 16 KB ? Is it enough ?
    // Sim, os ícones que estamos usam possuem no máximo 2KB.
    // See: base/

    unsigned long tmp_size = (4*4096);

    // See: window.h
    shared_buffer_app_icon       = (void *) allocPages(4);
    shared_buffer_file_icon      = (void *) allocPages(4);
    shared_buffer_folder_icon    = (void *) allocPages(4);
    shared_buffer_terminal_icon  = (void *) allocPages(4);
    shared_buffer_cursor_icon    = (void *) allocPages(4);
    // ...


    if ( (void *) shared_buffer_app_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_app_icon\n");
    }

    if ( (void *) shared_buffer_file_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_file_icon\n");
    }

    if ( (void *) shared_buffer_folder_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_folder_icon\n");
    }

    if ( (void *) shared_buffer_terminal_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_terminal_icon\n");
    }

    if ( (void *) shared_buffer_cursor_icon == NULL ){
        panic ("windowLoadGramadoIcons: shared_buffer_cursor_icon\n");
    }

	//
	// Load
	//

    // app icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS,
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "APP     BMP", 
                               (unsigned long) shared_buffer_app_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: APP.BMP\n");
    }

    // file icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS,
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES, 
                               "FILE    BMP", 
                               (unsigned long) shared_buffer_file_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: FILE.BMP\n");
    }

    // folder icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "FOLDER  BMP", 
                               (unsigned long) shared_buffer_folder_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: FOLDER.BMP\n");
    }

    // terminal icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES, 
                               "TERMINALBMP", 
                               (unsigned long) shared_buffer_terminal_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: TERMINAL.BMP\n");
    }


    // cursor icon
    fRet = (unsigned long) fsLoadFile ( 
                               VOLUME1_FAT_ADDRESS, 
                               VOLUME1_ROOTDIR_ADDRESS, 
                               FAT16_ROOT_ENTRIES,
                               "CURSOR  BMP", 
                               (unsigned long) shared_buffer_cursor_icon,
                               tmp_size );
    if ( fRet != 0 ){
        panic ("windowLoadGramadoIcons: CURSOR.BMP\n");
    }

	// More ?

    return 0;
}




// Get a shared buffer to a system icon.
// it is gonna be used by the window server.
// It is a pre allocated buffer containg an bmp icon loaded at it.
// Service 9100

void *ui_get_system_icon ( int n )
{
    if (n <= 0){ return NULL; }

    // See: window.h
    switch (n){

    case 1: return (void *) shared_buffer_app_icon;       break;
    case 2: return (void *) shared_buffer_file_icon;      break;
    case 3: return (void *) shared_buffer_folder_icon;    break;
    case 4: return (void *) shared_buffer_terminal_icon;  break;
    case 5: return (void *) shared_buffer_cursor_icon;    break;
    // ...
    };

    return NULL;
}



// =========================

unsigned long get_update_screen_frequency(void)
{
    return (unsigned long) flush_fps;
}

void set_update_screen_frequency(unsigned long fps)
{
    if(fps==0){fps=1;};
    if(fps>=1000){fps=1000;};

// See: sched.h
    flush_fps = (unsigned long) (fps&0xFFFF);

    presence_level = (unsigned long)(1000/flush_fps);
    presence_level = (unsigned long) (presence_level & 0xFFFF);
}

unsigned long get_presence_level(void)
{
    return (unsigned long) presence_level;
}

void set_presence_level(unsigned long value)
{
    if(value==0){ value=1; }
    if(value>1000){ value=1000; }
    presence_level = value;
}


// Called by task_switch
void schedulerUpdateScreen(void)
{
    int i=0;
    struct thread_d *TmpThread;

    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("schedulerUpdateScreen: w h\n");
        panic       ("schedulerUpdateScreen: w h\n");
    }

// Atualizado pelo timer.
    if( UpdateScreenFlag != TRUE )
        return;

    deviceWidth  = (deviceWidth & 0xFFFF);
    deviceHeight = (deviceHeight & 0xFFFF);


// ============================
// Redraw and flush stuff.

// Calling the window manager inside the window server
// at gwssrv.bin
// 9091 = message code for calling the compositor.

    if( gUseWMCallbacks == TRUE)
        wmSendInputToWindowManager(0,9091,0,0);

// ============================

    // Precisamos apenas validar todos retangulos
    // porque fizemos refresh da tela toda.
    
    int validate_all= FALSE;

//
// Flush the whole screen and exit.
//

// The whole screen is invalidated.
    if ( screen_is_dirty == TRUE )
    {
        refresh_screen();
        validate_all = TRUE;
        
        // Validate the screen
        screen_is_dirty = FALSE;
    }


/*
//=========================
// Blue bar:
    unsigned long fps = get_update_screen_frequency();
    char data[32];
    drawDataRectangle( 
        0, 0, deviceWidth, 24, COLOR_BLUE, 0 );
    sprintf(data,"  FPS %d       ",fps);
    data[12]=0;
    draw_string(0,8,COLOR_YELLOW,data);
    refresh_rectangle ( 0, 0, deviceWidth, 24 );
//=========================
*/


//
// Flush a list of dirty surfaces.
//

    for ( i=0; i < THREAD_COUNT_MAX; ++i )
    {
        TmpThread = (void *) threadList[i];

        if ( (void *) TmpThread != NULL )
        {
            if ( TmpThread->used  == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == READY )
            {
                // #test 
                debug_print("  ----  Compositor  ----  \n");
                
                if ( (void *) TmpThread->surface_rect != NULL )
                {
                    if( TmpThread->surface_rect->used == TRUE && 
                        TmpThread->surface_rect->magic == 1234 )
                    {
                        // Como fizemos refresh da tela toda,
                        // então precisamos validar todos os retângulos.
                        
                        if ( validate_all == TRUE )
                            TmpThread->surface_rect->dirty = FALSE;

                        // dirty rectangle
                        // Se uma surface está suja de tinta.
                        // Precisamos copiar para o framebuffer.


                        if( TmpThread->surface_rect->dirty == TRUE )
                        {
                            refresh_rectangle ( 
                                TmpThread->surface_rect->left, 
                                TmpThread->surface_rect->top, 
                                TmpThread->surface_rect->width, 
                                TmpThread->surface_rect->height );
                            
                            // Validate the surface. (Rectangle)
                            TmpThread->surface_rect->dirty = FALSE;
                        }

                    }
                }
            }
        }
    };
    

// Chamamos o 3d demo do kernel.
// See: kgws.c

    if (DemoFlag==TRUE)
    {
        //demo0();
        DemoFlag=FALSE;
    }


// Atualizado pelo timer.
    UpdateScreenFlag = FALSE;
}












