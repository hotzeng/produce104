#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"
#include "sync.h"
/*
 * These are all of the programs which we include in our
 * ramdisk filesystem.
 *
 * It is VERY IMPORTANT that these functions do
 * not use any global variables.  This is because
 * each one might be running many times, and we do
 * not have any facility to duplicate their
 * data segments.
 *
 */
lock_t local_lock;

static void get_line(char *buffer, int maxlen);

/* The 'init' process is a shell
 * that lets you spawn other programs.
 */
void init_process(void)
{
  pid_t proc1 = spawn("p1");
  printf(10, 50, "Id of p1: %d", proc1);
  pid_t proc2 = spawn("p2");
  printf(11, 50, "Id of p2: %d", proc2);
  for(;;)
  {
    printf(20, 1, "Type kill to kill p1...");
    printf(21, 1, "$                           ");



    char buffer[100];
    get_line(buffer, 100);

    if(same_string(buffer, "kill")) {
      printf(20, 50, "begin to kill p1!!");
      kill(proc1);
      printf(20, 50, "killed p1!!       ");
    }
  }
}


/* The HELP process prints help, then exits
 */
void p1(void)
{
  lock_acquire(&local_lock); 
  for(;;) {
    printf(1, 0, "Lock has been acquired by p1, kill me to wake up p2!");
  }
}


void p2(void) {
  for(;;) {
    printf(3, 0, "p2 cannot access the lock!");
    lock_acquire(&local_lock);
    printf(4, 0, "p2 acquire lock successfully!");
  }  
}

static void get_line(char *buffer, int maxlen)
{
  // debug
  #ifdef debug
  printf(10, 0, "begin get_line");
  #endif
  int offset;
  for(offset=0; offset<maxlen; )
  {
    char c = get_char();
    if( c == '\n' || c == '\r' )
      break;
    else if( c == '\b' )
    {
      if( offset > 0 )
      {
        offset --;
        printf(21, 1 + 2 + offset, " ");
      }
      continue;
    }
    else
    {
      printf(21, 1 + 2 + offset, "%c", c);
      buffer[offset++] = c;
    }
  }
  if( offset >= maxlen )
    offset = maxlen - 1;
  buffer[offset] = '\0';
}

