

#ifndef  __WINDOW_H
#define  __WINDOW_H    1


#define KGWS_ZORDER_BOTTOM  0 
#define KGWS_ZORDER_TOP     1023   //top window
#define KGWS_ZORDER_MAX     1024   //max


// #importante:
// Tipos de mensagem de comunicação nos diálogos 
// e procesimento de janelas:
// 
// SIGNAL_       Sinal. Não contém argumentos.
// MSG_          Message. Contém os argumentos padrão.
// STREAMMSG_    Streams. O argumento é um ponteiro para uma stream.
// BUFFER_MSG_   Buffer. O argumento é um ponteiro para um buffer.
// CAT_MSG_      Concatenate. Os argumentos long1 e long devem ser concatenados.
 

// Window handle status
// Se uma janela está aberta ou não. 
#define HANDLE_STATUS_CLOSE 0
#define HANDLE_STATUS_OPEN 1
 
 
//used
#define WINDOW_NOTUSED   0
#define WINDOW_USED      1
#define WINDOW_GC        216   //Sinalizada para o GC.
//...

//magic
#define WINDOW_NOTMAGIC  0
#define WINDOW_MAGIC     1234 
#define WINDOW_CLOSED    4321
//... 


//desktop window. (Área de trabalho) 
//#define MAINWINDOW_DEFAULTX  ?
//#define MAINWINDOW_DEFAULTY  ?

 
// Número máximo de janelas.
//@todo: Aumentar esse tamanho.


// # window lock support #
#define WINDOW_LOCKED    1
#define WINDOW_UNLOCKED  0


/*
 ***********************************************
 * Messages.
 * mensagens para procedimentos de janelas e 
 * para diálogos dentro do gws.
 * Obs: Isso refere-se principalmente à janelas.
 */

 //??tipos de mensagens ??
#define MSG_NULL          0 
#define SIGNAL_NULL       0 
#define STREAM_MSG_NULL   0 
#define BUFFER_MSG_NULL   0 
#define CAT_MSG_NULL      0  

 
//window (1-19)  
#define MSG_CREATE        1
#define MSG_DESTROY       2
#define MSG_MOVE          3 
#define MSG_SIZE          4
#define MSG_RESIZE        5
//#define MSG_OPEN        6
#define MSG_CLOSE         7
#define MSG_PAINT         8
#define MSG_SETFOCUS      9
#define MSG_KILLFOCUS     10
#define MSG_ACTIVATE      11
#define MSG_SHOWWINDOW    12 
#define MSG_SETCURSOR     13
#define MSG_HIDE          14  // ?? (MIN)
//#define MSG_MINIMIZE    14  // ?? (MIN)
#define MSG_MAXIMIZE      15
#define MSG_RESTORE       16
#define MSG_SHOWDEFAULT   17


//keyboard (20-29)
#define MSG_KEYDOWN       20
#define MSG_KEYUP         21
#define MSG_SYSKEYDOWN    22
#define MSG_SYSKEYUP      23

//mouse (30 - 39)
//tem uma lista de eventos de mouse em events.h
#define MSG_MOUSEKEYDOWN  30
#define MSG_MOUSEKEYUP    31
#define MSG_MOUSEBUTTONDOWN	30
#define MSG_MOUSEBUTTONUP	31 
#define MSG_MOUSEMOVE	    32
#define MSG_MOUSEOVER	    33
#define MSG_MOUSEWHEEL	    34
#define MSG_MOUSEPRESSED	  35
#define MSG_MOUSERELEASED	  36
#define MSG_MOUSECLICKED	  37
#define MSG_MOUSEENTERED	  38    //?? capturou ??
#define MSG_MOUSEEXITED	       39   //?? descapturou ??
//#define MSG_MOUSEMOVEBYOFFSET
//#define MSG_MOUSEMOVETOELEMENT


//outros (40 - ...)
#define MSG_COMMAND       40
#define MSG_CUT           41
#define MSG_COPY          42
#define MSG_PASTE         43
#define MSG_CLEAR         44 
#define MSG_UNDO          45
#define MSG_INSERT        46
#define MSG_RUN_PROCESS   47
#define MSG_RUN_THREAD    48

//Quando um comando é enviado para o terminal. ele será atendido pelo
//módulo /sm no procedimento de janela do sistema.
//Todas as mensagens de terminal serão atencidas pelo procedimento de janela 
//nessa mensagem.
//#bugbug: temos outro grupo de mensagems abordadndo esse tema logo abaixo.

#define MSG_TERMINAL_COMMAND 49
#define MSG_TERMINAL_SHUTDOWN 50
#define MSG_TERMINAL_REBOOT   51

#define MSG_DEVELOPER 52


//UM TIMER SE ESGOTOU,
#define MSG_TIMER 53   
//...


