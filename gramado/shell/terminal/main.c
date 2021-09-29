/*
 * File: main.c
 *
 *     Simple virtual terminal.
 *     2021 - Created by Fred Nora.
 */

// #test
// For now it has a small embedded command line interpreter.

// The POSIX terminal interface.
// Raw or Canonical?
// See:
// https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html
// https://en.wikipedia.org/wiki/POSIX_terminal_interface
// https://en.wikipedia.org/wiki/Computer_terminal
// ...


// Connecting via AF_INET.

// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
 
/*
    To make a process a TCP server, you need to follow the steps given below −

    Create a socket with the socket() system call.

    Bind the socket to an address using the bind() system call. 
    For a server socket on the Internet, an address consists of a 
    port number on the host machine.

    Listen for connections with the listen() system call.

    Accept a connection with the accept() system call. 
    This call typically blocks until a client connects with the server.

    Send and receive data using the read() and write() system calls.
*/ 

// See:
// https://wiki.osdev.org/Message_Passing_Tutorial
// https://wiki.osdev.org/Synchronization_Primitives
// https://devblogs.microsoft.com/commandline/
// /windows-command-line-introducing-the-windows-pseudo-console-conpty/
// ...

//#include <ctype.h>

// #todo:
// We need to change the name of this document??
#include "noraterm.h"



int cursor_x = 0;
int cursor_y = 0;
unsigned int fg_color;
unsigned int bg_color;
unsigned int prompt_color;

//
// == ports ====================================
//

#define PORTS_WS 4040
#define PORTS_NS 4041
#define PORTS_FS 4042
// ...


// #test
// Tentando deixar o buffer aqui e aproveitar em mais funções.
char __buffer[512];

#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)




void __send_to_child (void);



//constructor.
void terminalTerminal (void);
void terminalInitWindowPosition(void);
void terminalInitWindowSizes(void);
void terminalInitWindowLimits (void);
void terminalInitSystemMetrics (void);


void clear_terminal_client_window(int fd);
void compareStrings(int fd);
void doPrompt(int fd);

//====================================================


void clear_terminal_client_window(int fd)
{
     gws_redraw_window(fd,Terminal.client_window_id,TRUE);

// Cursor do console do terminal.

     //#define SYSTEMCALL_SETCURSOR  34
     //gramado_system_call ( 34, 2, 2, 0 );

// Cursor do terminal.
    cursor_x = 0;
    cursor_y = 0;
}

void compareStrings(int fd)
{
    //printf("\n");

    if ( strncmp(prompt,"test",4) == 0 )
    {
        cursor_y++;

        cursor_x=0;   
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            '\\' ); 

        cursor_x=1;
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            'o' ); 

        cursor_x=2;
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            '/' ); 

        goto exit_cmp;
    }

    if ( strncmp(prompt,"reboot",6) == 0 )
    {
        rtl_reboot();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"cls",3) == 0 )
    {
        // Redraw and show the client window.
        gws_redraw_window(
            fd,
            Terminal.client_window_id,
            TRUE );
        cursor_x = 0;
        cursor_y = 0;
        goto exit_cmp;
    }

// Empty buffer
   if( *prompt == 0 )
       goto exit_cmp;

// Clone.
    rtl_clone_and_execute(prompt);

    //printf("Command not found\n");

exit_cmp:
    doPrompt(fd);
}


void doPrompt(int fd)
{
    int i=0;


    if(fd<0)
        return;

    // Clean prompt buffer.
    
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){ prompt[i] = (char) '\0'; };
    
    prompt[0] = (char) '\0';
    prompt_pos    = 0;
    prompt_status = 0;
    prompt_max    = PROMPT_MAX_DEFAULT;  

// Escrevia no console.
    // Prompt
    //printf("\n");
    //printf("cmdline: Type something\n");
    //printf("$ ");
    //fflush(stdout);

// Cursor do terminal.
    cursor_x = 0;
    cursor_y++;

// draw prompt symbol.
    gws_draw_char ( 
        fd, 
        Terminal.client_window_id, 
        (cursor_x*8), 
        (cursor_y*8), 
        prompt_color, 
        '>' ); 

// Increment x.
    cursor_x++;

// Refresh client window.

    if(Terminal.client_window_id < 0)
        return;

    gws_refresh_window(fd,Terminal.client_window_id);
}



