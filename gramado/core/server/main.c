/*
 * File: main.c
 *    Main file for the Gramado Window Server.
 *    History:
 *        2020 - Created by Fred Nora.
 */

// See:
// Transformations
// http://math.hws.edu/graphicsbook/c2/s3.html

// See:
// https://wiki.osdev.org/Graphics_stack
// https://wayland-book.com/introduction/high-level-design.html
// ...

// See:
// x window system core protocol
// https://en.wikipedia.org/wiki/X_Window_System_core_protocol
// ...

// See:
// https://www.x.org/wiki/Releases/History/

// hostname:D.S
// [host]:<display>[.screen]
// [host]:<display>.[screen]

// main.c
// Arquivo principal do gws.
// As funções começam com o nome do módulo
// para que em client side começem com gws_

/*
See: https://wiki.osdev.org/Graphics_stack

    > Application Layer
    > Interoperation Layers
        +Desktop Management Layer
        +Window Management Layer 
    > Presentation Layers
        +Compositing Layer
        +Widget Toolkit Layer
        +Rendering Layer 
    > Display Layers
        +Device Driver Layer
        +Hardware Layer 

*/

// This file is part of this project.
// It is NOT a library.

#include <gws.h>


#define VERSION  "0.1"
#define VERSION_MAJOR  0
#define VERSION_MINOR  1


//
// == Gramado Network Protocol ===============================
//

// #standard
#define GNP_WID        0
#define GNP_MESSAGE    1
#define GNP_LONG1      2
#define GNP_LONG2      3
// #extension
#define GNP_LONG3      4
#define GNP_LONG4      5
#define GNP_LONG5      6
#define GNP_LONG6      7
// #body
#define GNP_BODY_OFFSET    16
// ...

// =========================
// h:d.s
char *hostName;
char *displayNum;
char *screenNum;

int running = FALSE;

int ____saved_server_fd = -1;

// The wm sends us its pid
int ____saved_wm_magic_pid = -1;


int Notify_PongClient=FALSE;
int NoReply = FALSE;

// #test
#define MSG_OFFSET_SHORTSTRING  64
#define SHORTSTRING_SIZE        64
#define MSG_OFFSET_LONGSTRING  128
#define LONGSTRING_SIZE        256
// ...

static int connection_status = 0;
static int IsTimeToQuit = FALSE;
static int IsAcceptingConnections = FALSE;



//
// == Prototypes ============================================
//

// Get system message from the thread's queue.
void xxxHandleNextSystemMessage (void);


int
gwsProcedure (
    int client_fd,
    struct gws_window_d *window,
    int msg,
    unsigned long long1,
    unsigned long long2 );

int initGraphics(void);
void create_background (void);
void create_taskbar (void);

void gwssrv_init_client_support (void);
void init_client_struct ( struct gws_client_d *c );

//
// ==============================================================
//

// Print a simple string in the serial port.
void gwssrv_debug_print (char *string)
{
    //if( (void*) string == NULL ){ return; }
    //if(*string == 0){ return; }

    gramado_system_call ( 289,
        (unsigned long) string,
        (unsigned long) string,
        (unsigned long) string );
}


// Clone and execute a process.
// #todo: We can use the rtl.
int gwssrv_clone_and_execute ( char *name )
{
    
    if ( (void*) name == NULL ){ 
        printf ("gwssrv_clone_and_execute: [FAIL] name\n");
        return -1; 
    }
    
    if (*name == 0){ 
        printf ("gwssrv_clone_and_execute: [FAIL] *name\n");
        return -1; 
    }

    // #deprecated
    // return (int) gramado_system_call ( 900, (unsigned long) name, 0, 0 );
    
    // #todo
    // Use this one.
    return (int) sc82 ( 900, (unsigned long) name, 0, 0 );
}


// #todo
// We can use the rtl.
unsigned long gwssrv_get_system_metrics (int index)
{

    //if (index<0){
        //gde_debug_print ("gswsrv_get_system_metrics: fail\n");
        //return 0;
    //}
    //#define	SYSTEMCALL_GETSYSTEMMETRICS  250 

   return (unsigned long) gramado_system_call ( 250, 
                               (unsigned long) index, 
                               (unsigned long) index, 
                               (unsigned long) index );
}



// enter critical section
// close the gate
void gwssrv_enter_critical_section (void)
{
    int S=0;

    // Pega o valor do spinlock principal.
    // Se deixou de ser 0 então posso entrar.
    // Se ainda for 0, continuo no while.
    // TRUE = OPEN.
    // FALSE = CLOSED.
    // yield thread if closed.

    while (1){
        S = (int) gramado_system_call ( 226, 0, 0, 0 );

        if ( S == 1 ){ goto done; }
        
        //yield thread.
        //gramado_system_call (265,0,0,0); 
        sc82 (265,0,0,0);
    };

    // Close the gate. turn FALSE.
done:
    gramado_system_call ( 227, 0, 0, 0 );
    return;
}


// exit critical section
// open the gate.
void gwssrv_exit_critical_section (void)
{
    gramado_system_call ( 228, 0, 0, 0 );
}




//
// =============================================================
//


//??
// Quando um cliente tenta se desconectar.
/*
// internal
void client_shutdown (int fd);
void client_shutdown (int fd)
{
	//remove client
	//deallocate resources.
}
*/


//?? Die after to handle a request.
// no multiple requests.
/*
void client_die(int fd);
void client_die(int fd)
{
    client_shutdown(fd);
}
*/


// ??
/*
int client_start(int fd);
int client_start(int fd)
{
    return -1;//todo
}
*/



/*
int client_send_error_response(int fd);
int client_send_error_response(int fd)
{
    return -1;//todo
}
*/


// Getting the server pid.
/*
int get_server_pid();
int get_server_pid()
{
    return (int) ... ; //#todo
}
*/

// getting the client id
/*
int get_client_id();
int get_client_id()
{
    return (int) ... ; //#todo
}
*/


// setting the client id
/*
void set_client_id ( int id );
void set_client_id ( int id )
{
	//#todo
    //____client_id = id;
}
*/


/*
int
__WriteToClient ( 
    struct gws_client_d *who, 
    char *buf, 
    int count );
int
__WriteToClient ( 
    struct gws_client_d *who, 
    char *buf, 
    int count )
{
    return -1;
}
*/


/*
int WriteToClient (int fd);
int WriteToClient (int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];   
    int n_writes = 0;    // For responses.
 
    char *m = (char *) (&__buffer[0] + 16);
    sprintf( m, "gwssrv: This is a response from GWS!\n");

    // Primeiros longs do buffer.
    message_buffer[0] = next_response[0];         // Window ID.
    message_buffer[1] = SERVER_PACKET_TYPE_REPLY; // next_response[1] 
    message_buffer[2] = next_response[2];         // Return value (long1)
    message_buffer[3] = next_response[3];         // Return value (long2)

    if (fd<0)
        return 0;

   // #bugbug
   // How many times ?
   
__again:

    // #todo:
    // while(1){...}
    
    gwssrv_debug_print ("gwssrv: Sending response ...\n");

    //
    // Send
    //

    n_writes = write ( fd, __buffer, sizeof(__buffer) );
    //n_writes = send ( fd, __buffer, sizeof(__buffer), 0 );
    
    if (n_writes<=0){
        gwssrv_yield();
        goto __again;
    }


    // Cleaning
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

    // Cleaning
    int c=0;
    for(c=0; c<32; c++)  //todo: 512
        next_response[c] = 0;

    gwssrv_debug_print ("gwssrv: Response sent\n");  
    
    return (int) n_writes;
}
*/


// Worker
// There is a vetor with values for the next response.
// Called by dispatcher().
// IN:
// fd, 1=REPLY | 2=EVENT | 3=ERROR
int __send_response(int fd, int type)
{
// Reusing the same buffer from the request.
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];

    int n_writes = 0;  // For responses.

    int Status=0;

    //gwssrv_debug_print ("Sending response ...\n");  

//# it works.
    char *m = (char *) (&__buffer[0] + 16);
    sprintf( m, "~ Response from gwssrv \n");

    // Primeiros longs do buffer.
    message_buffer[0] = next_response[0];         // Window ID.


// Type of reply

    switch(type){
    case 1:  // normal reply
        message_buffer[1] = SERVER_PACKET_TYPE_REPLY;
        break;
    case 2:  // event
        message_buffer[1] = SERVER_PACKET_TYPE_EVENT;
        break;
    case 3:  // error
        message_buffer[1] = SERVER_PACKET_TYPE_ERROR;
        break;
    default:  // error
        message_buffer[1] = SERVER_PACKET_TYPE_ERROR;
        break;
    };


    message_buffer[2] = next_response[2];         // Return value (long1)
    message_buffer[3] = next_response[3];         // Return value (long2)

// The event it self.
    if(type == 2)
    {
        message_buffer[4] = next_response[4];
        message_buffer[5] = next_response[5];
        message_buffer[6] = next_response[6];
        message_buffer[7] = next_response[7];
    }

//__again:

