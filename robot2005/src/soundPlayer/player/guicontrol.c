/*
 * this file is a part of amp software, (C) tomislav uzelac 1996,1997
 * Interface avec le robot 2004
*/
/* guicontrol.c
 *
 * Edouard Lafargue, 12 May 1997
 * Laurent Saint-Marcel, 5 november 2003
 */ 

#include <unistd.h>        
#define   SOUND_INFO
#define   SOUND_SHM_INFO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <pthread.h>

#include "../../include/soundList.h"
//#include "../../include/robotTypes.h"


#include "amp.h"

#include <sys/types.h>
#include <sys/stat.h>


#include <sys/uio.h>


#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "audio.h"
#include "formats.h"
#include "getbits.h"
#include "huffman.h"
#include "layer2.h"
#include "layer3.h"
#include "position.h"
#include "rtbuf.h"
#include "transform.h"
#include "controldata.h"

typedef int bool;

#define MAX_SECOND_WITHOUT_SOUND  5 // joue automatique un son toutes les 20s

#define SOUND_FILENAME_LENGTH  256
#define SOUND_STACK_SIZE  15
// all sound must be 22kHz, 8bit, mono to have a valid delay time 
// when play two sounds one just after the other
#define SOUND_LENGTH_TIME_FACTOR  60 // 1/22kHz * 1 000 000 us

static SoundShmStruct *shm_=NULL;
static int stackIndex_=0;
static SoundShmStruct stack_[SOUND_STACK_SIZE];

static int volume_=3;
static pthread_t thread_;
static unsigned char playExit_=0;
static char important_=1;

int  init();
void playSound(SoundId sound);
void playStop();
void playPause();
void playFinished();
void playFile(const char* file);
void* shmReadThread(void* data);
int parseCommand(SoundId command, 
		 unsigned char value);
int checkNonUrgentOrder(int* index);
int checkUrgentOrder(int* index);

// ----------------------------------------------------------------------------
// shmCreate
// ----------------------------------------------------------------------------
int shmCreate(const char* shmKeyFile,
	      int mode,
	      int shmSize)
{
    key_t key;
    int shm_id;

    /* Linking to shared memory segment */
    printf("Creating a shared memory with key=%s\n", 
             shmKeyFile);

    /* Create the key */
    if ((key = ftok(shmKeyFile, 'S')) == -1) {
        printf("cannot create shared memory (step 1-3) %s\n", 
                  shmKeyFile);
        return 1;
    }
    
    /* Connect to (and possibly create) the segment */
    if ((shm_id = shmget(key, shmSize, mode)) == -1) {
        if ((shm_id = shmget(key, shmSize, mode | IPC_CREAT)) == -1) {
            printf("cannot create shared memory (step 2-3) %s\n", 
		      shmKeyFile);
            return 2;
        } 
    } 
    
    /* Attach to the segment to get a pointer to it */
    shm_ = (void *)shmat(shm_id, (void *)0, 0);
    if ((char *)(shm_) == (char *)(-1)) {
        printf("cannot create shared memory (step 3-3) %s\n", 
                  shmKeyFile);
        return 3;
    }
    printf("SHM created %s\n", shmKeyFile);
    return 0;
}