// interna
// Isso chama o aplicativo true.bin
// que se conecta a esse via tty e nos envia uma mensagem.
void test_tty_support(int fd)
{
    gws_debug_print("test_tty_support: [FIXME] undefined reference\n");
    return;
    
    /*
    char buffer[32];
    int nread = 0;



    gws_debug_print("test_tty_support:\n");

    int ____this_tty_id = (int) gramado_system_call ( 266, getpid(), 0, 0 );



   // lançando um processo filho.  
   gramado_system_call ( 900, 
       (unsigned long) "true.bin", 0, 0 );


    gws_yield();
    
    
    int i=0;
    while(1){

        nread = read_ttyList ( ____this_tty_id, buffer, 32 ); 
        
        if( nread>0){
            
            for(i=0;i<32;i++){
                if( buffer[i] != 0)
                    terminal_write_char(fd, buffer[i]);
            }
            return;
        }
        gws_yield();
        
        //i++;
        //if(i>20) i=0;
    }
    */
   
    /*
    int nwrite = -1; // bytes escritos.
    size_t __w_size2=0;
    while(1)
    {
        // Escrevendo na tty desse processo e na tty slave pra leitura.
        nwrite = write_ttyList ( ____this_tty_id, 
                     buffer, 
                     __w_size2 = sprintf (buffer,"THIS IS A MAGIC STRING\n")  );
    
        if (nwrite > 0)
           return 0;//goto __ok;
    }
    */
}


// f4
// Send the content of prompt[] to stdin.
// The child process will read this.
// This is why the stdin needs to be a tty.
// (Canonical) 
// If the child is a shell it will read a line.
// (Raw) 
// If the child is a text editor it will read a single char.
// Maybe the shell can change stdin for it's child.
// For now, the shell will need a line.

void __send_to_child (void)
{
    char *shared_flag   = (char *) (0xC0800000 -0x210);   // flag
    char *shared_memory = (char *) (0xC0800000 -0x200);   // input
    //char *shared_memory = (char *) (0xC0800000 -0x100); // output
   
   
    // There is a '\n' terminated line in prompt[].
    // #bugbug: Não podemos mandar uma linha sem '\n'.
    fseek(stdin, 0, SEEK_SET); 
    write ( fileno(stdin), prompt, 80);
    

    //copy to shared memory
    //send a system message.(maybe)
    //flag?


    int i=0;
    // Send the command line to the shared memory.
    for(i=0; i<80; i++){ shared_memory[i] = prompt[i]; }
    // Clear prompt.
    for(i=0; i<80; i++){ prompt[i]=0; }
    prompt_pos = 0; 
    
    // Notify the child that it has a message in the shared memory.
    shared_flag[0] = 1; 
}



// Write something in the standard stream and call shell.bin.
void test_standard_stream(int fd)
{
    char buffer[4096];
    int nread = 0;
    

    gws_debug_print("test_standard_stream:\n");  


    //FILE *f;
    //f = fopen("syscalls.txt", "r+"); 
    //f = fopen("gramado.txt", "r+"); 
    //f = fopen("kstderr.txt", "r+");
    //f = fopen("g.txt", "r+");


    // Testar   
    //gramado_system_call ( 900, 
        //(unsigned long) "tprintf.bin", 0, 0 );


   //gramado_system_call ( 900, 
     //  (unsigned long) "tprintf.bin", 0, 0 );

   //gramado_system_call ( 900, 
     //  (unsigned long) "tprintf.bin", 0, 0 );
     
    //fseek(f, 0, SEEK_END);   // seek to end of file
    //size_t size = ftell(f);  // get current file pointer
    //fseek(f, 0, SEEK_SET);   // seek back to beginning of file
    //printf (">>>> size %d \n",size);  


    fseek(stdin, 0, SEEK_SET); 
    fseek(stdout, 0, SEEK_SET); 
    fseek(stderr, 0, SEEK_SET); 


    input('\n');
    input('\0');
    //input(-1);

    //#bugbug
    //Não podemos escrever usando o tamanho do buffer
    //porque o arquivo é menor que isso.
    write(fileno(stdin), prompt, 20);//#bugbug sizeof(prompt));    
    write(fileno(stdout),prompt, 20);//#bugbug sizeof(prompt)); 
    write(fileno(stderr),prompt, 20);//#bugbug sizeof(prompt)); 

    //fseek(stdin, 0, SEEK_SET); 
    //fseek(stdout, 0, SEEK_SET); 
    //fseek(stderr, 0, SEEK_SET); 


    int ii=0;
    prompt_pos = 0;
    for(ii=0;ii<32;ii++) {prompt[ii]=0;}

    //gramado_system_call ( 900, 
      //(unsigned long) "tprintf.bin", 0, 0 );

    gramado_system_call ( 900, 
      (unsigned long) "shell.bin", 0, 0 );

    return;
    //while(1);
    

    /*

    int i=0;
    while(1){

        //nread = read ( fileno(f), buffer, sizeof(buffer) ); 
        nread = read ( fileno(f), buffer, size ); 
        //nread = read ( fileno(stdin), buffer, size ); 
        //nread = read ( fileno(stdout), buffer, size ); 

        if( nread>0){
            
            for(i=0;i< size ;i++){

                if( buffer[i] == 0){ printf("FIM0\n"); return; }
                
                //eof
                if( buffer[i] == EOF){ printf("FIM1\n"); return; }
                
                if( buffer[i] != 0){
                    //terminal_write_char(fd, buffer[i]);
                    tputc ((int) fd, window, (int) buffer[i], (int) 1); //com tratamento de escape sequence.
                }
            };
            printf("FIM2\n");
            return;
        }
    };
    */
}