//
// == Response ============================
//

    gwssrv_debug_print ("dispacher: Sending response ...\n");

    // #todo:
    // while(1){...}

    /*
    // Is current client connected.
    if (currentClient->is_connected == 0)
    {
        // [FAIL] Not connected.
        // close?
    }
    */


    if(fd<0){
        Status = -1;
        goto exit2;
    }

//
// Send
//

    n_writes = write ( fd, __buffer, sizeof(__buffer) );
    //n_writes = send ( fd, __buffer, sizeof(__buffer), 0 );

    // limpa.
    // se a resposta der certo ou se der errado.

    // Cleaning
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    register int b=0;
    for (b=0; b<MSG_BUFFER_SIZE; ++b){
        __buffer[b] = 0;
    };


    // Cleaning
    // 32. #todo 512.
    register int c=0;
    for (c=0; c<NEXTRESPONSE_BUFFER_SIZE; ++c){
        next_response[c] = 0;
    };


    // No. We couldn't send a response.
    // O que acontece se nao conseguirmos enviar uma resposta?
    // Certamente o cliente tentara ler e tera problemas.
    // Deveriamos fechar a conexao?
    // Deveriamos enviar um alerta
    
    if (n_writes<=0){
        gwssrv_debug_print ("__send_response: response fail\n");
        printf             ("__send_response: Couldn't send reply\n");
        //close(fd);
        Status=-1;
        goto exit2;
    }

    // YES, We sent a response.
    if (n_writes>0){
        gwssrv_debug_print ("__send_response: Response sent\n");
        Status=0;
        goto exit0;
    }

    // ??

exit2:
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    message_buffer[4] = 0;
    message_buffer[5] = 0;
exit1:
    gwssrv_yield();
exit0:

// Sync
// set response
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REPLY );

    return (int) Status;
}




/*
 ****************************
 * dispacher:
 *
 */

// internal.
// Messages sent via socket.
// obs: read and write use the buffer '__buffer'
// in the top of this file.

// #todo:
// No loop precisamos de accept() read() e write();
// Get client's request from socket.

void dispacher (int fd)
{
    // Isso permite ler a mensagem na forma de longs.
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];

    int n_reads  = 0;  // For requests.

    int Status=-1;
    int SendErrorResponse=FALSE;

    // #todo:
    // No loop precisamos de accept() read() e write();

    //#atenção:
    // A função accept vai retornar o descritor do 
    // socket que usaremos ... por isso poderemos fecha-lo
    // para assim obtermos um novo da próxima vez.


    gwssrv_debug_print ("dispacher: \n");

    // Fail, cleaning.
    if (fd<0){
        gwssrv_debug_print ("dispacher: fd\n");
        goto exit2;
    }


//__loop:

    //
    // == Request ============================
    //

    // Requests may generate replies, events, and errors;
    // Request packets are numbered sequentially by the server 
    // as soon as it receives them.

    // See:
    // https://en.wikipedia.org/wiki/Round-trip_delay

    // If the request is (XNextEvent), so the reply will be the event.

    /*
    // Is current client connected.
    if (currentClient->is_connected == 0)
    {
        // [FAIL] Not connected.
        // close?
    }
    */

// #important
// We can handle only requests.
// Drop it!

    int value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
    if ( value != ACTION_REQUEST ){
        goto exit2;
    }

// #todo
// Devemos escrever em nosso próprio
// socket e o kernel copia??
// o kernel copia para aquele arquivo ao qual esse estivere conectado.
// olhando em accept[0]
// Precisamos fechar o client e yield se a leitura der errado?

//
// Recv
//

    n_reads = read ( fd, __buffer, sizeof(__buffer) );

    if (n_reads <= 0){
        gwssrv_debug_print ("dispacher: read fail\n");
        goto exit2;
    }

//
// == Processing the request =============================
//

// Invalid request. 
// Clean and yield.

    if (message_buffer[1] == 0 ){
        gwssrv_debug_print ("dispacher: Invalid request\n");
        goto exit2;
    }

// Um cliente solicitou um evento.
// Vamos sinalizar o tipo de resposta que temos que enviar,
// caso nenhum erro aconteça.

    int doSendEvent = FALSE;
    if ( message_buffer[1] == GWS_GetNextEvent )
    {
        doSendEvent = TRUE;  // The response is an EVENT, not a REPLY.
    }


// Process request.
// Do the service.

    debug_print ("dispacher: Process request\n");

// OUT
// <0 : error 

    Status = gwsProcedure (
       (int) fd,
       (struct gws_window_d *) message_buffer[0], 
       (int)                   message_buffer[1], 
       (unsigned long)         message_buffer[2], 
       (unsigned long)         message_buffer[3] );

// Como o serviço não pode ser prestado corretamente.
// Então logo abaixo mandaremos uma resposta de erro
// e não uma resposta normal.

    if( Status < 0){
         SendErrorResponse = TRUE;
    }


//
// == Sending reply =======================================
//

// Alguns requests nao exigem resposta.
// Entao precisamos modificar a flag de sincronizaçao.
// que ainda deve estar sinalizando um request.
    
    if (NoReply == TRUE){
        rtl_set_file_sync( 
            fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        goto exit0;
    }

//
// == reponse ====================================================
//

// IN:
// fd, 1=REPLY | 2=EVENT | 3=ERROR


// ==========================================
// ERROR: (3)
// Se o serviço não pode ser prestado corretamente.
    if( SendErrorResponse == TRUE )
        Status = (int) __send_response(fd,3); //error message

// ==========================================
// EVENT: (2)
// Se o serviço foi prestado corretamente.
// Era uma solicitação de evento

    if (doSendEvent == TRUE)
        Status = (int) __send_response(fd,2);  // event

// ==========================================
// REPLY: (1)
// Se o serviço foi prestado corretamente.
// Era uma solicitação de serviço normal,
// então vamos enviar um reponse normal. Um REPLY.

    if (doSendEvent != TRUE)
        Status = (int) __send_response(fd,1);  // normal reply


// A mensagem foi enviada normalmente,
// Vamos sair normalmente.

    //if(Status >= 0)
        //goto exit0;

// Fall

exit2:
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    message_buffer[4] = 0;
    message_buffer[5] = 0;
exit1:
    //gwssrv_yield();
exit0:
    return;
}



/*
 //#test
void ____get_system_message( unsigned long buffer );
void ____get_system_message( unsigned long buffer )
{
    // Get message.
    gwssrv_enter_critical_section();
    gramado_system_call ( 111,
            (unsigned long) buffer,
            (unsigned long) buffer,
            (unsigned long) buffer );
    gwssrv_exit_critical_section();
}
*/

/*
 ********************************** 
 * xxxGetNextSystemMessage: 
 *
 */

// internal
// System ipc messages. (It's like a signal)
// Get system message from the thread's queue.

void xxxHandleNextSystemMessage (void)
{
    printf("xxxHandleNextSystemMessage: deprecated\n");
}


#define wsMSG_KEYDOWN       20
#define wsMSG_KEYUP         21
#define wsMSG_SYSKEYDOWN    22
#define wsMSG_SYSKEYUP      23

#define wsVK_F1    0x3B  //59    // No DOS é 0x170.  
#define wsVK_F2    0x3C  //60 
#define wsVK_F3    0x3D  //61 
#define wsVK_F4    0x3E  //62 


#define wsVK_RETURN    0x1C
#define wsVK_TAB       0x0F

#define wsCOLOR_BLACK    0x000000
#define wsCOLOR_GRAY     0x808080 



int 
wsInputProcedure ( 
    struct gws_window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    switch (msg)
    {
        // 20 = MSG_KEYDOWN
        case wsMSG_KEYDOWN:
            switch (long1)
            {

                // [Enter] - Finalize the command line and compare.
                case wsVK_RETURN:
                    input('\0');
                    //shellCompare();
                    //asm("int $3");
                    goto done;
                    break; 

                case wsVK_TAB: 
                    printf("\t");
                    goto done; 
                    break;

                // keyboard arrows
                case 0x48: printf ("UP   \n"); goto done; break;
                case 0x4B: printf ("LEFT \n"); goto done; break;
                case 0x4D: printf ("RIGHT\n"); goto done; break;
                case 0x50: printf ("DOWN \n"); goto done; break;


                case 0x47: 
                    printf ("HOME\n");
                    goto done; 
                    break;
                    
                case 0x4F: 
                    printf ("END \n"); 
                    goto done; 
                    break;

                //pageup pagedown
                case 0x49: printf ("PAGEUP   \n"); goto done; break;
                case 0x51: printf ("PAGEDOWN \n"); goto done; break;

                // insert delete
                case 0x52: printf ("INSERT\n"); goto done; break;
                case 0x53: printf ("DELETE\n"); goto done; break;

                default:
                    input ( (unsigned long) long1 );  
                    printf("%c",long1); fflush(stdout);
                    break;
            };
            break;

  // 22 = MSG_SYSKEYDOWN
        case wsMSG_SYSKEYDOWN:
            switch (long1)
            {
                case wsVK_F1: printf ("F1\n");  break;
                case wsVK_F2: printf ("F2\n");  break;
            };
            break;
    };

done:
    return 0;
}



/*
 **********************************
 * gwsProcedure:
 *     Main dialog.
 */

