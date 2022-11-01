
#include "minifmod.h"
#include "mixer.h"
#include "mixer_clipcopy.h"
#include "mixer_fpu_ramp.h"
#include "music.h"
#include "sound.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

//= GLOBALS ==================================================================

FSOUND_CHANNEL FSOUND_Channel[256]; // channel pool
int            FSOUND_MixRate;      // mixing rate in hz.
char           FSOUND_SoundBuffer[(1<<FSOUND_FRAGSIZE)*FSOUND_NUMFRAGS];

// mixing info
signed char * FSOUND_MixBufferMem; // mix buffer memory block
signed char * FSOUND_MixBuffer;    // mix output buffer (16bit or 32bit)
float         FSOUND_OOMixRate;    // mixing rate in hz.
int           FSOUND_BufferSize;   // size of 1 'latency' ms buffer in bytes
int           FSOUND_BlockSize;    // LATENCY ms worth of samples

// thread control variables
volatile signed char FSOUND_Software_Exit = FALSE;      // mixing thread termination flag
volatile signed char FSOUND_Software_UpdateMutex = FALSE;
volatile signed char FSOUND_Software_ThreadFinished  = TRUE;
volatile int         FSOUND_Software_hThread = 0;

static FILE * output = NULL;

int FSOUND_Init(int mixrate, int vcmmode)
{
  int count;

  FSOUND_MixRate = mixrate;

  FSOUND_OOMixRate = 1.0f / (float)mixrate;
  FSOUND_BlockSize = 1 << FSOUND_FRAGSIZE;    // Number of *samples*
  FSOUND_BufferSize = FSOUND_BlockSize;        // in linux oss do double-buffering

  mix_volumerampsteps = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
  mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

  memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);

  for (count = 0; count < 256; count++)
  {
    FSOUND_Channel[count].index = count;
    FSOUND_Channel[count].speedhi = 1;
  }
  
  output=fopen( "dump.raw","wb+" );

  return(TRUE);
}

void FSOUND_Close()
{
 fclose( output );
}

void FSOUND_Software_Fill()
{
  memset(FSOUND_MixBuffer, 0, FSOUND_BlockSize << 3);

  {
    int MixedSoFar = 0;
    int MixedLeft = FMUSIC_PlayingSong->mixer_samplesleft;
    int SamplesToMix;

    // keep resetting the mix pointer to the beginning of this portion of the ring buffer
    signed char * MixPtr = FSOUND_MixBuffer;

    while (MixedSoFar < FSOUND_BlockSize)
    {
      if (!MixedLeft)
      {
        FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong); // update new mod tick
        SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
        MixedLeft = SamplesToMix;
      } else SamplesToMix = MixedLeft;

      if (MixedSoFar + SamplesToMix > FSOUND_BlockSize)
        SamplesToMix = FSOUND_BlockSize - MixedSoFar;

      FSOUND_Mixer_FPU_Ramp(MixPtr, SamplesToMix, FALSE);

      MixedSoFar += SamplesToMix;
      MixPtr += (SamplesToMix << 3);
      MixedLeft -= SamplesToMix;

      FMUSIC_PlayingSong->time_ms += (int)(((float)SamplesToMix * FSOUND_OOMixRate) * 1000);
    }

    FMUSIC_TimeInfo.ms = FMUSIC_PlayingSong->time_ms;
    FMUSIC_TimeInfo.row = FMUSIC_PlayingSong->row;
    FMUSIC_TimeInfo.order = FMUSIC_PlayingSong->order;

    FMUSIC_PlayingSong->mixer_samplesleft = MixedLeft;
  }

  FSOUND_MixerClipCopy_Float32(FSOUND_SoundBuffer, FSOUND_MixBuffer, FSOUND_BlockSize);
}

void * FSOUND_Software_OutputThread(void * arg)
{
  FSOUND_Software_ThreadFinished = FALSE;
  while (!FSOUND_Software_Exit)
  {
    FSOUND_Software_UpdateMutex = TRUE;
    FSOUND_Software_Fill();
	fwrite(FSOUND_SoundBuffer,FSOUND_BlockSize << 2,1,output);
    FSOUND_Software_UpdateMutex = FALSE;
  }
  FSOUND_Software_ThreadFinished = TRUE;
  pthread_exit(NULL);
}