//o servidor de rede se comunica com o processo.
#define MSG_AF_INET 54
#define MSG_NET_DATA_IN 55

//o driver de network está notificando um processo em ring3.
#define MSG_NETWORK_NOTIFY_PROCESS 56


//
// mouse support: continuação ...
//
#define MSG_MOUSE_DOUBLECLICKED   60
#define MSG_MOUSE_DRAG            61
#define MSG_MOUSE_DROP            62
//...

//
//  terminal commands
//

#define MSG_TERMINALCOMMAND      100
#define TERMINALCOMMAND_PRINTCHAR 1000
//#define TERMINALCOMMAND_PRINT??? 1001
//...


// o evento de rolagem aconteceu ...
// O número do evento será entregue em long1.
#define MSG_HSCROLL 2000
#define MSG_VSCROLL 2001



// 
// ==== Window Type ====
//

#define WT_NULL          0 
#define WT_SIMPLE        1
// Igual simples, mais uma bordinha preta.
#define WT_EDITBOX       2
// Sobreposta (completa)(barra de titulo + borda +client area). 
#define WT_OVERLAPPED    3  
// Um tipo especial de sobreposta, usada em dialog ou message box. 
// (com ou sem barra de titulo ou borda)
#define WT_POPUP         4
// Caixa de seleção. Caixa de verificação. 
// Quadradinho.  
#define WT_CHECKBOX      5
// Cria uma scroll bar. 
// Para ser usada como janela filha.  
#define WT_SCROLLBAR     6  
#define WT_EDITBOX_MULTIPLE_LINES 7
#define WT_BUTTON        8   
#define WT_STATUSBAR     9
// Pequeno retângulo com uma imagem bmp e talvez texto.
#define WT_ICON          10
#define WT_TITLEBAR 11
//... 



// window style
#define WINDOW_STYLE_FLOATING 1000 
#define WINDOW_STYLE_DOCKING  2000  //(atracada em algum canto.)
//...


// window status
#define WINDOW_STATUS_ACTIVE       1
#define WINDOW_STATUS_INACTIVE     0
//...

//window relationship status. (seu status em relação as outras janelas.)
//Obs: tem uma estreita ligação com o status da thread que está trabalahndo com ela 
//e com a prioridade dessa thread e do processo que a possui.
// *** RELAÇÃO IMPLICA PRIORIDADE ***
#define WINDOW_REALATIONSHIPSTATUS_FOREGROUND     1000
#define WINDOW_REALATIONSHIPSTATUS_BACKGROUND     2000
#define WINDOW_REALATIONSHIPSTATUS_OWNED          3000  //Possuida por outra janela.
#define WINDOW_REALATIONSHIPSTATUS_ZAXIS_TOP      4000
#define WINDOW_REALATIONSHIPSTATUS_ZAXIS_BOTTOM   6000
//...




// Apresentação.
#define VIEW_NULL       0
#define VIEW_FULL       1
#define VIEW_MAXIMIZED  2  
#define VIEW_MINIMIZED  4
#define VIEW_NORMAL     8  // Normal (restaurada)
// ...


//
// ## botoes  ##
//

//button state
#define BS_NULL 0 
#define BS_DEFAULT 1
#define BS_FOCUS   2
#define BS_PRESS   3
#define BS_HOVER   4
#define BS_DISABLED 5
#define BS_PROGRESS 6
//...


    //button states:
    //0. NULL.
	//1. Default 
    //2. Focus
    //3. Expanded/Toggled/Selected
    //4. Disabled
    //5. Hover and Active	

//#define BN_CLICKED  200
//#define BN_DOWN     1
//#define BN_UP       2
//#define BN_SELECTED 3



 
//@todo: what ??? 
//?? um handle para o desktop. 
#define HWND_DESKTOP 0


 
 
/*
 * Dimensões: 
 * 
 * Parametro principal para dimensões de janela.
 * todos os outros tomam esse como refêrencia.
 * depende do modo que estiver usando.
 *
 * vesa 112:
 *
 * 640x480x24bit - (3 bytes por pixel)
 * 
 * @todo, o kernel usará dimensões 640x480 no modo gráfico.
 */   
#define KERNEL_COL_MAX 640 
#define KERNEL_LIN_MAX 480 

#define	BAR_STEPS   46
#define LINE KERNEL_COL_MAX 

//dimensões - provisorio
#define COL_MAX   KERNEL_COL_MAX 
#define LINHA_MAX KERNEL_LIN_MAX  






/*
 * Constants para a task bar.
 */
 
//#define TASKBUTTONS_BASE 40
//#define CLOCK_BASE  (KERNEL_COL_MAX-40)


//=========================================
//    ****    KERNEL WINDOW    ****
//Definições padronizadas para janelas do kernel usadas para 
//fornecer informações sobre o sistema.
// (Retângulo grande no topo da tela.)
// #bugbug
// Isso não é uma coisa boa.

