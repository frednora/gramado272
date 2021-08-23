



#include <kernel.h>


/* 
 *************************************
 * drawDataRectangle: (API)
 *     Draw a rectangle on backbuffer. 
 */

// Service 9.
// #bugbug
// Agora precisamos considerar o limite de apenas 2mb
// de lfb mapeados e de apenas 2 mb de backbuffer mapeados.
// Pois nao queremos escrever em area nao mapeada.


void 
drawDataRectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags )
{

    debug_print("drawDataRectangle: r0 :)\n");

// Copy.

    unsigned long X      = (x      & 0xFFFF);
    unsigned long Y      = (y      & 0xFFFF);
    unsigned long Width  = (width  & 0xFFFF); 
    unsigned long Height = (height & 0xFFFF);
    unsigned int Color   = color;

    
    // #todo
    // Get the clipping window/rectangle.

    struct rect_d  Rect;
    struct rect_d  ClippingRect;


// flag
    int UseClipping = TRUE;

// dc: Clipping
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();


    //loop
    unsigned long internal_height = (unsigned long) Height;

    // Clipping support.
    
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("drawDataRectangle: [PANIC] w h\n");
        //panic       ("drawDataRectangle: [PANIC] w h\n");
        return;
    }

//
// Clipping rectangle
//

// #todo
// It need to be a blobal thing.
// We need to handle the surfaces used by 
// this embedded window server and the loadable one.

    ClippingRect.left   = 0;
    ClippingRect.top    = 0;
    ClippingRect.width  = (unsigned long) (deviceWidth  & 0xFFFF);
    ClippingRect.height = (unsigned long) (deviceHeight & 0xFFFF);

    ClippingRect.right  = (ClippingRect.left + ClippingRect.width);
    ClippingRect.bottom = (ClippingRect.top + ClippingRect.height);


// #debug
// Provisório

    if ( ClippingRect.width > 800 )
       panic("drawDataRectangle: width");

    if ( ClippingRect.height > 600 )
       panic("drawDataRectangle: height");


    if ( ClippingRect.right > 800 )
       panic("drawDataRectangle: right");

    if ( ClippingRect.bottom > 600 )
       panic("drawDataRectangle: bottom");


//
// == Target rectangle ================
//

    Rect.bg_color = (unsigned int) Color;

    // Dimensions
    Rect.x = 0;
    Rect.y = 0;
    Rect.width  = Width;
    Rect.height = Height;

    // Margins
    Rect.left   = X; 
    Rect.top    = Y;
    Rect.right  = (unsigned long) (Rect.left + Rect.width);
    Rect.bottom = (unsigned long) (Rect.top  + Rect.height); 


//
// Clipping
//

	// Limits.
	
	// #todo: 
	// Repensar os limites para uma janela.
	// Uma janela poderá ser maior que as dimensões de um dispositivo.
	// mas não poderá ser maior que as dimensões do backbuffer.
	// Ou seja: O dedicated buffer de uma janela deve ser menor que
	// o backbuffer.

    //if ( Rect.right  > SavedX ){  Rect.right  = SavedX;  }
    //if ( Rect.bottom > SavedY ){  Rect.bottom = SavedY;  }


    if ( Rect.left   < ClippingRect.left   ){  Rect.left   = ClippingRect.left;   }
    if ( Rect.top    < ClippingRect.top    ){  Rect.top    = ClippingRect.top;    }
    if ( Rect.right  > ClippingRect.right  ){  Rect.right  = ClippingRect.right;  }
    if ( Rect.bottom > ClippingRect.bottom ){  Rect.bottom = ClippingRect.bottom; }


//
// Draw
//


    // Draw lines on backbuffer.

    if ( internal_height > 600 )
       panic("drawDataRectangle: internal_height");

//
// Paint
//

// Paint lines.
// Incrementa a linha a ser pintada.

    while (1)
    {
        my_buffer_horizontal_line ( 
            Rect.left, Y, Rect.right, Rect.bg_color, rop_flags );
 
        Y++;
        
        // #??
        // Porque podemos desejar escrever no backbuffer
        // um retângulo que ultrapasse a área do frontbuffer.
        
        if ( UseClipping == TRUE )
        {
            if ( Y > ClippingRect.bottom ){ break; };
        }

        // Decrementa o contador.
        internal_height--;
        if (internal_height == 0)
            break;
    };


// ??
// Send the rectangle to a list.

// Invalidate
// Sujo de tinta.

    Rect.dirty = TRUE;

    debug_print("drawDataRectangle: Done\n");
}




/*
 ***********************************************************
 * refresh_rectangle:
 *     Copiar um retângulo do backbuffer para o frontbuffer. 
 * 
 *     @todo: Rotina parecida com essa pode ser criada e usada para manipular 
 * regiões da tela, como área de cliente efetuar scroll de buffer em páginas 
 * de navegador ou menus .. mas para isso, a cópia seria dentro do próprio 
 * backbuffer ou de um terceiro buffer para o backbuffer. 
 *
 * Histórico:
 *     2017 - Criado por Frederico Lamberti Pissarra.
 *     2018 - Fred Nora.
 */