// Called by dispacher.

    // #todo
    // Dependendo do tipo de request, então construiremos
    // a resposta ou prestatemos os serviço.
    // Para cada tipo de request o servidor precisa construir
    // uma resposta diferente.
    // O request afeta os campos da mensagem.
    // Esses campos estão em um buffer, mas poderiam estar
    // em um arquivo json.

    // Types:
    // + Null: fail.
    // + Identify: The server needs to identify itself.
    // + Get all objects:
    // + Set inspected object:
    // + Set property: Probably setting a property of an object.
    // + Disconnect:
    // ...

// OUT
// <0 : error 

int
gwsProcedure ( 
    int client_fd,
    struct gws_window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    int Status=0;  //ok

    int my_pid = -1;

    // #debug
    debug_print ("gwssrv: gwsProcedure\n");
    
    //#debug
    //return 0;

    // See:
    // globals.h

    switch (msg){

    //case GWS_KeyDown:
    //case GWS_KeyUp:
    //case GWS_SysKeyDown:
    //case GWS_SysKeyUp:
        //gwssrv_debug_print ("gwssrv: GWS_KeyDown\n");
        //#todo: Call a helper function to handle keyboard events.
        //break;

    // 20
    // Mensagens de digitação.
    //case GWS_KeyDown:
        //gwssrv_debug_print ("gwssrv: GWS_KeyDown\n");
        //break;

    // 21
    //case GWS_KeyUp:
        //gwssrv_debug_print ("gwssrv: GWS_KeyDown\n");
        //break;

    // 22
    //case GWS_SysKeyDown:
        //gwssrv_debug_print ("gwssrv: GWS_KeyDown\n");
        //break;

    // 23
    //case GWS_SysKeyUp:
        //gwssrv_debug_print ("gwssrv: GWS_SysKeyUp\n");
        //switch ( long1)
        //{
        //    case VK_F1:
        //        gwssrv_debug_print("gwssrv: gwsProcedure VK_F1 Connection ON\n");
        //        connection_status = 1;
        //        break;
        //    case VK_F2:
        //        gwssrv_debug_print("gwssrv: gwsProcedure VK_F2 Connection OFF\n");
        //        connection_status = 0;
        //        break;
        //    case VK_F3:
        //        gwssrv_debug_print ("gwssrv: F3\n");
        //        bmpDisplayBMP (0,0,0); //#todo: test.
        //        break;
        //    case VK_F4:
        //        gwssrv_debug_print ("gwssrv: F4 [EXIT] Exiting the server ...\n");
        //        running = 0;
        //        break;
            // ...
        //    default:
        //        gwssrv_debug_print ("gwssrv: MSG_SYSKEYUP\n");
        //        break;
        //}
        //break;

    // #todo
    // Isso poderia significar que existe uma estrutura
    // de request no mesmo estilo do X presente no
    // final do buffer de mensage, por exemplo
    // no offset 400
    // Mesma coisa aconteceria com as respostas.
    // A biblioteca client side pode conter um wrapper
    // com o protocolo X, colocando a estrutura de request 
    // do final do buffer da mensagem e sinalizar no header do
    // buffer que esse buffer contem uma mensagem que segue o
    // protocolo X.
    // #todo: Criar o arquivo Xproto.h ... xproto.h
    // GWS_X_Request:
         //break;

    // ===========================
    // Here starts the gws requests

    // Hello!
    // Draw text inside a window.
    // #bugbug:
    // O window server não tem esse ponteiro de janela.
    // ele até aceitaria um handle.
    // #bugbug: Something is very wrong with this routine.

    case GWS_Hello:
        gwssrv_debug_print ("gwssrv: Message number 1000\n");
        //#bugbug: Esse endereço de estrutura esta mudando para um valor
        //que nao podemos acessar em ring3.
        if ( (void*) gui->screen_window != NULL)
        {
            //if ( gui->screen->used == 1 && gui->screen->magic == 1234 ){
                dtextDrawText ( 
                    (struct gws_window_d *) gui->screen_window,
                    long1, long2, COLOR_GREEN,
                    "gwssrv: Hello friend. This is the Gramado Window Server!");
            //}
        } 
        gws_show_backbuffer();
        NoReply = FALSE;
        break;

    // Create Window
    // Usará o buffer global
    // See: wm.c
    case GWS_CreateWindow:
        gwssrv_debug_print ("gwssrv: [1001] serviceCreateWindow\n");
        serviceCreateWindow(client_fd);
        NoReply = FALSE;   // #bugbug: Why not? We need to return the window id.
        break; 

    // backbuffer putpixel
    case GWS_BackbufferPutPixel:
        servicepixelBackBufferPutpixel(); 
        NoReply = FALSE;
        break;

    // backbuffer draw horizontal line
    case GWS_DrawHorizontalLine:
        servicelineBackbufferDrawHorizontalLine();
        NoReply = FALSE;
        break;

    // Draw char
    // See: char.c
    case GWS_DrawChar:
        gwssrv_debug_print ("gwssrv: [1004] serviceDrawChar\n");
        serviceDrawChar();
        NoReply = FALSE;
        break;

    // Draw text
    // See: dtext.c
    case GWS_DrawText:
        gwssrv_debug_print ("gwssrv: [1005] serviceDrawText\n");
        serviceDrawText();
        NoReply = FALSE;
        break;

    // Refresh window
    case GWS_RefreshWindow:
        gwssrv_debug_print ("gwssrv: [1006] serviceRefreshWindow\n");
        serviceRefreshWindow();
        NoReply = FALSE;
        break;

     // Redraw window
     case GWS_RedrawWindow:
         gwssrv_debug_print ("gwssrv: [1007] serviceRedrawWindow\n");
         serviceRedrawWindow();
         NoReply = FALSE;
         break;

    // Resize window
    case GWS_ResizeWindow:
        gwssrv_debug_print ("gwssrv: [1008] serviceResizeWindow\n");
        serviceResizeWindow();
        NoReply = FALSE;
        break;

    case GWS_ChangeWindowPosition:
        gwssrv_debug_print ("gwssrv: [1009] serviceChangeWindowPosition\n");
        serviceChangeWindowPosition();
        NoReply = FALSE;
        break;

    // ...

    // backbuffer putpixel. (again)
    // IN: Color, x, y
    case GWS_BackbufferPutPixel2:
        grBackBufferPutpixel ( 
            (unsigned long) COLOR_PINK, 
            (unsigned long) long1, 
            (unsigned long) long2 );
        NoReply = FALSE;
        break;

    // ...


    // Disconnect.
    // shutdown.
    // Um cliente quer se desconectar.
    case GWS_Disconnect:
        gwssrv_debug_print ("gwssrv: [2010] Disconnect\n");
        // DisconnectCurrentClient = TRUE;  // #todo
        // NoReply = TRUE;  // #todo
        break;


    // Refresh screen 
    // refresh screen using kgws service. 
    case GWS_RefreshScreen:
        gwssrv_debug_print ("gwssrv: [2020] gws_show_backbuffer\n");
        gws_show_backbuffer();
        //NoReply = FALSE;      // #todo
        break;

    // Refresh rectangle ... 
    case GWS_RefreshRectangle:
        gwssrv_debug_print ("gwssrv: [2021] serviceRefreshRectangle\n");
        serviceRefreshRectangle();
        //NoReply = FALSE;  // #todo
        break;

    // ?? #bugbug: The client only sends requests.
    // GWS_GetSendEvent
    case 2030:
        gwssrv_debug_print ("gwssrv: [2030] serviceClientEvent\n");
        //serviceClientEvent();
        //NoReply = FALSE;  // #todo
        break;

// The server will return an event from the its client's event queue.
    case GWS_GetNextEvent:
        gwssrv_debug_print ("gwssrv: [2031] serviceNextEvent\n");
        serviceNextEvent(client_fd);
        NoReply = FALSE;
        break;

    // See: grprim.c
    case GWS_GrPlot0:  
        gwssrv_debug_print ("gwssrv: [2040] serviceGrPlot0\n");
        serviceGrPlot0();  
        NoReply = FALSE;
        break;

    // See: grprim.c
    case GWS_GrCubeZ:  
        gwssrv_debug_print ("gwssrv: [2041] serviceGrCubeZ\n");
        serviceGrCubeZ();  
        NoReply = FALSE;
        break;

    // See: grprim.c
    case GWS_GrRectangle:  
        gwssrv_debug_print ("gwssrv: [2042] serviceGrRectangle\n");
        serviceGrRectangle();  
        NoReply = FALSE;
        break;

    // #todo
    // Segue serviços graficos 3d.
    // ...

    // #test
    // async command: 
    case GWS_AsyncCommand:
        gwssrv_debug_print ("gwssrv: [2222] calling serviceAsyncCommand\n");
                  //printf ("gwssrv: [2222] calling serviceAsyncCommand\n");
        serviceAsyncCommand();
        NoReply = TRUE;
        break;


    case GWS_PutClientMessage:
        gwssrv_debug_print ("gwssrv: [GWS_PutClientMessage]\n");
        servicePutClientMessage();
        NoReply = TRUE;
        break;

    case GWS_GetClientMessage:
        gwssrv_debug_print ("gwssrv: [GWS_GetClientMessage]\n");
        serviceGetClientMessage();
        NoReply = FALSE;
        break;


// Let's get one event from the client's event queue.
// Send it as a response.
    case GWS_DrainInput:
        gwssrv_debug_print("gwssrv: gwsProcedure 8080\n");
        break;


    // ...

    default:
        gwssrv_debug_print ("gwssrv: Default message number\n");
        //printf ("msg=%d ",msg);
        // NoReply = TRUE; //#todo
        Status = -1;  // Not ok.
        break;
    }

//#todo ???
// NoReply = TRUE; 

//done:
    return (int) Status;
}


