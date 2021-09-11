

#include <kernel.h>



unsigned char in8 (unsigned int port)
{
    unsigned char ret=0;
 
    asm volatile (
        "inb %w1, %b0"
        : "=a" (ret)
        : "d"  (port) );

    return (unsigned char) (ret & 0xFF);
}
unsigned short in16 (unsigned int port)
{
    unsigned short ret=0;

    asm volatile (
        "inw %w1, %w0" 
        : "=a" (ret) 
        : "d" (port) );

    return (unsigned short) (ret & 0xFFFF);
}
unsigned int in32 (unsigned int port)
{
    unsigned int ret=0;

    asm volatile (
        "inl %w1, %k0" 
        : "=a" (ret) 
        : "d"  (port) );

    return (unsigned int) (ret & 0xFFFFFFFF);
}

//===============================

void out8 ( unsigned int port, unsigned char data )
{
    asm volatile (
        "outb %b0, %w1"
        :  // Nothing
        : "a" (data), "d" (port) );
}
void out16 (unsigned int port, unsigned short data)
{
    asm volatile (
        "outw %w0, %w1" 
        :  // Nothing
        : "a" (data), "d" (port) );
}
void out32 ( unsigned int port, unsigned int data )
{
    asm volatile (
        "outl %k0, %w1" 
        :  // Nothing
        : "a" (data), "d" (port) );
}




//
// --------------
//



void __x86_io_delay (void)
{
    asm ("xorl %%eax, %%eax" ::);
    asm ("outb %%al, $0x80" ::);
    return;
}

void wait_ns (int count)
{
    count /= 100; 

    while (--count)
        io_delay ();
}


unsigned int portsx86_IN ( int bits, unsigned int port )
{
    switch (bits){

        case 8:
            return (unsigned int) in8 ((int) port);
            break;

        case 16:
            return (unsigned int) in16 ( (int) port);
            break;

        case 32:
            return (unsigned int) in32 ( (int) port);
            break;

        default:
            //debug_print ("portsx86_IN: FAIL\n");
            return 0;
            break;
    };
}


void 
portsx86_OUT ( 
    int bits, 
    unsigned int port, 
    unsigned int value )
{

    switch (bits){

        case 8:
            out8 ( (int) port, (unsigned char) value );
            return;
            break;

        case 16:
            out16 ( (int) port, (unsigned short) value );
            return;
            break;

        case 32:
            out32 ( (int) port, (unsigned int) value );
            return;
            break;

        default:
            //debug_print ("portsx86_OUT: FAIL\n");
            return;
            break;
    };
}