void
test_child_message(void)
{
   // lançando um processo filho.  
   gramado_system_call ( 900, 
       (unsigned long) "sh1.bin", 0, 0 );

}

//
// =======================
//

//int prev;

void 
terminal_write_char (
    int fd, 
    int window, 
    int c )
{
    static char prev=0;
    unsigned long x = (cursor_x*8);
    unsigned long y = (cursor_y*8);


    if (fd<0)
        return;

    if (window<0)
        return;

    if (c<0)
        return;

// #todo
// Ver no kernel esse tipo de rotina
// tab;
    

    if ( c == '\r' )
    {
        cursor_x=0;
        prev = c;
        return;
    }

    //if ( c == '\n' && prev == '\r' ) 
    if ( c == '\n')
    {
         //printf("NEWLINE\n");
         cursor_x=0; // começo da linha ...(desnecessário)
         cursor_y++;  //linha de baixo
         //começo da linha
         prev = c; 
         return;
    }

// Draw!
// Draw the char into the given window.
// Vamos pintar o char na janela usando o window server.
// White on black
// IN: fd, wid, l, t, color, ch.

    gws_draw_char (
        (int) fd,
        (int) window,
        (unsigned long) (x & 0xFFFF),
        (unsigned long) (y & 0xFFFF),
        (unsigned long) fg_color,
        (unsigned long) c );

// Coloca no buffer de linhas e colunas.
    terminalInsertNextChar ( (char) c ); 

// Circula
    cursor_x++;
    if (cursor_x>__wlMaxColumns)
    {
        cursor_y++;  //próxima linha.
        cursor_x=0;  //começo da linha
    }
}



/*
 ***************************************************
 * terminalInsertNextChar:
 *     Coloca um char na próxima posição do buffer.
 *     Memória de vídeo virtual, semelhante a vga.
 *     #todo: Esse buffer poderia ser um arquivo que o kernel
 * pudesse usar, ou o servidor de recursos gráficos pudesse usar.
 */

	//#importante:
	//o refresh é chamado no default do procedimento de janela

void terminalInsertNextChar (char c)
{

	// #todo
	// para alguns caracteres temos que efetuar o flush.
	// \n \r ... ??
			
	// Coloca no buffer.

    LINES[cursor_y].CHARS[cursor_x] = (char) c;
}


// # terminal stuff
void terminalInsertNullTerminator (void)
{
    terminalInsertNextChar ( (char) '\0' );
}


// # terminal stuff
void terminalInsertLF (void)
{
    terminalInsertNextChar ( (char) '\n' );
}

// # terminal stuff
void terminalInsertCR (void)
{
    terminalInsertNextChar ( (char) '\r' );
}


// # terminal stuff
//line feed
void lf (void)
{
    // Enquanto for menor que o limite de linhas, avança.

    if ( cursor_y+1 < __wlMaxRows )
    {
        cursor_y++; 
        return;
    }

	//#todo: Scroll up;
	//scrup();
}




// # terminal stuff
//carriege return
void cr (void)
{
    cursor_x = 0;
}


// # terminal stuff
// ??
//voltando uma linha.
void ri (void)
{
	//if ( screen_buffer_y > top ){
		
		// Volta uma linha.
	//	screen_buffer_y--;
	//	screen_buffer_pos = (screen_buffer_pos - columns); 
	//	return;
	//}
	
	//@todo:
	//scrdown();
}


// # terminal stuff
void del (void)
{
    LINES[cursor_y].CHARS[cursor_x] = (char) '\0';
    LINES[cursor_y].ATTRIBUTES[cursor_x] = 7;
}



/*
 ***************** 
 * tputc:
 * 
 */

// #todo
// See: https://github.com/gramado/st/blob/tlvince/st.c

// #Atenção: A libc do app foi configurada dinamicamente
// para que printf coloque chars no arquivo. Mas 
// a libc desse terminal ainda não foi. Então a printf
// desse terminal escreve no backbuffer e exibe na tela.
// #bugbug: 
// O problema é que dessa forma nada foi colocado no buffer de arquivo.
//#todo
// fazer essa função colocar os chars no buffer de arquivo. 
// Usaremos no scroll.
// void tputc (int fd, char *c, int len){

