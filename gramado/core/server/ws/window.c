/*
 * File: createw.c 
 * 
 *      Creates a window.
 *      It's part of the gws project.
 * 
 * History:
 *     2019 - Created by Fred Nora.
 */
 
    // #importante
    // O frame de uma janela deve ser parte do Window Manager.

// #bugbug
// #todo
// Para lidarmos com essas estruturas do kernel
// devemos usar a chamada sci2. int 0x82. ??


// todo
// Quando estivermos contrindo as molduras das janelas,
// as rotinas devem retornar suas alturas, para assim
// o posicionamento do 'top' da área de cliente possa ser
// atualizado. (rect)
// Lembrando que possivelmente o window manager fará esse trabalho
// sobrando para o qindo server apenas rotinas de baixo nível.
// ~> mas o sistema tem que funcionar somente com o window server.

// wm: ~> o ws oferecerá rotinas de redimencionamento e 
// reposicionamento de janelas. Essas rotinas serão a base 
// do window manager.



#include <gws.h>


#define OVERLAPPED_MIN_WIDTH    80
#define OVERLAPPED_MIN_HEIGHT   80

#define EDITBOX_MIN_WIDTH    8
#define EDITBOX_MIN_HEIGHT   8

#define BUTTON_MIN_WIDTH    8
#define BUTTON_MIN_HEIGHT   8


static int config_use_transparency=FALSE;


// habilitando/desabilitando globalmente 
// alguns componentes da janela

int gUseShadow = TRUE;
int gUseFrame = TRUE;
//int gUseShadow = TRUE;
// ...



void gws_enable_transparence(void)
{
   config_use_transparency=TRUE;
}


void gws_disable_transparence(void)
{
   config_use_transparency=FALSE;
}




// #todo
// Essas rotina serão chamada pelo request assincrono sem resposta.

/*
void useShadow( int value );
void useShadow( int value )
{
    // Ativando.
    if ( value == TRUE ){
        gUseShadow = TRUE;
        return;
    }

    gUseShadow = FALSE;
}
*/

/*
void useFrame( int value );
void useFrame( int value )
{
    // Ativando.
    if ( value == TRUE ){
        gUseFrame = TRUE;
        return;
    }

    // No frames.
    // It means that the loadable window manager 
    // will create some kind of frames for all the windows, or not.
    
    gUseFrame = FALSE;
}
*/





/*
 *************************************************************
 * xxxCreateSurfaceWindow: 
 * 
 *     Cria uma janela com base em uma struct.
 *     Retorna o endereço da estrutura da janela criada. 
 *     Para que possa ser registrada na lista windowList[].
 *     Essa lista tambem fica em kgws no kernel.
 * 
 * Cria a janela dependendo do tipo:  
 * =================================
 * 1 - POP-UP.
 * 2 - EDIT-BOX.
 * 3 - NORMAL ( bg + client area ).
 * 4 - NORMAL_EDIT ( bg + editbox).
 * 5 - MENUBAR.
 * 6 - TOOLBAR.
 * 7 - STATUSBAR.
 * 8 - TASKBAR.
 * 9 - ? 
 * CONTINUA ...
 *
 */


void *xxxCreateSurfaceWindow( 
    unsigned long type,        // 1, Tipo de janela (popup,normal,...)
    unsigned long status,      // 2, Estado da janela (ativa ou nao)
    unsigned long view,        // 3, (min, max ...)
    char *windowname,          // 4, Título. 
    unsigned long x,           // 5, Deslocamento em relação às margens do Desktop. 
    unsigned long y,           // 6, Deslocamento em relação às margens do Desktop.
    unsigned long width,       // 7, Largura da janela.
    unsigned long height,      // 8, Altura da janela.
    void *pSurfaceWindow,      // 9, Endereço da estrutura da janela mãe.
    unsigned long desktop_id,  //10, Desktop id.
    unsigned long clientcolor, //11, Cor da área de cliente
    unsigned long color )      //12, Color (bg) (para janela simples).
{ 

    // opaque
    // A ring0 structure pointer.
    void *wObjectPointer;
    
	// Enviando tudo via argumento.
	// Esse método dá a possibilidade de enviarmos ainda 
	// mais argumentos. 
	// #importante: Isso está funcionado, Vamos fazer assim e 
	// não do jeito antigo.


	unsigned long message_buffer[12];



    gwssrv_debug_print ("xxxCreateSurfaceWindow: \n");

	message_buffer[0]  = (unsigned long) type;
	message_buffer[1]  = (unsigned long) status;
	message_buffer[2]  = (unsigned long) view;
	message_buffer[3]  = (unsigned long) windowname;
	message_buffer[4]  = (unsigned long) x;
	message_buffer[5]  = (unsigned long) y;
	message_buffer[6]  = (unsigned long) width;
	message_buffer[7]  = (unsigned long) height;
	message_buffer[8]  = (unsigned long) pSurfaceWindow;
	message_buffer[9]  = (unsigned long) desktop_id;
	message_buffer[10] = (unsigned long) clientcolor;
	message_buffer[11] = (unsigned long) color;

    // #bugbug
    // Maybe we need to use sc82()

    wObjectPointer = (void *) gramado_system_call ( 
                          118 , 
                          (unsigned long) &message_buffer[0], 
                          (unsigned long) &message_buffer[0], 
                          (unsigned long) &message_buffer[0] );

    if ( (void *) wObjectPointer == NULL ){
        gwssrv_debug_print ("xxxCreateSurfaceWindow: [FAIL] wObjectPointer\n");
        return NULL;  
    }

    // ??
    // #teste
    // Vamos enviar a mensagem MSG_CREATE para o procedimento de janela.
    // Assim ele pode terminar de pintar nessa mesma janela.

    return (void *) wObjectPointer;    
}


// not tested yet
struct gws_surface_d *xxxCreateSurface( 
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned long color )
{

    struct gws_surface_d *surface;

    void *surfaceWindowObject;

//
// surface
//
    surface = (struct gws_surface_d *) malloc ( sizeof( struct gws_surface_d ) );

    if ( (void*) surface == NULL )
    {
        //todo: message
        return NULL;
    }

    surface->initialized = FALSE;

//
// window
//

    surfaceWindowObject = (void *) xxxCreateSurfaceWindow ( 
                                       WT_SIMPLE, 
                                       1, 
                                       1, 
                                       "Surface",
                                       left, top, width, height, 
                                       0, 0, COLOR_BLACK, COLOR_BLACK );  

    if ( (void*) surfaceWindowObject == NULL )
    {
        //todo: message
        return NULL;
    }
    
    
//
// structure
//

    surface->used  = TRUE;
    surface->magic = 1234;
    surface->window_object = (void *) surfaceWindowObject;
    surface->left   = left;
    surface->top    = top;
    surface->width  = width;
    surface->height = height;

    surface->dirty = FALSE;
    surface->locked = FALSE;
    
    surface->next = NULL;
    surface->initialized = TRUE;

    return (struct gws_surface_d *) surface;
}



// Strings
//char *title_when_no_title = "Window";


void demoTerry(void)
{

    CreateWindow ( 
         WT_SIMPLE, 
         0,  //style
         1,  //status
         1,  //view
         "demoTerry",  
         40-8, 40-8, 200+8+8, 320+8+8,   
         gui->screen_window, 0, 
         COLOR_WHITE, COLOR_WHITE );


     struct gws_window_d *terry;
     terry = (struct gws_window_d *) CreateWindow ( 
         WT_SIMPLE, 
         0,  //style
         1,  //status
         1,  //view
         "demoTerry-2",  
         40, 40, 200, 320,   
         gui->screen_window, 0, 
         COLOR_WHITE, COLOR_WHITE );


    // IN: l,r,t,b, solid
    DrawBorder ( 
        40, 40+200, 
        40, 40+320, 
        1 );

    // IN: l,r,t,b, solid
    //DrawBorder ( 
    //    8, 240, 
    //    8, 320, 0 );


    if ( (void*) terry != NULL){
        dtextDrawText ( (struct gws_window_d *) terry,
            20, 20, COLOR_BLUE, "Demo: Terry's window" );
        dtextDrawText ( (struct gws_window_d *) terry,
            20, 32, COLOR_RED,  "Gramado Community :)" );
    }

}