// #todo 
//kgws não pode acessar o lfb, devemos chamar o diálogo em x/video.c


//#importante
//É bem mais rápido com múltiplos de 4.

// #bugbug
// Agora precisamos considerar o limite de apenas 2mb
// de lfb mapeados e de apenas 2 mb de backbuffer mapeados.
// Pois nao queremos escrever em area nao mapeada.

void 
refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height )
{

    debug_print("refresh_rectangle: r0 :)\n");

    void *dest       = (void *)      FRONTBUFFER_ADDRESS;
    const void *src  = (const void*) BACKBUFFER_ADDRESS;

    // loop
    register unsigned int i=0;
    register unsigned int lines=0;
    unsigned int line_size=0; 
    register int count=0; 



    // Screen pitch.
    // screen line size in pixels * bytes per pixel.
    unsigned int screen_pitch=0;  
    // Rectangle pitch
    // rectangle line size in pixels * bytes per pixel.
    unsigned int rectangle_pitch=0;  




    unsigned int offset=0;

    // = 3; 24bpp
    int bytes_count=0;


    int FirstLine = (int) y;

    //int UseVSync = FALSE;
    int UseClipping = TRUE;


    // dc
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("refresh_rectangle: w h\n");
        panic       ("refresh_rectangle: w h\n");
    }


//
// Internal
//

    unsigned long X = (unsigned long) (x & 0xFFFF);
    unsigned long Y = (unsigned long) (y & 0xFFFF);

    line_size = (unsigned int) (width  & 0xFFFF); 
    lines     = (unsigned int) (height & 0xFFFF);

    switch (SavedBPP){

        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        // ... #todo
        
        default:
            panic ("refresh_rectangle: SavedBPP\n");
            break;
    };

//
// Pitch
//

// Screen pitch.
// Screen line size in pixels plus bytes per pixel.
    screen_pitch    = (unsigned int) (bytes_count * deviceWidth);

// Rectangle pitch.
// rectangle line size in pixels * bytes per pixel.
//(line_size * bytes_count) é o número de bytes por linha. 
    rectangle_pitch = (unsigned int) (bytes_count * line_size);


// #atenção.
//offset = (unsigned int) BUFFER_PIXEL_OFFSET( x, y );

    offset = (unsigned int) ( (Y*screen_pitch) + (bytes_count*X) );


    dest = (void *)       (dest + offset); 
    src  = (const void *) (src  + offset); 


	// #bugbug
	// Isso pode nos dar problemas.
	// ?? Isso ainda é necessário nos dias de hoje ??

    //if ( UseVSync == TRUE){
        //vsync();
    //}




// ================================
// Se for divisível por 8.
// Copy lines
// See:'strength reduction'
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy64: 8 bytes per time.

    if ( (rectangle_pitch % 8) == 0 )
    {
        count = (rectangle_pitch>>3);
        for ( i=0; i < lines; i++ ){
            if ( UseClipping == TRUE ){
                if ( (FirstLine + i) > deviceHeight ){ break; }
            }
            memcpy64 ( (void *) dest, (const void *) src, count );
            dest += screen_pitch;
            src  += screen_pitch;
        };
        return;
    }

// ================================
// Se for divisível por 4.
// Esse não será usado se for divisóvel por 8.
// Mas será chamado se for menor que 8, apenas 4.
// Copy lines
// See:'strength reduction'
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy32: 4 bytes per time.

    if ( (rectangle_pitch % 4) == 0 )
    {
        count = (rectangle_pitch>>2);
        for ( i=0; i < lines; i++ ){
            if ( UseClipping == TRUE ){
                if ( (FirstLine + i) > deviceHeight ){ break; }
            }
            memcpy32 ( (void *) dest, (const void *) src, count );
            dest += screen_pitch;
            src  += screen_pitch;
        };
        return;
    }


// ================================
// Se não for divisível por 4. (slow)
// Copy lines
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy: 1 byte per time.

    if ( (rectangle_pitch % 4) != 0 )
    {
        for ( i=0; i < lines; i++ ){
            if ( UseClipping == TRUE ){
                if ( (FirstLine + i) > deviceHeight ){ break; }
            }
            memcpy ( (void *) dest, (const void *) src, rectangle_pitch );
            dest += screen_pitch; 
            src  += screen_pitch; 
        };
        return;
    }
}



// ??
// A ideia aqui é efetuar o refresh de um retângulo 
// que esteja em um dado buffer.
// ?? Not tested yet

