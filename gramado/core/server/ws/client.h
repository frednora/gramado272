

#ifndef ____CLIENT_H
#define ____CLIENT_H    1



// Nesse arquivo fica a estrutura de cliente.
// Essa estrutura vai conter as informações
// e características dos clientes.


//
// Current
//

// Mandaremos as mensages para a fila de mensagens do cliente atual
// para isso o window server precisa constantemente ler sua fila de
// mensagens.
// O kernel alimenta a fila de mensagens do ws qunado tem um ws
// instalado.

int current_client;


struct gws_client_d
{

    int id;
    int used;
    int magic;

// #todo
    unsigned long flags;

// Para qual versão do Gramado esse aplicativo foi feito.
// ex: 0x0101  (1.1)
    unsigned short gramado_version;

// Flags para gerenciamento de compatibilidade entre
// versões
    unsigned long compat_flags;

// maybe
// The main window of this client.
// We can send messages to this window.
    //struct gws_window_d *main_window;

    int is_connected;

    // host, display, screen
    struct gws_host_d     host;
    struct gws_display_d  display;  
    struct gws_screen_d   screen;

    // Socket for this client.
    int fd;


    // The PID of the client.
    pid_t pid; 
    gid_t gid;


// Spin
// Lets count how many times this client
// is looking for the next event.
// It can be useful for changing the client's priority.

    unsigned long spin;

    // ...
};

struct gws_client_d  *serverClient;
struct gws_client_d  *currentClient;
// ...

#define SERVER_CLIENT_INDEX 0
#define CLIENT_COUNT_MAX 32
unsigned long connections[CLIENT_COUNT_MAX];


//
// == prototypes =============================
//

int servicePutClientMessage(void);
int serviceGetClientMessage(void);

#endif    