//

void create_taskbar (void)
{
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();
    int WindowId = -1;


    gwssrv_debug_print ("gwssrv: create_taskbar\n");


    if ( w==0 || h==0 )
    {
        gwssrv_debug_print ("create_taskbar: w h\n");
        printf             ("create_taskbar: w h\n");
        exit(1);
    }



    __taskbar_window = (struct gws_window_d *) CreateWindow ( 
                                            WT_SIMPLE, 
                                            0, //style
                                            1, //status 
                                            1, //view
                                            "TaskBar",  
                                            0, h-40, w, 40,   
                                            gui->screen_window, 0, 
                                            COLOR_RED, COLOR_RED );    


    // #debug
    // asm ("int $3");

    if ( (void *) __taskbar_window == NULL )
    {
        gwssrv_debug_print ("create_taskbar: __taskbar_window\n"); 
        printf             ("create_taskbar: __taskbar_window\n");
        exit(1);
        //return;
    }

    /*
    // #debug
    if ( __root_window->used != TRUE || __root_window->magic != 1234 )
    {
        gwssrv_debug_print ("create_background: __root_window validation\n"); 
        printf             ("create_background: __root_window validation\n");
        exit(1);
        //return;
    }
    */

// Register
    WindowId = RegisterWindow(__taskbar_window);
    if (WindowId<0)
    {
        gwssrv_debug_print ("create_taskbar: Couldn't register window\n");
        printf             ("create_taskbar: Couldn't register window\n");
        exit(1);
    }


    //#debug
    //asm ("int $3");

    //__root_window->dirty = 1;


    // See: 
    // gws.c
/*
    if (current_mode == GRAMADO_JAIL)
    {
        gwssrv_debug_print ("gwssrv: create_taskbar: Calling refresh_screen\n");
        //refresh_screen();
    }
*/

    gwssrv_debug_print ("gwssrv: create_taskbar: done\n");

    //#debug
    //while(1){}
}



void create_background (void)
{
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();
    int WindowId = -1;


    gwssrv_debug_print ("gwssrv: create_background\n");


    if ( w==0 || h==0 )
    {
        gwssrv_debug_print ("create_background: w h\n");
        printf             ("create_background: w h\n");
        exit(1);
    }


// The background window
// #todo
// Se estivermos em JAIL, podemos arriscar algum background melhor.
// Talvez alguma imagem.

    __root_window = (struct gws_window_d *) CreateWindow ( 
                                            WT_SIMPLE, 
                                            0, //style
                                            1, //status 
                                            1, //view
                                            "gwssrv-bg",  
                                            0, 0, w, h,   
                                            gui->screen_window, 0, 
                                            COLOR_BACKGROUND, COLOR_BACKGROUND );    


    // #debug
    // asm ("int $3");

    if ( (void *) __root_window == NULL )
    {
        gwssrv_debug_print ("create_background: __root_window\n"); 
        printf             ("create_background: __root_window\n");
        exit(1);
        //return;
    }

    /*
    // #debug
    if ( __root_window->used != TRUE || __root_window->magic != 1234 )
    {
        gwssrv_debug_print ("create_background: __root_window validation\n"); 
        printf             ("create_background: __root_window validation\n");
        exit(1);
        //return;
    }
    */

// Register
    WindowId = RegisterWindow(__root_window);
    if (WindowId<0)
    {
        gwssrv_debug_print ("create_background: Couldn't register window\n");
        printf             ("create_background: Couldn't register window\n");
        exit(1);
    }


    //#debug
    //asm ("int $3");

    //__root_window->dirty = 1;


    // See: 
    // gws.c

    if (current_mode == GRAMADO_JAIL)
    {
        gwssrv_debug_print ("gwssrv: create_background: Calling refresh_screen\n");
        //refresh_screen();
    }

    gwssrv_debug_print ("gwssrv: create_background: done\n");

    //#debug
    //while(1){}
}



/*
 ****************************** 
 * initGraphics:
 *     Initialize the graphics support.
 */

// Initialize the window server infrastructure.
// The current display and the current screen.
// Initialize the 3d support.

