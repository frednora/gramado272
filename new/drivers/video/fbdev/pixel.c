
//D - Destination bitmap
//P - Selected brush (also called pattern)
//S - Source bitmap

// See:
// bit block transfer
// https://en.wikipedia.org/wiki/Bit_blit
// https://docs.microsoft.com/en-us/windows/win32/gdi/ternary-raster-operations




#include <kernel.h>

/*
 *********************************
 * backbuffer_putpixel:
 *     Ok. 
 *     O servidor kgws pode acessar um backbuffer. Mas não tem acesso
 * ao frontbuffer. Para isso ele precisa usasr o diálogo do driver 
 * de vídeo.
 * IN: 
 *     color, x, y, 0, rop_flags
 */

// #todo
// + Change the names of these parameters.
// + Create a parameter for the address of the buffer.

void 
backbuffer_putpixel ( 
    unsigned int ax,    // color, 32bit 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx,
    unsigned long rop_flags )
{

	// #importante
	// Esse é o original. Isso funciona.
	// Não usar.
	// hal_backbuffer_putpixel ( ax, bx, cx, dx );


	// #test
	// tentando um novo método.
	// usando o endereço virtual do backbuffer.
	// precisamos de uma variável global para isso.

    // # todo
    // Estamos determinando um valor.
    // Precisamos de uma variável de devinição do sistema.
    
    // BACKBUFFER_VA = 0xC0800000
    // See: gramado/kernel/include/gva.h

// Buffer address
    unsigned char *where = (unsigned char *) BACKBUFFER_VA;

// Color
    unsigned int color = (unsigned int) (ax & 0xFFFFFFFF);


// A cor passada via argumento.
// Color bytes
    char b, g, r, a;

    // bgra
    b = (color & 0xFF);
    g = (color & 0xFF00) >> 8;
    r = (color & 0xFF0000) >> 16;
    a = (color >> 24) + 1;


// Positions
    int x = (int) (bx & 0xFFFF);
    int y = (int) (cx & 0xFFFF);



// 3 = 24 bpp
// 2 = 16 bpp
// ...

    int bytes_count=0;

//
// bpp
//

// #danger
// This is a global variable.
// Esse valor foi herdado do bootloader.


    switch (SavedBPP){

        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;

		//#testando
		//case 16:
		//	bytes_count = 2;
		//	break;
		
		//case 8:
		//	bytes_count = 1;
		//	break;

        default:
            //#todo: Do we have panic ar this moment?
            //panic ("backbuffer_putpixel: SavedBPP\n");
            debug_print_string("backbuffer_putpixel: SavedBPP\n");
            while(1){}
            break;
    };

// #importante
// Pegamos a largura do dispositivo.

    int width = (int) SavedX;
    
    width = (int) (width & 0xFFFF);


    int offset=0;
    //int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );

    // 32bpp
    if (bytes_count==4){
        offset = (int) ( ((width<<2)*y) + (x<<2) );
    }

    // 24bpp
    if (bytes_count==3){
        offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );
    }

    // 16bpp
    //if (bytes_count==2){
    //    offset = (int) ( ((width<<1)*y) + (x<<1) );
    //}


//
// == Modify ==============================
//

// a cor no framebuffer
    char b2, g2, r2, a2;

// get
    b2 = where[offset];
    g2 = where[offset +1];
    r2 = where[offset +2];
    if ( SavedBPP == 32 ){ a2 = where[offset +3]; };


// A cor transformada.
// A cor a ser gravada.
    char b3, g3, r3, a3;

// The first byte;
// 0 ~ FF
    int operation = (int) (rop_flags & 0xFF);

// flags
// 16, 32, 64, 128 


// Sem modificação
// A cor a ser registrada é a mesma enviada por argumento.
    if ( operation == 0 ){
        r3=r;  g3=g;  b3=b;  a3=a;
    }

// 1 = or
    if ( operation == 1 )
    {
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
    }

// 2 = and
    if ( operation == 2 )
    {
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }

    // 3 = xor
    if ( operation == 3 )
    {
        r3 = (r2 ^ r);
        g3 = (g2 ^ g);
        b3 = (b2 ^ b);
        a3 = a2;
    }

// ============

    // red
    if ( operation == 10)
    {
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2; 
        a3 = a2;
    }

    // green
    if ( operation == 11)
    {
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2; 
        a3 = a2;
    }

    // blue
    if ( operation == 12)
    {
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE); 
        a3 = a2;
    }


// ============

    // gray
    if ( operation == 20)
    {
        r3 = (r2 & 0x80);
        g3 = (g2 & 0x80);
        b3 = (b2 & 0x80);
        a3 = a2;
    }

    // gray
    if ( operation == 21)
    {
        r3 = (r2 & 0x00);
        g3 = (g2 & 0xFF);
        b3 = (b2 & 0xFF);
        a3 = a2;
    }


//
// == Register =============================================
// 


//
// BGR and A
//

    where[offset]    = b3;
    where[offset +1] = g3;
    where[offset +2] = r3;
    if ( SavedBPP == 32 ){ where[offset +3] = a3; };

}