#define KERNEL_WINDOW_DEFAULT_LEFT 0
#define KERNEL_WINDOW_DEFAULT_TOP  0
#define KERNEL_WINDOW_DEFAULT_WIDTH  800
#define KERNEL_WINDOW_DEFAULT_HEIGHT (600/4) 
#define KERNEL_WINDOW_DEFAULT_CLIENTCOLOR  xCOLOR_GRAY2
#define KERNEL_WINDOW_DEFAULT_BGCOLOR      xCOLOR_GRAY1
//...



//
// ESSA VARIÁVEL BLOQUEIA O FOCO NA JANELA DO DESENVOLVEDOR 
//
int _lockfocus;


// ESSA SERÁ USADA DEPOIS QUANDO A INTERFACE GRÁFICA ESTIVER MAIS ROBUSTA;
int gFocusBlocked;   



// #todo: deletar.
//unsigned long g_mainwindow_width;
//unsigned long g_mainwindow_height;
//unsigned long g_navigationbar_width;
//unsigned long g_navigationbar_height;





/*
 **************************************************
 * rect_d:
 *     Estrutura para gerenciamento de retângulos.
 *     Um retângulo pertence à uma janela.
 */

// #todo
// Usar isso para lidar com dirty rectangles.

struct rect_d 
{
    object_type_t  objectType;
    object_class_t objectClass;

    int used;
    int magic;

//
// Invalidate
//

    // Sujo de tinta.
    // If the rectangle is dirty, so it needs to be flushed into 
    // the framebuffer.
    // When we draw a window it needs to be invalidated.

    int dirty;



    int flag;

    // Estilo de design
    int style;



    unsigned long x;
    unsigned long y;
    unsigned long cx;
    unsigned long cy;

    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    unsigned long right;
    unsigned long bottom;

    // 32 bit
    unsigned int bg_color; 

    struct rect_d *next;
};


// Isso pode ser útil principalmente
// para passar um retângulo de um ambiente para outro.
// É muito mais didático que a figura do retângulo como objeto.
struct surface_d
{
    int used;
    int magic;
    int dirty;
    struct rect_d *rect;
    
    struct surface_d *next;
};


// #todo
// struct surface_d *backbuffer_surface;


/* rgba */
struct tagRGBA
{
    object_type_t  objectType;
    object_class_t objectClass;

   char red;
   char green;
   char blue;
   char alpha;
};
struct tagRGBA *RGBA;


//
// Window Class support.
//

// ?? rever. 
// Enumerando classe de janela 
typedef enum {
    WindowClassNull,
    WindowClassClient,    // 1 cliente
    WindowClassKernel,    // 2 kernel
    WindowClassServer,    // 3 servidor
}wc_t;


//classes de janelas controladas pelos aplicativos.
typedef enum {
    WindowClassApplicationWindow,	
	//...
}client_window_classes_t;

//??bugbug: tá errado.
//classes de janelas controladas exclusivamente pelo kernel.
typedef enum {
    WindowClassKernelWindow,    //janelas criadas pelo kernel ... coma a "tela azul da morte"
    WindowClassTerminal,  //janela de terminal usada pelos aplicativos que não criam janela e gerenciada pelo kernel	
    WindowClassButton,
    WindowClassComboBox,
    WindowClassEditBox,
    WindowClassListBox,
    WindowClassScrollBar,
    WindowClassMessageOnly, //essa janela não é visível, serve apenas para troca de mensagens ...
    WindowClassMenu,
    WindowClassDesktopWindow,
    WindowClassDialogBox,
    WindowClassMessageBox,
    WindowClassTaskSwitchWindow,
    WindowClassIcons,
    WindowClassControl,   //??
    WindowClassDialog,
    WindowClassInfo,
    //...	
}kernel_window_classes_t;


//classes de janelas controladas pelos servidores.
typedef enum {
    WindowClassServerWindow,
    //...	
}server_window_classes_t;


//estrutura para window class
struct window_class_d
{
	
	//Que tipo de window class.
	// do sistema, dos processos ...
    //tipo de classe.
	
	wc_t windowClass; 

	//1
    client_window_classes_t	clientClass;
	
	//2
	kernel_window_classes_t	kernelClass;
	
	//3
	server_window_classes_t serverClass;
	
	//Endereço do procedimento de janela.
	//(eip da thread primcipal do app)
	unsigned long procedure;
    //...
};


// Single message struct model.
struct msg_d 
{

// validation
    int used;
    int magic;

// Standard header.
    struct window_d *window;
    int msg;
    unsigned long long1;
    unsigned long long2;

// extra payload.
    unsigned long long3;
    unsigned long long4;

// Extention:

    pid_t sender_pid;
    pid_t receiver_pid;

    int sender_tid;
    int receiver_tid;

// ...

