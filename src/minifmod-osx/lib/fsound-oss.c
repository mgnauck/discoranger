// ===========================================================================
//  fsound.c
// ---------------------------------------------------------------------------
//  MiniFMOD public source code release.
//  This source is provided as-is.  Firelight Multimedia will not support
//  or answer questions about the source provided.
//  MiniFMOD Sourcecode is copyright (c) 2000, Firelight Multimedia.
//  MiniFMOD Sourcecode is in no way representative of FMOD 3 source.
//  Firelight Multimedia is a registered business name.
//  This source must not be redistributed without this notice.
// ---------------------------------------------------------------------------
//  Some parts of this source is copyright (c) 2002, Luks <luks@host.sk>
// ===========================================================================

#include "minifmod.h"
#include "mixer.h"
#include "mixer_clipcopy.h"
#include "mixer_fpu_ramp.h"
#include "music.h"
#include "sound.h"

#include <unistd.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

//= GLOBALS ==================================================================

FSOUND_CHANNEL FSOUND_Channel[256];     // channel pool
int FSOUND_MixRate;                     // mixing rate in hz.
int FSOUND_FileDSP = -1;
char FSOUND_SoundBuffer[(1<<FSOUND_FRAGSIZE)*FSOUND_NUMFRAGS];

// mixing info
signed char * FSOUND_MixBufferMem;      // mix buffer memory block
signed char * FSOUND_MixBuffer;         // mix output buffer (16bit or 32bit)
float FSOUND_OOMixRate;                 // mixing rate in hz.
int FSOUND_BufferSize;                  // size of 1 'latency' ms buffer in bytes
int FSOUND_BlockSize;                   // LATENCY ms worth of samples

// thread control variables
volatile signed char FSOUND_Software_Exit = FALSE;      // mixing thread termination flag
volatile signed char FSOUND_Software_UpdateMutex = FALSE;
volatile signed char FSOUND_Software_ThreadFinished  = TRUE;
volatile int FSOUND_Software_hThread = 0;

int FSOUND_Init(int mixrate, int vcmmode)
{
  int count;

  // =========================================================================
  //  SET SOME DEFAULTS
  // =========================================================================
  FSOUND_MixRate = mixrate;
  {
    int temp;
//    int error;
    // =======================================================================
    // INITIALIZE OSS
    // =======================================================================
    if ((FSOUND_FileDSP = open("/dev/dsp", O_WRONLY)) == -1)
     {
	  //printf( "!!! /dev/dsp : not opened.\n" );
      return(FALSE);
     }
    temp = 16;
    ioctl(FSOUND_FileDSP, SNDCTL_DSP_SAMPLESIZE, &temp);
    if (temp != 16)
    {
      FSOUND_Close();
	  //printf( "!!! sample size (%d) ...\n",error );
      return(FALSE);
    }
    temp = 1;
    ioctl(FSOUND_FileDSP, SNDCTL_DSP_STEREO, &temp);
    if (temp != 1)
    {
	  //printf( "!!! stereo (%d) ...\n",error );
      FSOUND_Close();
      return(FALSE);
    }
    temp = mixrate;
    ioctl(FSOUND_FileDSP, SNDCTL_DSP_SPEED, &temp);
//    if (temp != mixrate)
//    {
//printf( "!!! speed (%dHz : %d) ...\n",temp,error );
//      FSOUND_Close();
//      return(FALSE);
//    }
    temp = (FSOUND_NUMFRAGS<<16)+FSOUND_FRAGSIZE;
    ioctl(FSOUND_FileDSP, SNDCTL_DSP_SETFRAGMENT, &temp);
    if ((temp & 0xFFFF)  != FSOUND_FRAGSIZE)
    {
	  //printf( "!!! fragment (%d : %d) ...\n",temp,error );
      FSOUND_Close();
      return(FALSE);
    }
  }

  // =========================================================================
  // INITIALIZE SOFTWARE MIXER
  // =========================================================================
  FSOUND_OOMixRate = 1.0f / (float)mixrate;
  FSOUND_BlockSize = 1 << FSOUND_FRAGSIZE;    // Number of *samples*
  FSOUND_BufferSize = FSOUND_BlockSize;        // in linux oss do double-buffering

  mix_volumerampsteps = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
  mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

  // =========================================================================
  // ALLOC GLOBAL CHANNEL POOL
  // =========================================================================
  memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);

  // =========================================================================
  // SET UP CHANNELS
  // =========================================================================
  for (count = 0; count < 256; count++)
  {
    FSOUND_Channel[count].index = count;
    FSOUND_Channel[count].speedhi = 1;
  }

  return(TRUE);
}

void FSOUND_Close()
{
  // ========================================================================================================
  // SHUT DOWN OUTPUT DRIVER
  // ========================================================================================================
  if (FSOUND_FileDSP != -1)
    close(FSOUND_FileDSP);
}

void FSOUND_Software_Fill()
{
  // =========================================================================
  // MIXBUFFER CLEAR
  // =========================================================================
  memset(FSOUND_MixBuffer, 0, FSOUND_BlockSize << 3);

  // =========================================================================
  // UPDATE MUSIC
  // =========================================================================
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
//        FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong); // update new mod tick
		FMUSIC_UpdateXM(FMUSIC_PlayingSong);
        SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
        MixedLeft = SamplesToMix;
      }
      else
        SamplesToMix = MixedLeft;

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

  // =========================================================================
  // CLIP AND COPY BLOCK TO OUTPUT BUFFER
  // =========================================================================
  FSOUND_MixerClipCopy_Float32(FSOUND_SoundBuffer, FSOUND_MixBuffer, FSOUND_BlockSize);

}

void * FSOUND_Software_OutputThread(void * arg)
{
  FSOUND_Software_ThreadFinished = FALSE;
  while (!FSOUND_Software_Exit)
  {
    usleep(10000);

    FSOUND_Software_UpdateMutex = TRUE;

    FSOUND_Software_Fill();

    write(FSOUND_FileDSP, FSOUND_SoundBuffer, FSOUND_BlockSize << 2);

    FSOUND_Software_UpdateMutex = FALSE;
  }
  FSOUND_Software_ThreadFinished = TRUE;
  pthread_exit(NULL);
}