void 
tputc ( 
    int fd, 
    int window,
    int c, 
    int len )
{

    unsigned char ascii = (unsigned char) c;
    //unsigned char ascii = *c;


    // ?? #bugbug
    // Control codes
    int control = (ascii < '\x20' || ascii == 0177);
    //bool control = ascii < '\x20' || ascii == 0177;

 
    /*
    if(fd<0){
        printf("tputc: fd\n"); //debug
        return;
    }
    */
     
    //??
    //if(iofd != -1) {}

 
    //
    // #importante
    // Se não é controle é string ou escape sequence.
    //
 
    //string normal
    //if(term.esc & ESC_STR) 
    if (__sequence_status == 0)
    {
        switch (ascii)
        {
            // [Esc]
            // Deixou de ser string normal e entramos em uma sequência.
            // Logo abaixo esse char será tratado novamente.
            case '\033':
                term.esc = ESC_START;
                __sequence_status = 1;
                break;

             // #importante
             // Imprimindo caracteres normais.
             // #todo: talvez possamos usar a API para isso.
             // como acontece nos caracteres digitados no shell interno.
             // #importante
             // Isso vai exibir o caractere mas também
             // na colocar ele no buffer ba posição atual.
             default:
                 terminal_write_char ( fd, window, (int) ascii); 
                 //printf ("%c",ascii);  //debug
                 return;
         };
    }


    // Control codes. 
    // (dentro de um range)

    if (control){
 
        switch(ascii)
        {
            //case '\v':    /* VT */
            //case '\a':    /* BEL */    
            case '\t':      /* HT */
            case '\b':      /* BS */
            case '\r':      /* CR */
            case '\f':      /* LF */
            case '\n':      /* LF */
                terminal_write_char (fd, window, (int) ascii);
                //printf ("%c",ascii); //debug
                return;
                break;

            //^[   (Esc)
            //case '\e':
            //case '\033':
            case '\x1b':
                term.esc = ESC_START;
                __sequence_status = 1;
                terminal_write_char ( fd, window, (int) '$');  //debug
                //printf (" {ESCAPE} ");  //debug
                return;
                break;


            case '\016':    /* SO */
            case '\017':    /* SI */
                return;
                break;


            case '\032':    /* SUB */
            case '\030':    /* CAN */
                //csireset ();
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {reset?} "); //debug
                return;
                break;


            case '\005':    /* ENQ (IGNORED) */
            case '\000':    /* NUL (IGNORED) */
            case '\021':    /* XON (IGNORED) */
            case '\023':    /* XOFF (IGNORED) */
            //case 0177:    /* DEL (IGNORED) */
                //Nothing;
                return;

            // ...
        };

        // ... 

    // Um 1b já foi encontrado.
    } else if (term.esc & ESC_START) {

        // Um [ já foi encontrado.
        // #todo parse csi
        if(term.esc & ESC_CSI){

            switch(ascii)
            {
                // Quando acaba a sequência.
                case 'm':
                    term.esc = 0;
                    __sequence_status = 0;
                    terminal_write_char (fd, window, (int) '$'); //debug
                    //printf (" {m} "); //debug
                    return;
                    break;  

                 // ??
                 //case ';':
                     //return;
                     //break;


                // Vamos apenas colocar no buffer
                // para analizarmos depois.
                // Colocamos no tail e retiramos no head.
                default:
                    terminal_write_char ( fd, window, (int) '$'); //debug
                    //printf (" {.} "); //debug
                    CSI_BUFFER[__csi_buffer_tail] = ascii;
                    __csi_buffer_tail++;
                    if ( __csi_buffer_tail >= CSI_BUFFER_SIZE )
                    {
                        __csi_buffer_tail = 0;
                    }
                    return;
                    break;
            };


        } else if (term.esc & ESC_STR_END){ 
 
            // ...

        } else if (term.esc & ESC_ALTCHARSET){

            switch(ascii)
            {
                case 'A':  /* UK (IGNORED) */
                case '<':  /* multinational charset (IGNORED) */
                case '5':  /* Finnish (IGNORED) */
                case 'C':  /* Finnish (IGNORED) */
                case 'K':  /* German (IGNORED) */
                    break;
            };


        } else if (term.esc & ESC_TEST) {

            // ...
 
        }else{

            switch (ascii){

            case '[':
                term.esc |= ESC_CSI;
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {CSI} "); //debug
                return;
                break; 
   
            case '#':
                 term.esc |= ESC_TEST;
                 break;

            case 'P':  /* DCS -- Device Control String */
            case '_':  /* APC -- Application Program Command */
            case '^':  /* PM -- Privacy Message */
            case ']':  /* OSC -- Operating System Command */
            case 'k':  /* old title set compatibility */
                term.esc |= ESC_STR;
                break; 

            /* Set primary charset G0 */ 
            case '(': 
                term.esc |= ESC_ALTCHARSET;
                break;    

            case ')':  /* set secondary charset G1 (IGNORED) */
            case '*':  /* set tertiary charset G2 (IGNORED) */
            case '+':  /* set quaternary charset G3 (IGNORED) */
                term.esc = 0;
                __sequence_status = 0;
                break;  


            /* IND -- Linefeed */
            case 'D': 
                term.esc = 0;
                terminal_write_char ( fd, window, (int) '$');  //debug
                //printf (" {IND} ");  //debug
                break;

            /* NEL -- Next line */ 
            case 'E': 
                term.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {NEL} "); //debug
                break;


            /* HTS -- Horizontal tab stop */
            case 'H':   
                term.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {HTS} "); //debug
                break;


            /* RI -- Reverse index */
            case 'M':     
                term.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {RI} "); //debug
                break;

            /* DECID -- Identify Terminal */
            case 'Z':  
                 term.esc = 0;
                 terminal_write_char (fd, window, (int) '$'); //debug
                 //printf (" {DECID} "); //debug
                 break;


            /* RIS -- Reset to inital state */
            case 'c': 
                 term.esc = 0;
                 terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {reset?} "); //debug
                 break; 

            /* DECPAM -- Application keypad */
            case '=': 
                 term.esc = 0;
                 terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {=} "); //debug
                 break;

            /* DECPNM -- Normal keypad */
            case '>': 
                term.esc = 0;
                terminal_write_char (fd, window, (int) '$'); //debug
                //printf (" {>} "); //debug
                break;


            /* DECSC -- Save Cursor */ 
            //case '7':     
               //  term.esc = 0;
               //  break;

            /* DECRC -- Restore Cursor */ 
            //case '8': 
               //  term.esc = 0;
               //  break;

            /* ST -- Stop */
            //0x9C ST String Terminator ???
            //case '\\':   
                 //term.esc = 0;
                 //break;
  
            //erro    
            //default:
                //break; 
            };
        };
        
        // ...

        return;
    };
 
    // ...
}