// ----------------------------------------------------------------------------
// soundPlayerSIGINT
// ----------------------------------------------------------------------------
// catch ctrl+C and stop the sound player
// ----------------------------------------------------------------------------
void soundPlayerSIGINT(int sig) {
  playStop();
  playExit_=1;
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
// create the shared memeory and the receving thread
// ----------------------------------------------------------------------------
int init()
{
  int res=0;
  // enregistre la detection du Ctrl+C et du callback correspondant qui 
  // stoppe le programme immediatement
  (void) signal(SIGINT, soundPlayerSIGINT);
   // create or connect to the shared memory
  
  res = shmCreate(SOUND_SHM_FILENAME, 
                  0644,
                  sizeof(SoundShmStruct));

  if (res != 0 || shm_ == NULL) {
    printf("SHM create error: %d\n", res);
    return res;
  } 

  res =  pthread_create( &thread_,				
			 NULL,					
			 shmReadThread,
			 NULL ); 
  if (res != 0) {
    printf("Thread create error: %d\n", res);
  }
  return !res;
}

// ----------------------------------------------------------------------------
// parseCommand
// ----------------------------------------------------------------------------
// Run a command which does not correpond to a sound 
// ----------------------------------------------------------------------------
int parseCommand(SoundId command, 
		 unsigned char value)
{
  int i;
  switch(command) {
  case SOUND_CLEAR:
      playStop();
      printf("SOUND_CLEAR\n");
      for(i=0;i<SOUND_STACK_SIZE;i++) stack_[i].newData=0;
      break;
  default:
      playSound(command);
    break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// playStop
// ----------------------------------------------------------------------------
// stop playing a file. 
// ----------------------------------------------------------------------------
void playStop()
{
   GUI_STOP = TRUE;
  if (GUI_PLAYING) audioBufferFlush();
}

// ----------------------------------------------------------------------------
// playFinished
// ----------------------------------------------------------------------------
// This function is run when playFile finishes. It select the next random music
// ----------------------------------------------------------------------------
void playFinished()
{
  
}

// ----------------------------------------------------------------------------
// playPause
// ----------------------------------------------------------------------------
// start/stop a pause while playing a file
// ----------------------------------------------------------------------------
void playPause() 
{
  if(GUI_PAUSE)
    GUI_PAUSE= FALSE;
  else 
    GUI_PAUSE=TRUE;
}

// ----------------------------------------------------------------------------
// playFile
// ----------------------------------------------------------------------------
// Play an mp3 file
// ----------------------------------------------------------------------------
void playFile(const char* filename)
{
  int fd=-1;
  int length = strlen(filename);
  if (strcmp(filename+length-3, "mp3") == 0) {
    // prepare les arguments de exec dans le cas d'un wav
    printf("Play: %s\n", filename);
    fd = open(filename, O_RDONLY);
    if (fd==-1) {
      printf("Cannot open %s\n", filename);
      return;
    }
    GUI_FD_TO_PLAY = fd;
    if (GUI_PLAYING) audioBufferFlush();
  } else {
    printf("Cannot play %s\n", filename);
  }
}


// ----------------------------------------------------------------------------
// playSound
// ----------------------------------------------------------------------------
// Play the sound which informations is stored in soundStack_
// ----------------------------------------------------------------------------
void playSound(SoundId sound)
{   
    // set filename and volume parameters
    static char file[SOUND_FILENAME_LENGTH];
    if (sound<SOUND_NBR) {
      // prepare les arguments de exec dans le cas d'un wav
      sprintf(file, "%s/%s", 
	      SOUND_DIRECTORY,
	      soundList[sound].filename);
      playFile(file);
    } 
}

// ----------------------------------------------------------------------------
// shmReadThread
// ----------------------------------------------------------------------------
// Thread that check the shared memory and store new 
// informations in soundStack_
// ----------------------------------------------------------------------------
void* shmReadThread(void* data)
{
  int i;
  stackIndex_=0;
  while(shm_ && !playExit_) {
    // wait for data in the shared memory and store them in an array
    if (shm_->newData) {
      printf("Receive new data: %d %d\n", shm_->id, shm_->data);
      memcpy(&(stack_[stackIndex_]), shm_, sizeof(SoundShmStruct));
      if (stack_[stackIndex_].id == SOUND_CLEAR) {
	playStop();
	printf("SOUND_CLEAR\n");
	for(i=0;i<SOUND_STACK_SIZE;i++) stack_[i].newData=0;
      } else {
	if (++stackIndex_ >= SOUND_STACK_SIZE) stackIndex_ = 0;
	stack_[stackIndex_].newData = 0;
      }
      shm_->newData=0;
    }
    usleep(10000);
    shm_->aliveCounter++;
  }
  return NULL;
}

// ----------------------------------------------------------------------------
// GUIstatusDisplay
// ----------------------------------------------------------------------------
// This routine sends the current frame, and where we are in the file
// (in percent)
// ----------------------------------------------------------------------------
void GUIstatusDisplay(int frameno)
{
  //  printf(" frame: %d\n", frameno);
}

// ----------------------------------------------------------------------------
// checkNonUrgentOrder
// ----------------------------------------------------------------------------
// check messages that arrives when waiting for new messages without
// playing anyting
// ----------------------------------------------------------------------------
int checkNonUrgentOrder(int* index)
{
  if (stack_[*index].newData) {
    parseCommand(stack_[*index].id, stack_[*index].data);
    stack_[*index].newData = 0;
    *index = (*index+1)%SOUND_STACK_SIZE;
    return 1;
  } else {
    return 0;
  }
}

// ----------------------------------------------------------------------------
// checkUrgentOrder
// ----------------------------------------------------------------------------
// check the messages that arrives while playing a file 
// ----------------------------------------------------------------------------
int checkUrgentOrder(int* index)
{
  // check priority sounds
  int indexLastStack = stackIndex_-1;
  unsigned char data=0;
  if (indexLastStack<0) indexLastStack=SOUND_STACK_SIZE-1;
  if (stack_[indexLastStack].newData) {
    SoundId sound = stack_[indexLastStack].id;
    if ((sound >= SOUND_NBR)
	|| (stack_[indexLastStack].data == SND_PRIORITY_URGENT)) {
      data = stack_[indexLastStack].data;
      parseCommand(SOUND_CLEAR, 0);
      parseCommand(sound, data);
      *index = (indexLastStack+1)%SOUND_STACK_SIZE;
      return 1;
    }
  }
  return 0;
}

// ----------------------------------------------------------------------------
// gui_control
// ----------------------------------------------------------------------------
// entry point of the program
// ----------------------------------------------------------------------------
void gui_control(void)
{
  int index=0;
  int timeWithoutNothing;
  init();

  GUI_FD_TO_PLAY = -1;
  while(!playExit_)
    {
      GUI_PAUSE = FALSE;
      GUI_STOP = FALSE;
      GUI_STOPPED = TRUE;
      GUI_PLAY = FALSE;
      GUI_PLAYING = FALSE;
      important_=1;
      checkNonUrgentOrder(&index);
      
      if (GUI_FD_TO_PLAY != -1) {
	while(GUI_FD_TO_PLAY != -1 && !playExit_)
	  {
	    timeWithoutNothing=0;
	    GUI_PAUSE = FALSE;
	    GUI_STOP = FALSE;
	    GUI_STOPPED = TRUE;
	    GUI_PLAY = FALSE;
	    GUI_PLAYING = FALSE;
	    shm_->isPlayingSound = 1;
	    decodeMPEG_2(GUI_FD_TO_PLAY, &index);
	  }
      } else {
	usleep(50000);
	timeWithoutNothing++;
      }
    }   
}


// ----------------------------------------------------------------------------
// decodeMPEG
// ----------------------------------------------------------------------------
// decopde an mp3 fila end play it
// ----------------------------------------------------------------------------
int decodeMPEG_2(int inFilefd, int* index)
{
  struct AUDIO_HEADER header;
  int cnt,g,err=0;

  if ((in_file=fdopen(inFilefd,"r"))==NULL) {
    return(1);
  }

  append=data=nch=0; /* initialize globals */

  GUI_STOPPED = FALSE;
  GUI_PLAYING = TRUE;
  GUI_FD_TO_PLAY = -1;
  
  for (cnt=0;;cnt++) {
    if ((g=gethdr(&header))!=0) {
      switch (g) {
      case GETHDR_ERR: die("error reading mpeg bitstream. exiting.\n");
	break;
      case GETHDR_NS : warn("this is a file in MPEG 2.5 format, which is not defined\n");
	warn("by ISO/MPEG. It is \"a special Fraunhofer format\".\n");
	warn("amp does not support this format. sorry.\n");
	break;
      case GETHDR_FL1: warn("ISO/MPEG layer 1 is not supported by amp (yet).\n");
	break;
      case GETHDR_FF : warn("free format bitstreams are not supported. sorry.\n");
	break;	
      case GETHDR_SYN: warn("oops, we're out of sync.\n");
	break;
      default: 
        break;
      }
      break;
    }
    
    if (!(cnt%10)){
        GUIstatusDisplay(cnt);
    }
    
    if (GUI_STOP || (GUI_FD_TO_PLAY != -1)){
        break;
    }
    if ((!important_ && checkNonUrgentOrder(index))
	|| (important_ && checkUrgentOrder(index))) {
	if (GUI_STOP || (GUI_FD_TO_PLAY != -1)){
	  break;
	}
    }

    /* crc is read just to get out of the way.
     */
    if (header.protection_bit==0) getcrc();
    
    if (!cnt && A_AUDIO_PLAY) { /* setup the audio when we have the frame info */
      if (AUDIO_BUFFER_SIZE==0) {
	audioOpen(t_sampling_frequency[header.ID][header.sampling_frequency],
		  (header.mode!=3),
		  A_SET_VOLUME);
	
      } else {
	audioBufferOpen(t_sampling_frequency[header.ID][header.sampling_frequency],
			(header.mode!=3),
			A_SET_VOLUME);
      }
    }
    
    if (layer3_frame(&header,cnt)) {
      warn(" error. blip.\n");
      err=1;
      break;
    } 
    
  }
  fclose(in_file);

  if (A_AUDIO_PLAY) {
    if (AUDIO_BUFFER_SIZE!=0)
      audioBufferClose();
    else
      audioClose();
  } else {
    /* fclose(out_file);*/
  }

  GUI_STOPPED = TRUE;
  GUI_PLAYING = FALSE;
  shm_->isPlayingSound = 0;
  if (!(GUI_STOP) && (GUI_FD_TO_PLAY == -1)) {
    playFinished();
  }
  return(err);
  dummyFunction();
}
