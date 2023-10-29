
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

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

//= GLOBALS ==================================================================

//#define USE_ALUT
//#define FILL_PROC

FSOUND_CHANNEL FSOUND_Channel[256];     // channel pool
int            FSOUND_MixRate;                     // mixing rate in hz.
char           FSOUND_SoundBuffer[(1<<FSOUND_FRAGSIZE)*FSOUND_NUMFRAGS];

// mixing info
signed char   FSOUND_MixBuffer[((1 << FSOUND_FRAGSIZE) << 3 ) + 256]; // mix output buffer (16bit or 32bit)
float         FSOUND_OOMixRate;    // mixing rate in hz.
int           FSOUND_BlockSize;    // LATENCY ms worth of samples

// thread control variables
volatile signed char FSOUND_Software_Exit = FALSE; // mixing thread termination flag
volatile signed char FSOUND_Software_UpdateMutex = FALSE;
volatile signed char FSOUND_Software_ThreadFinished  = TRUE;
volatile int         FSOUND_Software_hThread = 0;

#ifndef USE_ALUT
ALCdevice  * device;
ALCcontext * context;
#endif
#define OPENAL_BUFFERS 2
ALuint       buffer[OPENAL_BUFFERS];
ALuint       source;

int FSOUND_Init(int mixrate, int vcmmode)
{
 int count;

 FSOUND_MixRate = mixrate;
 FSOUND_OOMixRate = 1.0f / (float)mixrate;
 FSOUND_BlockSize = 1 << FSOUND_FRAGSIZE;

 mix_volumerampsteps = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
 mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

// memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);
 for ( count=0;count < 256;count++ )
  {
   FSOUND_Channel[count].index=count;
   FSOUND_Channel[count].speedhi=1;
  }

#ifdef USE_ALUT
 alutInit( 0,NULL );
#else
 device = alcOpenDevice(NULL);
#ifndef NO_ERRORCHECK
 if (device == NULL) return FALSE;
#endif
 context = alcCreateContext(device,NULL);
#ifndef NO_ERRORCHECK
 if (context == NULL) return FALSE;
#endif
 alcMakeContextCurrent(context);
#endif

 return TRUE;
}

void FSOUND_Close()
{
#ifdef USE_ALUT
 alutDone();
#else
#ifndef NO_STOPSONG
 alSourceStop( source );
#endif
#ifndef NO_FREE
 alSourceUnqueueBuffers( source,OPENAL_BUFFERS,buffer );
 alDeleteSources( 1,&source );
 alDeleteBuffers( OPENAL_BUFFERS,buffer );
#endif
#endif
}

#ifdef FILL_FUNC
void FSOUND_Software_Fill()
{
 // keep resetting the mix pointer to the beginning of this portion of the ring buffer
 signed char * MixPtr = &FSOUND_MixBuffer;
 int           MixedSoFar = 0;
 int           MixedLeft = FMUSIC_PlayingSong->mixer_samplesleft;
 int           SamplesToMix;

 memset(FSOUND_MixBuffer, 0, FSOUND_BlockSize << 3);
 while (MixedSoFar < FSOUND_BlockSize)
  {
   if (!MixedLeft)
    {
//     FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong); // update new mod tick
	 FMUSIC_UpdateXM(FMUSIC_PlayingSong);
     SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
     MixedLeft = SamplesToMix;
    } else SamplesToMix = MixedLeft;

   if ( MixedSoFar + SamplesToMix > FSOUND_BlockSize ) SamplesToMix=FSOUND_BlockSize - MixedSoFar;
   FSOUND_Mixer_FPU_Ramp( MixPtr,SamplesToMix,FALSE );
   MixedSoFar+=SamplesToMix;
   MixPtr+=( SamplesToMix << 3 );
   MixedLeft-=SamplesToMix;
   FMUSIC_PlayingSong->time_ms+=(int)( ( (float)SamplesToMix * FSOUND_OOMixRate ) * 1000 );
  }

 FMUSIC_PlayingSong->mixer_samplesleft=MixedLeft;
 FSOUND_MixerClipCopy_Float32( FSOUND_SoundBuffer,FSOUND_MixBuffer,FSOUND_BlockSize );
}
#endif

void * FSOUND_Software_OutputThread(void * arg)
{
 int    processed;
 int    bs = FSOUND_BlockSize << 2;
 ALuint b;

 alGenBuffers( OPENAL_BUFFERS,buffer );
 alGenSources( 1,&source );
// alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
// alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
// alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
// alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
// alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );

 for ( b=0;b < OPENAL_BUFFERS;b++ )
  {
#ifdef FILL_FUNC
   FSOUND_Software_Fill();
#endif
   alBufferData( buffer[b],AL_FORMAT_STEREO16,FSOUND_SoundBuffer,bs,FSOUND_MixRate );
  }

 alSourceQueueBuffers( source,b,buffer );
 alSourcePlay( source );

 FSOUND_Software_ThreadFinished=FALSE;
 while ( ! FSOUND_Software_Exit )
  {
   usleep( 20000 );
   FSOUND_Software_UpdateMutex=TRUE;
   alGetSourcei( source,AL_BUFFERS_PROCESSED,&processed );
   while ( processed-- )
	{
	 alSourceUnqueueBuffers( source,1,&b );

#ifdef FILL_FUNC
     FSOUND_Software_Fill();
#else
     {
      // keep resetting the mix pointer to the beginning of this portion of the ring buffer
      signed char * MixPtr = &FSOUND_MixBuffer;
      int           MixedSoFar = 0;
      int           MixedLeft = FMUSIC_PlayingSong->mixer_samplesleft;
      int           SamplesToMix;

      memset(FSOUND_MixBuffer, 0, FSOUND_BlockSize << 3);
      while (MixedSoFar < FSOUND_BlockSize)
       {
        if (!MixedLeft)
         {
//          FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong); // update new mod tick
		  FMUSIC_UpdateXM(FMUSIC_PlayingSong);
          SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
          MixedLeft = SamplesToMix;
         } else SamplesToMix = MixedLeft;

        if ( MixedSoFar + SamplesToMix > FSOUND_BlockSize ) SamplesToMix=FSOUND_BlockSize - MixedSoFar;
        FSOUND_Mixer_FPU_Ramp( MixPtr,SamplesToMix,FALSE );
        MixedSoFar+=SamplesToMix;
        MixPtr+=( SamplesToMix << 3 );
        MixedLeft-=SamplesToMix;
        FMUSIC_PlayingSong->time_ms+=(int)( ( (float)SamplesToMix * FSOUND_OOMixRate ) * 1000 );
       }

      FMUSIC_PlayingSong->mixer_samplesleft=MixedLeft;
      FSOUND_MixerClipCopy_Float32( FSOUND_SoundBuffer,FSOUND_MixBuffer,FSOUND_BlockSize );
     }
#endif

     alBufferData(b,AL_FORMAT_STEREO16,FSOUND_SoundBuffer,bs,FSOUND_MixRate);
     alSourceQueueBuffers( source,1,&b );
    }
   FSOUND_Software_UpdateMutex=FALSE;
  }
 FSOUND_Software_ThreadFinished=TRUE;
#ifndef NO_FREE
 pthread_exit( NULL );
#endif
 return NULL;
}
