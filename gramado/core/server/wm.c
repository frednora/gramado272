/*
 * File: wm.c 
 *
 *     The Window Manager.
 *
 * History:
 *     2020 - Create by Fred Nora.
 */


#include <gws.h>


#define WM_DEFAULT_BACKGROUND_COLOR   COLOR_GRAY


// Local structure

struct wm_d
{

// Background
    unsigned int default_background_color;
    int has_custom_background_color;
    unsigned int custom_background_color;
    // image?

// Wallpaper
    int has_wallpaper;

// Window stack
// Quando uma janela foi invalidada, significa que ela foi pintada e que
// precisa receber o refesh, mas também pode significar
// que outras janelas foram afetadas pelo pintura da janela.
// Nesse caso precisado repintar a janelas.
// Se tiver uma janela em fullscreen então pintamos, invalidamos
// seu retângulo e validamos todos os outros.

    // struct gws_window_d *fullscreen_window;

// #test
// z-order for all the layers.
// linked list
    //struct gws_window_d *layer1_list;
    //struct gws_window_d *layer2_list;
    //struct gws_window_d *layer3_list;
    //struct gws_window_d *layer4_list;
};

struct wm_d  WindowManager;


//#todo
//GetWindowRect
//GetClientRect


static unsigned long ____old_time=0;
static unsigned long ____new_time=0;



void __set_default_background_color( int color )
{
    WindowManager.default_background_color = (unsigned int) color;
}

unsigned int __get_default_background_color(void)
{
    return (unsigned int) WindowManager.default_background_color;
}


void __set_custom_background_color( int color )
{
    WindowManager.custom_background_color = (unsigned int) color;
    WindowManager.has_custom_background_color = TRUE;
}

unsigned int __get_custom_background_color(void)
{
    return (unsigned int) WindowManager.custom_background_color;
}

int __has_custom_background_color(void)
{
    if ( WindowManager.has_custom_background_color == TRUE )
        return TRUE;

    return FALSE;
}

int __has_wallpaper(void)
{
    if ( WindowManager.has_wallpaper == TRUE )
        return TRUE;

    return FALSE;
}



// Internal
// Called by wm_process_windows().

void __update_fps(void)
{

    debug_print ("__update_fps:\n");

    // counter
    frames_count++;

    //
    // == time =========================================
    //
    
    // delta
    unsigned long dt=0;
    ____new_time = rtl_get_progress_time();
    dt = (unsigned long) (____new_time - ____old_time);

//===================================================================
// ++  End

    //t_end = rtl_get_progress_time();
    //__refresh_rate =  t_end - t_start;
    //__refresh_rate = __refresh_rate/1000;
    //printf ("@ %d %d %d \n",__refresh_rate, t_now, t_old);


    //====================================
    // fps++
    // conta quantos frames. 
    char rate_string[32];
    // se passou um segundo.
    if ( dt > 1000 )
    {
        // Save old time.
        ____old_time = ____new_time;
        
        fps = frames_count; // quantos frames em 1000 ms aproximadamente?
        itoa(fps,rate_string); 

        if ( show_fps_window == TRUE ){
            yellow_status(rate_string);
        }

        // Clean for next round.
        frames_count=0;
        fps=0;
        dt=0;
    }
    //fps--
    //=======================
}


/*
 ********************************
 * wmDrawFrame:
 * 
 */

// Called by gwsCreateWindow in createw.c

// #importante:
// Essa rotina será chamada depois que criarmos uma janela básica,
// mas só para alguns tipos de janelas, pois nem todos os tipos 
// precisam de um frame. Ou ainda, cada tipo de janela tem um 
// frame diferente. Por exemplo: Podemos considerar que um checkbox 
// tem um tipo de frame.
// Toda janela criada pode ter um frame.
// Durante a rotina de criação do frame para uma janela que ja existe
// podemos chamar a rotina de criação da caption bar, que vai criar os
// botões de controle ... mas nem toda janela que tem frame precisa
// de uma caption bar (Title bar).
// Estilo do frame:
// Dependendo do estilo do frame, podemos ou nao criar a caption bar.
// Por exemplo: Uma editbox tem um frame mas não tem uma caption bar.


// IN:
// parent = parent window ??
// window = The window where to build the frame.
// x
// y
// width
// height
// style = Estilo do frame.

// OUT:
// 0   = ok, no erros;
// < 0 = not ok. something is wrong.

