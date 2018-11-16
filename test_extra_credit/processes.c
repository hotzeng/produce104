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
  lock_init(&local_lock);
  pid_t proc1 = spawn("p1");
  pid_t proc2 = spawn("p2");

  printf(20, 1, "Type kill to kill p1...");
  printf(21, 1, "$                           ");

  //sleep(3000);

  char buffer[1000];
  get_line(buffer, 1000);

  int killed = same_string(buffer, "kill");
  if(killed) {
    kill(proc1);
  }
  while(1){
    if(killed)
      printf(6, 0, "killed p1!!            ");
    else
      printf(6, 0, "p1 not killed! Restart ");
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
  printf(3, 0, "p2 cannot access the lock!");
  lock_acquire(&local_lock);
  //while(1) {
    printf(4, 0, "p2 acquire lock successfully!");
    do_exit();
  //}  
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

