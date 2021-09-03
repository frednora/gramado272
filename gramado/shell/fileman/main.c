/*
 * File: main.c
 *
 *     File manager. (Gramado Shell)
 * 
 * 2020 - Created by Fred Nora.
 */

// rtl
#include <rtl/gramado.h>
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <packet.h>

// The client-side library
#include <gws.h>

// Local
#include <fileman.h>


// ==================================================
// Windows
// :: main window
static int Main_window = 0;
// :: Header window
// The top window has two windows: The address bar and the button.
static int Header_window = 0;
static int addressbar_window = 0;
static int button = 0;
// :: The menu window
// That one at the left side.
static int Menu_window = 0;
// :: The logo window
static int Logo_window = 0;
// :: The navigation window
static int Navigation_window = 0;
static int nw_button1 = 0;
static int nw_button2 = 0;
static int nw_button3 = 0;
// :: Client window
static int Client_window = 0;
static int cw_button1 = 0;
// ==================================================


//
// == ports ====================================
//

#define PORTS_WS 4040
#define PORTS_NS 4041
#define PORTS_FS 4042
// ...

// JAIL,P1 ...
int current_mode;



#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)


int fileman_init_globals(void)
{
    gws_debug_print("fileman_init_globals:\n");
    
    gScreenWidth  = gws_get_system_metrics(1);
    gScreenHeight = gws_get_system_metrics(2);

    
    //...
    
    return 0;
}


int fileman_init_windows(void)
{
    int i=0;


    gws_debug_print("fileman_init_windows:\n");
    for(i=0;i<WINDOW_COUNT_MAX;i++)
    {
        windowList[i] = 0;
    };
    return 0;
}


//char *hello = "Hello there!\n";
/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/


//
// Main
//

int main ( int argc, char *argv[] ){

    int client_fd = -1;

    struct sockaddr_in addr_in;

    addr_in.sin_family = AF_INET;
    addr_in.sin_port   = PORTS_WS;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);

    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);


    debug_print ("-------------------------\n");
    debug_print ("fileman: Initializing ...\n");

// Gramado mode.
    current_mode = rtl_get_system_metrics(130);

// #debug
    //printf ("The current mode is %d\n",current_mode);
    //exit(0);

//
// socket
// 

// #debug
    printf ("fileman: Creating socket\n");

    client_fd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( client_fd < 0 ){
        printf ("fileman: Couldn't create socket\n");
        exit(1);
    }

//
// connect
// 

// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.

    printf ("fileman: Connecting to ws via inet ...\n");

    while (1){
        if (connect (client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){
            gws_debug_print("fileman: Connection Failed \n");
            printf         ("fileman: Connection Failed \n");
        }else{ break; }; 
    };



//
// == Main ==============
//

    // #hackhack
    unsigned long titlebarHeight = 32;

    unsigned long wLeft   = 0;
    unsigned long wTop    = 0;
    unsigned long wWidth  = w;  //(w >> 1);
    unsigned long wHeight = h;

    if (current_mode == GRAMADO_JAIL ){
        wLeft=0;  wTop=0;  wWidth=w;  wHeight=h;
    }

// main window
// Locked and maximized.
// style: 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen | 0x0008=statusbar?
// style: 0x8000=locked

    Main_window = gws_create_window ( 
                      client_fd,
                      WT_OVERLAPPED, 1, 1, "Gramado Shell",
                      wLeft, wTop, wWidth, wHeight,
                      0,
                      0x8008,
                      COLOR_GRAY, COLOR_GRAY );

    if ( Main_window < 0 ){
        debug_print("fileman: Main_window fail\n");
        printf     ("fileman: Main_window fail\n");
        exit(1);
    }


//
// == Header_window ======================
//

    Header_window = gws_create_window ( 
                        client_fd,
                        WT_SIMPLE, 1, 1,"HeaderWin",
                        2,                  //left 
                        titlebarHeight +2,  //top 
                        wWidth -2 -2,       //width 
                        32 +2 +2,           //height
                        Main_window, 0, xCOLOR_GRAY7, xCOLOR_GRAY7 );

    if ( Header_window < 0 ){
        debug_print("fileman: Header_window fail\n"); 
        printf     ("fileman: Header_window fail\n"); 
        exit(1);
    }


//
// == Address bar =========================
//

// #bugbug
// The window server needs to fix the client area.
// So 0,0 needs to mean the top/left of the client area.

// address bar
    addressbar_window = gws_create_window ( 
                            client_fd,
                            WT_EDITBOX, 1, 1,"AddressBar",
                            4, titlebarHeight +4, 
                            (wWidth-40), 32,
                            Main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if ( addressbar_window < 0 ){
        debug_print("fileman: addressbar_window fail\n"); 
        printf     ("fileman: addressbar_window fail\n"); 
        exit(1);
    }


//
// == Button =========================================
//

    // [>]
    // button
    button = gws_create_window ( 
                 client_fd,
                 WT_BUTTON, BS_DEFAULT, 1, ">",
                 (wWidth-32-4), titlebarHeight +4, 32, 32,
                 Main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if ( button < 0 ){
        debug_print("fileman: button fail\n"); 
        printf     ("fileman: button fail\n"); 
        exit(1);
    }


//
// == Menu_window ============================
//

    unsigned long mwLeft   = 2;
    unsigned long mwTop    = (titlebarHeight +40);
    unsigned long mwWidth  = (wWidth/4) -4;
    unsigned long mwHeight = (wHeight - mwTop -4 -titlebarHeight);

    Menu_window = gws_create_window ( 
                        client_fd,
                        WT_SIMPLE, 1, 1,"MenuWin",
                        mwLeft, mwTop, mwWidth, mwHeight, 
                        Main_window, 0, COLOR_BLUE, COLOR_BLUE );

    if ( Menu_window < 0 ){
        debug_print("fileman: Menu_window fail\n"); 
        printf     ("fileman: Menu_window fail\n"); 
        exit(1);
    }


//
// == Logo_window ============================
//

    unsigned long lwLeft   = 0;
    unsigned long lwTop    = 0;
    unsigned long lwWidth  = mwWidth;
    unsigned long lwHeight = 40;

    Logo_window = gws_create_window ( 
                        client_fd,
                        WT_SIMPLE, 1, 1,"LogoWin",
                        lwLeft, lwTop, lwWidth, lwHeight, 
                        Menu_window, 0, COLOR_BLUE2CYAN, COLOR_BLUE2CYAN );

    if ( Logo_window < 0 ){
        debug_print("fileman: Logo_window fail\n"); 
        printf     ("fileman: Logo_window fail\n"); 
        exit(1);
    }

    /*
    gws_draw_text (
        (int) client_fd,      // fd,
        (int) Logo_window,    // window id,
        (unsigned long) 8,    // left,
        (unsigned long) 8,    // top,
        (unsigned long) COLOR_BLACK,
        " -- Gramado OS  -- ");
    */

//
// == Navigation_window ===============
//

    unsigned long nwLeft   = 0;
    unsigned long nwTop    = (mwHeight -40);  // altura da menuwindow - 40
    unsigned long nwWidth  = mwWidth;         // largura da menuwindow
    unsigned long nwHeight = 40;

    Navigation_window = gws_create_window ( 
                            client_fd,
                            WT_SIMPLE, 1, 1,"NaviWin",
                            nwLeft, nwTop, nwWidth, nwHeight, 
                            Menu_window, 0, COLOR_YELLOW, COLOR_YELLOW );

    if ( Navigation_window < 0 ){
        debug_print("fileman: Navigation_window fail\n"); 
        printf     ("fileman: Navigation_window fail\n"); 
        exit(1);
    }


// [<] button
    if ( Navigation_window > 0 ){
    nw_button1 = gws_create_window ( 
                                   client_fd,
                                   WT_BUTTON, BS_DEFAULT, 1, "<",
                                   (nwWidth/5)*1, 
                                   8,
                                   ((nwWidth/5) -4), 
                                   24,
                                   Navigation_window, 
                                   0, 
                                   COLOR_WHITE, COLOR_WHITE );
    }
    if ( nw_button1 < 0 ){
        debug_print("fileman: nw_button1 fail\n"); 
    }


// [?] button
    if ( Navigation_window > 0 ){
    nw_button2 = gws_create_window ( 
                                   client_fd,
                                   WT_BUTTON, BS_DEFAULT, 1, "?",
                                   (nwWidth/5)*2, 
                                   8,
                                   ((nwWidth/5) -4), 
                                   24,
                                   Navigation_window, 
                                   0, 
                                   COLOR_WHITE, COLOR_WHITE );
    }
    if ( nw_button2 < 0 ){
        debug_print("fileman: nw_button2 fail\n"); 
    }

// [!] button
    if ( Navigation_window > 0 ){
    nw_button3 = gws_create_window ( 
                                   client_fd,
                                   WT_BUTTON, BS_DEFAULT, 1, "!",
                                   (nwWidth/5)*3, 
                                   8,
                                   ((nwWidth/5) -4), 
                                   24,
                                   Navigation_window, 
                                   0, 
                                   COLOR_WHITE, COLOR_WHITE );
    }
    if ( nw_button3 < 0 ){
        debug_print("fileman: nw_button3 fail\n"); 
    }


//
// == Client window =====================================
//

    unsigned long cwLeft   = (wWidth/4);
    unsigned long cwTop    = titlebarHeight +40;
    unsigned long cwWidth  = (wWidth - cwLeft -4);
    unsigned long cwHeight = (wHeight - cwTop -4 -titlebarHeight);

    // client window (White)
    Client_window = gws_create_window ( 
                        client_fd,
                        WT_SIMPLE, 1, 1, "ClientWin",
                        cwLeft, cwTop, cwWidth, cwHeight,
                        Main_window, 
                        0x0001,  // style: 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen 
                        COLOR_WHITE, 
                        COLOR_WHITE );

    if ( Client_window < 0 ){
        debug_print("fileman: Client_window fail\n"); 
    }


// [/] button
    if ( Client_window > 0 ){
    cw_button1 = gws_create_window ( 
                                   client_fd,
                                   WT_BUTTON, BS_DEFAULT, 1, "/",
                                   8, 8, 50, 24,
                                   Client_window, 
                                   0, 
                                   COLOR_WHITE, COLOR_WHITE );
    }
    if ( cw_button1 < 0 ){
        debug_print("fileman: cw_button1 fail\n"); 
    }


//
// =============================================================
//

// dir entries

    int e=0;
    int max=22;
    if (current_mode==GRAMADO_JAIL){max=5;}

    /*
    for (e=3; e<max; e++){
    // fd, window_id, left, top, color, name.
    gws_draw_text ( (int) client_fd, (int) Client_window,
        8, (e*16), COLOR_BLACK, "FAKEFILE.TXT");
    }
    */


// ============================================
// focus
// It needs to be an EDITBOX window.

// We need a wrapper for that thing.

    gws_async_command(
         client_fd,
         9,             // set focus
         addressbar_window,
         addressbar_window );



// #importante
// Se não usarmos o loop acima, então podemos pegar
// as mensagens do sistema....
// O ws pode mandar mensagens de sistema para o
// wm registrado.


    /*
    struct gws_event_d *Event;
     
    for(;;){
        
        Event = (struct gws_event_d *) gws_next_event();
        
        if (Event.type == 0){
           gws_debug_print("gwm: event 0\n");
        
        }else if (Event.type == 1){
           gws_debug_print("gwm: event 1\n");
        
        }else if (Event.type == 2){
           gws_debug_print("gwm: event 2\n");
        
        }else{
           gws_debug_print("gwm: Not valid event!\n");
        };
    };
    */

    /*
    //=================================
    //get current thread
    int cThread = (int) sc82 (10010,0,0,0);
    //set foreground thread.
    sc82 (10011,cThread,cThread,cThread);
    
    while(1){
        if ( rtl_get_event() == TRUE ){  
            filemanProcedure( (void*) RTLEventBuffer[0], RTLEventBuffer[1], RTLEventBuffer[2], RTLEventBuffer[3] );
        }
    };
    //=================================
    */

    while(1){}

    debug_print ("fileman: bye\n"); 
    printf      ("fileman: bye\n");
    return 0;
}


//
// End.
//