int 
wmDrawFrame ( 
    struct gws_window_d *parent,
    struct gws_window_d *window,
    unsigned long border_size,
    unsigned int border_color1,
    unsigned int border_color2,
    unsigned int border_color3,
    unsigned int ornament_color1,
    unsigned int ornament_color2,
    int style ) 
{

    int useFrame       = FALSE;
    int useTitleBar    = FALSE;
    int useTitleString = FALSE;
    int useBorder      = FALSE;
    int useIcon        = FALSE;
    // ...


// #bugbug
// os parâmetros 
// parent, 
// x,y,width,height 
// não estão sendo usados.


// Overlapped.
// Janela de aplicativos.

// Title bar.
    struct gws_window_d  *tbWindow;
// Status bar.
    struct gws_window_d  *sbWindow;


    int id=-1;  //usado pra registrar janelas filhas.

    int Type=0;

    unsigned long BorderSize = (border_size & 0xFFFF);

    unsigned int  BorderColor1 = border_color1;
    unsigned int  BorderColor2 = border_color2;
    unsigned int  BorderColor3 = border_color3;
 
    unsigned int  BorderColor = border_color1;

    unsigned int OrnamentColor1 = ornament_color1;
    unsigned int OrnamentColor2 = ornament_color2;


    unsigned int TitleBarColor = COLOR_BLUE1;   // Light blue (Um pouco fosco) 
    //unsigned long TitleBarColor = 0x001473E6;  // Claro.  
    //unsigned long TitleBarColor = 0x00000E80;  // Dark blue
    // ...

    //unsigned long X = (x & 0xFFFF);
    //unsigned long Y = (y & 0xFFFF);
    //unsigned long Width = (width & 0xFFFF);
    //unsigned long Height = (height & 0xFFFF);


    gwssrv_debug_print ("wmDrawFrame:\n");


// #todo
// Se estamos minimizados ou a janela mãe está minimizada,
// então não temos o que pintar.

// #todo
// O estilo de frame é diferente se estamos em full screen ou maximizados.
// não teremos bordas laterais

// #todo
// Cada elemento da frame que incluimos, incrementa
// o w.top do retângulo da área de cliente.

    // #todo
    // check parent;
    //if ( (void*) parent == NULL ){}

// #todo
// check window.

    if ( (void*) window == NULL )
    {
        gwssrv_debug_print ("wmDrawFrame: [FAIL] window\n");
        return -1;
    }

    if (window->used != TRUE || window->magic != 1234 )
        return -1;

// #bugbug
// Estamos mascarando pois os valores anda corrompendo.

    window->left   = (window->left   & 0xFFFF);
    window->top    = (window->top    & 0xFFFF);
    window->width  = (window->width  & 0xFFFF);
    window->height = (window->height & 0xFFFF);


    // #todo
    // Desenhar o frame e depois desenhar a barra de títulos
    // caso esse estilo de frame precise de uma barra.

    // Editbox
    // EDITBOX NÃO PRECISA DE BARRA DE TÍTULOS.
    // MAS PRECISA DE FRAME ... QUE SERÃO AS BORDAS.
    
//
// Type
//

// Qual é o tipo da janela em qual precisamos
// criar o frame. Isso indica o tipo de frame.

    Type = window->type;

    switch (Type){
    
    case WT_EDITBOX:     
        useFrame=TRUE; 
        useIcon=FALSE;
        useBorder=TRUE;
        break;
    
    case WT_OVERLAPPED:  
        useFrame=TRUE; 
        useIcon=TRUE;
        useTitleBar=TRUE;  // Normalmente uma janela tem a barra de t[itulos.
        if ( window->style & 0x0001 ){ useTitleBar=FALSE; }    //maximized
        //if ( window->style & 0x0004 ){ useTitleBar=FALSE; }  //fullscreen
        useTitleString=TRUE;
        useBorder=TRUE;
        break;
    
    case WT_BUTTON:      
        useFrame=TRUE;
        useIcon=FALSE; 
        break;
    };

    if ( useFrame == FALSE ){
        gwssrv_debug_print ("wmDrawFrame: [ERROR] This type does not use a frame.\n");
        return -1;
    }


    // ===============================================
    // editbox
    
    if ( Type == WT_EDITBOX )
    {

        // #todo
        // The window structure has a element for border size
        // and a flag to indicate that border is used.
        // It also has a border style.

        // Se tiver o foco.
        if ( window->focus == TRUE ){
            BorderColor = COLOR_BLUE;
            BorderSize = 4;
        }else{
            BorderColor = COLOR_BLACK;  // COLOR_INACTIVEBORDER;
            BorderSize = 2;
        };
        
        window->border_size = 0;
        window->borderUsed = FALSE;
        if (useBorder==TRUE){
            window->border_color = BorderColor;
            window->border_size  = BorderSize;
            window->borderUsed   = TRUE;
        }

        // Draw the border of an edit box.

        // board1, borda de cima e esquerda.
        rectBackbufferDrawRectangle( 
            window->left, window->top, 
            window->width, window->border_size, 
            window->border_color, TRUE,0 );

        rectBackbufferDrawRectangle( 
            window->left, window->top, 
            window->border_size, window->height, 
            window->border_color, TRUE,0 );

        // board2, borda direita e baixo.
        rectBackbufferDrawRectangle( 
            (window->left + window->width - BorderSize), window->top,  
            window->border_size, window->height, 
            window->border_color, TRUE,0 );

        rectBackbufferDrawRectangle ( 
            window->left, (window->top + window->height - window->border_size), 
            window->width, window->border_size, 
            window->border_color, TRUE,0 );

        // ok
        return 0;
    }


// ===============================================
// overlapped

    // string at center?
    size_t tmp_size = (size_t) strlen ( (const char *) window->name );
    unsigned long offset = 
        ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) gcharWidth) ) / 2 );

    if ( Type == WT_OVERLAPPED )
    {

        // #todo
        // Maybe we nned border size and padding size.
        
        // Consistente para overlapped.
        BorderSize = METRICS_BORDER_SIZE;
        // ...
        
        // #todo
        // The window structure has a element for border size
        // and a flag to indicate that border is used.
        // It also has a border style.

        // Se tiver o foco.
        if ( window->focus == TRUE ){
            BorderColor = COLOR_BLUE1;
        }else{
            BorderColor = COLOR_INACTIVEBORDER;
        };

        window->border_size = 0;
        window->borderUsed = FALSE;
        if (useBorder==TRUE){
            window->border_color = BorderColor;
            window->border_size  = BorderSize;
            window->borderUsed   = TRUE;
        }


        // Quatro bordas de uma janela overlapped.
         
        // board1, borda de cima e esquerda.
        rectBackbufferDrawRectangle( 
            parent->left + window->left, parent->top + window->top, 
            window->width, window->border_size, 
            window->border_color, TRUE,0 );

        rectBackbufferDrawRectangle( 
            parent->left + window->left, parent->top + window->top, 
            window->border_size, window->height, 
            window->border_color, TRUE,0 );

        //board2, borda direita e baixo.
        rectBackbufferDrawRectangle( 
            (parent->left + window->left + window->width - window->border_size), (parent->top + window->top), 
            window->border_size, window->height, 
            window->border_color, TRUE,0 );

        rectBackbufferDrawRectangle ( 
            (parent->left + window->left), (parent->top + window->top + window->height - window->border_size), 
            window->width, window->border_size, 
            window->border_color, TRUE,0 );

        //
        // Title bar
        //

        // #todo
        // The window structure has a flag to indicate that
        // we are using titlebar.
        // It also has a title bar style.
        // Based on this style, we can setup some
        // ornaments for this title bar.

        // #todo
        // Simple title bar.
        // We're gonna have a wm inside the window server.
        // The title bar will be very simple.
        // We're gonna have a client area.
        
        // #bugbug
        // Isso vai depender da resolução da tela.
        // Um tamanho fixo pode fica muito fino em uma resolução alta
        // e muito largo em uma resolução muito baixa.
        
        // Title bar
        // #todo: Essa janela foi registrada?
        if (useTitleBar == TRUE)
        {
            window->titlebar_height = 32;
            window->titlebar_color = TitleBarColor; //0x00AC81

            tbWindow = (void *) xxxCreateWindow ( 
                                    WT_SIMPLE, 0, 1, 1, "Titlebar", 
                                    BorderSize, 
                                    BorderSize, 
                                    (window->width - BorderSize - BorderSize), 
                                    window->titlebar_height, 
                                    (struct gws_window_d *) window, 
                                    0, window->titlebar_color, window->titlebar_color, 
                                    0 );   // rop_flags  

            if ( (void *) tbWindow == NULL ){
                gwssrv_debug_print ("wmDrawFrame: tbWindow fail \n");
                return -1;
            }
            tbWindow->type = WT_SIMPLE;
            window->titlebar = tbWindow;
            // Register window
            id = gwsRegisterWindow(tbWindow);
            if (id<0){
                gwssrv_debug_print ("wmDrawFrame: Couldn't register window\n");
                return -1;
            }
        }
        

        //
        // Status bar
        // 
        
        // Se for maximized ou fullscreen
        // #todo: Essa janela foi registrada?
        if ( window->style & 0x0008 )
        {
            window->statusbar_height = 32;
            window->statusbar_color = 0x00AC81;

            sbWindow = (void *) xxxCreateWindow ( 
                                WT_SIMPLE, 0, 1, 1, "Statusbar", 
                                BorderSize,                                                //left
                                (window->height - window->statusbar_height - BorderSize),  //top
                                (window->width - BorderSize - BorderSize),                 //width 
                                window->statusbar_height,                                  //height
                                (struct gws_window_d *) window, 
                                0, window->statusbar_color, window->statusbar_color, 
                                0 );   // rop_flags  
            if ( (void *) sbWindow == NULL ){
                gwssrv_debug_print ("wmDrawFrame: sbWindow fail \n");
                return -1;
            }
            sbWindow->type = WT_SIMPLE;
            window->statusbar = sbWindow;
            // Register window
            id = gwsRegisterWindow(tbWindow);
            if (id<0){
                gwssrv_debug_print ("wmDrawFrame: Couldn't register window\n");
                return -1;
            }
        }


        // Ornamento:
        // border on bottom.
        // Usado para explicitar se a janela é ativa ou não
        // e como separador entre a barra de títulos e a segunda
        // área da janela de aplicativo.
        // Usado somente por overlapped window.
        
        window->frame.ornament_color1   = OrnamentColor1;  //COLOR_BLACK;
        window->titlebar_ornament_color = OrnamentColor1;  //COLOR_BLACK;
        
        rectBackbufferDrawRectangle ( 
            tbWindow->left, ( (tbWindow->top) + (tbWindow->height) - METRICS_TITLEBAR_ORNAMENT_SIZE ),  
            tbWindow->width, METRICS_TITLEBAR_ORNAMENT_SIZE, 
            OrnamentColor1, TRUE,
            0 );  // rop_flags

        //
        // Icon
        //

        // #
        // O posicionamento em relação
        // à janela é consistente por questão de estilo.
        
        // See: bmp.c
        // IN: index, x, y.

        window->titlebarHasIcon = FALSE;
        window->frame.icon_id = 1;
        if( useIcon == TRUE ){
            gwssrv_display_system_icon( 
                (int) window->frame.icon_id, 
                (tbWindow->left + METRICS_ICON_LEFT), 
                (tbWindow->top  + METRICS_ICON_TOP) );
             window->titlebarHasIcon = TRUE;
         }

        //
        // string
        //
        
        window->titlebar_text_color = COLOR_WHITE;
        
        // #todo
        // Temos que gerenciar o posicionamento da string.
        
        // #bugbug: Use 'const char *'
        tbWindow->name = (char *) strdup ( (const char *) window->name );
        
        //#todo: validation
        //if ( (void*) tbWindow->name == NULL ){}
        
        if ( useTitleString == TRUE ){
            grDrawString ( 
                (tbWindow->left) + offset, 
                (tbWindow->top)  + 8, 
                COLOR_WHITE, 
                tbWindow->name );
        }

        //  control ?
        // ... 
        
        // ok
        return 0;
    }


    // ===============================================
    // button

    //button
    if ( Type == WT_BUTTON )
    {
        gwssrv_debug_print ("wmDrawFrame: [TODO] frame for button\n");
        
        //todo frame or not
        //just like the edit box.   
        
        // ok     
        return 0;
    }

    // ===============================================
    // more ... ??

    // ok
    return 0;
}



// =====================================
// The worker for compositor()
// + repinta as janelas.
// + rasterization.
// + raytracing.
// + Invalida todos os retângulos pintados bo backbuffer,
//   para que as rotinas de refresh façam o flush deles 
//   para o lfb no próximo passo. 

// called by compositor()
void wmCompositor(void)
{
    gwssrv_debug_print("wmCompositor: #todo\n");
}



void wm_flush_rectangle(struct gws_rect_d *rect)
{
    if( (void*) rect != NULL )
        gwssrv_refresh_this_rect(rect);
}

void wm_flush_window(struct gws_window_d *window)
{
    if( (void*) window != NULL )
        gws_show_window_rect(window);
}


void wm_flush_screen(void)
{
    gwssrv_show_backbuffer();
}


// Refresh screen via kernel.
// Copy the backbuffer in the frontbuffer(lfb).
// #??
// It uses the embedded window server in the kernel.
//#define	SYSTEMCALL_REFRESHSCREEN        11
// #todo
// trocar o nome dessa systemcall.
// refresh screen será associado à refresh all windows.

void gwssrv_show_backbuffer (void)
{
    gramado_system_call(11,0,0,0);
}


/*
 *************************************************************** 
 * wmRefreshDirtyRectangles: 
 *     X times per second.
 */

// O compositor deve ser chamado para compor um frame 
// logo após uma intervenção do painter, que reaje às
// ações do usuário.
// Ele não deve ser chamado X vezes por segundo.
// Quem deve ser chamado X vezes por segundo é a rotina 
// de refresh, que vai efetuar refresh dos retângulos sujos e
// dependendo da ação do compositor, o refresh pode ser da tela toda.