/*
 ********************
 * xxxCreateWindow: 
 * 
 *     Cria uma janela com base em uma struct. Retorna o endereço da estrutura 
 * da janela criada, para que possa ser registrada na lista windowList[].
 * Obs: A contagem é feita quando registra a janela.
 *
 * @todo: É necessário definir claramente os conceitos de window e window frame...
 *        A construção dos elementos gráficos precisam de organização e hierarquia.
 *
 * Obs: Essa rotian pode criar tanto uma janela, quanto um frame ou um botão.
 *
 * @todo: Corrigir as dimensões da janela: 
 * *Importante: OS ARGUMENTOS DE DIMENSÕES SÃO PARA O RETÂNGULO QUE FORMA UMA ÁREA
 * QUE INCLUI DA ÁREA DO CLIENTE + BARRA DE FERRAMENTAS + BARRA DE MENU.
 * a barra de títulos faz parte da moldura.
 * O AS DIMENSÕES DO FRAME PODEM SER VARIADOS, DEPENDENDO DO ESTILO DA JANELA.
 *
 * Cria a janela dependendo do tipo:
 * =================================
 * WT_NULL          0 
 * WT_SIMPLE        1
 * WT_EDITBOX       2  // igual simples, mais uma bordinha preta.
 * WT_OVERLAPPED    3  // sobreposta(completa)(barra de titulo + borda +client area)
 * WT_POPUP         4  // um tipo especial de sobreposta,  //usada em dialog ou 
 *                        message box. (com ou sem barra de titulo ou borda)
 * WT_CHECKBOX      5  // Caixa de seleção. Caixa de verificação. Quadradinho.
 * WT_SCROLLBAR     6  // Cria uma scroll bar. Para ser usada como janela filha.
 * CONTINUA ...
 */

//1  - Tipo de janela (popup,normal,...) 
//2  - style
//3  - Estado da janela. (poderia ter vários bits ??)
//4  - (min, max ...)
//5  - Título.
//6  - Deslocamento em relação às margens do Desktop.
//7  - Deslocamento em relação às margens do Desktop.
//8  - Largura da janela.
//9  - Altura da janela.
//10  - Endereço da estrutura da janela mãe.
//11 - desktop ID. (# Para levarmos em conta as características de cada desktop).
//12 - Client Area Color.
//13 - color (bg) (para janela simples)