    // #todo
    // We need some synchronization flags.
    // Maybe its better putting this flag into the thread struct.
    // t->msg_flags;

    //unsigned long flags;
};


// #deprecated
struct window_d
{

    int used;
    int magic;

/*

    object_type_t   objectType;
    object_class_t  objectClass;
    //struct object_d *object;

    int used;
    int magic;

    // #todo
    // Change to 'int'
    unsigned long id;    //Window Id. 

    char *name;          //Window name.
    unsigned long type;  //tipo ... (editbox, normal, ...)  style???


    // Created by this thread.
    // This is valid for overlapped windows 
    // created by applications.
    // When the kernel creates primitives just like the root window,
    // so this window will receive the number -1.

    int tid;
    
//
// Características dessa janela..
//

	//Estado: (Full,Maximized,Minimized...)
    int view; 

    // Background color.
    unsigned int bg_color; 

	//dimensões e margens.
	
	//#bugbug 
	//Esses conceitos estão sob revisão.
	//Estou aprendendo ...

	//+margem é o deslocamento até encontrar a borda.
	//+padding é o deslocamento entre a borda e a janela.

    unsigned long left;        //margem esquerda 
    unsigned long top;         //margem superior
    unsigned long right;       //margem direita  
    unsigned long bottom;      //margem inferior  

    unsigned long width;
    unsigned long height;

	// ??
	// Deslocamento dentro da janela?
    unsigned long x;           //deslocamento x
    unsigned long y;           //deslocamento y 

    int z;                     // z order.

    //invalidadas.
    //indica que precisamos repintar essa quando
    //chamarmos a rotina redraw_windows ou redraw_screen
    //See wm.c
    // #todo: Deveríamos usar a flag 'dirty' ao invés dessa.
    
    int dirty;


//==================================================

    // Alertartando que exite uma mensagem para essa janela.
    int msgAlert;  

    //?? rever
    int sendOffset;
    int receiveOffset;

	//Filas para uma janela receber mensagens.
	//São os quatro argumentos de um procedimento de janela padrão.

//
// CURRENT MESSAGE !!!
// 

	//shared by producer and consumer 
    struct window_d *msg_window;  //arg1.
    int msg;                      //arg2.
    unsigned long long1;          //arg3.
    unsigned long long2;          //arg4.

    //O aplicativo depois de pegar os 4 elementos, autoriza o 
    //kernel a colocar uma nova mensagem.
    //'0' indica que não temos uma mensagem válida.
    //'1' indica que temos uma nova mensagem.
    int newmessageFlag;

//
// MESSAGE QUEUE !!!
//

    // #bugbug: too long
    //shared by producer and consumer
    unsigned long hwndList[32];
    unsigned long msgList[32];
    unsigned long long1List[32];
    unsigned long long2List[32];

    //struct msg_d *msg;

	// procedure support. 
	//(poderia ser um argumento ou elemento da classe.)
    unsigned long procedure;               //procedimento da janela
    //struct window_procedure_d *wProcedure; //procedure struct

//==================================================

//
// Active?
// 
    // #bugbug: See the flag 'status'
    //unsigned long status;              //ATIVA OU NÃO.

    //FAST FLAG. Essa será a flag de ativa ou não. (decidindo isso)
    int active; 


//
// Focus
//
    // Se tem o foco de entrada ou não.
    int focus; 

//
// Cursor
//

    // Cursor
    // #remember: 
    // We want to use the cursor structure.
    // We are using the current console structure.

    // Cursor position inside the window in chars.
    unsigned long CursorX;
    unsigned long CursorY;

    // #maybe
    //unsigned long CursorXInPixels;
    //unsigned long CursorYInPixels;

    unsigned int CursorColor;
    //int CursorIconIndex;
    //unsigned long CursorIconAddress;
    // ...

//
// Mouse pointer
//

    // #todo
    // The mouse pointer type.
    // The address of the pointer.
    // The position inside the window.
    // Cursor and pointer are not the same thing.

    // unsigned long mouse_pointer_icon_address;
    //unsigned long mouse_pointer_x_pixel;
    //unsigned long mouse_pointer_y_pixel;

//===============================================

// Bars support.
// Cada tipo de janela tem seus itens específicos.
// Esses são os status dos ítens. Se eles estão presentes ou não.

    int backgroundUsed;
    int shadowUsed;
    int titlebarUsed;
    int menubarUsed; 
    int toolbarUsed;
    int clientAreaUsed;
    int statusbarUsed;
    int minimizebuttonUsed;
    int maximizebuttonUsed;
    int closebuttonUsed;
    int borderUsed;
	// ...

//==================================================

	// Buffer.
	// DedicatedBuffer
	// DedicatedBuffer --> LFB.
	// Endereço de memória onde a janela foi pintada.
	// Obs: Toda janela deve ter seu próprio buffer de pintura para poder 
	// passar a janela direto de seu próprio buffer para o LFB, sem passar 
	// pelo Backbuffer.

    void *DedicatedBuffer;  // Qual buffer dedicado a janela usa.
    void *BackBuffer;       // Qual backbuffer a janela usa.
    void *FrontBuffer;      // Qual frontbuffer a janela usa. (LFB).


//==================================================
    // Window Class
    struct window_class_d *window_class;
//==================================================

//
// Desktop
//
    struct desktop_d  *desktop;

//==================================================

//
// == Windows ========
//

//
// Parent
//

    struct window_d  *parent;

//
// Childs
//

    // Se cada janela apontar para uma child, então temos uma lista.
    // Child support.
    struct window_d *childListHead;  //Lista encadeada de janelas filhas.
    int childCount;                  //Tamanho da lista.

    struct window_d *child;

    //Configuramos isso quando efetuamos um raise_window
    //colocando a janela acima das outras.
    struct window_d *child_with_focus; 

//==================================================

//
// Client window and client rectangle.
//
    // Client window support.
    // É a janela propriamente dita, 
    // excluindo a moldura e a barra de rolagem.
    // Tudo o que há dentro da janela menos o frame.
    // É a parte que é exibida quando a janela está em full screen.

    struct window_d  *client_window; 
    struct rect_d    *rcClient; 

    // Cor do retângulo da área do cliente.
    unsigned int clientrect_bg_color; 


//==================================================
    
    // We don't need menubar in this environment.
    //struct window_d  *menubar;

//
// Tool bar
//
    // Main toolbar.
    // That one below menubar.
    // How many toolbars a widnow can have in kgws?

    struct window_d  *toolbar;

//
// Status bar
//
    struct window_d  *statusbar;


//==================================================

//
// Terminal
//
    // #todo
    // Estrutura tty deve ser uma coisa associada ao uso de terminal.
    // ID da tty usada.
 
    int tty_id;


    // TERMINAL SUPPORT
    // Obs: 
    // Essas variáveis só serão inicializadas se o 
    // aplicativo decidir que conterá um terminal em sua janela.
    // Um aplicativo só poderá ter um terminal dentro de cada janela.
    // Ou seja, se um aplicativo quiser ter mais de um terminal virtual, 
    // vai precisar de uma janela para cada terminal dentro do aplicativo.
    // isso permite facilmente que um mesmo aplicativo rode vários
    // programas, um em cada aba.
    // Ao invés de criar um frame para cada aplicativo que rode em terminal,
    // é só criar uma nova aba no gerenciador de terminais virtuais ...
    // esse gerenciador de terminais virtuais poderia ser o shell.bin

	//flags

	//configura o status do terminal dentro da janela
    int terminal_used;     //Se é um terminal ou não.
	
	//validade e reusabilidade das variáveis de terminal 
	//dentro da estrutura de janela.	
    int terminal_magic;
	
	//tab
	//número da tab.
	//indica qual tab o terminal está usando.
	//@todo:
	// Criar uma forma de contar as tabs de terminal 
	// dentro do gerenciador de terminais.
    int terminal_tab; // em qual tab do gerenciador de terminais está o terminal.
	
	//rect
    unsigned long terminal_left;
    unsigned long terminal_top;
    unsigned long terminal_width;
    unsigned long terminal_height;

    unsigned long terminal_right;
    unsigned long terminal_bottom;

	//...

    // #todo: isso deve pertencer a uma janela.
    // se uma janela tiver o foco de entrada e for um terminal 
    // a disciplica de linhas poderá usar essas carcterística do terminal.
    // See: terminal.h

    //struct terminal_d  *wTerminal;

//==================================================

    // style: 
    // Isso poderia ser estilo de design ...
    // Qualquer janela pode ter vários estilos de design 
    // ex: um editbox poderá ser de vários estilos.	

	//window style:
	//WINDOW_STYLE_FLOATING (flutuante) 
	//WINDOW_STYLE_DOCKING   (atracada em algum canto)

    int style;


//==================================================

	//*full screen mode = modo tela cheia. 
	//( utiliza a resolução atual do dispositivo )
	// deve ser a janela em primeiro plano. acima de todas as outras.
	//mas podemos configurar para que uma jenela esteja em full screen 
	//enquanto outra janela é a janela ativa e ainda outra tenha o foco de entrada.
	//uma janela em modo full screen pode conter barras de rolagem.
	//*embedded mode = dentro de uma janela ou de um navegador. 

//==================================================    

    // Seu estatus de relacionamento com outras janelas.
    unsigned long relationship_status;   

//==================================================    

	//ordem na pilha de janelas do eixo z.
	//A janela mais ao topo é a janela foreground.
    //int zIndex;    

	//z-order global.
	//Sua ordem em relação a janela gui->main.    
    //struct zorder_d *zorder;

//==================================================    

    //Qual buffer dedicado a janela usa.
    //void *buffer;        

//==================================================    

	//?? Se mudar para Rect pode deletar alguns elementos acima
	//como x, y, width ...
    struct rect_d *rcWindow;

	//Lista encadeada dos retângulos que pertencem a essa janela.
	//Quando uma janela for deletada, devemos desalocar toda a memória 
	//uada por esses recursos.
    struct rect_d *rectList;

//==================================================    

//
// CLists
//

	// Um ponteiro para um array de ponteiros de estruturas de linhas
	// Explicando: 
	// Nesse endereço teremos um array. Cada ponteiro armazenado
	// nesse array é um ponteiro para uma estrutura de linha.
	// Obs: 
	// #todo: 
	// Todos esses elementos podem formar uma estrutura e ficaria aqui 
	// apenas um ponteiro para ela.

    void *LineArray;
    int LineArrayIndexTotal;    //Total de índices presentes nesse array.
    int LineArrayIndexMax;      //Número máximo de índices permitidos no array de linhas.
    int LineArrayTopLineIndex;  //Indice para a linha que ficará no topo da página.
    int LineArrayPointerX;      //Em qual linha o ponteiro está. 	
    int LineArrayPointerY;      //em qual coluna o ponteiro está.
	//...

//==================================================    

	//thread de controle
	//Usada para input de mensagens e sinais.
    struct thread_d *control;

//==================================================    


    //  A que processo a janela pertence??
    struct process_d *process;

//==================================================    

	//
	// Menus support.
	//

	//?? Qual janela o menu usará.
    struct window_d *menu_window;   //Menu Window.

    //struct menu_d *sysMenu;         //menu de sistema.(control menu)
    //struct menu_d *barMenu;         //menu da barra de menu.
    //struct menu_d *defaultMenu;     //menu da janela (*importante)
	//...

    int selected;     //seleção  de item de menu.


//==================================================    

    // This window is a ...

    int isMenu;
    int isMenuItem;
    int isControl;
    int isButton;
    int isEditBox;
    int isCheckBox;
    int isIcon;
    // ...

//==================================================    

	// Se a janela for do tipo botão, então essa será a 
	// estrutura para o gerenciamento do botão.
    //struct button_d *button;
    
//==================================================    

    // ??
    const char *text; 

//==================================================    

    // Controls

    struct window_d  *minimize;
    struct window_d  *maximize;
    struct window_d  *close;
    int isMinimize;
    int isMaximize;
    int isClose;

//==================================================    

    // Qual janela é um ícone para essa janela.
    // Por exemplo, o ícone que fica na área de trabalho.
    struct window_d *icon;

//==================================================    

    //
    // Actions.
    //

    int draw; 
    int redraw;
    int show;   //se precisa ou não mostrar a janela.	
    // Continua ...

//==================================================    

	// Text Cursor support.
	// fica para uma versão estendida da estrutura.
	// Estrutura de cursor para a janela.
    //struct cursor_d *cursor;

//==================================================    

	//unsigned long bgcolor;		// Background color.
	//unsigned long fgcolor;		// Foreground color. 

//==================================================    

    //struct button_d *current_button;  //Botão atual.      
    //struct button_d *buttonList;      //Lista encadeada de botões em uma janela.

//==================================================    

	// ??
	// Status do puxador da janela.
	// Se está aberta ou não.
	// HANDLE_STATUS_OPEN ou HANDLE_STATUS_CLOSE
	int handle_status;


//==================================================
    // Trava. 
    // Se travada, não pode mudar nada.
    // Podemos travar a janela mãe quando 
    // estamos criando uma janela filha.
    // Enables or disables mouse and keyboard input to the 
    // specified window or control.

    int locked; 

//==================================================

    //
    // Terminal.
    //
    
    int isTerminal;
    //struct vt_d *terminal;

    // Navigation.
    struct window_d *prev; 
    struct window_d *next; 
    */
};