// # terminal stuff
// Insere um caractere sentro do buffer.

char 
terminalGetCharXY ( 
    unsigned long x, 
    unsigned long y )
{

    if ( x >= __wlMaxColumns || y >= __wlMaxRows )
    {
        return;
    }

    return (char) LINES[y].CHARS[x];
}


// # terminal stuff
// Insere um caractere dentro do buffer.

void 
terminalInsertCharXY ( 
    unsigned long x, 
    unsigned long y, 
    char c )
{

    if ( x >= __wlMaxColumns || y >= __wlMaxRows )
    {
        return;
    }

    LINES[y].CHARS[x] = (char) c;
    LINES[y].ATTRIBUTES[x] = 7;
}


// # terminal stuff
static void save_cur (void)
{
    textSavedCol = cursor_x;
    textSavedRow = cursor_y;
}


// # terminal stuff
static void restore_cur (void)
{
    cursor_x = textSavedCol;
    cursor_y = textSavedRow;
}


/*
 ****************************************
 * shellClearBuffer:
 *     Limpa o buffer da tela.
 *     Inicializamos com espaços.
 */

void terminalClearBuffer (void){

    int i=0;
    int j=0;

    for ( i=0; i<32; i++ )
    {
        for ( j=0; j<80; j++ )
        {
		    LINES[i].CHARS[j] = (char) ' ';
		    LINES[i].ATTRIBUTES[j] = (char) 7;
        };
		
		LINES[i].left = 0;
		LINES[i].right = 0;
		LINES[i].pos = 0;
    };
}



//Qual será a linha que estará no topo da janela.
void textSetTopRow ( int number )
{
    textTopRow = (int) number; 
}


int textGetTopRow (void)
{
    return (int) textTopRow;
}


//Qual será a linha que estará na parte de baixo da janela.
void textSetBottomRow ( int number )
{
    textBottomRow = (int) number; 
}


int textGetBottomRow (void)
{
    return (int) textBottomRow; 
}

void textSetCurrentRow ( int number )
{
    cursor_y = (int) number; 
}


int textGetCurrentRow (void)
{
    return (int) cursor_y;
}



void textSetCurrentCol ( int number )
{
    cursor_x = (int) number; 
}


int textGetCurrentCol (void)
{
    return (int) cursor_x; 
}



/*
 * move_to:
 *    Move o cursor de posição.
 *    Assim o próximo char será em outro lugar da janela.
 */

void move_to ( unsigned long x, unsigned long y )
{

    if ( x > __wlMaxColumns || y > __wlMaxRows )
        return;

	//screen_buffer_x = x;
	//screen_buffer_y = y;
	
    cursor_x = x;
    cursor_y = y;

	//screen_buffer_pos = ( screen_buffer_y * __wlMaxColumns + screen_buffer_x ) ;
}


/* credits: bsd*/
/* Pad STRING to COUNT characters by inserting blanks. */

int pad_to (int count, char *string){

    register int i=0;

    i = strlen(string);

    if (i >= count){
        string[i++] = ' ';
    }else{

        while (i < count)
            string[i++] = ' ';
    };
    string[i] = '\0';

    return (i);
}



//char *hello = "Hello there!\n";
/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/



//interna
int __terminal_clone_and_execute ( char *name )
{
    return (int) gramado_system_call ( 900, (unsigned long) name, 0, 0 );
}



