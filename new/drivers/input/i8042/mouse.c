


#include <kernel.h>  

// quando tem uma interrupção de mouse eu desligo o teclado e espero por ack
// mas quando tem uma interrupção de teclado, então eu desligo o mouse mas não espero o ack.

__VOID_IRQ 
irq12_MOUSE (void)
{
    debug_print ("irq12_MOUSE: [TODO]\n");

// Not initialized.
    if ( PS2.mouse_initialized != TRUE )
    {
        return;
    }

    wait_then_write (0x64,0xAD);    // Disable keyboard.
    //keyboard_expect_ack();
    
//
// #todo
//

// #bugbug
// This is a good routine.
// It works easily on qemu.
// But we need to fix some stuff to work
// also in the real machine.

    DeviceInterface_PS2Mouse();

    //#debug
    //printf("$\n");
    //refresh_screen();
    
    // #bugbug
    // E se isso falhar?
    wait_then_write (0x64,0xAE);    // Reanable keyboard.
    //keyboard_expect_ack();
}





