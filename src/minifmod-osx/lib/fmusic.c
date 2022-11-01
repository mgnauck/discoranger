// ===========================================================================
//  fmusic.c
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

#include <stdio.h>
#include <string.h>

#if defined( __LINUX__ ) || defined( __MACOSX__ )
# include <pthread.h>
# include <unistd.h>
#endif

#include "minifmod.h"
#include "music_formatxm.h"
#include "sound.h"

FMUSIC_MODULE * FMUSIC_PlayingSong = NULL;
FMUSIC_CHANNEL FMUSIC_Channel[32];
#if defined( __LINUX__ )
FMUSIC_TIMMEINFO FMUSIC_TimeInfo;
#endif
#ifdef __WIN32__
FMUSIC_TIMMEINFO *FMUSIC_TimeInfo;
#endif
FSOUND_SAMPLE FMUSIC_DummySample;
FSOUND_CHANNEL MUSIC_DummyChannel;
FMUSIC_INSTRUMENT FMUSIC_DummyInstrument;

// = PRIVATE FUNCTIONS =======================================================

void FMUSIC_SetBPM(FMUSIC_MODULE *module, int bpm)
{
  module->bpm = bpm;
  module->mixer_samplespertick = (int)((float)FSOUND_MixRate * (1000.0f / ((float)bpm * 2.0f / 5.0f)) / 1000.0f);
}

// = API FUNCTIONS ===========================================================

FMUSIC_MODULE * FMUSIC_LoadSong(char * buffer, int size,SAMPLELOADCALLBACK sampleloadcallback)
{
  FMUSIC_MODULE *mod = FSOUND_Memory_Calloc(sizeof(FMUSIC_MODULE));
#ifndef NO_ERRORCHECK
  int retcode = FALSE;

  if ( ! mod ) return NULL;
#endif
#ifndef NO_SAMPLECALLBACK
  mod->samplecallback = sampleloadcallback;
#endif
#ifndef NO_ERRORCHECK
  retcode = FMUSIC_LoadXM(mod, buffer, size);
  if (!retcode)
  {
#ifndef NO_FREE
    FMUSIC_FreeSong(mod);
#endif
    return NULL;
  }
#else
  FMUSIC_LoadXM(mod, buffer, size);
#endif
  return mod;
}

#ifndef NO_FREE
int FMUSIC_FreeSong(FMUSIC_MODULE *mod)
{
  int count;

#ifndef NO_ERRORCHECK
  if (!mod)
    return(FALSE);
#endif

  BLOCK_ON_SOFTWAREUPDATE();
#ifndef NO_STOPSONG
  FMUSIC_StopSong(mod);
#endif

  // free samples
  if (mod->instrument)
  {
    for (count = 0; count < (int)mod->numinsts; count++)
    {
      int count2;
      FMUSIC_INSTRUMENT *iptr = &mod->instrument[count];
      for (count2 = 0; count2 < iptr->numsamples; count2++)
      {
        if (iptr->sample[count2])
        {
          FSOUND_SAMPLE *sptr = iptr->sample[count2];
          FSOUND_Memory_Free(sptr->buff);
          FSOUND_Memory_Free(sptr);
        }
      }
    }
  }

  // free instruments
  if (mod->instrument)
    FSOUND_Memory_Free(mod->instrument);

  // free patterns
  if (mod->pattern)
  {
    for (count=0; count<mod->numpatternsmem; count++)
      if (mod->pattern[count].data)
        FSOUND_Memory_Free(mod->pattern[count].data);
    if (mod->pattern)
      FSOUND_Memory_Free(mod->pattern);
  }

  // free song
  FSOUND_Memory_Free(mod);

  return(TRUE);
}
#endif

int FMUSIC_PlaySong(FMUSIC_MODULE *mod)
{
  int count;
#ifdef __WIN32__
  int totalblocks = FSOUND_BufferSize / FSOUND_BlockSize;
#endif

#ifndef NO_ERRORCHECK
  if (!mod) return FALSE;
  FMUSIC_StopSong(mod);
#endif

  mod->globalvolume = mod->defaultglobalvolume;
  mod->speed = (int)mod->defaultspeed;
//  mod->row = 0;
//  mod->order = 0;
  mod->nextorder = -1;
  mod->nextrow = -1;
//  mod->mixer_samplesleft = 0;
//  mod->tick = 0;
//  mod->patterndelay = 0;
//  mod->time_ms = 0;

#ifndef NO_PLAYCALLBACK
  if (mod->Play) mod->Play(mod);
#endif
  FMUSIC_SetBPM(mod, mod->defaultbpm);
  memset(FMUSIC_Channel, 0, mod->numchannels * sizeof(FMUSIC_CHANNEL));
  for (count = 0; count < mod->numchannels; count++)
   FMUSIC_Channel[count].cptr=&FSOUND_Channel[count];
  FMUSIC_PlayingSong = mod;

#ifdef __WIN32__
  FMUSIC_TimeInfo = FSOUND_Memory_Calloc(sizeof(FMUSIC_TIMMEINFO) * totalblocks);
#endif

#ifdef __WIN32__
  // =========================================================================
  // PREPARE THE OUTPUT
  // =========================================================================
  {
    WAVEHDR *wavehdr;
    int     length = 0;

    // create and start looping waveOut block
    wavehdr = &FSOUND_MixBlock.wavehdr;

    length = FSOUND_BufferSize;
    length <<= 2;   // 16bits

    FSOUND_MixBlock.data = FSOUND_Memory_Calloc(length);

    wavehdr->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
    wavehdr->lpData = (LPSTR)FSOUND_MixBlock.data;
    wavehdr->dwBufferLength = length;
    wavehdr->dwBytesRecorded = 0;
    wavehdr->dwUser = 0;
    wavehdr->dwLoops = -1;
    waveOutPrepareHeader(FSOUND_WaveOutHandle, wavehdr, sizeof(WAVEHDR));
  }
#endif

  // =========================================================================
  // ALLOCATE MIXBUFFER
  // =========================================================================
#if defined( __WIN32__ ) || defined( __LINUX__ )
  FSOUND_MixBufferMem = (signed char *)FSOUND_Memory_Calloc((FSOUND_BufferSize << 3) + 256);
  FSOUND_MixBuffer = (signed char *)(((unsigned int)FSOUND_MixBufferMem + 15) & 0xFFFFFFF0);
#endif

  // =========================================================================
  // PREFILL THE MIXER BUFFER
  // ==========================================================================
#ifdef __WIN32__
  FSOUND_Software_FillBlock = 0;
  do
  {
    FSOUND_Software_Fill();
  } while (FSOUND_Software_FillBlock);
#endif

  // =========================================================================
  // START THE OUTPUT
  // =========================================================================
#ifdef __WIN32__
  waveOutWrite(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));