void *xxxCreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status, 
    unsigned long view, 
    char *windowname, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktopid, 
    unsigned int clientcolor, 
    unsigned int color,
    unsigned long rop_flags ) 
{

    // #todo
    // Essa função deve chamar helpers que pintem sem criar objetos
    // gráficos que alocam memória. Dessa forma eles
    // poderão serem reusados nas funções de 'redraw'.

	// #todo: 
	// O argumento style está faltando.
	// Cada tipo de tanela poderá ter vários estilos.
	// Obs: 
	// Podemos ir usando apenas um estilo padrão por enquanto.


    // #todo:
    // Receberemos isso via parametro de função.
    // Default is FALSE.
    // We need to know the parent's bg color.
    int Transparent = FALSE;


    int Maximized=0;
    int Minimized=0;
    int Fullscreen=0;


// Bars
// A title bar é criadas pela função
// que cria o frame.

    // Title bar buttons. [v] [^] [X] 
    int MinimizeButton = FALSE;
    int MaximizeButton = FALSE;
    int CloseButton    = FALSE;
    // ...

    // Items.
    int Background    = FALSE;
    int ClientArea    = FALSE;
    int Shadow        = FALSE;
    int ButtonDown    = FALSE; 
    int ButtonUp      = FALSE;  // ??
    int ButtonSysMenu = FALSE;  // system menu na barra de títulos.
    int Border        = FALSE;  // usado no edit box.
    // ...


    // Desktop support.
    int ParentWindowDesktopId;    //Id do desktop da parent window.
    int WindowDesktopId;          //Id do desktop da janela a ser criada.


    //controle de janela
    struct gws_window_d *windowButton1;  // minimize
    struct gws_window_d *windowButton2;  // maximize
    struct gws_window_d *windowButton3;  // close

    // botões na barra de rolagem.
    struct gws_window_d *windowButton4;
    struct gws_window_d *windowButton5;
    struct gws_window_d *windowButton6;

    // Structs.
    struct gws_window_d *window;
    struct gws_window_d *Parent;

	//struct desktop_d *Desktop;  //suspenso.


//
// The client rectangle.
//

// #todo
// Isso é uma estrutura local para gerenciarmos o retangulo
// da área de cliente.
// Depois temos que copiar os valores para window->rcClient 

    struct gws_rect_d   clientRect;

//
// Border
//

    // #
    // Improvisando uma largura de borda.
    // Talvez devamos receber isso via parâmetros.

    unsigned int border_size = 0;
    unsigned int border_color = COLOR_BORDER;


    unsigned int __tmp_color=0;



// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF );
    unsigned long deviceHeight = (__device_height & 0xFFFF );

//
// Position and dimension.
//

// left, top, width, height

    unsigned long WindowX = (unsigned long) (x & 0xFFFF);
    unsigned long WindowY = (unsigned long) (y & 0xFFFF);
    unsigned long WindowWidth  = (unsigned long) (width  & 0xFFFF);
    unsigned long WindowHeight = (unsigned long) (height & 0xFFFF);

// #todo: right and bottom.



// Full ?
// Position and dimension for fullscreen mode.
// Initial configuration.
// It will change.
// #bugbug
// left and top needs to be '0'?

/*
    unsigned long fullWindowX      = (unsigned long) (WindowX + border_size);
    unsigned long fullWindowY      = (unsigned long) (WindowY + border_size);
    unsigned long fullWindowWidth  = (unsigned long) WindowWidth;
    unsigned long fullWindowHeight = (unsigned long) WindowHeight;
    // #todo: right and bottom.
*/

// Fullscreen support
    unsigned long fullWindowX      = (unsigned long) deviceLeft;
    unsigned long fullWindowY      = (unsigned long) deviceTop;
    unsigned long fullWindowWidth  = (unsigned long) deviceWidth;
    unsigned long fullWindowHeight = (unsigned long) deviceHeight;


//
// Button suport
//

    int buttonFocus    = FALSE;
    int buttonSelected = FALSE;

    unsigned int buttonBorderColor1=0;
    unsigned int buttonBorderColor2=0;
    unsigned int buttonBorderColor2_light=0;
    unsigned int buttonBorder_outercolor=0;  //Essa cor muda de acordo com o foco 

    debug_print ("xxxCreateWindow:\n");



//
// Max Min
//

// Maximized
    if ( style & 0x0001 ){
        Fullscreen = FALSE;
        Minimized = FALSE;
        Maximized = TRUE;  //:)
    }

// Minimized
    if ( style & 0x0002 ){
        Fullscreen = FALSE;
        Maximized = FALSE;
        Minimized = TRUE;  //:)
    }

//
// Fullscreen
//
    if ( style & 0x0004 ){
        Maximized = FALSE;
        Minimized = FALSE;
        Fullscreen = TRUE;  //:)
    }

    

	//salvar para depois restaurar os valores originais no fim da rotina.
	//unsigned long saveLeft;
	//unsigned long saveTop;
	
	
	// Desktop:
	// @todo: Configurar desktop antes de tudo. 
	// @todo: Quando criamos uma janela temos de definir que ela
	// pertence ao desktop atual se não for enviado por argumento 
	// o desktop que desejamos que a janela pertença.

	
	// O argumento onde:
	// Indica onde devemos pintar a janela. Serve para indicar as janelas 
	// principais. Ex: Se o valor do argumento for 0, indica que devemos 
	// pintar na tela screen(background) etc...


	// full screen mode ??
	// Se a janela a ser criada estiver no modo full screen, ela não deve ter
	// um frame, então as dimensões da janela serão as dimensões do retângulo
	// que forma a janela. Talvez chamado de Client Area.
	
	//
	// Parent window.
	//
	
	// Se a parent window enviada por argumento for inválida, 
	// então usaremos a janela gui->screen. ?? 
	// Talvez o certo fosse retornar com erro.
	// ?? Qual deve ser a janela mãe ? Limites ?
	// @todo: devemos checar used e magic da janela mãe.
	// #bugbug: E quando formos criar a gui->screen, quem será a janela mãe ?
	
	/*
	if ( (void *) pWindow == NULL ){
		
		Parent = (void *) gui->screen;

	} else {

		Parent = (void *) pWindow;
	};
    */
    

	// Devemos checar se a janela está no mesmo desktop 
	// que a ajnela mãe.
	// No caso aqui, criarmos uma janela no mesmo desktop que a 
	// janela mãe.
	
	// Devemos setar uma flag que indique que essa 
	// é uma janela filha, caso seja uma. Essa flag 
	// deve ser passada via argumento @todo.

	// @todo: Checar se é uma janela filha, 
	// se for uma janela filha e não tiver uma janela mãe associada a ela, 
	// não permita e encerre a função.

	
	//if(FlagChild == 1)
	//{
		//if(pWindow = NULL) return NULL;
	//}
	

	// @todo: A atualização da contagem de janela deve ficar aqui,
	// mas me parece que está em outro lugar, ou não tem. ainda.
	
	// @todo: Se essa não for uma janela filha, então temos que resetar 
	// as informações sobre a janela mãe. porque não procedem.	
	
	//ms. e se essa é uma janela filha de uma janela mãe que pertence à
	//outra thread e não está no desktop ??
	




	// *Importante: 
	// Checando se o esquema de cores está funcionando.

	/*
	if ( (void *) CurrentColorScheme == NULL ){
		
		panic ("CreateWindow: CurrentColorScheme");
		
	}else{
		
		if ( CurrentColorScheme->used != 1 || 
		     CurrentColorScheme->magic != 1234 )
		{
		    panic ("CreateWindow: CurrentColorScheme validation");
		}

		//Nothing.
	}
    */


	//
	// ## New window ##
	//

//CreateStruct:


    //Alocando memória para a estrutura da janela.
   // #todo: 
   // Onde o Kernel colocará essa mensagem de erro ?
   // Por enquanto no backbuffer.

    window = (void *) malloc( sizeof(struct gws_window_d) );

    if ( (void *) window == NULL )
    {
        // #fixme
        debug_print ("xxxCreateWindow: [ERROR] window\n");
        printf      ("xxxCreateWindow: [ERROR] window\n");
        // #debug
        gwssrv_show_backbuffer();
        while(1){}
        // #todo
        // We need to return.
        //return NULL; 
    }

    memset( window, 0, sizeof(struct gws_window_d) );


    window->used   = TRUE;
    window->magic  = 1234;


//
// Locked
//

    window->locked = FALSE;
    if ( style & 0x8000 ){
        window->locked = TRUE;
    }


// ===================================
// Input device

    window->ip_device = IP_DEVICE_NULL;
    window->ip_on = FALSE;  // desligado

// For keyboard
    window->ip_x = 0;       // in chars
    window->ip_y = 0;       // in chars
    window->ip_color = COLOR_BLACK;

    //window->ip_type = 0;    // #bugbug #todo
    //window->ip_style = 0;

// For mouse
// In pixel, for mouse pointer ip device.
    window->ip_pixel_x = 0;
    window->ip_pixel_y = 0;
// ===================================



    window->type  = (unsigned long) type;
    window->style = (unsigned long) style;  // A lot of flags.

    window->status = (int) (status & 0xFFFFFFFF );
    window->view   = (int) view;
    window->focus  = FALSE;
    window->dirty  = FALSE;  // Validate
    window->locked = FALSE;


    // We have a valid pointer.
    // Let's set up the elements using the parameters.

    // #todo
    // Object support.
    //window->objectType  = ObjectTypeWindow;
    //window->objectClass = ObjectClassGuiObjects;

    // #todo
    // #importante
    // Id. 
    // A janela recebe um id somente na hora de registrar.
    // window->id = ?.




//
// The window name
//

    // #test
    if ( (void*) windowname == NULL ){
        debug_print ("xxxCreateWindow: [ERROR] Invalid windowname\n");
    }

    // #test
    if ( *windowname == 0 ){
        debug_print ("xxxCreateWindow: [ERROR] Invalid *windowname\n");
    }

    window->name  = (char *) windowname;



// ===================================
// #test

//
// Parent window
//

    if ( (void*) pWindow == NULL ){
        debug_print ("xxxCreateWindow: [CHECK THIS] Invalid parent window\n");
    }
    Parent = (void *) pWindow;
    window->parent = Parent;
    window->child_list = NULL;


// ===================================


    //#todo: 
    // é importante definir o procedimento de janela desde já.
    //senão dá problemas quando chamá-lo e ele naõ estiver pronto.
    //Procedure support.
    //#todo: Devemos receber esses parâmetros e configurar aqui.
    //#bugbug: Talvez isso será configurado na estrutura
    // de window class. Se é que termos uma.
    //window->procedure = (unsigned long) &system_procedure;
    //window->wProcedure = NULL;  //Estrutura.




//
// == Status ============================
//

    // Qual é o status da janela, se ela é a janela ativa ou não.
    //?? Devemos definir quais são os status possíveis da janela.

    // Active 
    if ( window->status == WINDOW_STATUS_ACTIVE )
    { 
        active_window = (int) window->id;  
        //set_active_window(window); 
        //window->active = WINDOW_STATUS_ACTIVE;
        //window->status = (unsigned long) WINDOW_STATUS_ACTIVE;
        window->relationship_status = (unsigned long) WINDOW_REALATIONSHIPSTATUS_FOREGROUND; 
    
        //#todo
        //window->z = 0;  //z_order_get_free_slot()
        //...
    }

    // Inactive
    if ( status == WINDOW_STATUS_INACTIVE )
    { 
        //window->active = WINDOW_STATUS_INACTIVE;
        //window->status = (unsigned long) WINDOW_STATUS_INACTIVE;
        window->relationship_status = (unsigned long) WINDOW_REALATIONSHIPSTATUS_BACKGROUND;
   
        //todo
        //window->z = 0; //z_order_get_free_slot()
        //...
    }



//
// == Margins and dimensions ======================
//

// #todo:
// Se for uma janela filha o posicionamento deve ser somado às margens 
// da área de cliente da janela que será a janela mãe.
// #bugbug @todo 
// Esses valores de dimensões recebidos via argumento na verdade 
// devem ser os valores para a janela, sem contar o frame, que 
// inclui as bordas e a barra de títulos.

// Dimensions
    window->width  = (unsigned long) (WindowWidth  & 0xFFFF);
    window->height = (unsigned long) (WindowHeight & 0xFFFF);


// Maximized ?
    if ( Maximized == TRUE )
    {
        window->left   = deviceLeft;
        window->top    = deviceTop;
        window->width  = deviceWidth;
        window->height = (deviceHeight - 40); //menos a barra
    }

// Fullscreen
    if ( Fullscreen == TRUE )
    {
        window->left   = fullWindowX;
        window->top    = fullWindowY;
        window->width  = deviceWidth;
        window->height = deviceHeight; 
    }

    window->width_in_bytes  = (unsigned long) (window->width / 8);  //>>3
    window->height_in_bytes = (unsigned long) (window->height / 8); //>>3


// =================================

//
// == Frame rectangle ==
//

// =================================

    window->rcWindow.left   = (unsigned long) WindowX;       // window left
    window->rcWindow.top    = (unsigned long) WindowY;       // window top
    window->rcWindow.width  = (unsigned long) WindowWidth;   // window width
    window->rcWindow.height = (unsigned long) WindowHeight;  // window height



// #test

    window->rcClient.left   = (unsigned long) window->left;
    window->rcClient.top    = (unsigned long) window->height;
    window->rcClient.width  = (unsigned long) window->width;
    window->rcClient.height = (unsigned long) window->height;

//
// == Client area ==
//

// #todo
// Os valores da área de cliente são elaborados
// no decorrer da construção da janela.
// Inicialmente ela é do tamanho da janela.
// A área de cliente é relativa à janela.
// #todo: Qual tipo de janela tem área de cliente?
// #todo: Qual é o tamanho da área de cliente?
// é o valor passado via argumentos? e o frame é extra?
// Local

    clientRect.left   = 0;
    clientRect.top    = 0;
    clientRect.width  = (unsigned long) window->width;
    clientRect.height = (unsigned long) window->height;


// The original values. They are the same os the window.
    window->rcClient.left   = clientRect.left;    // window left
    window->rcClient.top    = clientRect.top;     // window top
    window->rcClient.width  = clientRect.width;   // window width
    window->rcClient.height = clientRect.height;  // window height

// =================================


// Deslocamento em relação a janela mãe.
    window->x = WindowX;
    window->y = WindowY;

//++
// Margens.
// Deslocamento em relação a tela. (Screen)
// Vai depender do deslocamento da janela mãe.
    if ( window->parent != NULL )
    {
        window->left = (window->parent->left + window->x); 
        window->top  = (window->parent->top  + window->y);

// No caso da primeira janela de todas.
    }
    else
    {
        window->left = window->x;
        window->top  = window->y; 
    };
    window->right  = (unsigned long) ( window->left + window->width );
    window->bottom = (unsigned long) ( window->top  + window->height ); 
//--

    if ( Maximized == TRUE || Fullscreen == TRUE )
    {
        window->left = deviceLeft;
        window->top  = deviceTop;
    }

// Full ?
// Margins and dimensions for fullscreen mode.
// #bugbug
// This is valid only for some types of window.
// #bugbug
// left and top needs to be '0'?

    window->full_left   = fullWindowX;
    window->full_top    = fullWindowY;
    window->full_right  = (fullWindowX + fullWindowWidth);
    window->full_bottom = (fullWindowY + fullWindowHeight);
    window->full_width  = fullWindowWidth;
    window->full_height = fullWindowHeight;


//
// Color
//

// Window background
    window->bg_color = (unsigned int) color;

// Client area rectangle
    window->clientrect_bg_color = (unsigned int) clientcolor;



		//@todo: As outras características do cursor.
		//Características.
		
		//Estrutura para cursor.
		//todo
		//window->cursor = NULL;
		
		// #todo: 
		// Uma opção é inicializarmos a estrutura de ponteiro depois ...
		// pois tem janela que não tem ponteiro. 
		// JÁ QUE NO MOMENTO ESTAMOS ENFRENTANDO ALGUNS TRAVAMENTOS.
		//
		//window->cursor = (void*) malloc( sizeof(struct cursor_d) );

		//@@todo: Criar uma função: Inicializarcursor(struct cursor_d *cursor).
		//if(window->cursor != NULL)
		//{
		//    window->cursor->objectType = ObjectTypeCursor;
		//    window->cursor->objectClass = ObjectClassGuiObjects;
		//	window->cursor->x = 0;
		//	window->cursor->y = 0;
		//	window->cursor->imageBuffer = NULL;
		//	window->cursor->imagePathName = NULL;
			//window->cursor->cursorFile = ??; //@todo: Difícil definir o tipo.
		//	window->cursor->cursorType = cursorTypeDefault;
		//};


		//Barras.
		//As flags que representam a presença de cada uma das barras
		//serão acionadas mais tarde, na hora da pintuda, de acordo com
		//o tipo de janela à ser pintada.


		//Buffers support.
		
		//Dedicated buffer: 
		//Cria um buffer dedicado de acordo com as dimensões da janela.

		//suspensa !!!

	//window->DedicatedBuffer = (void*) windowCreateDedicatedBuffer(window);
    window->DedicatedBuffer = NULL;

		// backbuffer and front buffer.
		//window->BackBuffer = (void *) g_backbuffer_va;
		//window->FrontBuffer = (void *) g_frontbuffer_pa;


// The child list

    window->child_list = NULL;


//
// Client window 
//

    // (#importante)
    // Client window support.
    // Obs: A área de cliente será um retângulo e não uma janela.
    // Mas nda impede da estrutra gerenciar uma janela que fique 
    // em cima da área de cliente.

    window->client_window = NULL;  // window. 

//
// Terminal window
//

    // #importante
    // Terminal support.
    // Suporte não tradicional à terminais.
    // manipulando variáveis ao invés de uma estrutura.

    window->terminal_used  = (int) 0;
    window->terminal_magic = (int) 0;
    window->terminal_tab   = (int) 0;

    //window->terminal_left = (unsigned long) 0;
    //window->terminal_top = (unsigned long) 0;
    //window->terminal_width = (unsigned long) 0;
    //window->terminal_height = (unsigned long) 0;


//
// Desktop support
//

    //window->desktop = (void*) Desktop; //configurado anteriormente.
    //window->desktop_id = Desktop->id;  //@todo: verificar elemento.

//
// Menu support
//

    window->menu_window  = NULL;
    window->sysMenu      = NULL;
    window->barMenu      = NULL;
    window->defaultMenu  = NULL;

    // What is that?
    window->isMenu    = 0;
    window->isButton  = 0;
    window->isEditBox = 0;


//
// Selected menu item.
//

    //Caso a janela seja um ítem de menu.
    window->selected = FALSE; 

    // Texto, caso a janela seja um ítem de menu
    // window->text = NULL; 

    // Draw support.
    window->draw   = 0;      //@todo: Cuidado com isso.
    window->redraw = 0;
    window->show   = 1; //Inicialmente presumimos que precisamos mostrar essa janela.
    // Continua ...

        //Abaixo, elementos referenciados com menor frequência.
	    
		//window->desktop = NULL; //@todo: Definir à qual desktop a janela perence.
		//window->process = NULL; //@todo: Definir à qual processo a janela perence.




    // Linked list.
    //window->linkedlist = NULL;


//
// Navigation
//

    // Prev e next.
    window->prev = (void *) Parent;
    window->next = NULL;


    //==========


    //Exemplos de tipos de janelas, segundo MS.	
    //Overlapped Windows
    //Pop-up Windows
    //Child Windows
    //Layered Windows
    //Message-Only Windows

	//
	// Preparando os parametros da pintura de acordo com o tipo.
	//

	// De acordo com o tipo de janela, preparamos a configuração
	// e a própria rotina create window está pintando.
	// Porém nesse momento o 'case' pode chamar rotinas de pintura em 
	// draw.c e retornar. 
	// CreateWindow é para criar a moldura principal ...
	// para so outros tipos de janelas podemos usar draw.c
	// pois quando chamarmos draw.c a estrutura de janela ja deve estar 
	// inicializada.
	// Rotinas grandes como pintar um barra de rolagem podem ficar em draw.c

	// #importante
	// Deveria ter uma variável global indicando o tipo de 
	// design atual, se é 3D ou flat.

	// Configurando os elementos de acordo com o tipo.
	// @todo: Salvar as flags para os elementos presentes
	// na estrutura da janela.


//
// Flags
//

    // Initializing the flag for all the elements.
    // not used by default.

    window->shadowUsed     = FALSE;  //  1
    window->backgroundUsed = FALSE;  //  2
    window->titlebarUsed   = FALSE;  //  3
    window->controlsUsed   = FALSE;  //  4
    window->borderUsed     = FALSE;  //  5  
    window->menubarUsed    = FALSE;  //  6 
    window->toolbarUsed    = FALSE;  //  7
    window->clientAreaUsed = FALSE;  //  8
    window->scrollbarUsed  = FALSE;  //  9
    window->statusbarUsed  = FALSE;  // 10

//
// Element style
//

    // Initialize style indicators.

    window->shadow_style     = 0;
    window->background_style = 0;
    window->titlebar_style   = 0;
    window->controls_style   = 0;
    window->border_style     = 0;
    window->menubar_style    = 0;
    window->toolbar_style    = 0;
    window->clientarea_style = 0;
    window->scrollbar_style  = 0;
    window->statusbar_style  = 0;

//
// Elements
//

    // Selecting the elements given the type.
    // Each type has it's own elements.

    switch (type){

        // Simple window. (Sem barra de títulos).
        case WT_SIMPLE:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = FALSE;
            Background = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;

        // Edit box. (Simples + borda preta).
        // Editbox não tem sombra, tem bordas. 
        case WT_EDITBOX:
            window->ip_device = IP_DEVICE_KEYBOARD;
            window->frame.used = TRUE;
            Background = TRUE;
            Border     = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;

        // Overlapped. (completa, para aplicativos).
        // Sombra, bg, título + borda, cliente area ...
        // #obs: Teremos recursividade para desenhar outras partes.
        case WT_OVERLAPPED:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = TRUE;
            Shadow         = TRUE;
            Background     = TRUE;
            ClientArea     = TRUE;
            MinimizeButton = TRUE;
            MaximizeButton = TRUE;
            CloseButton    = TRUE; 
            window->shadowUsed     = TRUE;
            window->backgroundUsed = TRUE;
            window->titlebarUsed   = TRUE;
            window->controlsUsed   = TRUE;
            window->clientAreaUsed = TRUE;
            window->background_style = 0;
            break;

        // Popup. (um tipo de overlapped mais simples).
        case WT_POPUP:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = FALSE;
            Shadow     = TRUE;
            Background = TRUE;
            window->shadowUsed     = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;
  
        // Check box. (Simples + borda preta).
        // Caixa de seleção. Caixa de verificação. Quadradinho.
        // #todo: checkbox has borders.
        case WT_CHECKBOX:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = FALSE;
            Background = TRUE;
            Border     = TRUE;
            window->backgroundUsed = TRUE;
            // #todo: structure element for 'border'
            window->background_style = 0;
            break;

        //case WT_SCROLLBAR:
            // Nothing for now.
            //break;

        // Only the bg for now.
        // #todo: Button has borders.
        case WT_BUTTON:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = TRUE;
            Background = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;

        // Status bar.
        // #todo: checkbox has borders sometimes.
        case WT_STATUSBAR:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = FALSE;
            Background = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;

        // Ícone da área de trabalho.
        // #todo: icons has borders sometimes.
        case WT_ICON:
            window->ip_device = IP_DEVICE_NULL;
            window->frame.used = FALSE;
            Background = TRUE;
            window->backgroundUsed = TRUE;
            window->background_style = 0;
            break;

        // barra de rolagem
        // botões de radio 
        // ...


        // #todo
        // #bugbug
        // We need to work on this case.
 
        default:
            debug_print("xxxCreateWindow: [DEBUG] default\n");
                 printf("xxxCreateWindow: [DEBUG] default\n");
            while(1){}
            //return NULL;
            break;
    };

    // #debug
    // while(1){}


//
// == Draw ========
//


	// Hora de pintar. 
	// Os elementos serão incluídos se foram 
	// selecionados anteriormente.


//drawBegin:

	// Obs: 
	// Se for uma janela, pintaremos apenas a janela.
	// Se for um frame, pintaremos todos os elementos
	// presentes nesse frame de acordo com as flags.
	// Obs:
	// A janela precisa ser pintada em seu buffer dedicado.
	// Nesse momento o buffer dedicado da janela já está na estutura
	// da janela. Rotinas de pintura que tenham acesso à estrutura da
	// janela devem utilizar o buffer dedicado que for indicado na estrutura.
	// Para que seja possível pintar uma janela em seu buffer dedicado,
	// devemos passar um ponteiro para a estrutura da janela em todas
	// as rotinas de pintura chamadas daqui pra baixo.
	// #todo: 
	// Passar estrutura de janela via argumento, para a rotina
	// de pintura ter acesso ao buffer dedicado.


	//if(DedicatedBuffer == 1){};


	// Se o view for igual NULL talvez signifique não pintar.
    if ( window->view == VIEW_NULL )
    {
		//#bugbug: fail.
		//window->show = 0;
		//window->redraw = 0;
		//return (void*) window;
    }


	// Minimized ? (Hide ?)
	// Se tiver minimizada, não precisa mostrar a janela, porém
	// é necessário pintar a janela no buffer dedicado, se essa técnica 
	// estiver disponível.
	// Talvez antes de retornarmos nesse caso seja necessário configurar 
	// mais elementos da estrutura.
	
	//#bugbug
	//se estamos contruindo a janela, então ela não foi registrada 
	//não podemos checar as coisas na estrutura ainda,
	//mas a estrutura ja existe a algumas coisas foram inicializadas.
	
	// #importante
	// Pois retornaremos no caso de janelas minimizadas.
	// Provavelmente isso foi usado quando criamos janelas 
	// de referência na inicialização da GUI.(root)

    /*
    Minimized = 0;
    Minimized = (int) is_window_minimized (window);

    if ( Minimized == 1 )
    {
		//window->draw = 1; //Devemos pintála no buffer dedicado.
		window->show = 0;
		window->redraw = 0;
		//...
		
		//@todo: Não retornar. 
		//como teste estamos retornando.
		
		goto done;
	    //return (void *) window;
    }
     */


	// #todo: 
	// Maximized ?
	// Para maximizar devemos considerar as dimensões da área de cliente
	// da janela mãe.
	// Se a jenela estiver maximizada, então deve ter o tamanho da área de 
	// cliente da janela main.
	// Essa área de cliente poderá ser a área de trabalho, caso a
	// janela mãe seja a janela principal.
	// Obs: se estiver maximizada, devemos usar as dimensão e coordenadas 
	// da janela gui->main.
	
	// #bugbug
	// Temos um problema com essa limitação.
	// Não conseguimos pintar janelas simples além do height da janela gui->main
	// para janelas overlapped funciona.
	
	/*
    Maximized = 0;
    Maximized = (int) is_window_maximized (window);

    if ( Maximized == 1 )
    {
		//#debug
		printf("file: createw.c: #debug\n");
		printf ("original: l=%d t=%d w=%d h=%d \n", 
		    window->left, gui->main->top, 
		    window->width, window->height );
		
		//Margens da janela gui->main
        window->left = gui->main->left;    
        window->top  = gui->main->top;

		//Dimensões da janela gui->main.
        window->width  = gui->main->width;
        window->height = gui->main->height; 
        
        window->right = (unsigned long) window->left + window->width;
        window->bottom = (unsigned long) window->top  + window->height;       

		// ??
		// Deslocamentos em relação às margens.
		// Os deslocamentos servem para inserir elementos na janela, 
		// como barras, botões e textos.
		window->x = 0;
        window->y = 0;
		
		//#debug
		printf ("corrigido: l=%d t=%d w=%d h=%d \n", 
		    window->left, gui->main->top, 
		    window->width, window->height );
		
		//#debug
		refresh_screen ();
		while (1){}
	}
    */


// =================================
//  # Shadow
//
// Sombra:
//     A sombra pertence à janela e ao frame.
//     A sombra é maior que a própria janela.
//     ?? Se estivermos em full screen não tem sombra ??

// ========
// 1
    if ( Shadow == TRUE )
    {
        window->shadowUsed = TRUE;

		//CurrentColorScheme->elements[??]
		
		//@todo: 
		// ?? Se tiver barra de rolagem a largura da 
		// sombra deve ser maior. ?? Não ...
		//if()
		
        // @todo: Adicionar a largura das bordas verticais 
		// e barra de rolagem se tiver.
		// @todo: Adicionar as larguras das 
		// bordas horizontais e da barra de títulos.
		// Cinza escuro.  CurrentColorScheme->elements[??] 
		// @TODO: criar elemento sombra no esquema. 

        if ( (unsigned long) type == WT_OVERLAPPED )
        {
            if (window->focus == 1){ __tmp_color = xCOLOR_GRAY1; }
            if (window->focus == 0){ __tmp_color = xCOLOR_GRAY2; }

            //ok funciona
            //rectBackbufferDrawRectangle ( window->left +1, window->top +1, 
            //    window->width +1 +1, window->height +1 +1, 
            //    __tmp_color, 1 ); 
         
            //test
            //remeber: the first window do not have a parent.
            if ( (void*) Parent == NULL )
            { 
                gwssrv_debug_print ("xxxCreateWindow: [Shadow] Parent\n"); 

                rectBackbufferDrawRectangle ( 
                    (window->left +1),     (window->top +1), 
                    (window->width +1 +1), (window->height +1 +1), 
                    __tmp_color, TRUE,
                    rop_flags );   //rop_flags 
            }

            if ( (void*) Parent != NULL )
            {
                rectBackbufferDrawRectangle ( 
                    (window->left +1),     (window->top +1), 
                    (window->width +1 +1), (window->height +1 +1), 
                    __tmp_color, TRUE,
                    rop_flags );  //rop_flags 
            }

            window->shadow_color = __tmp_color;
        }

        // E os outros tipos, não tem sombra ??
   
        // Os outros tipos devem ter escolha para sombra ou não ??
        // Flat design pode usar sombra para definir se o botão 
        // foi pressionado ou não.
        // ...
    }


// ===============================================
// ## Background ##
// Background para todo o espaço ocupado pela janela e pelo seu frame.
// O posicionamento do background depende do tipo de janela.
// Um controlador ou um editbox deve ter um posicionamento relativo
// à sua janela mãe. Já uma overlapped pode ser relativo a janela 
// gui->main ou relativo à janela mãe.

// ========
// 2
    if ( Background == TRUE )
    {
        window->backgroundUsed = TRUE;

        // Select background color.
        switch (type){
            case WT_SIMPLE:
            case WT_POPUP:
            case WT_EDITBOX:
            case WT_CHECKBOX:
            case WT_ICON:
            case WT_BUTTON:
                window->bg_color = color;
                break;
            default:
                //#fixme
                window->bg_color = COLOR_PINK;  //BLACK?
                //window->bg_color = CurrentColorScheme->elements[csiWindowBackground]; 
                break;
        };

        // Se a janela for edibox, vamos colocar ela 
        // dentro da área de cliente da janela mãe.
        // Editbox só pode existir dentro da área de cliente.
        if(type==WT_EDITBOX)
        {
            // agora seu posicionamento em relação a tela
            // também leva em conta o posicionamento 
            // da área de cliente da janela mãe
            if( (void*) window->parent != NULL ){
            window->left = (window->left + window->parent->rcClient.left);
            window->top  = (window->top  + window->parent->rcClient.top);
            }
        }


        // Draw 

        //#bugbug
        //Remember: The first window do not have a parent.

        if ( (void*) Parent == NULL )
        { 
            gwssrv_debug_print ("xxxCreateWindow: [Background] Parent\n"); 
            rectBackbufferDrawRectangle ( 
                    window->left, window->top, 
                    window->width, window->height, 
                    window->bg_color, TRUE,
                    rop_flags );  // rop_flags
        }  

        if ( (void*) Parent != NULL )
        {
            gwssrv_debug_print ("xxxCreateWindow: [Background] No Parent\n"); 
            rectBackbufferDrawRectangle ( 
                window->left, window->top, 
                window->width, window->height, 
                window->bg_color, TRUE,
                rop_flags );  //rop_flags
        }
    }

    //#debug
    //asm ("int $3");


// Client area

    if( ClientArea == TRUE )
    {
        window->rcClient.left   = (unsigned long) window->left;
        window->rcClient.top    = (unsigned long) window->height;
        window->rcClient.width  = (unsigned long) window->width;
        window->rcClient.height = (unsigned long) window->height;
    }


//
// == Button ====================
//

    // Termina de desenhar o botão, mas não é frame
    // é só o botão...
    // caso o botão tenha algum frame, será alguma borda extra.

    if ( (unsigned long) type == WT_BUTTON )
    {

        //border color
        //o conceito de status e state
        //está meio misturado. ja que estamos usando
        //a função de criar janela para criar botão.
        //#bugbug
    
        switch ( status )
        {
            case BS_FOCUS:
                buttonFocus = TRUE;
                buttonBorderColor1       = COLOR_BLUE;
                buttonBorderColor2       = COLOR_BLUE;
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLUE;
                break;

            case BS_PRESS:
                buttonSelected = TRUE;
                buttonBorderColor1       = COLOR_WHITE; 
                buttonBorderColor2       = xCOLOR_GRAY3;
                buttonBorderColor2_light = xCOLOR_GRAY5; 
                buttonBorder_outercolor  = COLOR_BLACK;
                break;

            case BS_HOVER:
                break;
                    
            case BS_DISABLED:
                buttonFocus = FALSE;
                buttonBorderColor1 = COLOR_GRAY;
                buttonBorderColor2 = COLOR_GRAY;
                buttonBorder_outercolor  = COLOR_GRAY;
                break;

            case BS_PROGRESS:
                break;

            case BS_DEFAULT:
            default: 
                buttonFocus = FALSE;
                buttonSelected = FALSE;
                buttonBorderColor1       = COLOR_WHITE;
                buttonBorderColor2       = xCOLOR_GRAY3; 
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLACK;
                break;
        };

        // center?
        size_t tmp_size = (size_t) strlen ( (const char *) windowname );
        unsigned long offset = 
            ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) gcharWidth) ) >> 1 );


        //#debug
        if ( (void*) Parent == NULL ){
            gwssrv_debug_print ("xxxCreateWindow: [WT_BUTTON] Parent NULL\n"); 
        }