void wmRefreshDirtyRectangles(void)
{
    int __Dirty = -1;
    int background_status = -1;

// #debug
    //gwssrv_debug_print("wmRefreshDirtyRectangles:\n");

//
// == Check frame validation  ===========================
//

// Se algo foi modificado no frame.
// Nothing to do.
// Ok, we can return or sleep.
// Mostrar o status se ele estiver habilitado.
// O problema é que isso conta como frame também aqueles
// que não foram modificados. 
// ok, por enquanto.
// Temos que marcar mesmo quando não ha commits.
// na verdade devemos contar o fps por janela. 


    /*
    __Dirty = isdirty();
    if (__Dirty == FALSE)
    {
        //gwssrv_debug_print("wm_process_windows: [Not dirty] Nothing to do\n");
        //rtl_invalidate_window();
        __update_fps();
        return;
    }
    */

//===================================================================
// ++  Start

    //t_start = rtl_get_progress_time();
    
//
// == Dirty background ==============================
//

// Check the background validation.
// The background is dirty.
// Show the whole screen.
// #todo: update fps
// See:

    background_status = is_background_dirty();

    if (background_status == TRUE)
    {
        gws_show_backbuffer();
        
        validate_background();  // Validate the background.
        validate();             // Validate the frame.
        return;
    }



//
// == Update screen ====================
//

// Redrawing all the windows.
// redraw using zorder.
// refresh using zorder.
// Invalidating all the windows ... 
// and it will be flushed into the framebuffer for the ring0 routines.

    struct gws_window_d  *tmp;
    
    register int i=0;


//
// Update
//

   /*
// update message/hour/date ...
    if ( (void*) __root_window != NULL ) 
    {
        if ( __root_window->magic == 1234 )
        {
                //gwssrv_redraw_window(tmp,FALSE);  //bugbug
                grDrawString(
                    __root_window->left, 
                    __root_window->top,
                    COLOR_WHITE,
                    "Gramado OS");
               invalidate_window(__root_window);
               //__root_window->redraw = FALSE;
        }
    }
    */


// Is it time to update all the windows?

    int UpdateScreenFlag= FALSE;

    if(UpdateScreenFlag == TRUE)
    {

        // z-order ...
        //outra opção é lista encadeada ... see: WindowManager->layer1_list;
        for (i=0; i<ZORDER_MAX; ++i)
        {
            //tmp = (struct gws_window_d  *) zList[i];
            tmp = (struct gws_window_d  *) windowList[i];

            if ( (void*) tmp != NULL )
            {
                if ( tmp->used == TRUE && tmp->magic == 1234 )
                {
                    if ( tmp->dirty == TRUE ){
                        gws_show_window_rect(tmp);
                        tmp->dirty = FALSE;
                    }
                    
                    //if ( tmp->type == WT_OVERLAPPED )
                    //{
                        
                        //grDrawString(tmp->left,tmp->top,COLOR_GREEN,"x");
                        //invalidate_window(tmp);
                    //}
                    
                    // draw and invalidate.
                    //if (  tmp != __root_window )
                    //{
                        //gwssrv_redraw_window(tmp,FALSE);  //bugbug
                        //invalidate_window(tmp);
                    //}
                    
                    //if( tmp->id == window_with_focus ) 
                    //{
                        //grDrawString(tmp->left,tmp->top,COLOR_GREEN,"x");
                        //tmp->bg_color = COLOR_GREEN;
                        //gwssrv_redraw_window(tmp,FALSE);
                        //invalidate_window(tmp);
                    //}
                }
            }
        };
    }


//flush all the screens?
//Do we have more than one monitor?
    //for( ...
    //gws_show_backbuffer();

    // #todo
    // call a helper function for that.

    __update_fps();
    
    // Validate the frame.
    validate();
}   


void flush_frame(void)
{
    wm_flush_screen();
}


// #danger: Not tested yet.
// Repinda todas as janelas seguindo a ordem da lista
// que está em last_window.
void wm_update_desktop(void)
{
    struct gws_window_d *w;

    w = (struct gws_window_d *) first_window;

    while(1){
        
        if((void*)w==NULL){ break; }

        // Just draw, don't show.
        if( (void*) w != NULL )
        {
            gwssrv_redraw_window(w,FALSE);
            invalidate_window(w);
        }
        
        w = (struct gws_window_d *) w->next; 
    }; 
}


// not tested yet
void wm_add_window_into_the_list( struct gws_window_d *window)
{

    if( (void*) window == NULL )
        return;

    if ( window->used != TRUE )
        return;

    if ( window->magic != 1234 )
        return;

    if ( window->type != WT_OVERLAPPED )
        return;

// Se essa for a primeira janela da lista.
// Não usaremos a rootwindow, pois não é overlapped.
    if ( (void*) last_window == NULL )
    {
        first_window = (struct gws_window_d *) window;
        last_window  = (struct gws_window_d *) window;
        //activate
        set_active_window(window->id);
        return;
    }

// Não é a primeira.
    if ( (void*) last_window != NULL )
    {
        last_window->next = (struct gws_window_d *) window;
        last_window = (struct gws_window_d *)last_window->next;
        last_window->next = NULL;
        //activate
        set_active_window(window->id);
    }
}


// not tested yet
void wm_remove_window_from_list_and_kill( struct gws_window_d *window)
{
    struct gws_window_d *w;
    struct gws_window_d *pick_this_one;

    if( (void*) window == NULL )
        return;

    w = (struct gws_window_d *) first_window;

    if( (void*) w == NULL )
        return;

    while(1)
    {

        if( (void*) w == NULL )
            break;

        if(w==window)
        {
            // remove
            pick_this_one = (struct gws_window_d *) w;
            
            // Glue the list.
            w = w->next;
            
            // Kill
            pick_this_one->used = FALSE;
            pick_this_one->magic = 0;
            pick_this_one = NULL;
            break;
        }
        w = w->next;
    };
}


// ====================

// Local worker
void 
__draw_char_into_the_window(
    struct gws_window_d *window, 
    int ch )
{
// draw char support.
    unsigned char _string[4];


    if( (void*)window == NULL)
        return;
    if(window->magic!=1234)
        return;

   _string[0] = (unsigned char) ch;
   _string[1] = 0;


// types

    if( window->type == WT_OVERLAPPED )
        return;

    if( window->type == WT_SCROLLBAR )
        return;

    if( window->type == WT_STATUSBAR )
        return;

    if( window->type == WT_CHECKBOX )
        return;

    if( window->type == WT_BUTTON )
        return;

    // Isso pode receber char se tiver em modo de edição
    if( window->type == WT_ICON )
        return;

    if( window->type == WT_EDITBOX ||
        window->type == WT_EDITBOX_MULTIPLE_LINES )
    {
        dtextDrawText ( 
            (struct gws_window_d *) window,
            (window->ip_x*8), (window->ip_y*8), 
            COLOR_RED, (unsigned char *) &_string[0] );

        // x,y,w,h
        gws_refresh_rectangle ( 
            (window->left + (window->ip_x*8)), 
            (window->top  + (window->ip_y*8)), 
            8, 
            8 );

        window->ip_x++;
        if(window->ip_x >= window->width_in_bytes)
        {
            window->ip_y++; 
            window->ip_x=0;
        }   
   }
}


/*
//local worker
void __switch_window(void)
{
}
*/

// Local worker
void __switch_focus(void)
{
// Switch focus support.
    struct gws_window_d *next;

    if( window_with_focus < 0 )
    {
        window_with_focus=0;
        goto do_select;
    }

// Walk
    window_with_focus++;

    if ( window_with_focus >= WINDOW_COUNT_MAX )
    { 
        window_with_focus=0; 
    }

do_select: 

    next = (struct gws_window_d *) windowList[window_with_focus];

    if( (void*) next == NULL )
    {
        window_with_focus = 0;  //root
        next = __root_window;
    }

    if( (void*) next == NULL )
        return;

    if( next->magic != 1234 )
        return;

do_redraw: 

    gwssrv_redraw_window(next,TRUE);

    // Activate app window
    if( next->type == WT_OVERLAPPED )
    {
        active_window = next->id;
        return;
    }

    // focus
    next->focus = TRUE;
}


// local worker
// Colocaremos uma mensagem na fila de mensagens 
// da janela indicada via argumento.
int
__add_message_to_into_the_queue(
    struct gws_window_d *window,
    int msg,
    unsigned long long1,
    unsigned long long2 )
{

    debug_print("__add_message_to_into_the_queue:\n");

// Invalid window

    if( (void*) window == NULL )
    {
        debug_print("__add_message_to_into_the_queue: window\n");
        return -1;
    }

    if( window->used != TRUE )
    {
        debug_print("__add_message_to_into_the_queue: used\n");
        return -1;
    }


    if( window->magic != 1234 )
    {
        debug_print("__add_message_to_into_the_queue: magic\n");
        return -1;
    }


//
// Offset
//

// next position

    window->head_pos++;

// end of list
    if( window->head_pos < 0 || window->head_pos >= 32 )
        window->head_pos = 0;


// offset
    int offset = (int) window->head_pos;

    offset = (int) (offset & 0xFF);

//
// event
//

// standard
    window->window_list[offset] = (int) window->id;      // wid
    window->msg_list[offset]    = (int) (msg & 0xFFFF);  // message code
    window->long1_list[offset] = (unsigned long) long1;
    window->long2_list[offset] = (unsigned long) long2;

// extra
    window->long3_list[offset] = 0;
    window->long4_list[offset] = 0;


    debug_print("__add_message_to_into_the_queue: done\n");

    return 0;
}

