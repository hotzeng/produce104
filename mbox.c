#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "util.h"
#include "printf.h"
#include "kernel.h"


// buffers of messages as message box, each box has one
//static Message* buffer[MAX_MBOXEN][MAX_MBOX_LENGTH];
static int boxArr[MAX_MBOXEN];
static int free_head = 0;

typedef struct
{
  //TODO: Fill this in
  int val; 
} Message;

static Message msg[MAX_MBOXEN][MAX_MBOX_LENGTH];

typedef struct
{
  char name[MBOX_NAME_LENGTH];
  //TODO: Fill this in
  semaphore_t fullBuffer;
  semaphore_t emptyBuffer;
  semaphore_t mutex;
  int usageCnt;
  // a buffer of messages  
  Message* buffer;
  int buf_head;
  int buf_tail;
} MessageBox;


static MessageBox MessageBoxen[MAX_MBOXEN];

/* Perform any system-startup
 * initialization for the message
 * boxes.
 */
void init_mbox(void)
{
  (void) MessageBoxen;
  //TODO: Fill this in
  for(int i = 0; i < MAX_MBOXEN; i++) {
    bcopy("0", MessageBoxen[i].name, 1);
    semaphore_init(&MessageBoxen[i].fullBuffer, 0);
    semaphore_init(&MessageBoxen[i].emptyBuffer, MAX_MBOX_LENGTH);  
    semaphore_init(&MessageBoxen[i].mutex, 1);  
    MessageBoxen[i].usageCnt = 0;
    MessageBoxen[i].buffer = msg[i];
    MessageBoxen[i].buf_head = 0;
    MessageBoxen[i].buf_tail = 0;
  }
  // Initialize the boxArr
  for(int i = 0; i < MAX_MBOXEN; i++) {
    boxArr[i] = i;
  }
}

/* Opens the mailbox named 'name', or
 * creates a new message box if it
 * doesn't already exist.
 * A message box is a bounded buffer
 * which holds up to MAX_MBOX_LENGTH items.
 * If it fails because the message
 * box table is full, it will return -1.
 * Otherwise, it returns a message box
 * id.
 */


mbox_t do_mbox_open(const char *name)
{
  (void)name;
  //TODO: Fill this in
  int size = strlen(name);  
  if(free_head == 0) {
    bcopy(name, MessageBoxen[boxArr[free_head]].name, size);
    return boxArr[free_head++];
  }
  for(int i = 0; i < free_head; i++) {
    bool_t res = same_string( MessageBoxen[boxArr[i]].name, name );
    if(res == TRUE) {
      MessageBoxen[boxArr[i]].usageCnt++;
      current_running->mboxes[boxArr[i]] = 1;
      return boxArr[i];
    }
  }
  // If boxes have been used up, return -1
  if (free_head == MAX_MBOXEN)  
    return -1;
  bcopy(name, MessageBoxen[boxArr[free_head]].name, size);
  return boxArr[free_head++];
}

/* Closes a message box
 */
void do_mbox_close(mbox_t mbox)
{
  //(void)mbox;
  //TODO: Fill this in
  for(int i = 0; i < free_head; i++) {
    if(boxArr[i] == mbox) {
      MessageBoxen[mbox].usageCnt--;
      current_running->mboxes[i] = 0;

      if(MessageBoxen[mbox].usageCnt == 0) {
        free_head--;
        int tmp = boxArr[free_head];
        boxArr[free_head] = mbox;
        boxArr[i] = tmp;
        semaphore_init(&MessageBoxen[mbox].fullBuffer, 0);
        semaphore_init(&MessageBoxen[mbox].emptyBuffer, MAX_MBOX_LENGTH);
      }
      return;
    }    
  }
}

/* Determine if the given
 * message box is full.
 * Equivalently, determine
 * if sending to this mbox
 * would cause a process
 * to block.
 */
int do_mbox_is_full(mbox_t mbox)
{
  //(void)mbox;
  //TODO: Fill this in
  if(MessageBoxen[mbox].buf_head == (MessageBoxen[mbox].buf_tail + 1) % MAX_MBOX_LENGTH)
    return 1;
  else 
    return 0;
}

/* Enqueues a message onto
 * a message box.  If the
 * message box is full, the
 * process will block until
 * it can add the item.
 * You may assume that the
 * message box ID has been
 * properly opened before this
 * call.
 * The message is 'nbytes' bytes
 * starting at 'msg'
 */
void do_mbox_send(mbox_t mbox, void *msg, int nbytes)
{
  //(void)mbox;
  //(void)msg;
  //(void)nbytes;
  //TODO: Fill this in
  int nbytes_sended = 0;
  while (nbytes_sended < nbytes) {
      semaphore_down(&MessageBoxen[mbox].emptyBuffer);
      semaphore_down(&MessageBoxen[mbox].mutex);
      // debug
      //printf(20, 0,"%s sends %d    ", MessageBoxen[mbox].name, *(Message*)msg);
      MessageBoxen[mbox].buffer[MessageBoxen[mbox].buf_tail] = *((Message*)msg + nbytes_sended);
      MessageBoxen[mbox].buf_tail = (MessageBoxen[mbox].buf_tail + 1) % MAX_MBOX_LENGTH;
      nbytes_sended++;
      semaphore_up(&MessageBoxen[mbox].mutex);
      semaphore_up(&MessageBoxen[mbox].fullBuffer);
  }
}

/* Receives a message from the
 * specified message box.  If
 * empty, the process will block
 * until it can remove an item.
 * You may assume that the
 * message box has been properly
 * opened before this call.
 * The message is copied into
 * 'msg'.  No more than
 * 'nbytes' bytes will be copied
 * into this buffer; longer
 * messages will be truncated.
 */
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
  //(void)mbox;
  //(void)msg;
  //(void)nbytes;
  //TODO: Fill this in
  // debug
  #ifdef debug  
  printf(12, 0, "begin do_mbox_recv");
  #endif
  int nbytes_received = 0;
  while (nbytes_received < nbytes) {
      semaphore_down(&MessageBoxen[mbox].fullBuffer);
      semaphore_down(&MessageBoxen[mbox].mutex);
      //printf(20, 0,"%s receives %d    ", MessageBoxen[mbox].name, *(Message*)msg);
      *((Message*)msg + nbytes_received) = MessageBoxen[mbox].buffer[MessageBoxen[mbox].buf_head];
      MessageBoxen[mbox].buf_head = (MessageBoxen[mbox].buf_head + 1) % MAX_MBOX_LENGTH;
      nbytes_received++;
      semaphore_up(&MessageBoxen[mbox].mutex);
      semaphore_up(&MessageBoxen[mbox].emptyBuffer);
  }
  #ifdef debug  
  printf(13, 0, "finish do_mbox_recv");
  #endif
}

/* Returns the number of processes that have
 *  * opened but not closed this mailbox.
 *   */
unsigned int do_mbox_usage_count(mbox_t mbox)
{
  //(void)mbox;
  //TODO: Fill this in
  return MessageBoxen[mbox].usageCnt;
}