//
// Paint button
//

        if ( (void*) Parent != NULL )
        {

            // #todo
            // Esses valores precisam estar na estrutura para
            // podermos chamar a rotina redraw para repintar 
            // as bordas do botao.

            __draw_buttom_borders(
                (struct gws_window_d *)window,
                (unsigned int) buttonBorderColor1,
                (unsigned int) buttonBorderColor2,
                (unsigned int) buttonBorderColor2_light,
                (unsigned int) buttonBorder_outercolor );

            // Button label

            if (buttonSelected == TRUE){
                grDrawString ( 
                    (window->left) +offset, 
                    (window->top)  +8, 
                    COLOR_WHITE, window->name );
            }

            if (buttonSelected == FALSE){
                grDrawString ( 
                    (window->left) +offset,  
                    (window->top)  +8,  
                    COLOR_BLACK, window->name );
            }
        }

      //todo
      // configurar a estrutura de botão 
      // e apontar ela como elemento da estrutura de janela.
      //window->button->?
    
    } //button



//done:

// Invalidate rectangle?
// Yes, but maybe the caller will build a frame for this window.
// #todo: Mas se a janela estiver minimizada, então não precisa invalidar.

    window->dirty = TRUE;

    return (void *) window;
}


/*
 ******************************************************
 * createwCreateWindow:
 * 
 *     It creates a window
 */