void _draw(int fd, int c)
{

   //unsigned long x;
   //x=0x65666768; //last
   

    //printf ("%c",c);
    //fflush(stdout);
    //return;
   
   
                  /*
                    terminal_drawchar_request (
                        (int) fd,//fd,
                        (int) 0, //__response_wid, //window_id,
                        (unsigned long) __tmp_x,//left,
                        (unsigned long) __tmp_y,//top,
                        (unsigned long) COLOR_RED,
                        (unsigned long) x ); 
                        */
                    
                  gws_draw_char (
                      (int) fd,             // fd,
                      (int) 0,              // window id,
                      (unsigned long) __tmp_x,    // left,
                      (unsigned long) __tmp_y,    // top,
                      (unsigned long) fg_color,
                      (unsigned long) c );
      
                    
                        
                 __tmp_x = __tmp_x + 8;
                 
                 //if ( __tmp_x > (8*80) )
                 //{
                 //    __tmp_y = __tmp_y + 8;
                 //    __tmp_x = 0;
                 //}
                 
                //terminal_drawchar_response((int) fd);
}


// local
int 
terminalProcedure ( 
    int fd,
    int window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )

{

    if (fd<0)
        return -1;

    if (window<0)
        return -1;

    if (msg<0)
        return -1;

// ==================

    switch(msg)
    {
        case MSG_KEYDOWN:
            switch(long1)
            {
                case VK_RETURN:
                    //printf("RETURN \n");
                    //doPrompt(fd);
                    input('\0');
                    compareStrings(fd);
                    return 0;
                    break;

                // draw the char using the window server
                // Criar uma função 'terminal_draw_char()'
                default:
                    input(long1);
                    // draw char
                    gws_draw_char ( 
                        fd, 
                        window, 
                        (cursor_x*8), 
                        (cursor_y*8), 
                        fg_color, 
                        long1 );

                    // refresh window
                    gws_refresh_window(fd,window);

                    // update cursor positions
                    cursor_x++;
                    if( cursor_x > 20){cursor_x=0;}

                    return 0;

                    break;
            };
            break;

        default:
            break;
    };

// done
    return 0;
}


//
// Main
//

int main ( int argc, char *argv[] )
{
    int client_fd = -1;

    //porta para o Window Server 'ws' em gramado_ports[]
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    
    // Connecting to the window server in this machine.
    addr_in.sin_port   = PORTS_WS;   
    addr_in.sin_addr.s_addr = IP(127,0,0,1); 


    debug_print ("--------------------------\n");
    debug_print ("terminal: Initializing ...\n");

// Device info
// #todo: Check for 'zero'.

    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);

// Cursor
    cursor_x = 0;
    cursor_y = 0;

// Process info
    Terminal.pid = getpid();
    Terminal.uid = getuid();
    Terminal.gid = getgid();

    //setreuid(-1, -1);
    //setpgrp(0, getpid());

    __sequence_status = 0;

//
// socket
// 

    // #debug
    printf ("terminal: Creating socket\n");

    client_fd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( client_fd < 0 ){
       debug_print ("terminal: Couldn't create socket\n");
       printf      ("terminal: Couldn't create socket\n");
       exit(1);
    }

// Saving the fd in the main struct.
    Terminal.client_fd = client_fd;
    //...


//
// connect
// 

// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.

    printf ("terminal: Connecting to ws via inet ...\n");

    while (1){
        if (connect (client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            debug_print ("terminal: Connection Failed \n"); 
            printf      ("terminal: Connection Failed \n"); 
        }else{ break; };
    };


// Windows:

    int main_window = 0;
    int terminal_window = 0;


//
// main window
//

    unsigned long mwWidth  = (w >> 1);
    unsigned long mwHeight = (h >> 1);
    unsigned long mwLeft   = ( ( w - mwWidth ) >> 1 );
    unsigned long mwTop    = ( ( h - mwHeight) >> 1 ); 
    unsigned int mwColor   = COLOR_WINDOW; //COLOR_RED;

//
// Client area window
//

    unsigned long wLeft   = 2;   // por causa da borda.
    unsigned long wTop    = 34;  // por causa da title bar
    unsigned long wWidth  =  mwWidth  -2 -2;
    unsigned long wHeight =  mwHeight -2 -34;
    unsigned int wColor   = bg_color; //COLOR_BLACK;


// The surface of this thread.
    setup_surface_retangle ( 
        (unsigned long) mwLeft, 
        (unsigned long) mwTop, 
        (unsigned long) mwWidth, 
        (unsigned long) mwHeight );


// ===================================================

//
// main window
//

// style: 
// 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen

    main_window = gws_create_window (
                      client_fd,
                      WT_OVERLAPPED, 1, 1, "Terminal",
                      mwLeft, mwTop, mwWidth, mwHeight,
                      0,
                      0x0000,
                      mwColor, 
                      mwColor );

    Terminal.main_window_id = main_window;

// ===================================================

//
// Client area window
//

    terminal_window = gws_create_window (client_fd,
                          WT_SIMPLE, 1, 1, "ter-client",
                          wLeft, wTop, wWidth, wHeight,
                          main_window,0,wColor,wColor);
      
    // Saving the window id.
    Terminal.client_window_id = terminal_window;
    
    if (Terminal.client_window_id<0){
        gws_debug_print ("terminal: [FAIL] create main window return fail\n");
    }


// Set window with focus
    //gws_async_command(client_fd,9,0,terminal_window);


// Invalidate surface.
    invalidate_surface_retangle();

//
// Test 3
//

    /*
    __tmp_x = 40;
    __tmp_y = 40;

    // Testing draw a char in a window.
    terminal_drawchar_request (
        (int) client_fd,          //fd,
        (int) __response_wid,     //window_id,
        (unsigned long) __tmp_x,  //left,
        (unsigned long) __tmp_y,  //top,
        (unsigned long) COLOR_RED,
        (unsigned long) 'X' );

    terminal_drawchar_response((int) client_fd);
    */
    
   
    //#debug
    //hanging
    //while(1){}


// Initialize globals.
// #importante: 
// Isso será definido somente uma vez.

    __wlMaxColumns = DEFAULT_MAX_COLUMNS;
    __wlMaxRows    = DEFAULT_MAX_ROWS;


// Initializations
    terminalTerminal();


// Inicializando prompt[].
    input('\n');
    input('\0');

//
// Client
//

// #todo
// Vamos fazer isso outra hora.
// por hora vamos apenas usar o terminal,
// com o input no terminal
// Write something in the standard stream and call shell.bin.

    // test_standard_stream(client_fd);


// ============================================
// focus
// #bugbug
// It needs to be an 'editbox' for typing messages.

/*
    gws_async_command(
         client_fd,
         9,             // set focus
         terminal_window,
         terminal_window );
*/

    //rtl_focus_on_this_thread();



/*
//================
//cls
     gws_redraw_window(client_fd,Terminal.client_window_id,TRUE);
     //#define SYSTEMCALL_SETCURSOR  34
     gramado_system_call ( 34, 2, 2, 0 );
//=================
*/

    clear_terminal_client_window(client_fd);
    doPrompt(client_fd);

//
// Loop!
//

    int C=0;
    FILE *new_stdin;
    new_stdin = (FILE *) fopen("gramado.txt","a+");

// O kernel seleciona qual será 
// o arquivo para teclado ps2.

    gramado_system_call(
        8002,
        fileno(new_stdin),
        0,
        0 );

    rewind(new_stdin);

    while (1){
        C = fgetc(new_stdin);
        if (C > 0)
        {
            terminalProcedure( 
                client_fd,            // socket
                Terminal.client_window_id,   // window ID
                MSG_KEYDOWN,          // message code
                C,                    // long1 (ascii)
                C );                  // long2 (ascii)
        }
    };

//exit:
    debug_print ("terminal: bye\n"); 
    printf      ("terminal: bye\n");
    return 0;
}



/*
 ******************************************
 * terminalTerminal:
 *     Constructor.
 *     Não emite mensagens.
 */

// #bugbug
// essas configurações são configurações de janela,
// então estão mais para terminal do que para shell.


