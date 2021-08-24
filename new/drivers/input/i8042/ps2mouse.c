


#include <kernel.h>  


// Screen width and height
extern unsigned long SavedX; 
extern unsigned long SavedY; 


static long mouse_x = 0;
static long mouse_y = 0;

static int ps2_mouse_moving=0;

static unsigned char mouse_packet_data = 0;   // várias flags;
static char mouse_packet_x = 0;      // delta x
static char mouse_packet_y = 0;      // delta y
static char mouse_packet_scroll = 0;
//====================================================================
// Contador
static int count_mouse = 0;
// Buffer
static char buffer_mouse[3];
//=====================================================================

// i8042 mouse status bit.
// The bit in the first byte.

#define MOUSE_FLAGS_LEFT_BUTTON    0x01
#define MOUSE_FLAGS_RIGHT_BUTTON   0x02
#define MOUSE_FLAGS_MIDDLE_BUTTON  0x04
#define MOUSE_FLAGS_ALWAYS_1       0x08
#define MOUSE_FLAGS_X_SIGN         0x10
#define MOUSE_FLAGS_Y_SIGN         0x20
#define MOUSE_FLAGS_X_OVERFLOW     0x40
#define MOUSE_FLAGS_Y_OVERFLOW     0x80



//  mouseHandler support
#define MOUSE_DATA_BIT  1
#define MOUSE_SIG_BIT   2
#define MOUSE_F_BIT     0x20
#define MOUSE_V_BIT     0x08 



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



// local worker
void __update_mouse (void)
{

// O primeiro byte comtém um monte de flags.
// O segundo byte comtém o delta x
// O terceiro byte comtém o delta y

    unsigned char Flags=0;
    char DeltaX=0;
    char DeltaY=0;
    
    Flags  = (mouse_packet_data & 0xFF);
    DeltaX = (mouse_packet_x & 0xFF);
    DeltaY = (mouse_packet_y & 0xFF);


//======== X ==========
// Testando o sinal de x.
// Do the x pos first.

//pega o delta x
//testa o sinal para x
do_x:
    if ( Flags & MOUSE_FLAGS_X_SIGN ){  goto x_neg;  }

//Caso x seja positivo.
x_pos:
    mouse_x += DeltaX;
    goto do_y;

//Caso x seja negativo.
x_neg:
    mouse_x -= ( ~DeltaX + 1 );

    if (mouse_x > 0){  goto do_y;  }

    mouse_x = 0;

//======== Y ==========
// Testando o sinal de x. 
// Do the same for y position.

//Pega o delta y.
//Testa o sinal para y.
do_y:
    if ( Flags & MOUSE_FLAGS_Y_SIGN ){  goto y_neg;  }

//Caso y seja positivo.
y_pos:
    mouse_y -= DeltaY;

    if ( mouse_y > 0 ){  goto quit; }

    mouse_y = 0;
    goto quit;

//Caso y seja negativo. 
y_neg:
    mouse_y += ( ~DeltaY + 1 );

// Quit
quit:
    return;
}


