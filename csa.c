#include "csa.h"

//#define STATIC

void _klock( ) { trop_MutexLock(&kmutex); }
void _kunlock( ) { trop_MutexUnlock(&kmutex); }

void _trop_csa_run_clock( )
{
  if (CSA_CLOCK_THREAD>0) usleep(CSA_CLOCK_THREAD);
}

void *_trop_csa_run(void*data)
{
  struct trop_Csa *csa = (struct trop_Csa*)data;
  int trylock = -1;
  
  if (DEBUG==TRUE) printf(":_trop_csa_run.main\n");
  
  while(1)
  {
    while(1)
    {
      _klock();
      if (DEBUG==TRUE) printf("\t:_trop_csa_run:lockloop state %d\n",csa->state);
      if (csa->state == CSA_STATE_KLOCKED || csa->state == CSA_STATE_INIT)
      {
        _kunlock();
        break;
      }
      _kunlock();
      _trop_csa_run_clock();
    }
    
    while ((trylock = trop_MutexTrylock(&csa->mutex)) != TRYLOCK_UNLOCKABLE)
    { _trop_csa_run_clock(); }
    
    if (DEBUG==TRUE) printf(":trylock:%d\n",trylock);
    
    _klock();
    csa->state = CSA_STATE_WAIT; 				// can be unlocked
    _kunlock();
    trop_MutexLock(&csa->mutex); 				// create deadlock
    
    _klock();
    if (csa->sched == CSA_SCHED_KERNEL)
    {
      _kunlock();
      while(1)
      {
        _klock();
        if (csa->state == CSA_STATE_KUNLOCKED) 
        {
          _kunlock();
          break;
        }
        _kunlock();
        if (DEBUG==TRUE) printf(":_trop_csa_run:wait_loop\n");
        _trop_csa_run_clock();
      }
    }
    else
      _kunlock();
    
    if (DEBUG==TRUE) printf("\t\t\t:_trop_csa_run.cycle\n");
    
    if (csa->task != NULL)
    {
      csa->task->call(csa->task->data);
      csa->task = NULL;
    }
    
    _klock();
    if (csa->sched == CSA_SCHED_KERNEL) csa->state = CSA_STATE_KLOCKABLE;
    if (csa->sched == CSA_SCHED_FREE) csa->state = CSA_STATE_KLOCKED;
    _kunlock();
    if (DEBUG==TRUE) printf("\t\t\t:_trop_csa_run.cycle.end\n");
  }
}

int trop_CsaInit( struct trop_Csa *csa )
{
  if (!csa) return -1;
  trop_MutexInit(&csa->mutex);
  return 0;
}

int trop_CsaInitEx( struct trop_CsaKernel *kernel, struct trop_Csa *csa )
{
  if (kernel == NULL || csa == NULL) return -1;
  if (trop_CsaInit(csa) == -1) return -1;

  csa->state		= CSA_STATE_INIT;  
  csa->kernel 		= kernel;
  csa->atom.call 	= _trop_csa_run;
  csa->atom.data 	= csa;
  csa->task		= NULL;
  csa->sched		= CSA_SCHED_KERNEL;
  csa->id[0]		= 0x00;
  
  _klock();
  if (kernel->csa == NULL)
  {
    csa->next = NULL;
    kernel->csa = csa;
  }
  else
  {
    csa->next = kernel->csa;
    kernel->csa = csa;
  }
  trop_MutexLock(&csa->mutex);
  csa->state = CSA_STATE_INIT;
  _kunlock();
  
  trop_AtomInit(&csa->atom);
  return 0;
}

struct trop_Csa *new_TropCsa( )
{
  struct trop_Csa *csa = (struct trop_Csa*)malloc(sizeof(struct trop_Csa));
  return csa;
}

int trop_CsaTryUnlock( struct trop_Csa *csa )
{
  return _trop_CsaUnlock(csa,FALSE);
}

int trop_CsaUnlock( struct trop_Csa *csa )
{
  return _trop_CsaUnlock(csa,TRUE);
}

int _trop_CsaUnlock( struct trop_Csa *csa, BOOL loop )
{
  while(1)
  {
    if (DEBUG==TRUE) printf(":trop_CsaUnlock:loop\n");
    _klock();
    if (csa->state == CSA_STATE_WAIT)
    {
      if (DEBUG==TRUE) printf(":trop_CsaUnlock:CSA_STATE_WAIT\n");
      if (trop_MutexTrylock(&csa->mutex) == TRYLOCK_UNLOCKABLE)
      {
        trop_MutexUnlock(&csa->mutex);
        _kunlock();
        if (loop==FALSE) break;
        continue;
      }
      else
      {
        if (DEBUG==TRUE) printf(":lock1\n");
        trop_MutexUnlock(&csa->mutex);
      }
      
      csa->state = CSA_STATE_KUNLOCKED;
      _kunlock();
      break;
    }
    else
    {
      if (DEBUG==TRUE) printf(":trop_CsaUnlock.badstate\n");
    }
    _kunlock();
    if (loop==FALSE) break;
    _kernel_thread_clock();
  }
  if (DEBUG==TRUE) printf(":trop_CsaUnlock.end\n");
  return 0;
}

int trop_CsaKernelInit( struct trop_CsaKernel *kernel )
{
  if (!kernel) return -1;
  kernel->csa = NULL;
  kernel->task = NULL;
  trop_MutexInit(&kmutex);
  return 0;
}