// Essa será a função que atenderá a chamada a
// esse é o serviço de criação da janela.
// talvez ampliaremos o número de argumentos

// #todo
// Precisamos de uma estrutura de janela que fique aqui
// no servidor.
// Não podemos usar a estrutura de janela
// da api.

// #todo: change name to 'const char *'

void *CreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status, 
    unsigned long view, 
    char *windowname, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktopid, 
    unsigned int clientcolor, 
    unsigned int color ) 
{

   struct gws_window_d  *__w;
   unsigned long __rop_flags=0;


    // This function is able to create some few 
    // kinds of windows for now:
    // overlapped, editbox, button and simple.

    int ValidType=FALSE;


    gwssrv_debug_print ("CreateWindow: :)\n");


// See:
// config.h, main.c
    if(config_use_transparency==TRUE)
    {
        __rop_flags = 12;
        //__rop_flags = 22;  // This is a test yet.
    }


//
// #todo: Colocar mascara nos valores passados via parâmetro.
//


    switch (type){
    case WT_OVERLAPPED:  ValidType=TRUE; break;
    case WT_EDITBOX:     ValidType=TRUE; break;
    case WT_BUTTON:      ValidType=TRUE; break;
    case WT_SIMPLE:      ValidType=TRUE; break;
    };

    if ( ValidType == FALSE ){
        gwssrv_debug_print ("CreateWindow: Invalid type\n");
        return NULL;
    }



    //1. Começamos criando uma janela simples
    //2. depois criamos o frame. que decide se vai ter barra de títulos ou nao.
    
    /*
    __w = (void *) CreateWindow ( type, status, view, (char *) windowname, 
                           x, y, width, height, 
                           (struct window_d *) pWindow, desktopid, clientcolor, color );  
    */
    
    // No caso dos tipos com moldura então criaremos em duas etapas.
    // no futuro todas serão criadas em duas etapas e 
    // CreateWindow será mais imples.
    
    
    // #todo
    // Check parent window validation.
    // APPLICATION window uses the screen margins for relative positions.
    //if ( (void*) pWindow == NULL ){}
    

    // #todo
    // check window name validation.
    //if ( (void*) windowname == NULL ){}
    //if ( *windowname == 0 ){}


// ============================
// Types with frame.

    // Overlapped
    if ( type == WT_OVERLAPPED )
    {
        if ( width < OVERLAPPED_MIN_WIDTH ) { width=OVERLAPPED_MIN_WIDTH; }
        if ( height < OVERLAPPED_MIN_HEIGHT ){ height=OVERLAPPED_MIN_HEIGHT; }

        __w = (void *) xxxCreateWindow ( 
                           WT_SIMPLE, 
                           style, 
                           status, 
                           view, 
                           (char *) windowname, 
                           x, y, width, height, 
                           (struct gws_window_d *) pWindow, 
                           desktopid, 
                           clientcolor, color, 
                           __rop_flags ); 

         if ( (void *) __w == NULL ){
             gwssrv_debug_print ("CreateWindow: xxxCreateWindow fail \n");
             return NULL;
         }

        // Pintamos simples, mas a tipagem será overlapped.
        __w->type = WT_OVERLAPPED;

        wm_add_window_into_the_list(__w);
        set_active_window(__w->id);

        goto draw_frame;
    }


    // #todo
    // It does not exist by itself. It needs a parent window.
    
    //edit box
    if ( type == WT_EDITBOX )
    {
        //if ( (void*) pWindow == NULL ){ return NULL; }

        if ( width < EDITBOX_MIN_WIDTH )  { width=EDITBOX_MIN_WIDTH; }
        if ( height < EDITBOX_MIN_HEIGHT ){ height=EDITBOX_MIN_HEIGHT; }

        // Podemos usar o esquema padrão de cores ...
        __w = (void *) xxxCreateWindow ( 
                           WT_SIMPLE, 0, status, view, 
                           (char *) windowname, 
                           x, y, width, height, 
                           (struct gws_window_d *) pWindow, 
                           desktopid, clientcolor, color, 0 ); 

         if ( (void *) __w == NULL ){
             gwssrv_debug_print ("CreateWindow: xxxCreateWindow fail \n");
             return NULL;
         }

        //pintamos simples, mas a tipagem será  overlapped
        __w->type = WT_EDITBOX;   
        goto draw_frame;
    }


    // #todo
    // It does not exist by itself. It needs a parent window.

    //button
    if ( type == WT_BUTTON )
    {
        gwssrv_debug_print ("[DEBUG]: CreateWindow WT_BUTTON\n");
      
        //if ( (void*) pWindow == NULL ){ return NULL; }

        if ( width < BUTTON_MIN_WIDTH )  { width=BUTTON_MIN_WIDTH; }
        if ( height < BUTTON_MIN_HEIGHT ){ height=BUTTON_MIN_HEIGHT; }

        // Podemos usar o esquema padrão de cores ...
        __w = (void *) xxxCreateWindow ( 
                           WT_BUTTON, 0, status, view, 
                           (char *) windowname, 
                           x, y, width, height, 
                           (struct gws_window_d *) pWindow, 
                           desktopid, clientcolor, color, 0 ); 

         if ( (void *) __w == NULL ){
             gwssrv_debug_print ("CreateWindow: xxxCreateWindow fail \n");
             return NULL;
         }

        //pintamos simples, mas a tipagem será  overlapped
        __w->type = WT_BUTTON;   
        goto draw_frame;
    }


// ============================
// Types with no frame!

    if ( type == WT_SIMPLE )
    {
        __w = (void *) xxxCreateWindow ( 
                           WT_SIMPLE, 0, status, view, 
                           (char *) windowname, 
                           x, y, width, height, 
                           (struct gws_window_d *) pWindow, 
                           desktopid, clientcolor, color, 0 );  

         if ( (void *) __w == NULL ){
             gwssrv_debug_print ("CreateWindow: xxxCreateWindow fail \n");
             return NULL;
         }

        __w->type = WT_SIMPLE;
        return (void *) __w;
    }

//type_fail:

    gwssrv_debug_print ("CreateWindow: [FAIL] type \n");
    return NULL;
    
    
//
// == Draw frame ===============================
//

// #todo:
// Lembrando que frame é coisa do wm.
// Porém tem algumas coisas que o window server faz,
// como as bordas de um editbox.

draw_frame:

    // #IMPORTANTE
    // DESENHA O FRAME DOS TIPOS QUE PRECISAM DE FRAME.
    // OVERLAPED, EDITBOX, CHECKBOX ...

    // draw frame.
    // #todo:
    // Nessa hora essa rotina podera criar a barra de títulos.
    // o wm poderá chamar a rotina de criar frame.
    // See: window.c

    if ( type == WT_OVERLAPPED || 
         type == WT_EDITBOX || 
         type == WT_BUTTON )
    {
        
        // #todo
        // if __w is valid.
        
        // #todo
        // __w->useStandardFrame

        // See: wm.c

        if ( (void*) __w != NULL )
        {
            wmCreateWindowFrame ( 
                (struct gws_window_d *) pWindow,  //parent.
                (struct gws_window_d *) __w,      //bg do botão em relação à sua parent. 
                METRICS_BORDER_SIZE,       //border size
                (unsigned int)COLOR_BLACK, //border color 1
                (unsigned int)COLOR_BLACK, //border color 2
                (unsigned int)COLOR_BLACK, //border color 3
                (unsigned int)COLOR_BLACK, //ornament color 1
                (unsigned int)COLOR_BLACK, //ornament color 2
                1 );  //style
        }
    }


//
// z order for overlapped.
//


    // Quando criamos uma overlapped, ela deve vicar no topo da pilha.
    if ( type == WT_OVERLAPPED )
    {
        // #bugbug
        // refaz a lista de zorder...
        // somente com overlalled
        //reset_zorder();
        
        // #bugbug isso nao eh bom.
        //invalidate parent, if present
        //invalidate_window(__w->parent);

        // coloca a nova janela no topo.
        __w->zIndex = ZORDER_TOP;
        zList[ZORDER_TOP] = (unsigned long) __w;
    }


// ===============

//
// level
//

// #test
    
    if ( (void*) pWindow != NULL )
    {
        __w->level = (pWindow->level + 1);
    }

    if ( (void*) pWindow == NULL )
    {
        __w->level = 0;
    }

    // ===============


// Invalidate the window rectangle.

    __w->dirty = TRUE;

//done:
    gwssrv_debug_print ("CreateWindow: done\n");
    return (void *) __w;
}