int initGraphics (void){

    int __init_status = -1;


    debug_print("initGraphics\n");


    //printf("initGraphics: \n");

    // Initialize the window server infrastructure.
    // The current display and the current screen.
    // See: gws.c

//
// It will create the root window.
//

    //printf("initGraphics: [1] gwsInit() \n");

    __init_status = gwsInit();

    if (__init_status != 0)
    {
        debug_print ("initGraphics: [PANIC] Couldn't initialize the graphics\n");
        printf      ("initGraphics: [PANIC] Couldn't initialize the graphics\n");
        exit(1);
    }

    if ( (void*) gui == NULL ){
        debug_print ("initGraphics: gui\n");
        printf      ("initGraphics: gui\n");
        exit(1);
    }

    //while(1){}

    // #debug
    // Se o background a seguir falhar, entao veremos
    // pelo menos essa mensagem.
    
    // #bugbug #todo: 
    // Isso só mostrou em GRAMADO_JAIL
  
    if ( (void*) gui->screen_window != NULL )
    {
        dtextDrawText ( 
            (struct gws_window_d *) gui->screen_window,
            8, 8, COLOR_RED, "Initializing graphics" );
    
        // invalidate the surface in ring0.
        invalidate_surface_retangle();

        //gws_show_backbuffer();
    }

// #debug

    //while(1){}


    // Create background.

    //printf("initGraphics: [2] create_background() \n");

    // #bugbug
    // This is creating the root window again.

// cria a root window
    create_background();

// cria a taskbar
// ela vai nos mostrar qual janela eh a ativa ... e talvez o foco ... etc
    create_taskbar();

    wm_Update_TaskBar("Welcome!");

//#debug
    //gws_show_backbuffer();
    //while(1){}


//
// BMP
//

    // Suspended for now!

    
    // Testing bmp.
    // See:
    if (current_mode == GRAMADO_JAIL){
        gwssrv_display_system_icon ( 1, 8, 100 );
        //gwssrv_display_system_icon ( 2, 8, 120 );
        //gwssrv_display_system_icon ( 3, 8, 140 );
        //gwssrv_display_system_icon ( 4, 8, 160 );
        // ...
    }else{
        gwssrv_display_system_icon ( 1, 100, 100 );
        //gwssrv_display_system_icon ( 2, 200, 200 );
        //gwssrv_display_system_icon ( 3, 300, 300 );
        //gwssrv_display_system_icon ( 4, 400, 400 );
    };


    //#debug breakpoint
    //while(1){}
    
    
    
    //
    // == checks ===============================================
    //


    // Check if we already have the root window.

    if ( (void*) __root_window == NULL )
    {
        gwssrv_debug_print ("initGraphics: [FAIL] root window doesn't exist\n");
        printf             ("initGraphics: [FAIL] root window doesn't exist\n");
        exit(1);
    }


//#debug
    //gws_show_backbuffer();
    //while(1){}


    // Initialize the graphics support.
    // Now we can use 3d routines.
    // See: grprim.c
    
    gwssrv_debug_print ("initGraphics: Calling grInit() \n");
    //printf ("initGraphics: Calling grInit() \n");
    
    grInit();


//#debug
    //gws_show_backbuffer();
    //while(1){}


    // #debug
    //gwssrv_debug_print ("initGraphics: :)\n");
    //printf ("initGraphics: :)\n");
    //asm("int $3");
    //while(1){}


//
// == demos ==================================
//

// Always run some demo if we are in JAIL mode.
// It's an animation in the initialization.
// Seleciona a animaçao.
// Nao deve travar, deve ter timeout.


    if (current_mode == GRAMADO_JAIL)
    {
         //demos_startup_animation(1);   //ok
         //demos_startup_animation(2);   //it works
         //demos_startup_animation(3);   //it works
         //demos_startup_animation(4);   //it works.
         //demos_startup_animation(5);   //it works.
         //demos_startup_animation(6);   //ok
         //demos_startup_animation(7);   //ok
         //demos_startup_animation(8);   //ok
         //demos_startup_animation(9);     //ok
         //gwssrv_show_backbuffer();
         //while(1){}
         // ...
    }


//#debug
    //gws_show_backbuffer();
    //while(1){}



    // #debug
    //asm("int $3");

    //
    // == tests =================================================
    //
 
    /*
     //testes com z positivo
    grPlot0(0, 4, 4,COLOR_RED);     // q1
    grPlot0(3, 4, 4,COLOR_RED);     // q1 afasta para q1
    grPlot0(90, 4, 4,COLOR_RED);     // q1 afasta para q1
    
    grPlot0(0, 4,-4,COLOR_GREEN);   // q2
    grPlot0(3, 4,-4,COLOR_GREEN);   // q2  afasta  para q1   
    grPlot0(90, 4,-4,COLOR_GREEN);   // q2  afasta  para q1    
    
    grPlot0(0,-4,-4,COLOR_BLUE);    // q3
    grPlot0(3,-4,-4,COLOR_BLUE);    // q3 afasta  para q1
    grPlot0(90,-4,-4,COLOR_BLUE);    // q3 afasta  para q1
    
    grPlot0(0,-4, 4,COLOR_YELLOW);  // q4 
    grPlot0(3,-4, 4,COLOR_YELLOW);  // q4 afasta  para q1
    grPlot0(90,-4, 4,COLOR_YELLOW);  // q4 afasta  para q1
    */


     /*
     //testes com z negativo
    grPlot0 ( 0, 4, 4,COLOR_RED);     // q1
    grPlot0 (-3, 4, 4,COLOR_RED);     // q1 afasta para q3
    grPlot0 (-9, 4, 4,COLOR_RED);     // q1 afasta para q3
    grPlot0 (-11, 4, 4,COLOR_RED);     // q1 afasta para q3
    

    grPlot0( 0, 4,-4,COLOR_GREEN);   // q2
    grPlot0(-3, 4,-4,COLOR_GREEN);   // q2  afasta  para q3   
    grPlot0(-9, 4,-4,COLOR_GREEN);   // q2  afasta  para q3    
    grPlot0(-11, 4,-4,COLOR_GREEN);   // q2  afasta  para q3    
    
    grPlot0( 0,-4,-4,COLOR_BLUE);    // q3
    grPlot0(-3,-4,-4,COLOR_BLUE);    // q3 afasta  para q3
    grPlot0(-9,-4,-4,COLOR_BLUE);    // q3 afasta  para q3
    grPlot0(-11,-4,-4,COLOR_BLUE);    // q3 afasta  para q3
    
    grPlot0( 0, -4, 4,COLOR_YELLOW);  // q4 
    grPlot0(-3, -4, 4,COLOR_YELLOW);  // q4 afasta  para q3
    grPlot0(-9, -4, 4,COLOR_YELLOW);  // q4 afasta  para q3
    grPlot0(-11, -4, 4,COLOR_YELLOW);  // q4 afasta  para q3
    */    

    /*
    int y=0;
    int x=0;
    int z=0;
   
    z=0;
    for( y=0; y<32; y++)
    {
        for( x=0; x<32; x++)
            grPlot0 ( z, x, y,COLOR_RED);
    
        z++;
    }

    z=0;
    for( y=0; y<32; y++)
    {
        for( x=0; x<32; x++)
            grPlot0 ( z, -x, -y,COLOR_BLUE);
    
        z--;
    }
    */


    //int i=0;
   
    /*
    for (i=0; i<4; i++)
    {    
       //ok
       plotCircle ( i*8, i*8, 100, COLOR_GREEN); 
    };
    */

    /*
    for (i=0; i<4; i++)
    {
       plotEllipseRect (
            i*8, i*8, 
            30, 220, 
            COLOR_BLUE);
    };
    */

    /*
    int i=0;
    for (i=0; i<5; i++)
    { 
       plotLine3d ( 
           2, 2, 0,     // vertex 1
           20, i*20, 0,
           COLOR_RED );
    };
    */

     // Lines.

     /*
     grPlot0 ( 0, 0, 40, COLOR_WHITE ); //MARCADOR
     plotLine3d ( 
         0,    0,  0,
         0,   40,  0,
         COLOR_RED );

     grPlot0 ( 0, 40, 0, COLOR_WHITE ); //MARCADOR
     plotLine3d ( 
         0,    0,  0,
         40,   0,  0,
         COLOR_GREEN );


     grPlot0 ( 0, -40, -40, COLOR_WHITE ); //MARCADOR
     plotLine3d ( 
         0,    0,  0,
         -40,  -40,  0,
         COLOR_BLUE );
     */

    //rectangle(
    //    8,8,
    //    80,80, COLOR_YELLOW );

 
    /*
    cube ( 
        15,25,25,35,    // back
        20,10,30,30,   // front
        COLOR_WHITE );
    */


    /*
    int i=0;
    for (i=0; i<5; i++){
    cubeZ ( 
        15,25,25,35,    // back
        20,10,30,30,   // front
        COLOR_WHITE, i*40 );
    };
    */
   
   
    //Terry's window
    //demoTerry();
    
    
    //noraDrawingStuff();
   
   
    //noraDrawingStuff3(-1);
    //noraDrawingStuff3(0);
    //noraDrawingStuff3(1);
 
 
    /*
    int i=0;
    for (i=0; i<100; i++){
            //noraDrawingStuff3 (i,i,0);
            noraDrawingStuff3 (-i,-i,0);
            //rectangle(8,8,i,i,COLOR_BLUE);
            //rectangleZ(i,i,i+20,i+20,COLOR_BLUE,i);
            //plotCircle ( -i % 20, -i % 20, i%20, COLOR_GREEN);
            //plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);  //save this
            //plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);   //save this
            //cool
            plotCircleZ ( -i % fib(20), -i % fib(20), i % fib(20), COLOR_GREEN, i % fib(20) );
            //igual o de cima.
            //plotCircleZ ( -i % fib(20), -i % fib(20), i, COLOR_GREEN, i % fib(20) );
            //plotCircleZ ( -i % fib(7), -i % fib(7), i % fib(7), COLOR_GREEN, i % fib(7) );
            //plotEllipseRect ( i, i, i*5, i*5, COLOR_BLUE);
            //plotEllipseRectZ ( i%20, i%20, i, i, COLOR_BLUE,i%20);
    };
    */

 
    /*
    plotCharBackbufferDrawcharTransparentZ (  (8*0), 0, COLOR_RED, 'G', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*1), 0, COLOR_RED, 'R', 0 );   
    plotCharBackbufferDrawcharTransparentZ (  (8*2), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*3), 0, COLOR_RED, 'M', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*4), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*5), 0, COLOR_RED, 'D', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*6), 0, COLOR_RED, 'O', 0 );

    //plotCharBackbufferDrawcharTransparentZ (  0, (8*0),  COLOR_RED, 'G', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, (8*1),  COLOR_RED, 'R', 0 );   
    plotCharBackbufferDrawcharTransparentZ (  0, (8*2),  COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, (8*3),  COLOR_RED, 'M', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, (8*4),  COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, (8*5),  COLOR_RED, 'D', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, (8*6),  COLOR_RED, 'O', 0 );

    //plotCharBackbufferDrawcharTransparentZ (  -(8*0), 0, COLOR_RED, 'G', 0 );
    plotCharBackbufferDrawcharTransparentZ (  -(8*1), 0, COLOR_RED, 'R', 0 );   
    plotCharBackbufferDrawcharTransparentZ (  -(8*2), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  -(8*3), 0, COLOR_RED, 'M', 0 );
    plotCharBackbufferDrawcharTransparentZ (  -(8*4), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  -(8*5), 0, COLOR_RED, 'D', 0 );
    plotCharBackbufferDrawcharTransparentZ (  -(8*6), 0, COLOR_RED, 'O', 0 );

    //plotCharBackbufferDrawcharTransparentZ (  0, -(8*0),  COLOR_RED, 'G', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*1),  COLOR_RED, 'R', 0 );   
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*2),  COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*3),  COLOR_RED, 'M', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*4),  COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*5),  COLOR_RED, 'D', 0 );
    plotCharBackbufferDrawcharTransparentZ (  0, -(8*6),  COLOR_RED, 'O', 0 );
    */

    //matrix_demo1();

    
    //plotLineRect4(8,8,80,80, COLOR_BLACK);  //ok. it works.
    //plotLineRectZ ( 8,8,0, 80,80,0, COLOR_BLACK);    //ok. it works.
    //plotLineRectZ ( 8,8,100, 80,80,100, COLOR_WHITE);    
   
    //testLines(8,8,80,80, COLOR_BLACK); //ok triangulo invertido.
    //testLinesZ ( 8,8,0, 80,80,0, COLOR_BLACK); //ok triangulo invertido.    
    //testLinesZ ( 8,8,100, 80,80,100, COLOR_WHITE);    //ok triangulo invertido.



    //plotCharBackbufferDrawcharTransparent ( 80, 80, COLOR_RED, 'X' );
    
    /*
    plotCharBackbufferDrawcharTransparentZ (  10,  10, COLOR_RED, 'G', -10 );
    plotCharBackbufferDrawcharTransparentZ (   0,   0, COLOR_RED, 'R', -10 );
    plotCharBackbufferDrawcharTransparentZ ( -10, -10, COLOR_RED, 'A', -10 );
    plotCharBackbufferDrawcharTransparentZ ( -20, -20, COLOR_RED, 'M', -10 );
    plotCharBackbufferDrawcharTransparentZ ( -30, -30, COLOR_RED, 'A', -10 );
    plotCharBackbufferDrawcharTransparentZ ( -40, -40, COLOR_RED, 'D', -10 );
    plotCharBackbufferDrawcharTransparentZ ( -50, -50, COLOR_RED, '0', -10 );
    */
       
    //See: window.c
    //create_main_menu();
  
    //gws services
    //gws_refresh_rectangle(0,0,400,400);
    //gws_show_window_rect(gui->screen);
    
    
    //noraDrawingStuff3 (0,0,0);
   
    /* 
    cubeZ ( 
        0,     0,    100,     10,   // back
        0+40, 0-20, 100+40, 10-20,   // front
        COLOR_WHITE, 0 );
    
    cubeZ ( 
        0,     0,    100,     10,   // back
        0+40, 0-20, 100+40, 10-20,   // front
        COLOR_WHITE, 100 );
    */

    //ras_rectangleZ (
        //0+40, 0-20, 100+40, 10-20,
        //COLOR_WHITE, 0 ); 

    // inflate or deflate several times.
    // only jail.
    
    //cube_demo1();
    //cube_demo2();
    
    /* 
    // line
    plotQuadBezierSeg ( 
        0,  0, 0,    //x0, y0, z0, 
        40, 40, 0,   //x1, y1, z1,
       100, 20, 0,  //x2, y2, z2, 
       COLOR_WHITE );
   */
   
    
    //curveDemo();
    
    
    // fred demo ======
    
    //fredDemo0();
    //fredDemo1();
    //fredDemo2(); // $bootcastle
    //...


    /*
     //ok isso funcionou.
    struct gr_rectangle_d *rect;
    rect = (void *) malloc( sizeof( struct gr_rectangle_d ) );
    if ( (void*) rect != NULL )
    {

        rect->p[0].x = 0;
        rect->p[0].y = 20;
        rect->p[0].z = 0;
        rect->p[0].color = COLOR_PINK;
        
        rect->p[1].x = 20;
        rect->p[1].y = 20;
        rect->p[1].z = 0;
        rect->p[1].color = COLOR_PINK;
        
        rect->p[2].x = 20;
        rect->p[2].y = 0;
        rect->p[2].z = 0;
        rect->p[2].color = COLOR_PINK;
        
        rect->p[3].x = 0;
        rect->p[3].y = 0;
        rect->p[3].z = 0;
        rect->p[3].color = COLOR_PINK;
        
        // plot rectangle 
        rectangleZZ ( (struct gr_rectangle_d *) rect );
    }
    */
    
    /*
     //ok, it works
    plotCharBackbufferDrawcharTransparentZ (  (8*0), 0, COLOR_RED, 'G', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*1), 0, COLOR_RED, 'R', 0 );   
    plotCharBackbufferDrawcharTransparentZ (  (8*2), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*3), 0, COLOR_RED, 'M', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*4), 0, COLOR_RED, 'A', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*5), 0, COLOR_RED, 'D', 0 );
    plotCharBackbufferDrawcharTransparentZ (  (8*6), 0, COLOR_RED, 'O', 0 );
    */
    

    //
    // Refresh
    //

    // #debug
    //asm("int $3");

// #bugbug
// Fail!!!
// See: gws.c

    //gws_show_backbuffer();

// invalidate the surface in ring0.
    invalidate_surface_retangle();

    // #debug
    //asm("int $3");

    // while(1){}



    // Change
    // Se encontrar o 0x80 muda para 0x50 caso contrario mude para 0x00
    //grDCMono ( CurrentDisplay, 0x80, 0x50, 0x00 );
 
    // Se encontrar 0x80 mude para 0x50.
    //grDCColorChg ( CurrentDisplay, 0x80, 0x50 );
  
    // filtro
    // atenuando
    //grDCColorChg ( CurrentDisplay, 0x80, 0x80 - 0x30 );
    //grDCColorChg ( CurrentDisplay, 0xC0, 0x80 - 0x30 );
    //grDCColorChg ( CurrentDisplay, 0xFF, 0x80 - 0x30 );
    //grDCColorChg ( CurrentDisplay, 0x00, 0x80 - 0x30 );
    //while(1){}

    debug_print("gwssrv: InitGraphics done\n");
    
    //printf     ("gwssrv: InitGraphics done *hang\n");
    //while(1){}
 
    // ok
    return 0;
}


