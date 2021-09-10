



#include <kernel.h>    

/*
 **********************************************************
 * task_switch:
 *
 * Switch the thread.
 * Save and restore context.
 * Select the next thread and dispatch.
 * return to _irq0.
 * Called by KiTaskSwitch.
 */
 
void task_switch (void)
{

// Current
    struct process_d  *CurrentProcess;
    struct thread_d   *CurrentThread;

// Target
    struct process_d  *TargetProcess;
    struct thread_d   *TargetThread;

    pid_t pid = -1;


//
// Current thread
//
    // Check current thread limits.
    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX )
    {
        panic ("ts: current_thread\n");
    }

    CurrentThread = (void *) threadList[current_thread]; 

    if ( (void *) CurrentThread == NULL ){
        panic ("ts: CurrentThread\n");
    }
    
    // #todo
    // Check the thread's validation. 
    // used and magic.

//
// Current process
//
    pid = (pid_t) CurrentThread->ownerPID;

    // #todo: Check overflow too.
    if (pid<0){
        panic ("ts: pid\n");
    }

    CurrentProcess = (void *) processList[pid];

    if ( (void *) CurrentProcess == NULL ){
        panic ("ts: CurrentProcess\n");
    }

    // validation
    if ( CurrentProcess->used != TRUE && CurrentProcess->magic != 1234 )
    {
        panic ("ts: CurrentProcess validation\n");
    }

   // Update the global variable.

   current_process = (int) CurrentProcess->pid;

//
//  == Conting =================================
//

    // 1 second = 1000 milliseconds
    // sys_time_hz = 600 ticks per second.
    //1/600 de segundo a cada tick
    //1000/100 = 10 ms quando em 100HZ.
    //1000/600 = 1.x ms quando em 600HZ.
    //x = 0 + (x ms); 


// step: 
// Quantas vezes ela já rodou no total.
    CurrentThread->step++; 

// runningCount: 
// Quanto tempo ela está rodando antes de parar.
    CurrentThread->runningCount++;


    //
    // == #bugbug ==============================================
    //

	// #bugbug
	// Rever essa contagem
/*
The variables i have are:
Current->step = How many timer the thread already ran.
sys_time_hz = The timer frequency. (600Hz).
No double type, no float type.
----------
600Hz means that we have 600 ticks per second.
With 100 Hz we have 10 milliseconds per tick. ((1000/100)=10)
With 600Hz we have 1.66666666667 milliseconds per tick.   ((1000/600)=1)
------
Maybe i will try 500Hz.
With 600Hz we have 2 milliseconds per tick.   ((1000/500)=2)
----
This is a very poor incrementation method:
Current->total_time_ms = Current->total_time_ms + (1000/sys_time_hz);
The remainder ??
----
*/

	//quanto tempo em ms ele rodou no total.
    CurrentThread->total_time_ms = 
        (unsigned long) CurrentThread->total_time_ms + (1000/sys_time_hz);

	//incrementa a quantidade de ms que ela está rodando antes de parar.
	//isso precisa ser zerado quando ela reiniciar no próximo round.
    CurrentThread->runningCount_ms = 
        (unsigned long) CurrentThread->runningCount_ms + (1000/sys_time_hz);



//
// == Locked ? ===============================
//

    // Taskswitch locked? 
    // Return without saving.

    if ( task_switch_status == LOCKED ){
        IncrementDispatcherCount (SELECT_CURRENT_COUNT);
        debug_print ("ts: Locked $\n");
        return; 
    }


