


// Line
int servicelineBackbufferDrawHorizontalLine (void);

// Char
int serviceDrawChar(void);

// Text
int serviceDrawText(void);

// Rectangle
int serviceRefreshRectangle(void);

// Window
int serviceCreateWindow (int client_fd);

int serviceChangeWindowPosition(void);
int serviceResizeWindow(void);
int serviceRedrawWindow(void);
int serviceRefreshWindow(void);

// Button
int serviceDrawButton (void); 



// When a client send us an event
int serviceClientEvent(void);

// When a client get the next event from it's own queue.
int serviceNextEvent(int client_fd);

int serviceAsyncCommand (void);