struct window_d *CurrentWindow;    //Janela atual
struct window_d *ActiveWindow;     //Janela atual.
struct window_d *WindowWithFocus;  //Janela com o foco de entrada.   
//...

// Lista encadeada de janelas.
struct window_d *window_Conductor2;
struct window_d *window_Conductor;
struct window_d *window_rootConductor;
//...

//
// Window list.
//

unsigned long windowList[WINDOW_COUNT_MAX];


// ??
unsigned long Windows[KGWS_ZORDER_MAX];


//id da janela que o mouse está em cima.
int window_mouse_over; 


//
//  ## Ponteiros para ícones ##
// 
 
// Ponteiros para o endereço onde os ícones 
// foram carregados.

// queremos saber se o endereço alocado eh compartilhado ...
// para o window server usar ... entao chamaremos de sharedbufferIcon.

void *shared_buffer_app_icon;  //1
void *shared_buffer_file_icon; 
void *shared_buffer_folder_icon;
void *shared_buffer_terminal_icon;
void *shared_buffer_cursor_icon;
// ... 

//??
// struct window_d  *FULLSCREEN_TABWINDOW;   


//
// == z order support =========================
//

//#define ZORDER_COUNT_MAX  128  //??PROVISÓRIO   


//esses tipo indicam algum posicionamento dentro da xorder.
typedef enum {
	zordertypeNull,     //ignorado
    zordertypeTop,      //acima de todas
    zordertypeBottom,   //abaixo de rodas.
    //...	
}zorder_type_t;