// Initialize the client list.
// This is an array of connections.
// See: clients.h

void gwssrv_init_client_support (void)
{
    int i=0;


    gwssrv_debug_print ("gwssrv_init_client_support:\n");

    for (i=0; i<CLIENT_COUNT_MAX; i++)
    {
        connections[i] = 0;
    };
 
 
//
// The current client
//

// #todo
// We need the information about the current client
// And we need a list of the connected clientes.

    currentClient = (struct gws_client_d *) 0;


//
// The server client
//

    serverClient = (struct gws_client_d *) malloc ( sizeof( struct gws_client_d ) );
    if ( (void *) serverClient == NULL ){
        gwssrv_debug_print ("gwssrv_init_client_support: [FATAL] Couldn't create serverClient\n");
        printf             ("gwssrv_init_client_support: [FATAL] Couldn't create serverClient\n");
        exit(1);
    }
    
    
    serverClient->id = 0;
    serverClient->is_connected = FALSE;
        
    serverClient->fd = -1;
        
    serverClient->pid = getpid();
    serverClient->gid = getgid();
    // ...

    connections[SERVER_CLIENT_INDEX] = (unsigned long) serverClient;

//done:
    serverClient->used  = TRUE;
    serverClient->magic = 1234;
}



void init_client_struct ( struct gws_client_d *c )
{
    register int i=0;


    if ( (void*) c == NULL ){
        gwssrv_debug_print("init_client_struct: [FAIL] c\n");
        return;
    }

// ID
// #todo
    c->id = -1;  //fail

    c->is_connected = FALSE;
    c->fd  = -1;
    c->pid = -1;
    c->gid = -1;

//done:
    c->used  = TRUE;
    c->magic = 1234;
}

/*
 //Send the message in the buffer to all the clients.
 //This is a great opportunity to shutdown the clients
 //if it is not connected.

void gwssrv_message_all_clients(void);
void gwssrv_message_all_clients(void)
{
}
*/


// When a client sent us an event
// ??? mas o cliente envia as coisas via request, ???
int serviceClientEvent(void)
{
    //O buffer é uma global nesse documento.
    //unsigned long *message_address = (unsigned long *) &__buffer[0];


    return -1;
}


// When a client get the next event from it's own queue.
// The parameters 
// :::: Retira em head.
int serviceNextEvent (int client_fd)
{
    struct gws_window_d *window;


    debug_print("serviceNextEvent: \n");

// Invalid client socket.
    if ( client_fd < 0 ){
        debug_print("serviceNextEvent: client_fd\n");
        return -1;
    }

// Window with focus
    int wid = window_with_focus;


// invalid window
// #todo: Put the focus on root?

    if (wid<0){
        debug_print("serviceNextEvent: wid<0\n");
        return -1;
    }

// Invalid window
// #todo: Put the focus on root?

    if (wid >= WINDOW_COUNT_MAX){
        debug_print("serviceNextEvent: wid>=WINDOW_COUNT_MAX\n");
        return -1;
    }


// get window pointer

    window = (struct gws_window_d *) windowList[wid];

// Invalid window pointer
    if( (void*) window == NULL ){
        debug_print("serviceNextEvent: window\n");
        return -1;
    }

// Invalid window
    if( window->used != TRUE ){
        debug_print("serviceNextEvent: used\n");
        return -1;
    }

// Invalid window
    if( window->magic != 1234 ){
        debug_print("serviceNextEvent: magic\n");
        return -1;
    }


// O cliente autorizado a ler a janela com foco de entrada.
    int owner_client = (int) window->client_fd;

// Invalid owner client fd.
// #todo: Checar o limite superior.
    if (owner_client<0){
        debug_print("serviceNextEvent: owner_client<0\n");
        return -1;
    }


// check client.
// The caller and the owner needs to be the same
// or the caller will not be able to read the input.

    if(client_fd != owner_client){
        debug_print("serviceNextEvent: client_fd != owner_client\n");
        return -1;
    }

//
// Ok
// 

// Compose the response.
// The response will be the next valid event found in the list.

// Setup offset for the next event index.
    window->head_pos++;

// fim da lista.
// circulando.
    if (window->head_pos < 0 || window->head_pos >= 31)
    {
        window->head_pos = 0;
    }

// Estamos tentando consumir 
// mais do que foi colocado.
// Não vamos ultrapassar o offset de escrita.

    if ( window->head_pos > window->tail_pos )
    {
        debug_print("serviceNextEvent: [FAIL] window->head_pos > window->tail_pos\n");
        return -1;
    }

//
// The message buffer
//

// The header
    next_response[0] = window->id;                // The window with focus.
    next_response[1] = (SERVER_PACKET_TYPE_EVENT & 0xFFFF);  // msg code (This is a REPLY message).
    next_response[2] = 1234;  // Signature
    next_response[3] = 5678;  // Signature

// The event

    int offset = (int) window->head_pos;
    
    offset = (int) (offset & 0xFF);

// Get from queue.
    unsigned long data4 = (unsigned long) window->window_list[offset];
    unsigned long data5 = (unsigned long) window->msg_list[offset];
    unsigned long data6 = (unsigned long) window->long1_list[offset];
    unsigned long data7 = (unsigned long) window->long2_list[offset];
    unsigned long data8 = (unsigned long) window->long3_list[offset];
    unsigned long data9 = (unsigned long) window->long4_list[offset];

// Clear the slot where we got the info.
// We don't wanna get this again
    window->window_list[offset] = 0;
    window->msg_list[offset] = 0;
    window->long1_list[offset] = 0;
    window->long2_list[offset] = 0;
    window->long3_list[offset] = 0;
    window->long4_list[offset] = 0;

// 'int' type.
    data4 = (unsigned long) (data4 & 0xFFFF);  // wid
    data5 = (unsigned long) (data5 & 0xFFFF);  // message code

    next_response[4] = (unsigned long) data4;  //window id
    next_response[5] = (unsigned long) data5;  //message code
    next_response[6] = (unsigned long) data6;  //long1
    next_response[7] = (unsigned long) data7;  //long2

// extra: not implemented yet
    next_response[8] = (unsigned long) data8;  //long3
    next_response[9] = (unsigned long) data9;  //long4


    if(data5 <= 0)
        debug_print("serviceNextEvent: Invalid message\n");

    if(data5 > 0)
        debug_print("serviceNextEvent: Valid message :)\n");


// ok
    debug_print("serviceNextEvent: done\n");
    return 0;
}