//
// == Unlocked ? ==============================
//

    // Nesse momento a thread atual sofre preempção por tempo
    // Em seguida tentamos selecionar outra thread.

    if ( task_switch_status == UNLOCKED )
    {
//
// ## SAVE CONTEXT ##
//
        save_current_context();
        CurrentThread->saved = TRUE;

//
// Compositor
//

// Chamamos o compositor com o contexto salvo.
// O pit aciona essa flag a cada 32 ms.
// See: sched.c

        if (UpdateScreenFlag==TRUE){
            schedulerUpdateScreen();
        }


		// #obs:
		// A preempção acontecerá por dois possíveis motivos.
		// + Se o timeslice acabar.
		// + Se a flag de yield foi acionada.

		// #importante:
		// Se a thread ainda não esgotou seu quantum, 
		// então ela continua usando o processador.

        if ( CurrentThread->runningCount < CurrentThread->quantum )
        {
            // Yield support.
            // Atendendo o pedido para tirar a thread do estado de rodando
            // e colocar ela no estado de pronta.
            // Coloca no estado de pronto e limpa a flag.
            // Em seguida vamos procurar outra.
            if ( CurrentThread->state == RUNNING && CurrentThread->_yield == TRUE )
            {
                CurrentThread->state  = READY;
                CurrentThread->_yield = FALSE;
                goto try_next;
            }

            // A mesma thread vai rodar novamente,
            // Não precisa restaurar o contexto.
            // O seu contexto está salvo, mas o handler em assembly
            // vai usar o contexto que ele já possui.
            IncrementDispatcherCount (SELECT_CURRENT_COUNT);
            //debug_print (" The same again $\n");
            //debug_print ("s");  // the same again
            return; 

		// #importante
		// Nesse momento a thread [esgotou] seu quantum, 
		// então sofrerá preempção e outra thread será colocada 
		// para rodar de acordo com a ordem estabelecida 
		// pelo escalonador.

        // Fim do quantum.
        }else{

//
// ## PREEMPT ##
//

            // Preempt
            // >> MOVEMENT 3 (Running --> Ready).

            // sofrendo preempção por tempo.
            // #todo: Mas isso só poderia acontecer se a flag
            // ->preempted permitisse. 
            // talvez o certo seja ->preenptable.

            if ( CurrentThread->state == RUNNING )
            {
                debug_print (" P ");
                
                CurrentThread->state = READY;

                // #bugbug
                // As rotinas de fila estão suspensas.

                // Se pode sofrer preenpção vai para fila de prontos.
                if ( CurrentThread->preempted == PREEMPTABLE )
                {
                    //debug_print (" preempt_q1 ");
                    //queue_insert_head ( 
                        //queue, (unsigned long) Current, QUEUE_READY );
                }

                if ( CurrentThread->preempted == UNPREEMPTABLE )
                {
                    //debug_print (" preempt_q2 ");
                    //queue_insert_data ( 
                        //queue, (unsigned long) Current, QUEUE_READY );
                }
            }

            //debug_print (" ok ");

//
// == EXTRA ==========
//
            // Call extra routines scheduled to this moment.

            // #hackhack
            // Vamos validar isso, pois isso é trabalho de uma rotina
            // do timer qua ainda não esta pronta.
            //extra = TRUE;
            extra = FALSE;
            if (extra == TRUE){
                debug_print (" X "); 
                tsCallExtraRoutines();
                extra = FALSE;
            }

            //#provisório; Isso é extra também.
            //KiRequest();
            //request();
 
			// Dead thread collector
			// Avalia se é necessário acordar a thread do dead thread collector.
			// É uma thread em ring 0.
			// Só chamamos se ele ja estiver inicializado e rodando.
			
			// #bugbug
			// precismos rever essa questão pois isso pode estar
			// fazendo a idle thread dormir. Isso pode prejudicar
			// a contagem.

			// See: core/ps/threadi.c

            if (dead_thread_collector_status == TRUE)
            {
                debug_print (" C "); 

                // #bugbug
                // #todo: This is a work in progress!
                check_for_dead_thread_collector();
            }

//
// == Spawn thread ===============
//
            // Check for a thread in standby.
            // In this case, this routine will not return.
            // See: ts/sched/schedi.c

            check_for_standby(); 

            goto try_next;
        };
    }  //FI UNLOCKED


//
// == Crazy Fail ===========================
//

	// #bugbug
	// Não deveríamos estar aqui.
	// Podemos abortar ou selecionar a próxima provisóriamente.

    //panic ("ts.c: crazy fail");

    goto dispatch_current; 

//
// == TRY NEXT THREAD ========================================
//

try_next: 

#ifdef SERIAL_DEBUG_VERBOSE
    debug_print(" N ");
#endif

    // We have only ONE thread.
    // Is that thread the idle thread?
    // Can we use the mwait instruction ?

    if (UPProcessorBlock.threads_counter == 1)
    {
        //debug_print(" JUSTONE ");
        
        // Is this a pointer?
        Conductor = ____IDLE;
        
        // If we will run only the idle thread, 
        // so we can use the mwait instruction. 
        // asm ("mwait"); 
        
        goto go_ahead;
    }

    //
    // ==== Reescalonar se chegamos ao fim do round. ====
    //

	// #bugbug
	// Ao fim do round estamos tendo problemas ao reescalonar 
	// Podemos tentar repedir o round só para teste...
	// depois melhoramos o reescalonamento.
		
	// #importante:
	// #todo: #test: 
	// De pempos em tempos uma interrupção pode chamar o escalonador,
	// ao invés de chamarmos o escalonador ao fim de todo round.
	
	// #critério:
	// Se alcançamos o fim da lista encadeada cujo ponteiro é 'Conductor'.
	// Então chamamos o scheduler para reescalonar as threads.


    if ( (void *) Conductor->next == NULL )
    {

#ifdef SERIAL_DEBUG_VERBOSE
        debug_print(" LAST ");
#endif

        //printf ("ts: scheduler 1\n");
        
        // Essa rotina reescalona e entrega um novo current_thread.
        // e conductor.
        // KiScheduler();
        current_thread = KiScheduler();
        goto go_ahead;
    }


	// #critério
	// Se ainda temos threads na lista encadeada, então selecionaremos
	// a próxima da lista.
	// #BUGBUG: ISSO PODE SER UM >>> ELSE <<< DO IF ACIMA.

    if ( (void *) Conductor->next != NULL )
    {

#ifdef SERIAL_DEBUG_VERBOSE
        debug_print(" Q ");
#endif

        Conductor = (void *) Conductor->next;
        goto go_ahead;
    }

    // #bugbug
    panic ("ts: Unspected");

