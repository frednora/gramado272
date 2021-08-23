


#include <kernel.h>  


/*
 **************************************
 * xxx_mouse_read:
 *     Pega um byte na porta 0x60.
 */

unsigned char xxx_mouse_read (void)
{
    prepare_for_input();
    
    return (unsigned char) in8(0x60);
}



void expect_ack (void)
{
    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );


    unsigned char c=0;
    int tries=0;

    do 
    {
        if (tries>4)
            break;
        
        tries++;
        
        // #bugbug
        // Essa rotina so server para mouse.
        
        c = xxx_mouse_read();
    
    } while(c == 0xFE || c == 0);
    
    if ( c != 0xFA ){
        //#debug
        printf ("expect_ack: not ack\n");
        return;
        //return -1;
    }

    return;
    //return 0;
}