// Talvez precisaremos de mais parametros.
unsigned long 
wmProcedure(
    struct gws_window_d *window,
    int msg,
    unsigned long long1,
    unsigned long long2 )
{


// #debug
    //printf("wmProcedure: w=? m=%d l1=%d l2=%d\n", 
        //msg, long1, long2 );

// See:
// globals.h

    switch(msg)
    {
         case GWS_Create:
             printf("wmProcedure: [1] GWS_Create\n");
             break;
         case GWS_Destroy:
             printf("wmProcedure: [2] GWS_Destroy\n");
             break;
         
         case GWS_Move:
             printf("wmProcedure: [3] GWS_Move\n");
             break;
         
         case GWS_Size: //get size?
             printf("wmProcedure: [4] GWS_Size\n");
             break;
         case GWS_Resize: //set size ?
             printf("wmProcedure: [5] GWS_Resize\n");
             break;
         
         //...
         case GWS_Close:
             printf("wmProcedure: [7] GWS_Close\n");
             if (long1==0){
                 printf("Closing root window ...\n");
                 //exit(0);
             }
             break;
         case GWS_Paint:
             printf("wmProcedure: [8] GWS_Paint\n");
             break;
             
         case GWS_SetFocus: // set focus
             printf("wmProcedure: [9] GWS_SetFocus\n");
             break;
         case GWS_KillFocus: //kill focus
             printf("wmProcedure: [10] GWS_KillFocus\n");
             break;
         
         case GWS_Activate:
             printf("wmProcedure: [11] GWS_Activate\n");
             break;
         
         case GWS_ShowWindow:
             printf("wmProcedure: [12] GWS_ShowWindow\n");
             break;
         
         case GWS_SetCursor:
             printf("wmProcedure: [13] GWS_SetCursor\n");
             break;
         
         case GWS_Hide:
             printf("wmProcedure: [14] GWS_Hide\n");
             break;
         
         case GWS_Maximize:
             printf("wmProcedure: [15] GWS_Maximize\n");
             break;
         case GWS_Restore:
             printf("wmProcedure: [16] GWS_Restore\n");
             break;
         
         case GWS_ShowDefault:
             printf("wmProcedure: [17] GWS_ShowDefault\n");
             break;

         case GWS_SetFocus2:
             printf("wmProcedure: [18] GWS_SetFocus2\n");
             break;
         
         case GWS_GetFocus2:
             printf("wmProcedure: [19] GWS_GetFocus2\n");
             break;

         // #bugbug
         // Quando imprimir na tela e quando enviar para o cliente?
         // Uma flag deve indicar se o sistema deve ou nao imprimir 
         // previamente o char.
         // Caixas de edição podem deixar todo o trabalho
         // de teclas de digitação para o sistema, liberando o aplicativo
         // desse tipo de tarefa. Mas editores de texto querem 
         // processar cada tecla digitada.
         case GWS_KeyDown:
             // Imprime o char na janela indicada.
             // Essa é a janela com foco de entrada.
             //if( pre_print === TRUE)
             __draw_char_into_the_window(window,(int)long1);
             // Enfileirar a mensagem na fila de mensagens
             // da janela com foco de entrada.
             // O cliente vai querer ler isso.
             __add_message_to_into_the_queue(
                 (struct gws_window_d *)window,
                 (int)msg,
                 (unsigned long)long1,
                 (unsigned long)long2);
             
             return 0;
             break;

         case GWS_SysKeyDown:
             printf("wmProcedure: [?] GWS_SysKeyDown\n");
             //#test
             //wm_update_desktop(); // 
             break;
         
         //
         case GWS_SwitchFocus:
             printf("Switch "); fflush(stdout);
             __switch_focus();
             //printf("wmProcedure: [?] GWS_SwitchFocus\n");
             //next = window->next;
             //window->focus = TRUE;
             //gwssrv_redraw_window(window,1);
             
             break;
    };


    return 0;
}


// Entry point
// Order: rdi, rsi, rdx, rcx, r8, r9.
unsigned long 
wmHandler(
    unsigned long arg1_rdi,
    unsigned long arg2_rsi,
    unsigned long arg3_rdx,
    unsigned long arg4_rcx )
{

// Final message
    struct gws_window_d  *w;
    int msg=0;
    unsigned long long1=0;
    unsigned long long2=0;


    debug_print ("wmHandler:\n");


// #debug
    //printf("wmHandler: %x %x %x %x\n", 
        //arg1_rdi, arg2_rsi, arg3_rdx, arg4_rcx );

//
// wid
//

// Ignoring this parameter
// We ware called by the kernel, and the kernel has no information
// about the windows. So, the messages sent by the kernel are able
// to act on the active window, on the window with focus and
// on the server itself.

    //int wid=-1;
    //wid = (int) (arg1_rdi & 0xFFFF);

//
// Message
//

    msg = (int) (arg2_rsi & 0xFFFF);

    if ( msg == 9091){
        wmRefreshDirtyRectangles();
        return 0;  //important: We need to return.
    }


//
// Data
//

    long1 = (unsigned long) arg3_rdx;
    long2 = (unsigned long) arg4_rcx;



//
// Calling wmProcedure()
//

    unsigned long r=0;

    switch (msg){

    // Mensagens de digitação. Atuam sobre a janela com foco de entrada.
    // #bugbug: a janela com foco de entrada precisa ser válida.
    case GWS_KeyDown:
    case GWS_SwitchFocus:
        if( window_with_focus < 0 || window_with_focus >= WINDOW_COUNT_MAX ){ return 0; } //fail
        w = windowList[window_with_focus];
        goto do_process_message;
        break;
        
    // Mensagens de sistema. Atuam sobre a jenela ativa.
    // #bugbug: a janela com foco de entrada precisa ser válida.
    case GWS_SysKeyDown:
        if( active_window < 0 || active_window >= WINDOW_COUNT_MAX ){ return 0; } //fail
        w = windowList[active_window];
        goto do_process_message;
        break;

    //case 9091:
        //wmCompositor();
        //return 0;  //important
        //break;

    // Mensagens que atuam sobre o window server.
    default:
        printf("wmHandler: default message\n");
        return 0;
        break;
    };


do_process_message:

    if ( (void *) w == NULL ){
        printf ("wmHandler: GWS_KeyDown w\n");
        return 0;
    }

    if ( w->used != TRUE || w->magic != 1234 )
    {
        printf ("wmHandler: w validation\n");
        return 0;
    }

    r = (unsigned long) wmProcedure(
                            (struct gws_window_d *) w,
                            (int) msg,
                            (unsigned long) long1,
                            (unsigned long) long2 ); 

done:
    debug_print ("wmHandler: done\n");
    return (unsigned long) r;
}





// yellow bar. (rectangle not window)
// developer status.
void yellow_status( char *string )
{
    //methods. get with the w.s., not with the system.
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();


    unsigned long offset_string1 = 8;  //( 8*1 );
    unsigned long offset_string2 = ( 8*5 );
    unsigned long bar_size = w;


    debug_print ("yellow_status:\n");
    
    //#todo
    //if ( (void*) string == NULL ){ return; }
    //if ( *string == 0 ){ return; }


    // Desenha a barra no backbuffer

    if ( current_mode == GRAMADO_JAIL ){
        //bar_size = w;
        bar_size = (w>>1);
        rectBackbufferDrawRectangle ( 
            0, 0, bar_size, 24, COLOR_YELLOW, 1,0 );
    }else{

        bar_size = (offset_string2 + (4*8) );
        rectBackbufferDrawRectangle ( 
            0, 0, bar_size, 24, COLOR_YELLOW, 1,0 );
    };

// Escreve as strings
    grDrawString ( offset_string1, 8, COLOR_BLACK, string );
    grDrawString ( offset_string2, 8, COLOR_BLACK, "FPS" );
    
    // Mostra o retângulo.
     
    if (bar_size == 0)
        bar_size = 32;
 
    gws_refresh_rectangle(0,0,bar_size,24);
}



int 
is_within ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y )
{

    if ( (void*) window != NULL )
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            // yes!
            if ( x >= window->left   && 
                 x <= window->right  &&
                 y >= window->top    &&
                 y <= window->bottom )
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


// validate
void validate_window (struct gws_window_d *window)
{
    if ( (void*) window != NULL )
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            window->dirty = FALSE;
        }
    }
}


// Invalidate
void invalidate_window (struct gws_window_d *window)
{
    if ( (void*) window != NULL )
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            window->dirty = TRUE;
        }
    }
}



/*
void destroy_window (struct gws_window_d *window);
void destroy_window (struct gws_window_d *window)
{
    // #todo
    // if( window == __root_window)
        // return;
  
    if ( (void*) window != NULL )
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            // ...
        }
    }
}
*/



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

    Window = (struct gws_window_d *) gwsCreateWindow ( 
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
       gwssrv_debug_print ("gwssrv: gwsCreateWindow fail\n");
       next_response[1] = 0;
       return -1;
    }

// Register window
    id = gwsRegisterWindow(Window);
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

//===============================================




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

    // #debug
    // Show the window. 
    // Delete this in the future.

    // #todo: We need a flag here. Came from parameters.
    // if( Show == TRUE )
    gws_show_window_rect(Window);

    // #debug
    // Show the screen.
    //gws_show_backbuffer(); 

    gwssrv_debug_print ("serviceCreateWindow: done\n");

    return 0;
}



int serviceChangeWindowPosition(void)
{
	//o buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];


    struct gws_window_d *window;
    int window_id = -1;
    
    unsigned long x = 0;
    unsigned long y = 0;


    // #debug
    gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition\n");


    // Get
    
    window_id = message_address[0];  //wid
    // msg
    x         = message_address[2];  
    y         = message_address[3];  


    //
    // Window ID
    //
   
    // Limits
    if ( window_id < 0 || window_id >= WINDOW_COUNT_MAX ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition window_id\n");
        return -1;
    }

    //#todo
    // Get the window structure given the id.
    window = (struct gws_window_d *) windowList[window_id];
   
    if ( (void *) window == NULL ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition window\n");
        return -1;
    }

    if ( window->used != TRUE || window->magic != 1234 ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition validation\n");
        return -1;
    }

    gwssrv_change_window_position ( 
        (struct gws_window_d *) window, 
        (unsigned long) x, 
        (unsigned long) y );

    return 0;
}



int serviceResizeWindow(void)
{

	//o buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];


    struct gws_window_d *window;
    int window_id = -1;
    
    unsigned long w = 0;
    unsigned long h = 0;


    // #debug
    gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition\n");

    // #todo
    // Check all the header.


    // Get
    
    window_id = message_address[0];  //wid
    // msg
    w         = message_address[2];  
    h         = message_address[3];  


    //
    // Window ID
    //
   
    // Limits
    if ( window_id < 0 || window_id >= WINDOW_COUNT_MAX ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition window_id\n");
        return -1;
    }

    //#todo
    // Get the window structure given the id.
    window = (struct gws_window_d *) windowList[window_id];
   
    if ( (void *) window == NULL ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition window\n");
        return -1;
    }
    
    if ( window->used != 1 || window->magic != 1234 ){
        gwssrv_debug_print ("gwssrv: serviceChangeWindowPosition validation\n");
        return -1;
    }

    //do!
    
    gws_resize_window ( 
        (struct gws_window_d *) window, 
        (unsigned long) w, 
        (unsigned long) h );

    return 0;
}