void terminalTerminal (void)
{
    int i=0;
    int j=0;

// Internas.

    //shellStatus = 0;
    //shellError = 0;


    cursor_x=0;
    cursor_y=0;
    bg_color=COLOR_BLACK;
    fg_color=COLOR_WHITE;
    prompt_color = COLOR_GREEN;

//
// # Inicializando as estruturas de linha ##
//
	
	//inicializamos com espaços.
	for ( i=0; i<32; i++ )
	{
		for ( j=0; j<80; j++ )
		{
		    LINES[i].CHARS[j] = (char) ' ';
		    LINES[i].ATTRIBUTES[j] = (char) 7;
	    }
		
		LINES[i].left = 0;
		LINES[i].right = 0;
		LINES[i].pos = 0;
	};
	
	
	
	// Deve ser pequena, clara e centralizada.
	// Para ficar mais rápido.
	
	// #importante:
	// O aplicativo tem que confiar nas informações 
	// retornadas pelo sistema.
		
	// Usar o get system metrics para pegar o 
	// tamanho da tela.
	
//inicializa as metricas do sistema.
    terminalInitSystemMetrics();

//inicializa os limites da janela.
    terminalInitWindowLimits();

//inicia o tamanho da janela.
    terminalInitWindowSizes();

//inicializar a posição da janela.
    terminalInitWindowPosition();
 
// initialize visible area.
// #todo: criar função para isso
// É melhor que seja pequena por enquanto pra não ativar
// o scroll do kernel e só usar o scroll desse terminal.

	//textTopRow = 0;
	//textBottomRow = 24;
    //terminalNewVisibleArea ( 0, 19);

	//...	

	// Obs:
	// prompt[] - Aqui ficam as digitações. 

	//terminalClearBuffer ();

	
	//shellBufferMaxColumns = DEFAULT_BUFFER_MAX_COLUMNS;
	//shellBufferMaxRows    = DEFAULT_BUFFER_MAX_ROWS;
	
	//buffersize = (shellBufferMaxColumns * shellBufferMaxRows);
	

	
	//
	// @todo: E o fluxo padrão. Quem configurou os arquivos ???
	//        o kernel configuroru???
	//
	
    //...
	
	
	//for ( i=0; i<WORKINGDIRECTORY_STRING_MAX; i++ ){
	//	current_workingdiretory_string[i] = (char) '\0';
	//};
	
    //sprintf ( current_workingdiretory_string, 
	//    SHELL_UNKNOWNWORKINGDIRECTORY_STRING );    
	
	//...
	
//done:

    //ShellFlag = SHELLFLAG_COMMANDLINE;
	
    //#bugbug
	//Nossa referência é a moldura e não a área de cliente.
	//@todo:usar a área de cliente como referência
	//terminalSetCursor(0,0);
    //terminalSetCursor(0,4);
    
	//@todo
	//tentando posicionar o cursor dentro da janela
	//terminalSetCursor( (shell_info.main_window->left/8) , (shell_info.main_window->top/8));	

	//shellPrompt();
}


void terminalInitSystemMetrics(void)
{
	//Tamanho da tela. (full screen)
	smScreenWidth = gws_get_system_metrics(1);
	smScreenHeight = gws_get_system_metrics(2); 
	
	//cursor
	smCursorWidth = gws_get_system_metrics(3);
	smCursorHeight = gws_get_system_metrics(4);
	
	//mouse
	smMousePointerWidth = gws_get_system_metrics(5);
	smMousePointerHeight = gws_get_system_metrics(6);
	
	//char
	smCharWidth = gws_get_system_metrics(7);
	smCharHeight = gws_get_system_metrics(8);
	
	
	//#todo:
	//vertical scroll size
	//horizontal scroll size.
	
	//#importante
	//#todo: pegar mais.
	
	//...
	
	//#todo: Temos que criar essa variável.
	//InitSystemMetricsStatus = 1;
} 


void terminalInitWindowLimits (void){
	
	// #todo
	// Tem variáveis aqui que não podem ser '0'.
	
	//#todo: temos que criar essa variável.
	/*
	if (InitSystemMetricsStatus == 0)
	{
	    terminalInitSystemMetrics ();
	}
	*/
	
    //
    // ## Window limits ##
    //
	
	// problemas; 
	//if ( smScreenWidth == 0 || smScreenHeight )
	//{
	//	 printf ...
	//}

    //full screen support
    wlFullScreenLeft = 0;
    wlFullScreenTop  = 0;
    wlFullScreenWidth  = smScreenWidth;
    wlFullScreenHeight = smScreenHeight;
	
    //limite de tamanho da janela.
    wlMinWindowWidth  = (smCharWidth * 80);
    wlMinWindowHeight = (smCharWidth * 25);
    wlMaxWindowWidth  = wlFullScreenWidth;
    wlMaxWindowHeight = wlFullScreenHeight;


    //quantidade de linhas e colunas na área de cliente.
    wlMinColumns = 80;
    wlMinRows = 1;


	
	//dado em quantidade de linhas.
    textMinWheelDelta = 1;  //mínimo que se pode rolar o texto
    textMaxWheelDelta = 4;  //máximo que se pode rolar o texto	
    textWheelDelta = textMinWheelDelta;
	//...
}


void terminalInitWindowSizes(void)
{

//
//  ## Window size ##
//

    //wsWindowWidth = wlMinWindowWidth;
    //wsWindowHeight = wlMinWindowHeight;	
	
	//Tamanho da janela do shell com base nos limites 
    //que ja foram configurados.	
	
	wsWindowWidth =  WINDOW_WIDTH;
	wsWindowHeight = WINDOW_HEIGHT;
	
	
	if ( wsWindowWidth < wlMinWindowWidth ){
		wsWindowWidth = wlMinWindowWidth;
	}
	
	if ( wsWindowHeight < wlMinWindowHeight ){
	    wsWindowHeight = wlMinWindowHeight;
	}
}


void terminalInitWindowPosition(void)
{
	
	//window position
	wpWindowLeft = WINDOW_LEFT;
	wpWindowTop  = WINDOW_TOP;
	
	//wpWindowLeft = (unsigned long) ( (smScreenWidth - wsWindowWidth)/2 );
	//wpWindowTop = (unsigned long) ( (smScreenHeight - wsWindowHeight)/2 );  	
}


//
// End.
//