//essas são as camadas onde os objetos gráficos ficam ...
//estão associadas com formulários e containers.
typedef enum {
	zorderlayerNull,     //ignorado
    zorderlayerBack,     //back layer. é a área onde os métodos invocarão a construção de objetos gráficos.
    zorderlayerMiddle,   //middle layer. é onde os objetos gráficos e as etiquetas de controle são colocadas.
	zorderlayerFront,    //front layer. são colocados os controles não gráficos como: 
	                     //CommandButton, CheckBox e ListBox 
    //...	
}zorder_layer_t;


//
//Estrutura para controlar um índice de janela 
//ponteiros de instãncias dessa estrutura ficarão na lista zorderList[].
// Obs: uma estrutura de janela pode ter um poteiro para essa 
// estrutura que controlará todas as propriedades de zorder relaticas a aquela janela.
//
struct zorder_d
{
	// tipo ... top ou bottom.
	//encontraremos com facilidade se ela é top ou bottom.
	
	//zorder_type_t zorderType; 
	//zorder_layer_t zorderLayer;
	
	int zIndex;
    
	//?? ...
	
    struct window_d *window;
	
	//toda janela está na lista de zorder de outra janela.
	struct window_d *parent; //janela mãe... 
	
	struct zorder_d *next;
};

/* 
struct zorderInfo
{
    struct window *top_window;	
} 
*/