// #bugbug
// Usaremos a função create window para desenhar botões.
// #deletar !!!

int serviceDrawButton(void)
{

    // Deprecated !!
    gwssrv_debug_print("serviceDrawButton: deprecated\n");
    printf            ("serviceDrawButton: deprecated\n");
    exit(1);
    return -1;

    /*
    //O buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];

    unsigned long x=0;
    unsigned long y=0;
    unsigned long width=0;
    unsigned long height=0;


    x      = message_address[4]; 
    y      = message_address[5]; 
    width  = message_address[6]; 
    height = message_address[7]; 
    // ...

    // #todo
    // The label?
    
    gws_draw_button ("Label", 1,1,1, 
        x, y, width, height, GWS_COLOR_BUTTONFACE3 );


    // #bugbug
    // Used for debug. We don't need this thing 
    gws_show_backbuffer(); 
    return 0;
    */
}


// Redraw window.
int serviceRedrawWindow (void){
    
    //O buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];

    struct gws_window_d *window;

    // parameters
    int window_id = -1;
    int msg_code  = 0;
    unsigned long flags = 0;


    // #debug
    gwssrv_debug_print ("serviceRedrawWindow:\n");


    // Get wid and flag.
    window_id  = message_address[0];  // window id 
    msg_code   = message_address[1];  // message code
    flags      = message_address[2];  // flags
    //??       = message_address[3];  // nothing 

    // #todo
    //  Not tested yet.
    //if ( msg_code <= 0 ){
    //    gwssrv_debug_print ("serviceRedrawWindow:\n");
    //    goto fail;
    //}


    //
    // Window ID
    //
   
    // Limits
    if ( window_id < 0 || window_id >= WINDOW_COUNT_MAX ){
        gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window_id\n");
        goto fail;
    }

    // Get the window structure given the id.

    window = (struct gws_window_d *) windowList[window_id];

    if ( (void *) window == NULL ){
        gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window\n");
        goto fail;
    }else{
        if ( window->used != TRUE || window->magic != 1234 )
        {
            gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window validation\n");
            goto fail;
        }

        // redraw!
        gwssrv_redraw_window (
            (struct gws_window_d *) window, 
            (unsigned long) flags );

        return 0;
    };

fail:
    return -1;
}


int serviceRefreshRectangle (void){

	//o buffer é uma global nesse documento.
    unsigned long *message_address = (unsigned long *) &__buffer[0];

    unsigned long left,top,width,height;

    // #todo
    // Check all the header.

    left   = message_address[4];
    top    = message_address[5];
    width  = message_address[6];
    height = message_address[7];

    // #todo
    // Maybe we can test some limits here.

    gws_refresh_rectangle ( left, top, width, height );

    return 0;
}


// 1006
int serviceRefreshWindow (void){

    unsigned long *message_address = (unsigned long *) &__buffer[0];

    struct gws_window_d *window;
    int window_id = -1;
    
    //unsigned long x;
    //unsigned long y;
    //unsigned long color;
    //int __char;
    //char *text_buffer;    // #todo


    // #debug
    gwssrv_debug_print ("serviceRefreshWindow:\n");


    // #todo
    // Check all the header.

    //
    // == Window ID ============================
    //

    // Get it
    window_id = (int) message_address[0];   
   
    // #extra
    // Special case.
    // Will be used in the ghost frame routines.
    
    if ( window_id == (-4) )
    {
        gwssrv_debug_print("serviceRefreshWindow:\n");  //debug
        gwssrv_debug_print("== R (extra) ==\n");  //debug
        refresh_device_screen();
        return 0;
    }

 
    // Limits
    if ( window_id < 0 || window_id >= WINDOW_COUNT_MAX ){
        //printf("%d\n",window_id);
        gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window_id\n");
        return -1;
    }

    //#todo
    // Get the window structure given the id.
    window = (struct gws_window_d *) windowList[window_id];
   
    if ( (void *) window == NULL ){
        gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window\n");
        return -1;
    }
    
    if ( window->used != 1 || window->magic != 1234 ){
        gwssrv_debug_print ("serviceRefreshWindow: [FAIL] window validation\n");
        return -1;
    }

    gws_show_window_rect(window);
    
    //
    // Refresh
    //  
        
    //gws_show_backbuffer ();       // for debug   
    //gws_show_window_rect(window);   // something faster for now.
    //something faster.
    //gws_refresh_rectangle ( 
    //    window->left +x, 
    //    window->top  +y, 
    //    8,   //char width 
    //    8 ); // char height 

    return 0;
}


//
//===================================================================
//


// Let's redraw the window.
// Called by serviceRedrawWindow().
// IN: window pointer, show or not.

int 
gwssrv_redraw_window ( 
    struct gws_window_d *window, 
    unsigned long flags )
{
    unsigned int __tmp_color=0;


    gwssrv_debug_print ("gwssrv_redraw_window:\n");

    if ( (void *) window == NULL ){ return -1; }

    if (window->used!=TRUE || window->magic!=1234)
        return -1;


// Shadow
// A sombra pertence à janela e ao frame.
// A sombra é maior que a própria janela.
// ?? Se estivermos em full screen não tem sombra ??

    //gwssrv_debug_print ("gwssrv_redraw_window: Shadow\n");
    if ( window->shadowUsed == 1 )
    {

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

        if ( (unsigned long) window->type == WT_OVERLAPPED )
        {
            if (window->focus == 1){ __tmp_color = xCOLOR_GRAY1; }
            if (window->focus == 0){ __tmp_color = xCOLOR_GRAY2; }

            //ok funciona
            //rectBackbufferDrawRectangle ( 
            //    window->left +1, window->top +1, 
            //    window->width +1 +1, window->height +1 +1, 
            //    __tmp_color, 1, 0 ); 
            
            //test
            //remeber: the first window do not have a parent.
            //if ( (void*) window->parent == NULL ){
 
                //gwssrv_debug_print ("gwssrv_redraw_window: [Shadow] Parent"); 
                //exit(1); 
                rectBackbufferDrawRectangle ( 
                    (window->left +1), (window->top +1), 
                    (window->width +1 +1), (window->height +1 +1), 
                    __tmp_color, 1, 0 ); 
            //}
            
            //if ( (void*) window->parent != NULL ){
            //    rectBackbufferDrawRectangle ( 
            //        (window->left +1), (window->top +1), 
            //        (window->width +1 +1), (window->height +1 +1), 
            //        __tmp_color, 1, 0 ); 
            //}
        }

        // ??
        // E os outros tipos, não tem sombra ??
        // Os outros tipos devem ter escolha para sombra ou não ??
        // Flat design pode usar sombra para definir se o botão 
        // foi pressionado ou não.

       // ...
    } //fim do shadow
  

    // ## Background ##
    // Background para todo o espaço ocupado pela janela e pelo seu frame.
    // O posicionamento do background depende do tipo de janela.
    // Um controlador ou um editbox deve ter um posicionamento relativo
    // à sua janela mãe. Já uma overlapped pode ser relativo a janela 
    // gui->main ou relativo à janela mãe.
    gwssrv_debug_print ("gwssrv_redraw_window: Background\n");
    if ( window->backgroundUsed == 1 )
    {

        //window->bg_color = COLOR_PINK;
        //window->bg_color = CurrentColorScheme->elements[csiWindowBackground]; 

        // O argumento 'color' será a cor do bg para alguns tipos.
        // Talvez não deva ser assim. Talvez tenha que se respeitar o tema instalado.
        //if ( (unsigned long) window->type == WT_SIMPLE ) { window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_POPUP )  { window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_EDITBOX) { window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_CHECKBOX){ window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_SCROLLBAR){ window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_ICON )   { window->bg_color = color; }
        //if ( (unsigned long) window->type == WT_BUTTON ) { window->bg_color = color; }
        // ...

		// Pintar o retângulo.
		// #todo: 
		// ?? width Adicionar a largura da bordas bordas verticais.
		// #todo: 
		// ?? height Adicionar as larguras das bordas horizontais e da barra de títulos.

        /*
        if ( (unsigned long) type == WT_STATUSBAR )
        {
            drawDataRectangle ( window->left, window->top, 
                window->width -1, window->height, window->bg_color ); 

            grDrawString ( window->left +8, window->top +8, 
                COLOR_TEXT, window->name ); 
            goto done;
        }
        */

        // 
        // Draw background!
        //

        //#bugbug
        //Remember: The first window do not have a parent.
        //if ( (void*) window->parent == NULL ){ 
            //gwssrv_debug_print ("gwssrv_redraw_window: [Background] Parent\n"); 
            //exit(1); 
            rectBackbufferDrawRectangle ( 
                window->left, window->top, 
                window->width, window->height, 
                window->bg_color, 1, 0 );
        //}  
        
        //if ( (void*) window->parent != NULL ){
        //    rectBackbufferDrawRectangle ( 
        //        window->left, window->top, 
        //        window->width, window->height, 
        //        window->bg_color, 1, 0 );
        //}
        //?? More ...
    }  //fim do background

    //
    // botao
    //
    
    //Termina de desenhar o botão, mas não é frame
    //é só o botão...
    //caso o botão tenha algum frame, será alguma borda extra.
    int Focus=0;    //(precisa de borda)
    int Selected=0;
    unsigned int border1=0;
    unsigned int border2=0;

    gwssrv_debug_print ("gwssrv_redraw_window: Type Button\n");
    if ( (unsigned long) window->type == WT_BUTTON )
    {

        //border color
        //o conceito de status e state
        //está meio misturado. ja que estamos usando
        //a função de criar janela para criar botão.
        //#bugbug
        switch( window->status )
        {
            case BS_FOCUS:
                border1 = COLOR_BLUE;
                border2 = COLOR_BLUE;
                break;

            case BS_PRESS:
                Selected = 1;
                border1 = GWS_COLOR_BUTTONHIGHLIGHT3;
                border2 = GWS_COLOR_BUTTONSHADOW3;
                break;

            case BS_HOVER:
                break;
                    
            case BS_DISABLED:
                border1 = COLOR_GRAY;
                border2 = COLOR_GRAY;
                break;

            case BS_PROGRESS:
                break;

            case BS_DEFAULT:
            default: 
                Selected = 0;
                border1 = GWS_COLOR_BUTTONHIGHLIGHT3;
                border2 = GWS_COLOR_BUTTONSHADOW3;
                break;
        };


        size_t tmp_size = (size_t) strlen ( (const char *) window->name );
        unsigned long offset = 
        ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) gcharWidth) ) / 2 );
       

        //#debug
        //if ( (void*) window->parent == NULL ){
            //gwssrv_debug_print ("gwssrv_redraw_window: [WT_BUTTON] Parent NULL\n"); 
        //}

        //gwssrv_debug_print ("gwssrv_redraw_window: Button Border\n"); 
        //if ( (void*) window->parent != NULL )
        //{

            //board1, borda de cima e esquerda.
            rectBackbufferDrawRectangle ( 
                window->left, window->top,  
                window->width, 1, 
                border1, 1, 0 );

            rectBackbufferDrawRectangle ( 
                window->left, window->top,  
                1, window->height,
                border1, 1, 0 );

             //board2, borda direita e baixo.
             rectBackbufferDrawRectangle ( 
                 (window->left) + (window->width) -1, window->top, 
                 1, window->height, 
                 border2, 1, 0 );
                 
             rectBackbufferDrawRectangle ( 
                 window->left, (window->top) + (window->height) -1,  
                 (window->width), 1, 
                 border2, 1, 0 );

            // Button label
            gwssrv_debug_print ("gwssrv_redraw_window: [FIXME] Button label\n"); 
            /*
            if (Selected == 1){
                grDrawString ( 
                    (window->left) + offset, //(Parent->left   + window->left) + offset,
                    (window->top)  +8, //(Parent->top    + window->top)  +8, 
                    COLOR_WHITE, window->name );
            }else{
                // (largura do botão, menos a largura da string)/2
                // #debug: Rotina provisória
                //grDrawString ( x +20, y +20, COLOR_TERMINALTEXT, string );
                grDrawString ( 
                    (window->left) +offset, //(Parent->left   + window->left) +offset, 
                    (window->top)  +8, //(Parent->top    + window->top)  +8, 
                    COLOR_TERMINALTEXT, window->name );
            };
            */   
        //}

      //todo
      // configurar a estrutura de botão 
      // e apontar ela como elemento da estrutura de janela.
      //window->button->?
    }

    //#todo:
    if ( (unsigned long) window->type == WT_EDITBOX )
    {
        gwssrv_debug_print ("gwssrv_redraw_window: [TODO] Type Editbox\n");
        //...
    }
    
    // more types ?...