struct trop_CsaKernel *new_TropCsaKernel( )
{
  struct trop_CsaKernel *kernel = (struct trop_CsaKernel*)malloc(sizeof(struct trop_CsaKernel));
  return kernel;
}

void _kernel_thread_clock( )
{
  if (CSA_CLOCK_KERNEL>0) usleep(CSA_CLOCK_KERNEL);
}

void *_kernel_thread(void*data)
{
  if (DEBUG==TRUE) printf(":_kernel_thread\n");
  struct trop_CsaKernel *kernel = (struct trop_CsaKernel*)data;
  struct trop_Csa *current = NULL;
  struct trop_CsaTask *currenttask = NULL;
  BOOL hit;
  while(1)
  {
    hit = FALSE;
    _klock();
    if (DEBUG==TRUE) printf(":kernel.lock\n");
    
    current = kernel->csa;
    while(current)
    {
      if (DEBUG==TRUE) printf("\t\t:_kernel_thread.loop1 csa_state %d\n",current->state);
      if (current->state == CSA_STATE_KLOCKABLE)
      {
        if (DEBUG==TRUE) printf("\t:set_KLOCKED\n");
        current->state = CSA_STATE_KLOCKED;
        hit =  TRUE;
      }
      current = current->next;
    }
    
    currenttask = kernel->task;
    while(currenttask)
    {
      if (currenttask->next == NULL) break;
      currenttask = currenttask->next;
    }
    if (currenttask)
    {
      // run task in one of the registered CSAs
      current = kernel->csa;
      while(current)
      {
        if (current->state == CSA_STATE_WAIT)
        {
          if (DEBUG==TRUE) printf(":add task to csa\n");
          current->task = currenttask;
          // todo: remove task from task list
          _kunlock();
          trop_CsaTryUnlock(current); // possibly retried at next cycle
          _klock();
          break;
        }
        current = current->next;
      }
    }
    
    if (DEBUG==TRUE) printf(":kernel.unlock\n");
    _kunlock();
    if (hit == FALSE) _kernel_thread_clock();
  }
}

int trop_CsaKernelStart( struct trop_CsaKernel *kernel )
{
  kernel->atom.call = _kernel_thread;
  kernel->atom.data = kernel;
  trop_AtomInit(&kernel->atom);
  return 0;
}

int trop_CsaKernelTaskAdd( struct trop_CsaKernel *kernel, struct trop_CsaTask *task )
{
  struct trop_CsaTask *current = NULL;
  int tasknum;
  while(1)
  {
    tasknum = 0;
    _klock();
    current = kernel->task;
    while(current)
    {
      tasknum++;
      current = current->next;
    }
    _kunlock();
    if (tasknum < CSA_KERNEL_MAXTASKS)
    {
      break;
    }
    _kernel_thread_clock();
  }
  _klock();
  if (kernel->task == NULL)
  {
    task->next = NULL;
    kernel->task = task;
  }
  else
  {
    task->next = kernel->task;
    kernel->task = task;
  }
  _kunlock();
  return 0;
}

int trop_CsaTaskInit( struct trop_CsaTask *task, void *(*call)(void*data), void *data )
{
  if (task==NULL || call == NULL || data == NULL) return -1;
  task->call = call;
  task->data = data;
  return 0;
}

struct trop_CsaTask *new_tropCsaTask( )
{
  struct trop_CsaTask *task = (struct trop_CsaTask*)malloc(sizeof(struct trop_CsaTask));
  return task;
}

int trop_CsaSetID( struct trop_Csa *csa, char *id )
{
  if (!csa || !id ) return -1;
  memcpy((void*)csa->id,id,CSA_IDLEN);
  return 0;
}

struct trop_Csa *trop_CsaKernelIsCSAID( struct trop_CsaKernel *kernel, char *id )
{
  struct trop_Csa *current = kernel->csa;
  while(current)
  {
    if (memcmp((void*)current->id,(void*)id,CSA_IDLEN) == 0)
      return current;
    current = current->next;
  }
  return NULL;
}

/*
  parse a message and if the message is intended for a local CSA,
  provide the message to the CSA
*/
int trop_CsaKernelParseMSG( struct trop_CsaKernel *kernel, struct trop_CsaMsg *msg )
{
  struct trop_Csa *csa = NULL;
  if ((csa = trop_CsaKernelIsCSAID(kernel,(char*)msg->id)) == NULL)
    return -1;
  
  return 0;
}

#ifdef STATIC
  int main( )
  {
    trop_Init();
    struct trop_Csa *csa1 = new_TropCsa();
    struct trop_Csa *csa2 = new_TropCsa();
    struct trop_Csa *csa3 = new_TropCsa();
    struct trop_CsaKernel *kernel = new_TropCsaKernel();
    trop_CsaKernelInit(kernel);
    trop_CsaInitEx( kernel, csa1 );
    trop_CsaInitEx( kernel, csa2 );
    trop_CsaInitEx( kernel, csa3 );
    trop_CsaKernelStart(kernel);
    int i = 0;
    int j = 0;
    while(1) 
    {
      if (i == 5)
      {
        j += i;
        printf("cycle %d\n",j);
        i = 0;
      }
      else
        i++;
      if (DEBUG==TRUE) printf("\t:loop\n");
      trop_CsaUnlock(csa1);
      trop_CsaUnlock(csa2);
      trop_CsaUnlock(csa3);
      //usleep(50);
    }
    trop_Deinit();
    return 0;
  }
#endif