//
// == Go ahead ========================================
//
	// #importante:
	// Nesse momento já temos uma thread selecionada,
	// vamos checar a validade e executar ela.
	// #importante:
	// Caso a thread selecionada não seja válida, temos duas opções,
	// ou chamamos o escalonador, ou saltamos para o início dessa rotina
	// para tentarmos outros critérios.

go_ahead:

//################################//
//                                //
//    # We have a thread now #    //
//                                //
//################################//

    // Esse foi o ponteiro configurado pelo scheduler.

    TargetThread = (void *) Conductor;

    // Vamos checar sua validade.

    if( (void *) TargetThread == NULL )
    { 
        debug_print ("ts: Struct ");
        //KiScheduler();
        current_thread = KiScheduler();
        goto try_next;

    }else{

        if ( TargetThread->used != TRUE || TargetThread->magic != 1234 )
        {
            debug_print ("ts: val ");
            //KiScheduler ();
            current_thread = KiScheduler();
            goto try_next;
        }

        if ( TargetThread->state != READY )
        {
            debug_print ("ts: state ");
            //KiScheduler ();
            current_thread = KiScheduler();
            goto try_next;
        }

//
// == Dispatcher =================================
//
        // Current selected.

        current_thread = (int) TargetThread->tid;
        goto dispatch_current;
    }

//
// fail
//

//superCrazyFail:
    goto dispatch_current; 


// ===========================
//   # Dispatch current #
// ============================

dispatch_current:

#ifdef SERIAL_DEBUG_VERBOSE
    debug_print (" ts-dispatch_current: ");
#endif

    // Validation
    // Check current thread limits.

    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX )
    {
        panic ("ts: [Dispatch current] current_thread\n");
    }

    TargetThread = (void *) threadList[current_thread];

    if ( (void *) TargetThread == NULL ){
        panic ("ts-dispatch_current: TargetThread\n");
    }

    // Validation

    if ( TargetThread->used  != TRUE || 
         TargetThread->magic != 1234 || 
         TargetThread->state != READY )
    {
        panic ("ts-dispatch_current: validation\n");
    }

//
// Counters
//

// Clean
// The spawn routine will do something more.

    TargetThread->standbyCount = 0;
    TargetThread->standbyCount_ms = 0;
    
    TargetThread->runningCount = 0;
    TargetThread->runningCount_ms = 0;
    
    TargetThread->readyCount = 0;
    TargetThread->readyCount_ms = 0;
    
    TargetThread->waitingCount = 0;
    TargetThread->waitingCount_ms = 0;
    
    TargetThread->blockedCount = 0;
    TargetThread->blockedCount_ms = 0;


// E se o limite estiver errado?
//    if ( TargetThread->quantum > TargetThread->quantum_limit_max )
//        TargetThread->quantum = TargetThread->quantum_limit_max;


//
// Call dispatcher.
//
    // #bugbug
    // Talvez aqui devemos indicar que a current foi selecionada. 
        
    IncrementDispatcherCount (SELECT_DISPATCHER_COUNT);

//
// MOVEMENT 4 (Ready --> Running).
//
    dispatcher(DISPATCHER_CURRENT); 

//
// == DONE =====================
//
   // We will return in the end of this function.

//done:

    // Owner validation.

    if ( TargetThread->ownerPID < 0 || 
         TargetThread->ownerPID >= THREAD_COUNT_MAX )
    {
       printf ("ts: ownerPID ERROR \n", 
            TargetThread->ownerPID );
       die();
    }