draw_frame:

    if ( window->type == WT_OVERLAPPED || 
         window->type == WT_EDITBOX || 
         window->type == WT_BUTTON )
    {
        // #todo
        // Precisamos de uma rotina que redesenhe o frame,
        // sem alocar criar objetos novos.
    }

    if (flags == 1){
        gws_show_window_rect(window);
    }

    return 0;
}


/*
 * =====================================================
 * gwssrv_initialize_color_schemes:
 *     Isso configura os esquemas de cores utilizados 
 * pelo sistema.
 *     Essa rotina é chamada apenas uma vez na inicialização
 * do kernel.
 *     O esquema de cores a ser utilizado deve estar 
 * salvo no perfil do usuário que fez o logon.
 * Os dois esquemas padrão chamam-se: 'humility' e 'pride'.
 * + O esquema 'humility' são cores com tema cinza, 
 * lembrando interfaces antigas.
 * + O esquema 'pride' são cores modernas 
 *   ( Aquele verde e preto e cinza, das primeiras versões, 
 * com imagens publicadas. )
 * @todo: é preciso criar rotinas que selecionem entre os 
 * modo criados e habilitados.
 * É preciso criar rotinas que permitam que aplicativos 
 * em user mode criem esquemas de cores e habilite eles.
 */


void 
gwssrv_initialize_color_schemes (int selected_type)
{
    struct gws_color_scheme_d  *humility;
    struct gws_color_scheme_d  *pride;

	//
	// HUMILITY
	//
	
    //Criando o esquema de cores humility. (cinza)
    humility = (void *) malloc ( sizeof(struct gws_color_scheme_d) );

    if( (void *) humility == NULL ){
        gwssrv_debug_print ("gwssrv_initialize_color_schemes: humility\n");
        printf             ("gwssrv_initialize_color_schemes: humility\n"); 

        GWSHumilityColorScheme = NULL;

        // #bugbug
        // ? return ????
        
    }else{
		
		//Object.
		//humility->objectType = ObjectTypeColorScheme;
		//humility->objectClass = ObjectClassGuiObjects;

        humility->used  = TRUE;
        humility->magic = 1234;
        humility->name  = "Humility";
		
		//Colors
		//Definidas em ws.h
        humility->elements[csiNull] = 0;                             //0
        humility->elements[csiDesktop] = HUMILITY_COLOR_BACKGROUND;  //1		
        humility->elements[csiWindow] = HUMILITY_COLOR_WINDOW;       //2
        humility->elements[csiWindowBackground] = HUMILITY_COLOR_WINDOW_BACKGROUND;	 //3	
        humility->elements[csiActiveWindowBorder] = HUMILITY_COLOR_ACTIVE_WINDOW_BORDER;  //4
        humility->elements[csiInactiveWindowBorder] = HUMILITY_COLOR_INACTIVE_WINDOW_BORDER;  //5		
        humility->elements[csiActiveWindowTitleBar] = HUMILITY_COLOR_ACTIVE_WINDOW_TITLEBAR;  //6
        humility->elements[csiInactiveWindowTitleBar] = HUMILITY_COLOR_INACTIVE_WINDOW_TITLEBAR;  //7	 	
        humility->elements[csiMenuBar] = HUMILITY_COLOR_MENUBAR;                //8
        humility->elements[csiScrollBar] = HUMILITY_COLOR_SCROLLBAR;            //9  
        humility->elements[csiStatusBar] = HUMILITY_COLOR_STATUSBAR;            //10
        humility->elements[csiMessageBox] = HUMILITY_COLOR_MESSAGEBOX;		    //11
        humility->elements[csiSystemFontColor] = HUMILITY_COLOR_SYSTEMFONT;		//12
        humility->elements[csiTerminalFontColor] = HUMILITY_COLOR_TERMINALFONT;	//13
        // ...

		//Sanvando na estrutura padrão para o esquema humility.
        GWSHumilityColorScheme = (void*) humility;
    };

	//
	// PRIDE 
	//
	
    //Criando o esquema de cores PRIDE. (colorido)
    pride = (void *) malloc ( sizeof(struct gws_color_scheme_d) );

    if ( (void *) pride == NULL ){
        gwssrv_debug_print ("gwssrv_initialize_color_schemes: pride\n");
        printf ("gwssrv_initialize_color_schemes: pride\n"); 

        GWSPrideColorScheme = NULL;

        // #bugbug
        // ? return ????

    }else{

		//Object.
		//pride->objectType  = ObjectTypeColorScheme;
		//pride->objectClass = ObjectClassGuiObjects;

        pride->used  = TRUE;
        pride->magic = 1234;
        pride->name  = "Pride";

		//Colors
		//Definidas em ws.h
        pride->elements[csiNull] = 0;
        pride->elements[csiDesktop] = PRIDE_COLOR_BACKGROUND;  
        pride->elements[csiWindow] = PRIDE_COLOR_WINDOW;
        pride->elements[csiWindowBackground] = PRIDE_COLOR_WINDOW_BACKGROUND;
        pride->elements[csiActiveWindowBorder] = PRIDE_COLOR_ACTIVE_WINDOW_BORDER;  
        pride->elements[csiInactiveWindowBorder] = PRIDE_COLOR_INACTIVE_WINDOW_BORDER;  
        pride->elements[csiActiveWindowTitleBar] = PRIDE_COLOR_ACTIVE_WINDOW_TITLEBAR;    
        pride->elements[csiInactiveWindowTitleBar] = PRIDE_COLOR_INACTIVE_WINDOW_TITLEBAR;		
        pride->elements[csiMenuBar] = PRIDE_COLOR_MENUBAR;
        pride->elements[csiScrollBar] = PRIDE_COLOR_SCROLLBAR;  		
        pride->elements[csiStatusBar] = PRIDE_COLOR_STATUSBAR;    
        pride->elements[csiMessageBox] = PRIDE_COLOR_MESSAGEBOX;
        pride->elements[csiSystemFontColor] = PRIDE_COLOR_SYSTEMFONT;    //12
        pride->elements[csiTerminalFontColor] = PRIDE_COLOR_TERMINALFONT;  //13		
        // ...

		// Sanvando na estrutura padrão para o esquema pride.
        GWSPrideColorScheme = (void *) pride;
    };


    //
    // Select
    //

    // Configurando qual será o esquema padrão.
    // #todo: 
    // Criar uma função que selecione qual dois esquemas serão usados
    // apenas selecionando o ponteiro da estrutura.  


    switch (selected_type){

    case ColorSchemeNull:
        GWSCurrentColorScheme = (void *) GWSHumilityColorScheme;
        break;

    case ColorSchemeHumility:
        GWSCurrentColorScheme = (void *) GWSHumilityColorScheme;
        break;

    case ColorSchemePride:
        GWSCurrentColorScheme = (void *) GWSPrideColorScheme; 
        break;

    default:
        GWSCurrentColorScheme = (void *) GWSHumilityColorScheme;
        break;
    };
    
    //
    // Check current
    //
    
    if ( (void*) GWSCurrentColorScheme == NULL )
    {
        gwssrv_debug_print ("gwssrv_initialize_color_schemes: GWSCurrentColorScheme\n");
        printf             ("gwssrv_initialize_color_schemes: GWSCurrentColorScheme\n"); 
        exit(1);
    }
}


