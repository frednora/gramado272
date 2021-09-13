/*
 * File: dtext.c 
 * 
 *     Draw text routines.
 */


#include <gws.h>



// Draw text.
// Service 1005
int serviceDrawText (void)
{

    // Global buffer.
    unsigned long *message_address = (unsigned long *) &__buffer[0];


    struct gws_window_d  *window;
    
    int window_id = -1;      // index 4
    unsigned long x;         // index 5
    unsigned long y;         // index 6
    unsigned long color;     // index 7


// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF );
    unsigned long deviceHeight = (__device_height & 0xFFFF );


    // #debug
    gwssrv_debug_print ("gwssrv: serviceDrawText\n");


// Get values.

    window_id = (int)           message_address[4];
    x         = (unsigned long) message_address[5];
    y         = (unsigned long) message_address[6]; 
    color     = (unsigned long) message_address[7];


    window_id = (window_id & 0xFFFF);
    x         = (x         & 0xFFFF);
    y         = (y         & 0xFFFF);


    /*
    //size 256 bytes
    unsigned char *text_buffer = (unsigned char *) &message_address[MSG_OFFSET_LONGSTRING];  
   
    int s = sizeof(text_buffer);
   
    if(s<0 || s>= 256)
    {
        gwssrv_debug_print ("gwssrv: serviceDrawText [DEBUG]   SIZE \n");
        exit(1);
    }
    */


// Limits
// O texto começa fora dos limites da tela do dispositivo.

    //if( x >= deviceWidth )
        //return -1;

    //if( y >= deviceHeight )
        //return -1;


//
// == Draw ==========================================
//


    //#todo
    // Get the window structure given the id.
    //window = (struct gws_window_d *) windowList[window_id];
    
    //#test
    // Usando a janela screen por enquanto.

// ==================================
// Get string from message
// OK. string funcionou.

    unsigned char buf[256+1];
    int i=0;
    int string_off=8;
    for(i=0; i<256; i++)
    {
         buf[i] = message_address[string_off];
         string_off++;
    }
    buf[i] = 0;
// ==================================

//
// == Draw ===============================================
//
    
    //#todo
    //switch (alignment) {  ... }
    
    
    //dtextDrawText ( (struct gws_window_d *) gui->screen,
        //x, y, color, buf ); 


// ok
// Se a janela alvo tem um índice fora dos limites

    if ( window_id < 0 ){ return -1; }
    if ( window_id > WINDOW_COUNT_MAX )
        return -1;

// ok
// Se a janela alvo tem um índice dentro dos limites.

    if ( window_id > 0 && 
         window_id < WINDOW_COUNT_MAX )
    {
        window = (struct gws_window_d *) windowList[window_id];

        // Bad index. Invalid pointer.
        if ( (void*) window == NULL ){ return -1; }

        // Bad index. Invalid pointer.
        if (window->magic != 1234)   { return -1; }

        // Good window. Let's paint on it.
        dtextDrawText ( 
            (struct gws_window_d *) window,
            x, y, color, buf );

        // Flush the window into the framebuffer.
        // #todo: invalidate, not show.
        gws_show_window_rect(window);

        // ok
        return 0;
    }

crazy_fail:
    debug_print("serviceDrawText: [ERROR] crazy_fail\n");
    return -1;
}




/* 
 * grDrawString:
 *     Draw a string on the screen. 
 */

// No clipping

void 
grDrawString ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string )
{

    int Index=0;

    //#bugbug:  Determinando
    int cWidth = 8;   
	//int cWidth = get_char_width ();
	//int cHeight = get_char_height ();


	//if ( cWidth == 0 || cHeight == 0 )
    if ( cWidth == 0 )
    {
        //panic ("grDrawString: cWidth");
    
        //#debug
        //gde_message_box (3, "xxx","dtext-draw_string: cWidth");
        printf ("grDrawString: cWidth\n");
        while(1){}
    }

    //int size = sizeof(string);  
    //for ( Index=0; Index<size; Index++ )

// Draw

    for ( Index=0; string[Index] != 0; Index++ )
    {
        grBackbufferDrawCharTransparent ( 
            x, y, 
            (unsigned int) color, string[Index] );

        // gradient
        //if( string_flags & ? ){
        //grBackbufferDrawCharTransparent ( 
            //x, y, 
            //(unsigned int) interpolate_color(COLOR_BLACK, color, x), 
            //string[Index] );
        //}

        x += cWidth;
    };
}


/* 
 * dtextDrawText:
 *     Draw text on a window.
 *
 * #test: 
 *     Como opção, se nenhuma janela for especificada, 
 *     então pintaremos na janela principal.
 */

void 
dtextDrawText ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string )
{ 

    struct gws_window_d * __w;


    __w = (struct gws_window_d *) gui->screen_window;


    if ( (void *) window == NULL ){

        if ( (void*)__w != NULL )
        {
            grDrawString ( 
                (__w->left +x), 
                (__w->top  +y), 
                (unsigned int) color, 
                string );
            return;
        }
        gwssrv_debug_print("dtextDrawText: __w\n");
        return;

    }else{
        grDrawString ( 
            (window->left +x), 
            (window->top  +y), 
            (unsigned int) color, 
            string );
    };
}