/*
 *******************************************************
 * RegisterWindow: 
 *     Register a window.
 */
 
// OUT:
// < 0 = fail.
// > 0 = Ok. (index)
 
int RegisterWindow(struct gws_window_d *window)
{
    //loop
    register int __slot=0;
    
    struct gws_window_d *tmp; 



    if ( (void *) window == NULL )
    {
        //gws_debug_print ("RegisterWindow: window struct\n");
        return (int) -1;
    }


	// Contagem de janelas e limites.
	// (é diferente de id, pois id representa a posição
	// da janela na lista de janelas).

    windows_count++;

    if ( windows_count >= WINDOW_COUNT_MAX ){
        //gws_debug_print ("RegisterWindow: Limits\n");
        printf ("RegisterWindow: Limits\n");
        return -1;
    }


    // Search for empty slot
    for (__slot=0; __slot<1024; ++__slot)
    {
        tmp = (struct gws_window_d *) windowList[__slot];

        // Found!
        if ( (void *) tmp == NULL )
        {
            windowList[__slot] = (unsigned long) window; 
            window->id = (int) __slot;
            
            return (int) __slot;
        }
    };

// fail
    //gwssrv_debug_print("No more slots\n");
    return (int) (-1);
}


/*
 *****************************************
 * serviceCreateWindow:
 *
 *     Create a window.
 *     Service: GWS_CreateWindow.
 *     It's a wrapper.
 *     Chamaremos a função que cria a janela com base 
 * nos argumentos que estão no buffer, que é uma variável global 
 * nesse documento.
 */