// #todo
// Close all the clients and close the server.
void serviceExitGWS(void)
{
    printf ("serviceExitGWS: \n");
    
    // Kill all the clients.
    printf ("[TODO] Kill all the clients\n");

    // #todo
    // Deallocate resources used by the server.

    // Close the server.
    printf ("[TODO] Close all the clients\n");
    exit(0);
}

// #todo
// Now we put messages only in the window structure's message queue.
int servicePutClientMessage(void)
{
    debug_print("servicePutClientMessage: deprecated\n");
    return 0;
}


// #todo
// Now we get messages only in the window structure's message queue.
int serviceGetClientMessage(void)
{
    debug_print("serviceGetClientMessage: deprecated\n");
    return 0;
}




// No response.
int serviceAsyncCommand (void)
{
    //O buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];



    // int window_id=0;
    unsigned long message_id =0;
    unsigned long request_id =0;
    unsigned long subrequest_id = 0;
    unsigned long Data = 0;

    //
    // parameters
    //

    // window_id   = message_address[0];
    message_id     = message_address[1];   // message
    request_id     = message_address[2]; 
    subrequest_id  = message_address[3];
    Data           = message_address[4];


    // ...


    // Validate our message number.

    if (message_id != 2222)
    {
        gwssrv_debug_print ("serviceAsyncCommand: [ERROR] message id\n");
                    printf ("serviceAsyncCommand: [ERROR] message id\n");
        return -1;
    }


    // #debug
    // printf ("serviceAsyncCommand: [request %d] \n", request_id);
 
    switch (request_id){

        // 1 =  Exit GWS
        // #todo:
        // Close all the clients and close the server.
        case 1:
            gwssrv_debug_print ("serviceAsyncCommand: [request 1]  Exit GWS\n");
                        //printf ("serviceAsyncCommand: [request 1] Closing server\n");
            printf("serviceAsyncCommand: Exit GWS\n");
            serviceExitGWS();
            printf("serviceAsyncCommand: [FAIL] fail when closing the GWS\n");
            exit(0);
            break;

        case 2:
            gwssrv_debug_print ("serviceAsyncCommand: [request 2] \n");
            printf("PING\n");
            //Notify_CloseClient = TRUE;
            //Notify_PongClient = TRUE;
            //exit(0);
            return 0;
            break;

        case 3:
            gwssrv_debug_print ("serviceAsyncCommand: [request 3] hello\n");
            printf("HELLO\n");
            //exit(0);
            return 0;
            break;
        
        // See: demos.c
        case 4:
            if (current_mode == GRAMADO_JAIL)
            {
                gwssrv_debug_print("serviceAsyncCommand: [request 4] demo\n"); 
                demos_startup_animation(subrequest_id);
                gwssrv_show_backbuffer();
                return 0;
            }
            break;

        // Draw black rectangle.
        case 5:
           //if (current_mode == GRAMADO_JAIL)
           //{
               rectBackbufferDrawRectangle ( 
                   0, 0, 28, 8, COLOR_GREEN, TRUE,
                   0 );  //rop_flags
               return 0;
           //}
           break;

        // Setup if we will show or not the 'fps window'.
        case 6:
            if( subrequest_id == TRUE )
            {
                show_fps_window = TRUE;
                return 0;
            }
            show_fps_window = FALSE;
            break;

         // Register wm pid
         case 7:
            gwssrv_debug_print ("serviceAsyncCommand: [7] Register wm pid\n");
             //printf ("serviceAsyncCommand: [7] [BREAKPOINT] Register wm pid\n");
             ____saved_wm_magic_pid = (int) Data;
             //exit(0);
             return 0;
             break;
             
        // 8
        // Window Manager requests. Power Trio.
        // As mensages aqui interessam somente ao window manager
        // que esta dentro do window server.
        case 8:
            printf ("serviceAsyncCommand: [8] \n");
            if (subrequest_id = 1)
            {
                
                //exit(0);
            }
            break;
        
        case 9:
            set_window_with_focus(Data);
            break;

        // ...
                
        default:
            gwssrv_debug_print ("serviceAsyncCommand: [ERROR] bad request\n");
                     // printf ("serviceAsyncCommand: [ERROR] bad request\n");
            // return -1;
            break;
    };

    return -1;
}



char *gwssrv_get_version(void)
{
    return VERSION;
}


/*
 ******************************
 * main: 
 *     + Initializes the gws infrastructure.
 *     + Create the background.
 *     + Create the taskbar.
 *     + Register window server as the current window server 
 *       for this desktop.
 *     + Create the server socket.
 *     + bind it.
 *     + Spawn a client process. (gwst.bin)
 *     + Enter in the main loop, waiting for some types of event.
 *       The possible events are: Reading messages from the kernel,
 *       or Reading our socket.
 *       In this moment we can send a response. It depends on the
 *       message found in the sockeck we readed.
 */


