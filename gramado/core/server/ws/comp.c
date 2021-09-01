// comp.c


// window server routines.
#include <gws.h>

// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// ...

/*
 * compositor:
 * 
 * 
 */

// Respeitando a z-order.
// Show all the windows respecting the zorder.

// #goal
// Repintar os retângulos e as janelas marcadas marcada como 'redraw'.
// O efeitos serão aplicados dentro das janelas.
// Teremos o depth-buffer para janelas e não par as tela toda.
// Poderemos usar janelas em full screen.
// Clipping window.

    // + repinta as janelas.
    // + rasterization.
    // + raytracing.
    // + Invalida todos os retângulos pintados bo backbuffer,
    //   para que as rotinas de refresh façam o flush deles 
    //   para o lfb no próximo passo. 

    // Repinta as janelas e refresh nos retangulos ou tela toda.
    // See: wm.c

void compositor (void)
{
    gwssrv_debug_print("compositor:\n");
    wmCompositor();
    // rasterization();
    // raytracing();
    gwssrv_debug_print("compositor: done\n");
}