// Called by gwsProcedure in main.c

// #todo
// Receive the tid of the client in the request packet.
// Save it as an argument of the window structure.

int serviceCreateWindow (int client_fd)
{


// #test
// The structure for the standard request.

    gReq r;


    //loop
    register int i=0;

    // The buffer is a global variable.
    unsigned long *message_address = (unsigned long *) &__buffer[0];

    struct gws_window_d *Window;
    struct gws_window_d *Parent;
    int pw=0;

    int id = -1;

    // Arguments.
    unsigned long x=0;
    unsigned long y=0;
    unsigned long w=0;
    unsigned long h=0;
    unsigned int color=0;
    unsigned long type=0;


// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF );
    unsigned long deviceHeight = (__device_height & 0xFFFF );


    // tid da control thread do cliente.
    int ClientPID = -1;
    int ClientTID = -1;


    gwssrv_debug_print ("serviceCreateWindow:\n");
    //printf ("serviceCreateWindow:\n");

//
// Get the arguments.
//


// The header.
// 0,1,2,3

    r.wid  = message_address[0];  // window id
    r.code = message_address[1];  // message code
    r.ul2  = message_address[2];  // data1
    r.ul3  = message_address[3];  // data2

// l,t,w,h

    r.ul4 = message_address[4];
    r.ul5 = message_address[5];
    r.ul6 = message_address[6];
    r.ul7 = message_address[7];

    x = (unsigned long) (r.ul4 & 0xFFFF);
    y = (unsigned long) (r.ul5 & 0xFFFF);
    w = (unsigned long) (r.ul6 & 0xFFFF);
    h = (unsigned long) (r.ul7 & 0xFFFF);

// Background color.
    r.ul8 = message_address[8];
    color = (unsigned int) (r.ul8 & 0x00FFFFFF );

// type
    r.ul9 = message_address[9];
    type = (unsigned long) (r.ul9 & 0xFFFF);


// Parent window ID.
    r.ul10 = message_address[10]; 
    pw = (int) (r.ul10 & 0xFFFF); 

//#test
    unsigned long my_style=0;
    r.ul11 = message_address[11];  
    my_style = (unsigned long) r.ul11;  // 8 bytes  

// Client pid
    r.ul12 = message_address[12];  // client pid
    ClientPID = (int) (r.ul12 & 0xFFFF);

// Client caller tid
    r.ul13 = message_address[13];  // client tid
    ClientTID = (int) (r.ul13 & 0xFFFF);

//========
// 14:
// This is the start of the string passed via message.
// Copy 256 bytes of given string.
// Do we ha a limit?

//++
// String support 
// Copiando para nossa estrutura local.
    int string_off = 14; 
    for (i=0; i<256; ++i)
    {
        r.data[i] = message_address[string_off];
        string_off++;
    };
    r.data[i] = 0;
//--

// ========================================

    // #debug
    //printf ("serviceCreateWindow: pid=%d tid=%d *breakpoint\n", 
    //    ClientPID, ClientTID );
    //while(1){}


//
// Purify
//
    x = (x & 0xFFFF);
    y = (y & 0xFFFF);
    w = (w & 0xFFFF);
    h = (h & 0xFFFF);

// Final Limits

    //if( x >= deviceWidth )
        //return -1;

    //if( y >= deviceHeight )
        //return -1;


    //#debug
    //printf("%s\n",name_buffer);
    //while(1){}

//===================

    // Limits
    if (pw<0 || pw>=WINDOW_COUNT_MAX)
    {
        gwssrv_debug_print("serviceCreateWindow: parent window id fail\n");
        pw=0;
        exit(1);
    }

    // Get parent window structure pointer.
    Parent = (struct gws_window_d *) windowList[pw];

    // #bugbug
    // Ajuste improvidsado
    if ( (void *) Parent == NULL )
    {
        gwssrv_debug_print ("serviceCreateWindow: parent window struct fail\n");
        
        
        // #bugbug
        // #todo: panic here.
        if ( (void*) gui == NULL )
        {
            gwssrv_debug_print ("serviceCreateWindow: gui fail\n");
            //exit(1);
        }
        
        //if ( (void*) gui != NULL ) )
        //{
        //    Parent = gui->screen_window;
        //}
        
        //  #bugbug
        //  This is a test.
        exit(1); 
    }

//
// Draw
//

// See:
// createw.c

    Window = (struct gws_window_d *) CreateWindow ( 
                                         type, 
                                         my_style,     // style
                                         1,     // status 
                                         1,     // view
                                         r.data, 
                                         x, y, w, h, 
                                         Parent, 0, 
                                         COLOR_PINK, color ); 

    if ( (void *) Window == NULL )
    {
       gwssrv_debug_print ("gwssrv: CreateWindow fail\n");
       next_response[1] = 0;
       return -1;
    }

// Register window
    id = RegisterWindow(Window);
    if (id<0){
        gwssrv_debug_print ("gwssrv: serviceCreateWindow Couldn't register window\n");
        next_response[1] = 0;  // msg code.
        return -1;
    }


//===============================================

//
// The client!
//

    Window->client_fd = (int) client_fd;

//
// Input queue
// 

// Initialized the input queue

/*
    // #deprecated ...
    // Enviaremos mensagens para a fila na thread
    // associada a essa janela.
    // Indicado em client_tid.

    //int i=0;
    for ( i=0; i<32; ++i )
    {
        Window->window_list[i] = 0;
        Window->msg_list[i]    = 0;
        Window->long1_list[i]  = 0;
        Window->long2_list[i]  = 0;
        Window->long3_list[i]  = 0;
        Window->long4_list[i]  = 0;
    };
    Window->head_pos = 0;
    Window->tail_pos = 0;
*/
//===============================================


//#test

// prepara o nome
    int name_len = strlen(Window->name);
    if(name_len > 32){ name_len = 32; }
    char w_name[64];
    sprintf(w_name,":: ");
    strncat(w_name,Window->name,name_len);
    w_name[63]=0;


    if(Window->type == WT_OVERLAPPED)
    {
        // coloca na fila
        wm_add_window_into_the_list(Window);
    
        // #test: notifica na barra de tarefas
        wm_Update_TaskBar((char *) w_name);
    }





// #test
// Save the tid of the client.
    
    Window->client_pid = ClientPID;
    Window->client_tid = ClientTID;

    //
    // The client's fd.
    //

    // #todo
    // We need to register the client's fd.
    // It is gonna be used to send replies, just like
    // input events.
    
    // Window->client_fd = ?;


    // Building the next response.
    // It will be sent in the socket loop.

    next_response[0] = (unsigned long) id;        // window
    next_response[1] = SERVER_PACKET_TYPE_REPLY;  // msg code
    next_response[2] = 0;
    next_response[3] = 0;



//
// Show
//

    // #todo: We need a flag here. Came from parameters.
    // if( Show == TRUE )
    gws_show_window_rect(Window);

    gwssrv_debug_print ("serviceCreateWindow: done\n");

    return 0;
}



struct gws_rect_d *clientrect_from_window(struct gws_window_d *window)
{
    struct gws_rect_d *rect;

    if ( (void*) window == NULL )
        return NULL;

    if (window->used != TRUE)
        return NULL;

    if (window->magic != 1234)
        return NULL;

    rect = (struct gws_rect_d *) &window->rcClient;

    return (struct gws_rect_d *) rect;
}

struct gws_rect_d *rect_from_window(struct gws_window_d *window)
{
    struct gws_rect_d *rect;

    if ( (void*) window == NULL )
        return NULL;

    if (window->used != TRUE)
        return NULL;

    if (window->magic != 1234)
        return NULL;

    rect = (struct gws_rect_d *) &window->rcWindow;

    return (struct gws_rect_d *) rect;
}




//
// End.
//



