// This is a client-side library for the GNS. 

// rtl
#include <types.h>  
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <heap.h>   

//#test: usado por gws_open_display
#include <sys/socket.h>

#include <rtl/gramado.h>   


#include "include/gns.h"


char __gns_buffer[512];


// System call.
void *gns_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
    unsigned long __Ret = 0;

    //System interrupt.

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}


// Debug vai serial port. (COM1)
void gns_debug_print (char *string)
{
    gns_system_call ( 289, 
        (unsigned long) string,
        (unsigned long) string,
        (unsigned long) string );
}

void gns_yield(void)
{
    gns_system_call (265,0,0,0); //yield thread.
}


// hello request
int __gns_hello_request (int fd)
{
    // Isso permite ler a mensagem na forma de longs.
    unsigned long *message_buffer = (unsigned long *) &__gns_buffer[0];   

    int n_writes = 0;   // For sending requests.

    //unsigned long ____color = 0x00FF00;
    int i=0;

// loop:
// Loop for new message.

new_message:

    //
    // Write
    //

    // #debug
    debug_print ("gns.bin: Writing ...\n");      

    // Enviamos um request para o servidor.
    // ?? Precisamos mesmo de um loop para isso. ??

    // Write!
    // Se foi possível enviar, então saimos do loop.        
    // obs: podemos usar send();

    while (1){

        // messae: Solicita um hello!      
        message_buffer[0] = 0;       // window. 
        message_buffer[1] = 1000;    // msg=hello.
        message_buffer[2] = 0;
        message_buffer[3] = 0;
        // ...

        n_writes = write (
                       fd, 
                       __gns_buffer, 
                       sizeof(__gns_buffer) );

        if (n_writes>0)
           break;
        
        for (i=0; i<9; i++){
            gramado_system_call (265,0,0,0); //yield thread.
            //gnst_yield();
        }
    }

    return 0;
}


int __gns_hello_response(int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__gns_buffer[0];   
    int n_reads = 0;    // For receiving responses.


    //
    // waiting
    //

    // Espera para ler a resposta. 
    // Esperando com yield como teste.
    // Isso demora, pois a resposta só será enviada depois de
    // prestado o servido.
    // obs: Nesse momento deveríamos estar dormindo.

    // #debug
    debug_print ("__gns_hello_response: Waiting ...\n");      

    int y;
    for(y=0; y<15; y++)
        gns_yield();



    //
    // read
    //

    // #debug
    debug_print ("__gns_hello_response: reading ...\n");      


       //#caution
       //we can stay here for ever.
       //it's a test yet.
__again:
    n_reads = read ( 
                  fd, 
                  __gns_buffer, 
                  sizeof(__gns_buffer) );

    if (n_reads <= 0)
    {
        printf ("__gns_hello_response: recv fail\n");
        gns_yield();
        return -1;
    }
        
    // Get the message sended by the server.
    int msg = (int) message_buffer[1];
    
    switch (msg)
    {
        case GNS_SERVER_PACKET_TYPE_REQUEST:
            gns_yield();
            goto __again;
            break;
            
        case GNS_SERVER_PACKET_TYPE_REPLY:
            debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_REPLY received\n"); 
            goto process_reply;
            break;
            
        case GNS_SERVER_PACKET_TYPE_EVENT:
            //todo: call procedure.
            goto __again;
            break;
            
        case GNS_SERVER_PACKET_TYPE_ERROR:
            debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_ERROR\n");
            goto __again;
            //exit (-1);
            break;
        
        default:
            goto __again;
            break; 
    };


    //
    // done:
    //

    char response_buffer[64];
    response_buffer[0]=0;

process_reply:

    //#bugbug: Overflow. We need a limit here.
    //#todo: Get the size of the message
    //and then put the message into the tmp buffer.
    sprintf( response_buffer, (__gns_buffer+128) );
    response_buffer[63] = 0;  //finaliza

// Se tem alguma coisa no buffer.
    if( *response_buffer != 0 )
        printf("RESPONSE: {%s} \n", response_buffer );

    return TRUE;
}




// =============================================

int gns_hello (int fd)
{
    
    if (fd<0){
        debug_print("gns_hello: fd\n");
    }

    //gnst_hello_request(fd);
    __gns_hello_request(fd);
    
    // Avisamos que um request foi enviado.
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

    // Waiting to read the response.
    int value=0;
    while (1){
        value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        if (value == ACTION_REPLY ) { break; }
        if (value == ACTION_ERROR ) { return -1; }
        gns_yield();
    };

    //return (int) gnst_hello_response(fd);
    return (int) __gns_hello_response(fd);
}




















