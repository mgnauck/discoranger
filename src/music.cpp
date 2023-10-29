
 // music.cpp

#include "settings.h"
#include <stdio.h>
#include "minifmod/lib/minifmod.h"

typedef struct 
{
	int length;
	int pos;
	void *data;
} MEMFILE;

FMUSIC_MODULE	*mod;
bool			MusicPlaying = false;


unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = new MEMFILE;
	memfile->length = sizeof(d2);
	memfile->data = (void *)d2;
	memfile->pos = 0;

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	free(memfile);
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	return memfile->pos;
}

void InitMusic(char *Name)
{
	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);

	if(FSOUND_Init(44100, 0))
		mod = FMUSIC_LoadSong(Name, NULL);
	else
		MessageBox(NULL, "Error upon initialization!", TITLE, MB_ICONERROR );
}

void DoMusic()
{
	if(!MusicPlaying)
	{
		if(mod)
			FMUSIC_PlaySong(mod);
		else
			MessageBox(NULL, "Error while start playing!", TITLE, MB_ICONERROR );

		MusicPlaying = true;
	}
}

void StopMusic()
{
	if(MusicPlaying)
	{
		if(mod)
			FMUSIC_StopSong(mod);

		MusicPlaying = false;
	}
}

void QuitMusic()
{
	if(mod)
		FMUSIC_FreeSong(mod);

	FSOUND_Close();
}