// local worker
void __ps2mouse_parse_data_packet (void)
{
    unsigned char Flags=0;

    mouse_packet_data = buffer_mouse[0];    // Primeiro char
    mouse_packet_x    = buffer_mouse[1];    // Segundo char.
    mouse_packet_y    = buffer_mouse[2];    // Terceiro char.

    Flags = mouse_packet_data;     // Para uso interno.

// == Posicionamento ====

    saved_mouse_x = mouse_x;
    saved_mouse_y = mouse_y;
    __update_mouse (); 


// Agora vamos manipular os valores atualizados.

    mouse_x = (mouse_x & 0x000003FF );
    mouse_y = (mouse_y & 0x000003FF );

    // screen limits
    if ( mouse_x < 1 ){ mouse_x = 1; }
    if ( mouse_y < 1 ){ mouse_y = 1; }
    //if ( mouse_x > (SavedX-16) ){ mouse_x = (SavedX-16); }
    //if ( mouse_y > (SavedY-16) ){ mouse_y = (SavedY-16); }

    if ( mouse_x > 300 ){ mouse_x = 300; }
    if ( mouse_y > 150 ){ mouse_y = 150; }

// Cursor movement
// Comparando o novo com o antigo, pra saber se o mouse se moveu.
// #obs: Pra quem mandaremos a mensagem de moving ??

    if ( saved_mouse_x != mouse_x || saved_mouse_y != mouse_y )
    {
        // flag: o mouse está se movendo.
        // usaremos isso no keydown.
        // >> na hora de enviarmos uma mensagem de mouse se movendo
        // se o botão estiver pressionado então temos um drag (carregar.)
        // um release cancela o carregar.
        ps2_mouse_moving = TRUE;
       // Apaga o antigo.
       // + Copia no LFB um retângulo do backbuffer 
       // para apagar o ponteiro antigo.
       //refresh_rectangle ( saved_mouse_x, saved_mouse_y, 20, 20 );
       // Acende o novo.
       //+ Decodifica o mouse diretamente no LFB.
       // Copiar para o LFB o antigo retângulo  
       // para apagar o ponteiro que está no LFB.
       //bmpDisplayMousePointerBMP ( mouseBMPBuffer, mouse_x, mouse_y );   
       
       // #bugbug: Essa rotina pode estar falhando na maquina real.
       // por problemas na inicializaçao de variaveis
       //drawDataRectangle( mouse_x, mouse_y, 10, 10, COLOR_BLACK, 0 );
       
       // IN: color, x, y, 0, rop_flags
       backbuffer_putpixel ( COLOR_BLACK, mouse_x, mouse_y, 0, 0 );
       refresh_rectangle ( mouse_x, mouse_y, 10, 10);
       
       //printf("[%d,%d] ",mouse_x,mouse_y);
       //refresh_screen();
       
    }else{
        // Não redesenhamos quando o evento for um click, sem movimento.
        ps2_mouse_moving = FALSE;
    }; 
    
    
}


void DeviceInterface_PS2Mouse(void)
{
    int posX = 0;
    int posY = 0;
    char *_byte;



// Not initialized.
    if ( PS2.mouse_initialized != TRUE )
    {
        return;
    }


    *_byte = (char) zzz_mouse_read();

    // #todo
    // Temos que checar se o primeiro byte é um ack ou um resend.
    // isso acontece logo apos a inicialização.
    // #define ACKNOWLEDGE         0xFA
    // #define RESEND              0xFE
    if ( *_byte == 0xFA ){
        printf ("DeviceInterface_PS2Mouse: [test.first_byte] ack\n");
        refresh_screen();
    }
    if ( *_byte == 0xFE ){
        printf ("DeviceInterface_PS2Mouse: [test.first_byte] resend\n");
        refresh_screen();
    }

// Counter
    switch (count_mouse){

        case 0:
            buffer_mouse[0] = (char) *_byte;
            if (*_byte & MOUSE_FLAGS_ALWAYS_1){  count_mouse++;  }
            break;

        case 1:
            buffer_mouse[1] = (char) *_byte;
            count_mouse++;
            break;

        case 2:
            buffer_mouse[2] = (char) *_byte;
            count_mouse = 0;
            
            if( (*_byte & 0x80) || (*_byte & 0x40) == 0 ) 
            {
                __ps2mouse_parse_data_packet();  //LOCAL WORKER
            }
            
            //old_mouse_buttom_1 = mouse_buttom_1;
            //old_mouse_buttom_2 = mouse_buttom_2;
            //old_mouse_buttom_3 = mouse_buttom_3;
            break;

        default:
            in8(0x60);
            count_mouse = 0;
            //old_mouse_buttom_1 = 0;
            //old_mouse_buttom_2 = 0;
            //old_mouse_buttom_3 = 0;
            //mouse_buttom_1 = 0;
            //mouse_buttom_2 = 0;
            //mouse_buttom_3 = 0;
            break;
    };


    //#debug
    //printf("$\n");
    //refresh_screen();
}





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