#endif
  {
#ifdef __WIN32__
    DWORD   FSOUND_dwThreadId;
#endif

    // =======================================================================
    // CREATE THREADS / TIMERS (last)
    // =======================================================================
    FSOUND_Software_Exit = FALSE;
#if defined( __LINUX__ ) || defined( __MACOSX__ )
#ifndef NO_ERRORCHECK
	pthread_create((pthread_t *)&FSOUND_Software_hThread, NULL, FSOUND_Software_OutputThread, NULL);
#else
    if (pthread_create((pthread_t *)&FSOUND_Software_hThread, NULL, FSOUND_Software_OutputThread, NULL) != 0) FSOUND_Software_hThread = 0;
#endif
#endif
#ifdef __WIN32__
    FSOUND_Software_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FSOUND_Software_DoubleBufferThread, 0,0, &FSOUND_dwThreadId);
    SetThreadPriority(FSOUND_Software_hThread, THREAD_PRIORITY_TIME_CRITICAL);      // THREAD_PRIORITY_HIGHEST);
#endif
  }
  return(TRUE);
}

#ifndef NO_STOPSONG
int FMUSIC_StopSong(FMUSIC_MODULE *mod)
{
#ifndef NO_ERRORCHECK
  if (!mod) return FALSE;
#endif

  // Kill the thread
  FSOUND_Software_Exit = TRUE;
  // wait until callback has settled down and exited
  BLOCK_ON_SOFTWAREUPDATE();

  if (FSOUND_Software_hThread)
  {
    while (!FSOUND_Software_ThreadFinished)
    {
#if defined( __LINUX__ ) || defined( __MACOSX__ )
      usleep(50000);
#endif
#ifdef __WIN32__
      Sleep(50);
#endif
    }
    FSOUND_Software_hThread = 0;
  }

  // remove the output mixbuffer
#ifndef NO_FREE
#if defined( __WIN32__ ) || defined( __LINUX__ )
  if (FSOUND_MixBufferMem)
    FSOUND_Memory_Free(FSOUND_MixBufferMem);
#endif
#endif

#ifdef __WIN32__
  if (FSOUND_MixBlock.wavehdr.lpData)
  {
    waveOutUnprepareHeader(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));
    FSOUND_MixBlock.wavehdr.dwFlags &= ~WHDR_PREPARED;
  }
#endif

//  FMUSIC_PlayingSong = NULL;

#ifdef __WIN32__
 #ifndef NO_FREE
  if (FMUSIC_TimeInfo)
    FSOUND_Memory_Free(FMUSIC_TimeInfo);
 #endif
#endif

  return(TRUE);
}
#endif

// = INFORMATION FUNCTIONS ===================================================

#ifndef NO_GETORDER
int FMUSIC_GetOrder(FMUSIC_MODULE *mod)
{
#ifndef NO_ERRORCHECK
  if (!mod) return 0;
#endif
#ifdef __MACOSX__
  return(FMUSIC_PlayingSong->order);
#endif
#ifdef __LINUX__
  return(FMUSIC_TimeInfo.order);
#endif
#ifdef __WIN32__
  return(FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order);
#endif
}
#endif

#ifndef NO_GETROW
int FMUSIC_GetRow(FMUSIC_MODULE *mod)
{
#ifndef NO_ERRORCHECK
  if (!mod) return 0;
#endif
#ifdef __MACOSX__
  return(FMUSIC_PlayingSong->row);
#endif
#ifdef __LINUX__
  return(FMUSIC_TimeInfo.row);
#endif
#ifdef __WIN32__
  return(FMUSIC_TimeInfo[FSOUND_Software_RealBlock].row);
#endif
}
#endif

#ifndef NO_GETTIME
unsigned int FMUSIC_GetTime(FMUSIC_MODULE *mod)
{
#ifndef NO_ERRORCHECK
  if (!mod) return(0);
#endif
#ifdef __MACOSX__
  return(FMUSIC_PlayingSong->time_ms);
#endif
#ifdef __LINUX__
  return(FMUSIC_TimeInfo.ms);
#endif
#ifdef __WIN32__
  return(FMUSIC_TimeInfo[FSOUND_Software_RealBlock].ms);
#endif
}
#endif