int main (int argc, char **argv)
{

    int flagUseClient = FALSE;
    //int flagUseClient = TRUE;

    // #debug flags
    int UseCompositor = TRUE;


    IsTimeToQuit = FALSE;
    IsAcceptingConnections = TRUE;

// =============

// #test
// Registrando handlers de input.
// usados como aceleradores de teclado e mouse.
// Dessa forma o console poder'a digitar diretamente 
// na janela com foco de entrada.
// Os parametros serao passados via memoria compartilhada.
// poderao ser longos ou pequenos.
// >>>>>>> ou ainda esses handler pode receber apenas algumas mensagens,
// principalmente as mensagens relativas ao window manager.
// It also enable the kernel for calling our compositor.
// See the callback in wm.c

// adiamos essa rotina
// assim o kernel somente chamara o ws quando as coisas
// estiverem inicializadas por aqui.

/*
    gramado_system_call( 
        101234, 
        (unsigned long) &wmHandler, 
        (unsigned long) &wmHandler, 
        (unsigned long) &wmHandler );
*/



// =============
    //gws_enable_transparence();
    gws_disable_transparence();

// ===============

    //==================
    struct sockaddr server_address;
    socklen_t addrlen;

    server_address.sa_family = AF_GRAMADO;
    server_address.sa_data[0] = 'w';
    server_address.sa_data[1] = 's';

    addrlen = sizeof(server_address);
    //==================


    // files.
    int server_fd = -1; 
    int newconn = -1;
    int curconn = -1;
    
    int bind_status = -1;
    
    int i=0;
    int _status = -1;


//
// Window server
//

// The window server main struture.
// See: ?

    struct gws_d *window_server;
    
    window_server = (struct gws_d *) malloc ( sizeof( struct gws_d) );

    if ( (void*) window_server == NULL )
    {
        gwssrv_debug_print("gwssrv.bin: [FAIL] window_server \n");
        printf            ("gwssrv.bin: [FAIL] window_server \n");
        exit(1);
    }
    
    memset( window_server, 0, sizeof(struct gws_d) );
    
    // Saving the pointer.
    
    gws = window_server;

    // Version.
    
    window_server->version_major = VERSION_MAJOR;
    window_server->version_minor = VERSION_MINOR;

    // strings
    // #todo: we need to finalize these strings?

    // name

    sprintf( window_server->name, "Gramado Window Server" );
    strcat(window_server->name,"\0");
    
    // edition name

    sprintf( window_server->edition_name, "Aurora" );
    strcat(window_server->edition_name,"\0");

    // version string
    
    sprintf( window_server->version_string, "0.1" );
    strcat(window_server->version_string,"\0");


    // We need to register the server in the host system.

    window_server->registration_status = FALSE;

    // graphics initialization status.

    window_server->graphics_initialization_status = FALSE;


    // Se devemos ou não lançarmos o primeiro client.
    // #todo: Pegaremos essa informação dos parâmetros.

    window_server->launch_first_client = TRUE;

    // When to quit the window server.

    window_server->quit = FALSE;
    
    // #todo
    
    window_server->status = 0;


    // ===============

    // Used in this file?
    // Flag usada no loop.
    running = TRUE;

    /*
    FILE *input_fp;
    fflush(stdin);
    close(fileno(stdin));
    input_fp = fopen("kstdin.txt","r+");
    if( (void*) input_fp == NULL ){
        printf ("gwssrv: input_fp fail\n");
        exit(1);
    }
    */


    /*
    //#test
    char display[100];
    hostName    = "gramado";
    displayNum  = ":0";
    screenNum   = ".0";
    strcpy(display, hostName);
    strcat(display, displayNum);
    strcat(display, screenNum);
    printf("DISPLAY={%s}\n",display);
    while(1){}
    */


    // invalidate the frame.
    // invalidate all the background.
    // See: gws.c

    invalidate();
    invalidate_background();


    //++
    //==================
    // Main loop!
    // This loop will make the server restart and
    // call the childs again.

    char buf[32];
    int CanRead=-1;


// #todo:
// Initialize all the OS dependent stuff.
// ex: OsInit();

// #debug
    gwssrv_debug_print ("-----------------------\n");
    gwssrv_debug_print ("gwssrv: Initializing...\n");
    // printf             ("gwssrv: Initializing...\n");

//
// Init clients support.
//

// Inicializa a lista de clientes.

    gwssrv_init_client_support();


//
// #bugbug
//

// Essa estrutura nao esta sendo inicializada corretamente.
    
    if ( serverClient->used != TRUE || serverClient->magic != 1234 )
    {
         //asm ("int $3");
    } 


// Register.

// #bugbug
// I don't know if we can register more than one time.
        // We can fix it!

// Register.
// Register window server as the current window server 
// for this desktop.
// #bugbug: 
// Se tentarmos reiniciar o servidor, talvez
// nao consigamos registrar novamente, pois ja tem um registrado.
// Precisamos a opcao de desregistrar, para tentarmos 
// mais de um window server.
// See: connect.c

    _status = (int) register_ws();

    if (_status<0){
        gwssrv_debug_print ("gwssrv: Couldn't register the server \n");
        printf             ("gwssrv: Couldn't register the server \n");
        exit(1);
    }
    gwssrv_debug_print ("gwssrv: Registration ok \n");
    window_server->registration_status = TRUE;


// #todo
// Daqui pra frente é conexão com cliente.
// Lembrando que o servidor vai se conectar à mais de um cliente.
// ...

// #todo
// Aqui nos podemos criar vários sockets que serão usados
// pelo servidor.
//ex: CreateWellKnownSockets ();


//
// Socket
//

// + Creating the socket for the server.
// + Saving the socket fd.

    // #debug
    //printf ("gwssrv: [1] socket()\n");

    server_fd = (int) socket (AF_GRAMADO, SOCK_STREAM, 0);
    if (server_fd<0){
        gwssrv_debug_print ("gwssrv: [FATAL] Couldn't create the server socket\n");
        printf             ("gwssrv: [FATAL] Couldn't create the server socket\n");
        exit(1);
    }

// Global variable.
    ____saved_server_fd   = (int) server_fd;

// Window server structure.
    window_server->socket = (int) server_fd;

// The server itself has its own client structure.
    serverClient->fd      = (int) server_fd;


    // #debug
    //printf ("fd: %d\n", serverClient->fd);
    //while(1){}


//
// bind
//

    // #debug
    //printf ("gwssrv: [2] bind()\n");

    bind_status = bind (
                      server_fd, 
                      (struct sockaddr *) &server_address, 
                      addrlen );

    if (bind_status<0){
        gwssrv_debug_print ("gwssrv: [FATAL] Couldn't bind to the socket\n");
        printf             ("gwssrv: [FATAL] Couldn't bind to the socket\n");
        exit(1);
    }


    // #debug
    //printf ("fd: %d\n", serverClient->fd);
    //while(1){}


//
// listen
//

// #todo
// It will setup how many connection the kernel
// is able to have in the list.
// 5 clients in the list.

    // #debug
    //printf ("gwssrv: [3] listen()\n");

    listen (server_fd,5);


    // #debug
    //printf ("fd: %d\n", serverClient->fd);
    //while(1){}


//
// Init Graphics
//

// Draw something.
// Init gws infrastructure.
// Initialize the 3d graphics support.
// Let's create the atandard green background.

    // #debug
    //printf ("gwssrv: Init graphics\n");

// Init graphics
    initGraphics();

// Flag
    window_server->graphics_initialization_status = TRUE;

    //gws_show_backbuffer();
    //while(1){}

    // #debug
    //printf ("fd: %d\n", serverClient->fd);
    //while(1){}


//
// Child
//

    // Calling child.
    //printf ("gwssrv: Calling child \n"); 

    /*
    if ( window_server->launch_first_client == TRUE )
    {
        // #todo: Get the status.
        //rtl_clone_and_execute("gws.bin");
        //gwssrv_clone_and_execute ("gwm.bin");
        //gwssrv_clone_and_execute ("logon.bin");
    }
    */

    // #debug
    //printf ("fd: %d\n", serverClient->fd);
    //while(1){}


//
// Client
//

    //debug_print ("gwssrc: Calling client $$$$$\n");

/*
    if ( flagUseClient == TRUE )
    {
        //rtl_clone_and_execute("gws.bin");
        //rtl_clone_and_execute("gwm.bin");
        //rtl_clone_and_execute("editor.bin");
        //rtl_clone_and_execute("terminal.bin");
        //rtl_clone_and_execute("browser.bin");
        //rtl_clone_and_execute("fileman.bin");
        //rtl_clone_and_execute("logon.bin");
    }
*/



//
// =======================================
//

//
// Loop
//

// #todo
// Isso é um teste.
// #atenção
// Provavelmente precisamos de um loop
// contendo accept, read e write.
// Não presizamos criar o socket novamente,
// mas temos que refazer a conecção toda vez
// que enviarmos uma resposta.
// Então logo após enviarmos a resposta precisamos
// fechar o arquivo? Acabaria com o socket?
// #atenção:
// A função accept vai retornar o descritor do
// socket que usaremos ... por isso poderemos fecha-lo
// para assim obtermos um novo da próxima vez.

// loop:
    gwssrv_debug_print ("gwssrv: Entering main loop.\n");

//#todo:
// No loop precisamos de accept() read() e write();

    // Not used for now.
    connection_status = 1;

    //curconn = serverClient->fd;
    newconn = -1;
 
// Initialize frames counter.
    frames_count = 0;
    fps = 0;
 
 

// #important
// Here we are exporting some callback to the base kernel.
// This way the kernel is able to call this routine directly.
// It is possible because the window server and the base kernel
// are sharing the process structure and the same memory space.
// This is an unusual practice. :)

    gramado_system_call( 
        101234, 
        (unsigned long) &wmHandler, 
        (unsigned long) &wmHandler, 
        (unsigned long) &wmHandler );


//
// The message loop.
//

// + Accept connection from a client.
// + Call the dispatcher to porcess the message.

    while (running == TRUE)
    {

        // Flush
        wmRefreshDirtyRectangles();

        if (IsTimeToQuit == TRUE) { break; };


       // Accept

        newconn = accept ( 
                      ____saved_server_fd,
                      (struct sockaddr *) &server_address, 
                      (socklen_t *) addrlen );

        // Fail
        if (newconn <= 0){
            gwssrv_debug_print("gwssrv: accept returned FAIL\n");
        }

        // Dispatch
        if (IsAcceptingConnections == TRUE){
            gwssrv_debug_print("gwssrv: accept returned OK\n");
            dispacher(newconn);
            //close(newconn);
        }

        // Not accpeting
        if (IsAcceptingConnections == FALSE){
            //close(newconn);
        }
    };

// =======================================

    if (IsTimeToQuit != TRUE)
        debug_print ("gwssrv: [ERROR] Invalid IsTimeToQuit\n");


// #todo
// Now we will close the window server.  
// Free all the structure, one by one in cascade.
// See: 'gws' structure in gws.h
// We will call the kernel to unregister the window server.
// We will close all the sockets.
// ...


// Close the server's fd.
    if (server_fd>0)
        close(server_fd);

    // #todo
    // The kernel needs to react when the window server closes.
    // We can't live without it.
    
    // #debug
    // #bugbug:
    // Page fault  when exiting ... 

    gwssrv_debug_print ("gwssrv: [FIXME] Hang, not exit \n");
    printf             ("gwssrv: [FIXME] Hang, not exit \n");

// HANG
    while(1){};

// Suspended.
    return 0; 
}


// yield thread.
void gwssrv_yield(void)
{
    sc82(265,0,0,0);
}


void gwssrv_quit(void)
{
    IsTimeToQuit = TRUE;
}

//
// End
//