void 
refresh_rectangle2 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned long buffer1,
    unsigned long buffer2 )
{

	// #todo
	// Fazer a mesma otimizaçao que fizemos na outra rotina de refresh rectangle.


    void *p       = (void *)       buffer1;  // destino.
    const void *q = (const void *) buffer2;  // origem.


    register unsigned int i=0;
    register unsigned int lines=0;
    unsigned int line_size=0; 
    register int count=0; 

    unsigned int offset=0;

	// = 3; //24bpp
    int bytes_count;

	unsigned long Width = (unsigned long) screenGetWidth();
	unsigned long Height = (unsigned long) screenGetHeight();




//
// Internal
//

    unsigned long X = (unsigned long) (x & 0xFFFF);
    unsigned long Y = (unsigned long) (y & 0xFFFF);

    line_size = (unsigned int) (width  & 0xFFFF); 
    lines     = (unsigned int) (height & 0xFFFF);


    switch (SavedBPP)
    {
		case 32:  bytes_count = 4;  break;
		case 24:  bytes_count = 3;  break;
		//#todo: default
    };


	//offset = (unsigned int) BUFFER_PIXEL_OFFSET( x, y );
	offset = (unsigned int) ( (bytes_count*SavedX*(Y)) + (bytes_count*(X)) );

	p = (void *)       (p + offset);
	q = (const void *) (q + offset);

    //if( use_vsync)
    //vsync ();

	
	//(line_size * 3) é o número de bytes por linha. 
	
	//se for divisível por 4.
    if ( ((line_size * 3) % 4) == 0 )
    {
        count = ((line_size * 3) / 4);  	

	    for ( i=0; i < lines; i++ )
	    {
		    //copia uma linha ou um pouco mais caso não seja divisível por 
		    memcpy32 ( p, q, count );
		    
			q += (Width * 3);
	 	    p += (Width * 3);
	    };
    }

	//se não for divisível por 4.
    if ( ((line_size * 3) % 4) != 0 )
    {

        //count = (line_size * 3); 
	
	    for ( i=0; i < lines; i++ )
	    {
		    memcpy ( (void *) p, (const void *) q, (line_size * 3) );
		    q += (Width * 3);
		    p += (Width * 3);
	    };
    }

	/*
	
	for ( i=0; i < lines; i++ )
	{
		memcpy( p, q, (line_size * 3) );
		q += (Width * 3);
		p += (Width * 3);
	};	
	*/
}

/*
 ************************* 
 * scroll_screen_rect:
 * 
 *     Scroll a rectangle. ?
 */

// Helper function to scroll routine.
// Called by console_scroll() in tty/console.c


// Only for full screen console.
// See flag.

void scroll_screen_rect (void){

    register unsigned int i=0;

    unsigned int line_size=0;  // w 
    unsigned int lines=0;      // h

    unsigned int offset=0;

// Device info

    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();


    //int cWidth = get_char_width ();
    int cHeight = get_char_height ();

	// = 3; 
	//24bpp
    int bytes_count = 0;
    
    int count = 0; 


    // #debug
    //debug_print ("scroll_screen_rect:\n");

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("scroll_screen_rect: [PANIC] w h\n");
        panic       ("scroll_screen_rect: [PANIC] w h\n");
    }

    // #debug
    //if(cHeight == 8)
        //debug_print("8\n");

    // #debug
    //if(cHeight == 16)
        //debug_print("16\n");


    line_size = (unsigned int) deviceWidth; 
    lines     = (unsigned int) deviceHeight;

    switch (SavedBPP){
        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        // ...
        default:
            panic("scroll_screen_rect: SavedBPP");
            break;
    };


//
// Pointers
//

    // Destination and Source.
    // Destination is the first line.
    // Source is the second line. It has the height of a char.

    void *Dest = (void *) BACKBUFFER_ADDRESS;
    const void *Src  = (const void *) BACKBUFFER_ADDRESS + ( bytes_count * SavedX * cHeight ) ;


    //
    // Copy.
    //

	//#importante
	//É bem mais rápido com múltiplos de 4.	


    // Se for divisível por 4.
    // Copia uma linha, quatro bytes de cada vez.  
        
    if ( ((line_size * bytes_count) % 4) == 0 )
    {
        // #todo: Create a variable for 'pitch' and use streigh reduction
        count = ((line_size * bytes_count) / 4); 
        // count = (internal_pitch>>2);  // #todo: Use this one.

        for ( i=0; i < lines; i++ )
        {
            memcpy32 (Dest,Src,count);

            Src  += (deviceWidth * bytes_count);
            Dest += (deviceWidth * bytes_count);
        };
        return;
    }

    // Se não for divisível por 4.
    // Copia a linha, um bytes por vez.
    // #todo: Podemos suprimir esse if e deixarmos ssomente o for.
    
    if ( ((line_size * bytes_count) % 4) != 0 )
    {
        for ( i=0; i < lines; i++ )
        {
            memcpy ( 
                (void *) Dest, 
                (const void *) Src, 
                (line_size * bytes_count) );

            Src  += (deviceWidth * bytes_count);
            Dest += (deviceWidth * bytes_count);
        };
    }
}

// ??
// Copiando ...
// Destination is an Null pointer? 
// Source is an Null pointer? 
// Zero-sized copy? 
// Destination is Source?

void *rectStrCopyMemory32 ( 
    unsigned long *dest, 
    unsigned long *src, 
    int count ) 
{

    register int i=0;

    // Yes
    if ( (dest == NULL)  || 
         (src == NULL)   || 
         (count == 0)    || 
         (src == dest) ) 
    {
        return dest;
    }

    // GCC should optimize this for us :)
    // I'm lying.
    for ( i=0; i < count; i++ ) 
    {
        dest[i] = src[i];
    };

    return dest;
}    































