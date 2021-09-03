/*
 * File: main.c
 *
 *    Browser application. UI.
 *
 * 2020 - Created by Fred Nora.
 */


// rtl 
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <packet.h>

// The client-side library.
#include <gws.h>


//
// == ports ====================================
//

#define PORTS_WS 4040
#define PORTS_NS 4041
#define PORTS_FS 4042
// ...


#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)


//char *hello = "Hello there!\n";
/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/




/*
 ********************************* 
 * main: 
 * 
 */

int main ( int argc, char *argv[] ){

    int client_fd = -1;

    // Porta para o Window Server 'ws' em gramado_ports[]
    struct sockaddr_in addr_in;

    addr_in.sin_family      = AF_INET;
    addr_in.sin_port        = PORTS_WS;   
    addr_in.sin_addr.s_addr = IP(192, 168, 1, 112); 


    debug_print ("-------------------------\n");
    debug_print ("browser: Initializing ...\n");


// Device info.
    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);

    if ( w == 0 || h == 0 ){
        printf ("browser: w h \n");
        exit(1);
    }


//
// socket
// 

    // #debug
    printf ("browser: Creating socket\n");

    client_fd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( client_fd < 0 ){
       printf ("browser: Couldn't create socket\n");
       exit(1);
    }


//
// connect
// 

// Nessa hora colocamos no accept um fd.
// Então o servidor escreverá em nosso arquivo.

// #debug
    printf ("browser: Connecting to the address via inet  ...\n");    

    while (TRUE){
        if (connect (client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            debug_print("browser: Connection Failed \n"); 
            printf     ("browser: Connection Failed \n"); 
        }else{ break; }; 
    };


// ==============================================

// #todo: 
// Salvar em global.
// Por enquanto aqui.

    int main_window=0;
    int addressbar_window=0;    
    int client_window=0;
    int button=0;

// A janela é a metade da tela.
    unsigned long w_width  = (w/2);
    unsigned long w_height = (h/2); 

    unsigned long viewwindowx = ( ( w - w_width ) >> 1 );
    unsigned long viewwindowy = ( ( h - w_height) >> 1 );


    if ( w == 320 )
    {
        // dimensoes
        w_width  = w;
        w_height = h;
        
        // posicionamento
        viewwindowx = 0;
        viewwindowy = 0;
    }


// ===================
// main window
// style: 
// 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen | 0x0008 statusbar

    main_window = gws_create_window ( 
                      client_fd,
                      WT_OVERLAPPED, 1, 1, "Browser",
                      viewwindowx, viewwindowy, w_width, w_height,
                      0, 
                      0x0000,  
                      COLOR_GRAY, COLOR_GRAY );

    if ( main_window < 0 ){
        debug_print("browser: main_window fail\n"); 
        exit(1);
    }

// ===================
// address bar
    addressbar_window = gws_create_window (
                            client_fd,
                            WT_EDITBOX,1,1,"AddressBar",
                            4, 32 +4, 
                            (w_width-32-4-4-4), 32,
                            main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if ( addressbar_window < 0 ){
        debug_print("browser: addressbar_window fail\n"); 
    }

// IN: 
// fd, window id, left, top, color, string
    if( addressbar_window > 0 ){
        gws_draw_text (
            (int) client_fd,
            (int) addressbar_window,
             8, 8, (unsigned long) COLOR_BLACK,
            "https://github.com/frednora");
    }

// ===================
// button
    button = gws_create_window (client_fd,
        WT_BUTTON,1,1,">",
        (w_width-32-4), 32 +4, 
        32, 32,
        main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if ( button < 0 ) 
        debug_print("browser: button fail\n"); 


// ===================
// client window (White)
    client_window = gws_create_window (client_fd,
        WT_SIMPLE,1,1,"client",
        4, 32 +40, 
        w_width-8, w_height - 40 - 4 -32,
        main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if ( client_window < 0 )             
        debug_print("browser: client_window fail\n"); 

    if ( client_window > 0 ){
     gws_draw_text (
        (int) client_fd,        // fd,
        (int) client_window,    // window id,
        (unsigned long) 40,     // left,
        (unsigned long) 40,     // top,
        (unsigned long) COLOR_BLACK,
        "Hello there !!");
    }

//
// Refresh
//
    gws_refresh_window( client_fd, main_window );


// ============================================
// focus
// Editbox!

    gws_async_command(
         client_fd,
         9,             // set focus
         addressbar_window,
         addressbar_window );

// =======================================================

//
// Loop
//

// #test
// pegando um evento com o ws.
// See: libgws/

    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.msg = 0;
    lEvent.long1 = 0;
    lEvent.long2 = 0;

    struct gws_event_d *e;

// loop
    while(1){
        
        e = (struct gws_event_d *) gws_get_next_event(client_fd, (struct gws_event_d *) &lEvent);
        
        //#debug
        //if( (void*) e == NULL )
            //printf("."); fflush(stdout);
            
        //if( (void*) e != NULL )
        //{
            //if (e->magic != 1234 )
                //printf ("browser: Invalid struct\n");
            
            // print what we got
            // IMPRIME SOMENTA AS MENSAGENS DE DIGITAÇAO
            if( e->msg == MSG_KEYDOWN )
               printf ("event: >>[%c]\n",lEvent.long1);
           
            //#debug
            //if (e->magic == 1234 )
                //printf ("event: wid=%d msg=%d l1=%d l2=%d \n",
                    //lEvent.wid, lEvent.msg, lEvent.long1, lEvent.long2 );
        //}
    };

//HANG:
    while(1){}

    debug_print ("browser: bye\n"); 
    printf      ("browser: bye\n");

    return 0;
}


//
// End.
//








