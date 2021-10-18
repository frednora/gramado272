
// bitblt.c 



#include <gws.h>


//
// ## putpixel: 
// backbuffer e lfb ##
//
// IN: cor, x, y




// usando serviço do kernel
int 
grBackBufferPutpixel2 ( 
    unsigned int color, 
    int x, 
    int y )
{

    if (x<0){ return -1; }
    if (y<0){ return -1; }

    return (int) gramado_system_call ( 6, color, x, y );
}




int 
grBackBufferPutpixel ( 
    unsigned int color, 
    int x, 
    int y )
{

    // ...

    return (int) fb_BackBufferPutpixel(color,x,y,0);
}


/*
 *******************************************
 * fb_BackBufferPutpixel:
 *     Put pixel in the device screen.
 */

// #??
// Usando o endereço virtual do backbuffer
// Será que está mapeado ???
// Está em ring 3 ??? ou ring 0???

// Pinta um pixel no backbuffer.
// O buffer tem o tamanho da janela do dispositivo.
// A origem está em top/left.

// #bugbug
// #todo
// Precismos considerar o limite do backbuffer.
// Então teremos um Offset máximo.

// #todo
// Check some flags, just like rasterizations.
// We will need a lot of parameters in this kind of function
// Including the address of the backbuffer.


// Clipping against the device limits

// #todo
// rop_flags   ... raster operations
// See the same routine in the kernel side.

int 
fb_BackBufferPutpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long flags )  // #todo: Change to 'rop_flags'
{

    //debug_print("Pixel\n");

    unsigned char *where = (unsigned char *) ____BACKBUFFER_VA;


// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF );
    unsigned long deviceHeight = (__device_height & 0xFFFF );
    // #todo
    // Precismos considerar o limite do backbuffer.
    // Então teremos um Offset máximo.

    unsigned long tmpOffset=0;
    unsigned long MaxOffset=0;

    int Offset=0;
    
    // #todo
    // raster operation. rasterization.
    // unsigned long rop;


// 2MB limit
// Our buffer size.
// 2mb is the limit for 64bit full pagetable.
// #bubug: Não fazer multilicações
//MaxOffset = (int) (1024*10124*4);
//MaxOffset = (int) 0x00400000;
    MaxOffset = (int) 0x00200000;


    char b, g, r, a;
    b = (color & 0xFF);
    g = (color & 0xFF00)   >> 8;
    r = (color & 0xFF0000) >> 16;
    a = (color >> 24) + 1;

    // 3 = 24 bpp
    int bytes_count=0;



//
// Clipping
//

// Clipping against the device limits

    if (x<0){ goto fail; }
    if (y<0){ goto fail; }
    if ( x >= deviceWidth ) { goto fail; }
    if ( y >= deviceHeight ){ goto fail; }


// Purify
    x = ( x & 0xFFFF);
    y = ( y & 0xFFFF);


//
// bpp
//

// #danger
// Esse valor foi herdado do bootloader.

    switch (SavedBPP){
        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        //case 16:  bytes_count = 2;  break;
        //case 8:   bytes_count = 1;  break;
        default:
            printf("grBackBufferPutpixel: [ERROR] SavedBPP\n");
            goto fail;
            break;
    };

// #importante
// Pegamos a largura do dispositivo.

    //width = (int) SavedX; 


// unsigned long
// Nao pode ser maior que 2MB.
// Que eh o tamanho do buffer que temos ate agora.
    unsigned long pitch=0; 
    

    if (bytes_count!=3 && bytes_count!=4 )
        return -1;

    if (bytes_count==3)
    {
        pitch = (unsigned long) (deviceWidth*bytes_count);
        tmpOffset = (unsigned long) ( (pitch*y) + (x*bytes_count) );
    }

    if (bytes_count==4)
    {
        pitch = (unsigned long) (deviceWidth<<2);
        tmpOffset = (unsigned long) ( (pitch*y) + (x<<2) );
    }

    if( tmpOffset >= MaxOffset )
    {
        debug_print ("grBackBufferPutpixel: [ERROR] backbuffer limits > Max\n"); 
        return -1;
        //asm("int $3");
        
        //printf ("pixelBackBufferPutpixel: [ERROR] backbuffer limits > Max\n");
        //printf ("tmpOffset=%x\n",tmpOffset);
        //printf ("x=%d\n",x);
        //printf ("y=%d\n",y);
        //printf ("width=%d\n",width);
        //exit(1);
        //goto fail;
    }

    // int. menor que 4MB
    Offset = (int) tmpOffset;

    // #bugbug
    // #todo
    // Para não termos problemas com o offset, temos que checar
    // os limites de x e y.

    //
    // Backbuffer limit
    //
    
    // #bugbug
    // Escrever fora do backbuffer pode gerar PF.



// #todo
// The rop_flags will give us some informations.
// the lsb is the operation code.
// See the same routine in the kernel side.


/*
//
// == Modify ==============================
//

    char b2, g2, r2, a2;

// get
    b2 = where[Offset];
    g2 = where[Offset +1];
    r2 = where[Offset +2];
    if ( SavedBPP == 32 ){ a2 = where[Offset +3]; };

// change
    b = (b2 & 1);
*/

//
// == Record ==============================
//

//
// BGR and A
//

    where[Offset]    = b;
    where[Offset +1] = g;
    where[Offset +2] = r;
    if ( SavedBPP == 32 ){ where[Offset +3] = a; };

    //debug_print("Pixel done\n");
    
    return 0;

fail:
    //debug_print("Pixel fail\n");
    return -1;
}





// pega uma cor dado sua posição
unsigned long
grBackBufferGetPixelColor ( 
    int x, 
    int y )
{

    // 0xC0800000
    unsigned char *where = (unsigned char *) ____BACKBUFFER_VA;


	// 3 = 24 bpp

    int bytes_count=0;


    // #bugbug
    // Essa funçao eta errada,
    // precisamos passar o ponteiro para o retorno via parametro
    // e o retorno da funçao deve ser int, pra indicar sucesso ou nao.

    if (x<0){ return 0; }
    if (y<0){ return 0; }



    //
    // bpp
    //
    
    // #danger
    // Esse valor foi herdado do bootloader.

    switch (SavedBPP){

        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        //case 16:  bytes_count = 2;  break;
        //case 8:   bytes_count = 1;  break;
        
        default:
            printf("backbuffer_putpixel: [ERROR] SavedBPP\n");
            //panic ("backbuffer_putpixel: SavedBPP");
            break;
    };

	// #importante
	// Pegamos a largura do dispositivo.
	
    //int width = (int) SavedX; 
    
    int width = (int) SavedX; 
    
    int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );

    //
    // BGR and A
    //

    // bgra
    char b, g, r, a;

    // Get bytes.

    b = where[offset];
    g = where[offset +1];
    r = where[offset +2];
    if ( SavedBPP == 32 ){ a = where[offset +3]; };
    
    unsigned long ColorBuffer=0;
    unsigned char *c = (unsigned char *) &ColorBuffer;

    // Set bytes of ColorBuffer.

    c[0] = b; 
    c[1] = g;
    c[2] = r;
    c[3] = a;

    // return the color value.
    return (unsigned long) ColorBuffer;
}



//
// End.
//