int zorder;
int zorderCounter;         //contador de janelas incluidas nessa lista.   
int zorderTopWindow;
//...




/*
 * zorderList[] support.
 *     Sobreposição de janelas.    
 *     ?? Precisamos reorganizar a lista ??
 *     ?? seria melhor uma lista encadeada ??
 *     ??e quando fecharem uma janela no meio da lista ??
 *
 *  >> Quando criamos uma janela ela é incluída no primeiro lugar vago da lista.
 *  >> quando deletamos uma janela, apenas excluimos ela da lista, não precisamos reorganizar.
 *  >> uma janelas minimizada é excluida dessa lista, é zerada z_axis_order na sua estrutura.
 *  >> repintaremos começando do zero.
 */ 
//unsigned long zorderList[ZORDER_COUNT_MAX];


//
// Backbuffer support. (espelho da memória de video)
//

struct backbufferinfo_d
{
	//@todo: object support

    int used;
    int magic;
	
    unsigned long start;
    unsigned long end;
    unsigned long size;
    //...

    //@todo:
	// ?? O que nos podemos ter aqui ??	
	// terminal., window, line disciplice, cursor ...
	//input buffer? z-order ??
};
struct backbufferinfo_d *BackBufferInfo;


//
// Frontbuffer support. (memória de vídeo)
//

struct frontbufferinfo_d
{
	//@todo: object support
	
	int used;
	int magic;
	
    unsigned long start;
    unsigned long end;
    unsigned long size;
	
	unsigned long width;
    unsigned long height;
	unsigned long bpp;
	//
	
    //@todo:
	// ?? O que nos podemos ter aqui ??	
	// terminal., window, line disciplice, cursor ...	
};
struct frontbufferinfo_d *FrontBufferInfo;



/*
 **********************************************************
 * gui:
 *     Nível 0 
 *     ## gui  ##
 *
 * Obs: Foi incluído aqui os ponteiros para as janelas principais usadas nos 
 * principais recursos gráficos, como control menu do desktop por exemplo.
 *
 * Histórico: 
 * 2015 - Created.
 * 2016 - incluíndo novos elementos.
 * ...
 */

// #bugbug
// Muita coisa nessa estrutura precis ser revista.
// Tendo em vista que ela apenas contempla o kgws
// como provedor de recursos gráficos.
// Dessa forma essa estrutura só faz sentido no ambiente 
// de setup, que usa o kgws.

struct gui_d
{
    int initialised;

    // Procedimento da janela ativa.
    unsigned long procedure;  

    // #bugbug
    // precisamos de estrutura de device context,
    // onde teremos informações sobre o hardware
    // responsável pelos recursos gráficos.

    // BUFFERS
    // Ponteiros para os principais buffers usados pelo sistema.