//
// Target process 
//
    TargetProcess = (void *) processList[TargetThread->ownerPID];

    if ( (void *) TargetProcess == NULL ){
        printf ("ts: TargetProcess %s struct fail \n", TargetProcess->name );
        die();
    }

    if ( TargetProcess->used != 1 || TargetProcess->magic != 1234 ){
        printf ("ts: TargetProcess %s validation \n", TargetProcess->name );
        die();
    }

// Update global variable.

    current_process = (int) TargetProcess->pid;

    if ( (unsigned long) TargetProcess->pml4_PA == 0 )
    {
        printf ("ts: Process %s pml4 fail\n", TargetProcess->name );
        die();
    }

    // #bugug
    // #todo

    // current_process_pagedirectory_address = (unsigned long) P->DirectoryPA;
    // ?? = (unsigned long) P->pml4_PA;

    debug_print ("ts: done $\n");

    return;

fail:
    panic ("ts: Unspected error\n");
}


/*
 ****************************************************
 * psTaskSwitch:
 * 
 *     Interface para chamar a rotina de Task Switch.
 *     Essa rotina somente é chamada por hw.inc.
 *     KiTaskSwitch em ts.c gerencia a rotina de 
 * troca de thread, realizando operações de salvamento e 
 * restauração de contexto utilizado variáveis globais e 
 * extrutura de dados, seleciona a próxima thread através 
 * do scheduler, despacha a thread selecionada através do 
 * dispatcher e retorna para a função _irq0 em hw.inc, 
 * que configurará os registradores e executará a 
 * thread através do método iret.
 *
 * #importante:
 * Na verdade, é uma interface pra uma rotina que 
 * faz tudo isso.
 * 
 */
 
/*
	// @todo: Fazer alguma rotina antes aqui ?!
	
	// Obs: A qui poderemos criar rotinas que não lidem com a troca de 
	// threads propriamente, mas com atualizações de variáveis e gerenciamento 
	// de contagem.
	// >> Na entrada da rotina podemos atualizar a contagem da tarefa que acabou de rodar.
	// >> A rotina task_switch fica responsável apenas troca de contexto, não fazendo 
	// atualização de variáveis de contagem.
	// >> ?? Na saída ??
	
	// ?? quem atualizou as variáveis de critério de escolha ??? o dispacher ??
*/



// Called by:
// _irq0 in hw.asm

void psTaskSwitch (void)
{
    //debug_print ("TS ");

    // Check current process limits.
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX )
    {
        printf ("psTaskSwitch: current_thread %d", current_process );
        die();
    }

    // Check current thread limits.
    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX )
    {
        printf ("psTaskSwitch: current_thread %d", current_thread ); 
        die();
    }

#ifdef SERIAL_DEBUG_VERBOSE
    //debug_print (".");
    //debug_print ("ts ");
#endif

//
// the real task switch.
//
    task_switch();

    // obs: 
    // Nessa hora já temos um thread atual e um processo atual 
    // selecionados. Podemos checar as variáveis para conferir se 
    // não está fora dos limites. Se estiverem fora dos limites, 
    // podemos usar algum método para selecionarmos outro processo 
    // ou outra thread com limites válidos.
 
    // #importante:   
    // Retornando para _irq0 em x86/hw.inc.

    //debug_print ("TS_DONE \n");
}


/*
 * get_task_status:
 *     Obtem o status do mecanismo de taskswitch.
 * @todo: Mudar o nome dessa função para taskswitchGetStatus();.
 */

//#bugbug: Mudar para int.

unsigned long get_task_status (void)
{
    return (unsigned long) task_switch_status;
}

/*
 * set_task_status:
 *    Configura o status do mecanismo de task switch.
 *    Se o mecanismo de taskswitch estiver desligado 
 * não ocorrerá a mudança.
 *
 * @todo: Mudar o nome dessa função para taskswitchSetStatus(.);
 */ 

// #bugbug: Mudar para int.

void set_task_status( unsigned long status )
{
    task_switch_status = (unsigned long) status;
}


void taskswitch_lock (void){
    task_switch_status = (unsigned long) LOCKED;
}

void taskswitch_unlock (void){
    task_switch_status = (unsigned long) UNLOCKED;
}

// Internal
// Call extra routines scheduled to this moment.
// called by task_switch.
// #importante:
// Checaremos por atividades extras que foram agendadas pelo 
// mecanismo de request. Isso depois do contexto ter sido 
// salvo e antes de selecionarmos a próxima thread.

void tsCallExtraRoutines(void)
{

    debug_print ("tsCallExtraRoutines: [FIXME] \n");

    // Kernel requests.
    //KiRequest();

    // Unix signals.
    //KiSignal();

    // ...

    // #todo: 
    // Talvez possamos incluir mais atividades extras.
}