// seleciona o tipo ...isso é um serviço.
int gwssrv_select_color_scheme (int type)
{
	//#debug
	//printf("gwssrv_select_color_scheme: type={%d} \n", type);

    switch (type){
        case ColorSchemeHumility:  goto do_humility;  break;
        case ColorSchemePride:     goto do_pride;     break;
        default:
            gwssrv_debug_print("gwssrv_select_color_scheme: [FAIL] Type not defined\n");
            goto fail;
            break;
    };


do_humility:

    if ( (void *) GWSHumilityColorScheme == NULL ){
        gwssrv_debug_print("HumilityColorScheme fail\n");
        goto fail;
    }else{

        if ( GWSHumilityColorScheme->used  != TRUE || 
             GWSHumilityColorScheme->magic != 1234 )
        {
            gwssrv_debug_print("HumilityColorScheme sig fail\n");
            goto fail;
        }

        gwssrv_debug_print("Humility selected\n");
        GWSCurrentColorScheme = GWSHumilityColorScheme;
        goto done;
    };

do_pride:

    if ( (void *) GWSPrideColorScheme == NULL ){
        gwssrv_debug_print("GWSPrideColorScheme fail\n");
        goto fail;
    }else{
        if( GWSPrideColorScheme->used  != TRUE || 
            GWSPrideColorScheme->magic != 1234 )
        {
            gwssrv_debug_print("PrideColorScheme sig fail\n");
            goto fail;
        }

        gwssrv_debug_print ("Pride selected\n"); 
        GWSCurrentColorScheme = GWSPrideColorScheme;
        goto done;
    };

done:
    return 0;
fail:
    gwssrv_debug_print ("gwssrv_select_color_scheme: fail\n");
    return 1;
}


/*
 ***********************************************
 * gws_show_window_rect:
 * 
 *     Mostra o retângulo de uma janela que está no backbuffer.
 *     Tem uma janela no backbuffer e desejamos enviar ela 
 * para o frontbuffer.
 *     A rotina de refresh rectangle tem que ter o vsync
 *     #todo: criar um define chamado refresh_window.
 */

// ??
// Devemos validar essa janela, para que ela 
// não seja redesenhada sem antes ter sido suja?
// E se validarmos alguma janela que não está pronta?
// #test: validando

int gws_show_window_rect (struct gws_window_d *window)
{
    //struct gws_window_d  *p;

    debug_print("gws_show_window_rect:\n");


    if ( (void *) window == NULL )
    {
        debug_print ("gws_show_window_rect: [FAIL] window\n");
        printf      ("gws_show_window_rect: [FAIL] window\n");
        exit(1);
        //return (int) -1;
    }


    if ( window->used != TRUE || window->magic != 1234 )
    {
        debug_print ("gws_show_window_rect: window validation\n");
        printf      ("gws_show_window_rect: window validation\n");
        exit(1);
        //return (int) -1;
    }

			//#shadow 
			// ?? E se a janela tiver uma sombra, 
			// então precisamos mostrar a sombra também. 
			
			//#bugbug
			//Extranhamente essa checagem atraza a pintura da janela.
			//Ou talvez o novo tamanho favoreça o refresh rectangle,
			//ja que tem rotinas diferentes para larguras diferentes
			
			//if ( window->shadowUsed == 1 )
			//{

			    //window->width = window->width +4;
				//window->height = window->height +4;
			    
				//refresh_rectangle ( window->left, window->top, 
				//    window->width +2, window->height +2 ); 
				//return (int) 0;
			//}


    //p = window->parent;




//
// Refresh rectangle
//

    // See: rect.c   
    debug_print("gws_show_window_rect: Calling gws_refresh_rectangle\n");

    gws_refresh_rectangle ( 
        window->left, 
        window->top, 
        window->width, 
        window->height ); 
            

//
// Validate window
//
  
    // Com isso o compositor não vai redesenhar
    // até que alguém invalide ela.

    debug_print("gws_show_window_rect: Calling validate_window\n");

    validate_window(window);


//done:
    debug_print("gws_show_window_rect: done\n");
    return 0;
fail:
    // fail.
    debug_print("gws_show_window_rect: fail\n");
    return (int) -1;
}


/*
// #todo
// Retorna o ponteiro de estrutura de janela
// dado o id da janela.
struct gws_window_d *gws_window_from_id (int id);
struct gws_window_d *gws_window_from_id (int id)
{
    struct gws_window_d *w;
    
    // ...
    
    return (struct gws_window_d *) w;
}
*/


// Create root window
// Called by gwsInit in gws.c.

struct gws_window_d *createwCreateRootWindow(void)
{
    struct gws_window_d *w;

    // It's because we need a window for drawind a frame.
    // WT_OVERLAPPED needs a window and WT_SIMPLE don't.
    unsigned long rootwindow_valid_type = WT_SIMPLE;

    unsigned long left   = 0;
    unsigned long top    = 0;
    unsigned long width  = (__device_width  & 0xFFFF );
    unsigned long height = (__device_height & 0xFFFF );


// background color.
    unsigned int rootwindow_color;

    // default.
    __set_default_background_color( WM_DEFAULT_BACKGROUND_COLOR );
    rootwindow_color = (unsigned int) __get_default_background_color();

    // custom.
    __set_custom_background_color(WM_DEFAULT_BACKGROUND_COLOR);


// Begin paint
    asm("cli");

    debug_print("createwCreateRootWindow:\n");

    // (root window)
    // #bugbug: EStamos usado device info sem checar.
    
    w = (struct gws_window_d *) gwsCreateWindow ( 
                                    rootwindow_valid_type,  
                                    0, //style
                                    1, //status
                                    1, //view
                                    "RootWindow",  
                                    left, top, width, height,
                                    NULL, 0, rootwindow_color, rootwindow_color );
    if ( (void*) w == NULL)
    {
        debug_print("createwCreateRootWindow: [FAIL] w\n");
        printf     ("createwCreateRootWindow: [FAIL] w\n");
        exit(1);
    }
    
// Setup the surface in ring0
    setup_surface_retangle(left,top,width,height);

// invalidate the surface in ring0.
    invalidate_surface_retangle();

// Invalidate again.

    w->dirty = TRUE;

    w->locked = TRUE;

    w->used  = TRUE;
    w->magic = 1234;

    // Register.
    // WindowId = gwsRegisterWindow (__root_window);

    // if (WindowId<0){
    // gwssrv_debug_print ("create_background: Couldn't register window\n");
    //return;
    //}

    // Root window
    gwsDefineInitialRootWindow (w);

// End paint
    asm("sti");

    debug_print("createwCreateRootWindow: done\n");

    return (struct gws_window_d *) w;
}



int gwsDefineInitialRootWindow ( struct gws_window_d *window )
{
    if ( (void *) window == NULL )
    {
        debug_print("gwsInit: [FAIL] screen window\n");
        printf     ("gwsInit: [FAIL] screen window\n");
        exit(1);
        //return -1;
    }
    
    __root_window = window;
    
    // ...
    
    return 0;  //ok
}

/*
 *******************************************************
 * gwsRegisterWindow: 
 *     Register a window.
 */
 
// OUT:
// < 0 = fail.
// > 0 = Ok. (index)
 
