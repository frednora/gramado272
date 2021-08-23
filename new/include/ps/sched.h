

#ifndef __SCHED_H
#define __SCHED_H    1





//#define NR_TASKS ??
//#define FIRST_TASK ?
//#define LAST_TASK ? 

// Status do scheduler.
#define LOCKED    0
#define UNLOCKED  1  


/*
 * Scheduling policies
 */

#define SCHED_RR    0
//#define SCHED_??    1
// ...

struct scheduler_info_d
{
    int policy;
    unsigned long flags;
};

struct scheduler_info_d SchedulerInfo;


// ===============================================

// Quantum in ms.
#define QUANTUM_MIN    20
#define QUANTUM_MAX    100

#define QUANTUM_BASE   QUANTUM_MIN
#define QUANTUM_LIMIT  QUANTUM_MAX

// Limite de tempo esperando.
#define READY_LIMIT      QUANTUM_MAX
#define WAITING_LIMIT    QUANTUM_MAX
#define BLOCKED_LIMIT    QUANTUM_MAX
// ...




// Hz padrão. 100Hz. 
// Uma interrupção a cada 10ms.
#define HZ  100


// Tranca
// #define LATCH (1193180/HZ)

// Tranca
// #define LATCH (1193180/HZ)

// Tipos diferentes de scheduler. 
// Usado pelo desenvolvedor.
// RR. Priority ....

int schedulerType;

// @todo: 
// deletar isso, usa-se a fila do dispatcher. 
unsigned long schedulerQueue[4]; 



//
// == prototypes ============
//


// ==================



// ==================


int init_scheduler (unsigned long sched_flags);

// ::core
int __scheduler_rr (unsigned long sched_flags);
int scheduler (void);

void scheduler_lock (void);
void scheduler_unlock (void);
unsigned long scheduler_get_status (void);

void cut_round( struct thread_d *last_thread );


// ===

int KiScheduler (void);
void do_thread_blocked (int tid);
void do_thread_dead (int tid);
void do_thread_initialized (int tid);
void do_thread_ready (int tid);
void do_thread_running (int tid);
void do_thread_standby (int tid);
void do_thread_waiting (int tid);
void do_thread_zombie (int tid);

int do_waitpid (pid_t pid, int *status, int options);

int get_current (void);
void set_current (int id);

void wait_for_a_reason ( int tid, int reason );
int wakeup_thread_reason ( int tid, int reason );
int wakeup_scan_thread_reason ( int reason );
void wakeup_thread (int tid);
void yield (int tid);

void check_for_standby (void);

#endif    



