    // LFB:
    // O endereço do início da memória de vídeo do cartão de memória.
    // obs: Quantos desses ponteiros precisamos dentro da memória de
    // vídeo? E se tivermos várias placas de memória de vídeo, seria
    // um lfb para cada placa?
    // Esse valor foi configurado pelo BIOS pelo metodo VESA.

    unsigned long lfb;
    // unsigned long lfb_pa;  //use this one

    // Backbuffer
    // O Backbuffer pe o buffer para colocar a imagem de pano de fundo.
    // Ele será o buffer dedicado da janela principal gui->main. 

	// #importante.
	// Um backbuffer pode cobrir a área de vários monitores.
	// O conceito de backbuffer pode estar relacionado com o conceito de room,
	// (window station), com vários desktops e monitores.

    unsigned long backbuffer;
    //unsigned long backbuffer_pa;  //use this one

    void *backbuffer1; 
    void *backbuffer2; 
    void *backbuffer3;
    // ...


	/*
	 * Default dedicated buffers.
	 *     Esses ponteiros podem ser usados para aloca
	 * memória para buffer dedicado antes mesmo de se criar a estrutura
	 * da janela.
	 * Obs: Toda janela tem que ter um buffer dedicado, onde ela será pintada.
	 * Depois de pintada ela pertencerá a uma lista de janelas que serão
	 * enviadas para o LFB seguindo a ordem da lista.
	 */
	
	//void* defaultWindowDedicatedBuffer1;
	//void* defaultWindowDedicatedBuffer2;
	//void* defaultWindowDedicatedBuffer3;
    //...	


    // redraw
    // Flag para repintar todas as janelas.
    // #todo: #bugbug, Isso parece estranho. Cada janela
    // está pintada em seu buffer dedicado e fica por conta de
    // cada janela decidir se repinta ou não apenas ela.

    int redraw;

    // refresh
    // Flag para enviar do backbuffer para a memória de video.
    // Seguindo uma lista linkada, copiaremos o conteúdo do buffer
    // dedicado de cada janela da lista no LFB. Primeiro é Backbuffer
    // que é o buffer da janela principal, que funcionará como
    // Head da lista.

    int refresh;

    // status das janelas usadas pelo sistema.
	int screenStatus;
	int backgroundStatus;
	int logoStatus;
	int mainStatus;
	int navigationbarStatus;
	int menuStatus;
	int taskbarStatus;
	int statusbarStatus;
	int infoboxStatus;
	int messageboxStatus;
	int debugStatus;
	int gridStatus;

	/*
	 * Windows
	 *
	 * Obs: 
	 * Ponteiros para as janelas principais usadas na interfáce
	 * gráfica. Essas janelas não dependem dos processos e sim os
	 * processos dependem delas. Os processos do sistema poderão recriar
	 * essas janelas e registrar seus ponteiros aqui. Por exemplo, o control
	 * menu pode sofrer alterações e depois registrar o ponteiro aqui.
	 * Outro exemplo é o control menu da janela ativa, toda vez que
	 * trocar a janela ativa, deve-se registrar o ponteiro do control menu
	 * da janela aqui, para fácil acesso.
	 *
	 * Os ponteiros aqui serão organizados em grupos:
	 * ==============================================
	 * Screen - root window
	 * Grupo 0: Background, Logo, Desktop, Taskbar.
	 * Grupo 1: Main (Full Screen), Status Bar. 
	 * Grupo 2: Grid. (grid de ícones flutuantes)
	 * Grupo 3: Control menu.(Control menu da janel atual).
	 * Grupo 4: Info Box, ToolTip.
	 * Grupo 5: ??
	 * Grupo 6: Outras.
	 */

//
// Security
//
    struct usession_d  *CurrentUserSession;
    struct room_d      *CurrentRoom;
    struct desktop_d   *CurrentDesktop;

//
// User info.
//
    //struct user_info_d *User; 
    //...

};

// #importante
// Estrutura global. 
// (Usada para passar estutura entre funções)

struct gui_d  *gui; 


//
// == prototypes ================
//


//
// pixel
//

void 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

//
// char
//

void set_char_width ( int width );
void set_char_height (int height);
int get_char_width (void);
int get_char_height (void);


void 
d_draw_char ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor );

void 
d_drawchar_transparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c );

//
// string
//    

void 
draw_string ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string ); 


//
// x_panic
//

void x_panic( char *string );
void xxxDrawString( char *string );

//
// line
//

void 
my_buffer_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags );


//
// Draw rectangle into the backbuffer.
//

void 
drawDataRectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );

void 
refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );


void 
refresh_rectangle2 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned long buffer1,
    unsigned long buffer2 );

void scroll_screen_rect (void);

void *rectStrCopyMemory32 ( 
    unsigned long *dest, 
    unsigned long *src, 
    int count );



//
// Background
//

int Background_initialize(void);
void backgroundDraw (unsigned int color);


#endif    














