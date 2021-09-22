


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
       backbuffer_putpixel ( 
           COLOR_BLACK,  // color 
           mouse_x,      // x
           mouse_y,      // y
           0 );          // rop_flags
       refresh_rectangle ( mouse_x, mouse_y, 10, 10 );
       
       //printf("[%d,%d] ",mouse_x,mouse_y);
       //refresh_screen();
       
    }else{
        // Não redesenhamos quando o evento for um click, sem movimento.
        ps2_mouse_moving = FALSE;
    }; 
    
    
}


#define __PS2MOUSE_SET_DEFAULTS              0xF6
#define __PS2MOUSE_SET_RESOLUTION            0xE8

void ps2mouse_initialize_device (void)
{
    unsigned char status = 0;
    unsigned char device_id=0;

    debug_print ("ps2mouse_initialize_device:\n");

//++
//======================================================
    // #obs:
    // A rotina abaixo habilita o segundo dispositivo. O mouse.
    // Mas antes da habilitação temos uma rotina que não me lembro
    // pra que serve, haha.
    // #importante: 
    // TALVEZ. Tô achando que essa rotina deva ser feita no 
    // teclado, apos seu reset, e não aqui no mouse.
    
    // #bugbug
    // Essa nao eh uma rotina de habilitaçao do dispositivo secundario,
    // estamos apenas lendo a porta 60 e devolvendo o que lemos o que lemos
    // com alguma modificaçao;
    // #todo: me parece que para habilitar o secundario eh
    // preciso apenas mandar 0xA8 para a porta 0x64.

// ========================================
// Enable the interrupts

    // Dizemos para o controlador entrar no modo leitura.
    // Esperamos para ler e lemos.
    // 0x20 Read Command Byte
    // I8042_READ = 0x20
    wait_then_write(0x64,I8042_READ);
    status = wait_then_read(0x60) | 2;

    // Dizemos para o controlador entrar no modo escrita.
    // Esperamos para escrever e escrevemos.
    // Enable the PS/2 mouse IRQ (12).
    // The keyboard uses IRQ 1 (and is enabled by bit 0 in this register).
    // 0x60 Write Command Byte
    wait_then_write (0x64,I8042_WRITE);   // I8042_WRITE = 0x60
    wait_then_write (0x60,status);   
    
// ========================================
// Activate mouse port. (2nd port)

    // 0x64 <<< 0xA8 ?? enable aux
    // 0x64 <<< 0xA9 ?? check for mouse
    // #todo: See i8042.h for the commands used in the initialization.

    // #test
    // Habilitando o dispositivo secundario na forma bruta.
    // Reenable mouse port.
    // #bugbug: a rotina de inicializaçao da controladora ps2,
    // esta fazendo isso ao fim da rotina. nao precisamos fazer isso aqui.
    // Talvez atrapalhe a inicializaçao.
    // 0xA8 Enable Mouse
    // 0xA7 Disable Mouse
    // 0xA9 Check Mouse InterfaceReturns 0, if OK

    // Enable mouse.
    wait_then_write(0x64,0xA8);
    mouse_expect_ack();

// ========================================
// Reset
//  Isso reseta o controlador inteiro ?

    // Essa rotina avisa que vai escrever na porta do mouse
    // antes de escrever.
    // Então reseta somente o mouse e não o teclado.
    zzz_mouse_write(0xFF); 
    mouse_expect_ack();

// ========================================
// Default

    // 0xF6 Set default settings.
    zzz_mouse_write (__PS2MOUSE_SET_DEFAULTS);
    mouse_expect_ack();

// --------------------------------------
	// set sample rate
	zzz_mouse_write(0xF3);
	mouse_expect_ack(); // ACK
	
	zzz_mouse_write(200);
	mouse_expect_ack(); // ACK

	// set resolution
	zzz_mouse_write(__PS2MOUSE_SET_RESOLUTION);
	mouse_expect_ack(); // ACK
	
	zzz_mouse_write(3);
	mouse_expect_ack(); // ACK

	
	//set scaling 1:1
	zzz_mouse_write(0xE6);
	mouse_expect_ack(); // ACK

	// Enable the mouse
	zzz_mouse_write(0xF4);
	mouse_expect_ack(); // ACK

//
// #test
//


//++
//=================================================

/*
//__enable_wheel:    
  
    // #obs:
    // A rotina abaixo habilita a rodinha, se o dispositivo possui.
    // Credits: Serenity OS.    

    // Pega o device id e faz configurações de wheel.
    zzz_mouse_write (PS2MOUSE_GET_DEVICE_ID);
    mouse_expect_ack();
    
    device_id = zzz_mouse_read(); 
    
    if (device_id != PS2MOUSE_INTELLIMOUSE_ID){

        // Send magical wheel initiation sequence.
        zzz_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        mouse_expect_ack();
        zzz_mouse_write (200);
        mouse_expect_ack();
        zzz_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        mouse_expect_ack();
        zzz_mouse_write (100);
        mouse_expect_ack();
        zzz_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        mouse_expect_ack();
        zzz_mouse_write (80);
        mouse_expect_ack();

        zzz_mouse_write (PS2MOUSE_GET_DEVICE_ID);
        mouse_expect_ack();
        
        // Porque estamos lendo novamente?
        device_id = zzz_mouse_read();
    }

    if (device_id == PS2MOUSE_INTELLIMOUSE_ID){
        //m_has_wheel = true;
        ps2_mouse_has_wheel = 1;
        kprintf ("ps2mouse_initialize_device: Mouse wheel enabled!\n");
    } else {
        kprintf ("ps2mouse_initialize_device: No mouse wheel detected!\n");
    };
*/


/*
0xFF ResetMouse reset
0xFE ResendFor serial communications errors
0xF6 Set DefaultsSet default values
0xF5 Disable (Data Reporting)In stream mode, 
     should be sentbefore any other command
0xF4 Enable (Data Reporting)In stream mode only
0xF3 Set Sample RateSets state sampling rate
0xF0 Set Remote modeSend data on request only
0xEB Read DataSend data packet request
0xEA Set Stream ModeSend data on events
0xE9 Status RequestGet mouse configuration (3 bytes)
0xE8 Set Resolution
0xE7 Set Scaling 2:1Accelerationmode
0xE6 Set Scaling 1:1Linear mode
*/

//=================================================
//--

    debug_print ("ps2mouse_initialize_device: done\n");
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


    // #todo
    // O bit 6 indica que é o primeir byte do pacote.
    // Synchronize
    //if(( *_byte & 64)==64) { count_mouse=0; };


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







