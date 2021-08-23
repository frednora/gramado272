


#include <kernel.h>  


/*
#define PS2MOUSE_SET_RESOLUTION            0xE8
#define PS2MOUSE_STATUS_REQUEST            0xE9
#define PS2MOUSE_REQUEST_SINGLE_PACKET     0xEB
#define PS2MOUSE_GET_DEVICE_ID             0xF2
#define PS2MOUSE_SET_SAMPLE_RATE           0xF3
#define PS2MOUSE_ENABLE_PACKET_STREAMING   0xF4
#define PS2MOUSE_DISABLE_PACKET_STREAMING  0xF5
#define PS2MOUSE_SET_DEFAULTS              0xF6
#define PS2MOUSE_RESEND                    0xFE
#define PS2MOUSE_RESET                     0xFF
#define PS2MOUSE_INTELLIMOUSE_ID 0x03
*/

// Esta função será usada para escrever dados do mouse na porta 0x60, fora do IRQ12
void zzz_mouse_write(unsigned char data)
{
	kbdc_wait(1);
	out8(0x64,0xD4);
	kbdc_wait(1);
	out8(0x60,data);
}


/*
 **************************************
 * zzz_mouse_read:
 *     Pega um byte na porta 0x60.
 */

unsigned char zzz_mouse_read (void)
{
    prepare_for_input();
    
    return (unsigned char) in8(0x60);
}



void mouse_expect_ack (void)
{
    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );

    unsigned char ack_value=0;
    int timeout=100;

    while(1) 
    {
        timeout--;
        if (timeout<0){  break;  }

        ack_value = (unsigned char) zzz_mouse_read();
        
        // OK
        if (ack_value == 0xFA){
            return;  
        }
    }; 

// Acabou o tempo, vamos checar o valor.
// Provavelmente esta errado.
    if ( ack_value != 0xFA )
    {
        //#debug
        //printf ("expect_ack: not ack\n");
        return;
        //return -1;
    }

    return;
    //return 0;
}