int gwsRegisterWindow (struct gws_window_d *window)
{
    //loop
    register int __slot=0;
    
    struct gws_window_d *tmp; 



    if ( (void *) window == NULL )
    {
        //gws_debug_print ("gwsRegisterWindow: window struct\n");
        return (int) -1;
    }


	// Contagem de janelas e limites.
	// (é diferente de id, pois id representa a posição
	// da janela na lista de janelas).

    windows_count++;

    if ( windows_count >= WINDOW_COUNT_MAX ){
        //gws_debug_print ("gwsRegisterWindow: Limits\n");
        printf ("gwsRegisterWindow: Limits\n");
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
 * get_active_window:
 *     Obtem o id da janela ativa.
 *     @todo: Mudar para windowGetActiveWindowId().
 */
int get_active_window (void)
{
    return (int) active_window;
}


/*
 * set_active_window:
 */

void set_active_window (int id)
{

	// @todo: Limits. Max.
    if (id < 0){
        return;
    }

    active_window = (int) id;
}


int get_window_with_focus(void)
{
    return (int) window_with_focus;
}


int set_window_with_focus(int id)
{
    
    if(id<0){
        return -1;
    }

    window_with_focus = (int) id;
}




// Pegando a z-order de uma janela.
int get_zorder ( struct gws_window_d *window )
{
    if ( (void *) window != NULL ){
        return (int) window->zIndex;
    }

    return (int) -1;
}


int get_top_window (void)
{
    return (int) top_window;
}


//Setando a top window.
void set_top_window (int id)
{
    top_window = (int) id;
}


int gwssrv_get_number_of_itens (struct gwsssrv_menu_d *menu)
{
    if ( (void*) menu == NULL ){
        return -1;
    }

    return (int) menu->itens_count;
}


struct gwsssrv_menu_d *gwssrv_create_menu (
    struct gws_window_d *parent,
    int highlight,
    int count,
    unsigned long x,
    unsigned long y,
    unsigned long width,
    unsigned long height,
    unsigned long color )
{

    struct gwsssrv_menu_d  *menu;

    struct gws_window_d  *window;


    gwssrv_debug_print("gwssrv_create_menu:\n");


    menu = (struct gwsssrv_menu_d *) malloc ( sizeof(struct gwsssrv_menu_d) );

    if ( (void *) menu == NULL )
    {
        gwssrv_debug_print("gwssrv_create_menu: [FAIL] menu\n");
        return (struct gwsssrv_menu_d *) 0;
    }


    // Deslocamento em relação a janela mãe.
    menu->x = x;
    menu->y = y;
    menu->width  = width;
    menu->height = height;
    
    menu->color=color;
    menu->highlight = highlight;
    menu->itens_count = count;


    window = (struct gws_window_d *) gwsCreateWindow ( 
                                         WT_SIMPLE, 
                                         0,  //style
                                         1,  //status
                                         1,  //view
                                         "menu-bg",  
                                         menu->x, menu->y, 
                                         menu->width, menu->height,   
                                         (struct gws_window_d *) parent, 0, 
                                         color, color ); 

    if ( (void *) window == NULL )
    {
        gwssrv_debug_print ("gwssrv_create_menu: window fail\n");  
        return NULL;
    }


    //primeiro salva.
        
    menu->window = window; 
    menu->parent = parent;

    return (struct gwsssrv_menu_d *) menu;
}


// Create menu item
struct gwsssrv_menu_item_d *gwssrv_create_menu_item (
    char *label,
    int id,
    struct gwsssrv_menu_d *menu)
{

    struct gws_window_d *window; //menu item window
    
    struct gwsssrv_menu_item_d *item;
    
    
    gwssrv_debug_print("gwssrv_create_menu_item:\n");    
    
    if ( (void *) menu == NULL ){
        return (struct gwsssrv_menu_item_d *) 0;
    }
    
    //create menu item.
    item = (struct gwsssrv_menu_item_d *) malloc( sizeof(struct gwsssrv_menu_item_d) );

    if ( (void *) item == NULL ){
        return (struct gwsssrv_menu_item_d *) 0;
    }

    //provisório
    if(id>5 || id>menu->itens_count)
        return (struct gwsssrv_menu_item_d *) 0;


    item->id = id;

    item->width  = (menu->width -8);
    item->height = (menu->height / menu->itens_count);
    item->x = 4;
    item->y = (item->height*id);
    

    if( menu->window != NULL )
    {
        window = (struct gws_window_d *) gwsCreateWindow ( 
                                             WT_BUTTON,
                                             0, //style
                                             1, //status 
                                             1, //view 
                                             (char *) label,  
                                             item->x, item->y, item->width, item->height,   
                                             menu->window, 0, 
                                             COLOR_GRAY, COLOR_GRAY );    

        if ( (void*) window == NULL )
        {
            item->window = NULL;
            goto fail;
        }

        item->window = window;
        
        //ok
        return (struct gwsssrv_menu_item_d *) item;
    }

fail:
    return (struct gwsssrv_menu_item_d *) 0;
}


/*
struct gwsssrv_menu_item_d *gwssrv_get_menu_item(struct gwsssrv_menu_d *menu, int i);
struct gwsssrv_menu_item_d *gwssrv_get_menu_item(struct gwsssrv_menu_d *menu, int i)
{


     //return (struct gwsssrv_menu_item_d *) ?;
}
*/

/*
int gwssrv_redraw_menuitem(struct gwsssrv_menu_item_d *);
int gwssrv_redraw_menuitem(struct gwsssrv_menu_item_d *)
{
}
*/


/*
int gwssrv_redraw_menu ( struct gwsssrv_menu_d *menu );
int gwssrv_redraw_menu ( struct gwsssrv_menu_d *menu )
{
    int i=0;
    int n=0;

    if ( (void*) menu == NULL )
        return -1;
    
    n = (int) gwssrv_get_number_of_itens(menu);

    if ( n<=0 )
        return -1;
    
    for (i=0; i<n; i++)
    {
        mi = gwssrv_get_menu_item(i,menu);
        gwssrv_redraw_menuitem(mi);
    };
}
*/


//test
int create_main_menu(void){

    struct gwsssrv_menu_d *menu;


    // #testing (NEW)
    menu = gwssrv_create_menu (
               (int) gui->screen_window,
               (int) 0,   //highlight
               (int) 4,   //count
               (unsigned long) 8, 
               (unsigned long) 8,
               (unsigned long) 320,
               (unsigned long) 280,
               (unsigned long) COLOR_WHITE );


    if ( (void*) menu != NULL )
    {
               //menu item 0
               gwssrv_create_menu_item (
                  "Test mouse F3",
                  (int) 0,
                  (struct gwsssrv_menu_d *) menu );
 
               
               //menu item 1
               gwssrv_create_menu_item (
                  "Editor F10",
                  (int) 1,
                  (struct gwsssrv_menu_d *) menu );

               //menu item 2
               gwssrv_create_menu_item (
                  "Terminal F12",
                  (int) 2,
                  (struct gwsssrv_menu_d *) menu );

               //menu item 3
               gwssrv_create_menu_item (
                  "Reboot F4",
                  (int) 3,
                  (struct gwsssrv_menu_d *) menu );
              
    }

    
    gws_show_window_rect(menu->window);
        
    return 0;
}




/*
 *********************************************
 * gws_resize_window:
 *     Muda as dimensões da janela.
 */
 
int 
gws_resize_window ( 
    struct gws_window_d *window, 
    unsigned long cx, 
    unsigned long cy )
{

    if ( (void *) window == NULL ){
        return -1;
    }

    // Só precisa mudar se for diferente.
    if ( window->width  != cx ||
         window->height != cy )
    {
        window->width  = (unsigned long) cx;
        window->height = (unsigned long) cy;
    }
    

    // #test
    //window->dirty = TRUE;
    invalidate_window(window);

    //__root_window->dirty = 1;

    return 0;
}



// #test
// Isso so faz sentido num contexto de reinicializaçao 
// do desktop.
void reset_zorder(void)
{
     //loop
     register int i=0;
     
     struct gws_window_d *w;


     for ( i=0; i<WINDOW_COUNT_MAX; ++i)
     {
         w = (struct gws_window_d *) windowList[i];
         if ( (void*) w != NULL )
         {
             if ( w->used == TRUE && w->magic == 1234 )
             {
                 // Coloca na zorder as janelas overlapped.
                 if ( w->type == WT_OVERLAPPED )
                 {
                     zList[i] = windowList[i];
                 }
             }
         }
     };
}


/*
 ****************************************************
 * gws_change_window_position:
 *     Muda os valores do posicionamento da janela.
 */
 
int 
gwssrv_change_window_position ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y )
{
    // #??
    // Isso deve mudar apenas o deslocamento em relacao
    // a margem e nao a margem ?

    if ( (void *) window == NULL ){
        gwssrv_debug_print("gwssrv_change_window_position: window\n");
        return -1;
    }

    /*
    if ( window->left != x ||
         window->top  != y )
    {
        window->left = (unsigned long) x;
        window->top  = (unsigned long) y;
    }
    */
    
    window->x = x;
    window->y = y;
    window->left = (window->parent->left + window->x); 
    window->top  = (window->parent->top  + window->y); 
    
    //#test
    //window->dirty = 1;
    invalidate_window(window);
    
    return 0;
}



/*
 * gwsWindowLock:
 *     Bloqueia uma janela.
 *     @todo: Quem pode realizar essa operação??
 */
 
void gwsWindowLock (struct gws_window_d *window)
{

    if ( (void *) window == NULL ){
        return;
    }

    window->locked = (int) WINDOW_LOCKED;  //1.
}


/*
 * gwsWindowUnlock:
 *     Desbloqueia uma janela.
 *     @todo: Quem pode realizar essa operação??
 */
 
void gwsWindowUnlock (struct gws_window_d *window)
{
    if ( (void *) window == NULL ){
        return;
    }

    window->locked = (int) WINDOW_UNLOCKED;  //0.
}


/*
 *****************************************
 * gwssrv_init_windows:
 * 
 */

int gwssrv_init_windows (void)
{
    //loop
    register int i=0;

    //window.h
    windows_count     = 0;
    window_with_focus = 0;
    active_window     = 0;
    top_window        = 0;
    //...
    
    show_fps_window = FALSE;


    // Window list
    for (i=0; i<WINDOW_COUNT_MAX; ++i){  windowList[i] = 0;  };

    // z order list
    for (i=0; i<ZORDER_MAX; ++i){  zList[i] = 0;  };
        
    // ...

    return 0;
}


//
// == ajusting the window to fit in the screen. =======================
//

/*
 credits: hoppy os.
void 
set_window_hor (
    tss_struct *tss,
    int i,
    int j)
{
    
    int d = j-i;
    
    if ( i >= tss->crt_width) 
    {
        i = tss->crt_width-2;
        j = i+d;
    }
    
    if (j<0) 
    {
        j=0;
        i = j-d;
    }
    
    if (i>j) 
    {
        if (i>0)
           j=i;
        else
            i=j;
    }
    
    tss->window_left=i;
    tss->window_right=j;
}
*/


/*
 credits: hoppy os.
void 
set_window_vert ( 
    tss_struct *tss,
    int i,
    int j )
{

    int d = j-i;
    
    if (i >= tss->crt_height) 
    {
        // ajusta o i.
        i = tss->crt_height-1;
        j = i+d;
    }
    
    if (j<0) 
    {
        // ajusta o i.
        j = 0;
        i = j-d;
    }

    if (i>j) 
    {
        if (i>0)
            j=i;
        else
            i=j;
    }

    tss->window_top    = i;
    tss->window_bottom = j;
}
*/


//
// End.
//


