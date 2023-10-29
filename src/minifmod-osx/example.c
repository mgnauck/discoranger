
#include <stdio.h>
#include <stdlib.h>

#include "lib/minifmod.h"

int main( void )
{
	if (FSOUND_Init( 44100,0 ) == 0) {
		printf(" ! unable to initialize minifmod.\n" );
		return 1;
	}

	FILE *f = fopen("racers - green flag.xm", "rb");
	char music[42069];
	fread(music, sizeof(music), 1, f);
	fclose(f);

	FMUSIC_MODULE *m = FMUSIC_LoadSong(music, sizeof(music), NULL);
	FMUSIC_PlaySong(m);

// ... next step: reset your timer after starting the play

	while (1) {
		printf("\r playing... pos: %d:%d  %.2f sec   ",
#ifndef NO_GETORDER
			FMUSIC_GetOrder(m),
#else
			0,
#endif
#ifndef NO_GETROW
			FMUSIC_GetRow(m),
#else
			0,
#endif
#ifndef NO_GETTIME
			FMUSIC_GetTime(m) / 1000.0f
#else
			0.0f
#endif
		);		
		fflush(stdout);
		
#ifndef NO_GETORDER
		if (FMUSIC_GetOrder(m) == 24)
			break;
#endif

#if defined( __MACOSX__ ) || defined( __LINUX__ )		
		usleep(100000);
#elif defined( __WIN32__ )
		Sleep(100);
#endif
	}
	printf("\n...done.\n");

#ifndef NO_STOPSONG
	FMUSIC_StopSong(m);
#endif
#ifndef NO_FREE
	FMUSIC_FreeSong(m);
#endif
	
	return 0;
}
