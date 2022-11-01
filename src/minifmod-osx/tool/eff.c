
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "xm.h" /* XM file format common defs */

/* Error codes: */
enum EFF_ERROR_CODES{
	EFF_OK,         /* OK! */
	EFF_ERR_OUTPUT, /* Can't create output file */
	EFF_ERR_INPUT,  /* Can't open / read input file */
	EFF_ERR_FORMAT  /* Not a valid XM file */
};

FMUSIC_MODULE mod;
char special_effects[16],out[1024],file_name[64];
unsigned char effects[1024];
int hXM;

/* Effect names */
const char effect_names[][64] = {
	"FMUSIC_XM_ARPEGGIO_ACTIVE",
	"FMUSIC_XM_PORTAUP_ACTIVE",
	"FMUSIC_XM_PORTADOWN_ACTIVE",
	"FMUSIC_XM_PORTATO_ACTIVE",
	"FMUSIC_XM_VIBRATO_ACTIVE",
	"FMUSIC_XM_PORTATOVOLSLIDE_ACTIVE",
	"FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE",
	"FMUSIC_XM_TREMOLO_ACTIVE",
	"FMUSIC_XM_SETPANPOSITION_ACTIVE",
	"FMUSIC_XM_SETSAMPLEOFFSET_ACTIVE",
	"FMUSIC_XM_VOLUMESLIDE_ACTIVE",
	"FMUSIC_XM_PATTERNJUMP_ACTIVE",
	"FMUSIC_XM_SETVOLUME_ACTIVE",
	"FMUSIC_XM_PATTERNBREAK_ACTIVE",
	"",
	"FMUSIC_XM_SETSPEED_ACTIVE",
	"FMUSIC_XM_SETGLOBALVOLUME_ACTIVE",
	"FMUSIC_XM_GLOBALVOLSLIDE_ACTIVE",
	"",
	"",
	"FMUSIC_XM_KEYOFF_ACTIVE",
	"FMUSIC_XM_SETENVELOPEPOS_ACTIVE",
	"",
	"",
	"",
	"FMUSIC_XM_PANSLIDE_ACTIVE",
	"",
	"FMUSIC_XM_MULTIRETRIG_ACTIVE",
	"",
	"FMUSIC_XM_TREMOR_ACTIVE",
	"",
	"",
	"",
	"FMUSIC_XM_EXTRAFINEPORTA_ACTIVE"
};

/* Special effect names */
const char special_effect_names[][64] = {
	"",
	"FMUSIC_XM_FINEPORTAUP_ACTIVE",
	"FMUSIC_XM_FINEPORTADOWN_ACTIVE",
	"",
	"FMUSIC_XM_SETVIBRATOWAVE_ACTIVE",
	"FMUSIC_XM_SETFINETUNE_ACTIVE",
	"FMUSIC_XM_PATTERNLOOP_ACTIVE",
	"FMUSIC_XM_SETTREMOLOWAVE_ACTIVE",
	"FMUSIC_XM_SETPANPOSITION16_ACTIVE",
	"FMUSIC_XM_RETRIG_ACTIVE",
	"FMUSIC_XM_FINEVOLUMESLIDEUP_ACTIVE",
	"FMUSIC_XM_FINEVOLUMESLIDEDOWN_ACTIVE",
	"FMUSIC_XM_NOTECUT_ACTIVE",
	"FMUSIC_XM_NOTEDELAY_ACTIVE",
	"FMUSIC_XM_PATTERNDELAY_ACTIVE"
};

char define[256];
const char szEff_inc[] = "xmeffects.h";
char equ3[] = "FMUSIC_XM_INSTRUMENTVIBRATO_ACTIVE\n";
char equ4[] = "FMUSIC_XM_VOLUMEENVELOPE_ACTIVE\n";
char equ5[] = "FMUSIC_XM_PANENVELOPE_ACTIVE\n";
char equ6[] = "FMUSIC_XM_VOLUMEBYTE_ACTIVE\n";
char equ7[] = "FMUSIC_XM_AMIGAPERIODS_ACTIVE\n";
//char equ8[] = "FMUSIC_XM_VIBRATOVOLSLIDE_ACTIVE\n";
//char equA[] = "FMUSIC_XM_PORTATOVOLSLIDE_ACTIVE\n";
char equB[] = "FMUSIC_XM_VOLUMEENVELOPE_ACTIVE\n";

char h_undef[] = "#undef  ";
char h_define[] = "#define ";

/* Additional flags */
char dump_cpp            = 0;
char disable_info_api    = 0;
char disable_noloop      = 0;
char disable_pauseresume = 0;
char disable_volcontrol  = 0;
char disable_xm_file     = 0;

int ustrlen(char *str){
int len = 0;
	while(*str++) len++;
	return len;
}

void ustrcpy(char *dest,char *source){
	while(*source) *dest++ = *source++;
	*dest = 0;
}

void ustrcat(char *dest,char *source){
	while(*dest) dest++;
	ustrcpy(dest,source);
}

void umemcpy(void *dest,void *source,int size){
char *cdest = dest,*csource = source;
	while(size--) *cdest++ = *csource++;
}

int i2str(char *str,int i){
char tmp[12];
char *ptmp = tmp;
	while(i){
		*ptmp++ = '0' + i % 10;
		i /= 10;
	}
	i = ptmp-tmp;
	while(ptmp > tmp) *str++ = *(--ptmp);
	return i;
}

unsigned int f_read(void *buf,unsigned int size){ return read(hXM,buf,size); }
unsigned int f_seek(unsigned int size,int mode){ return lseek(hXM,size,mode); }
void z_memset(char *dest,int size){ while(size--) *dest++ = 0; }

int extract_effects(char *xm_file_name){
FMUSIC_INSTRUMENT inst;
FMUSIC_NOTE note;
unsigned int firstsampleoffset,skip_sample_bytes;
char *cursor;
unsigned char pat[12],skip,vibdepth,vibrate,vibtype,bad_file_format = 0;
unsigned char volume_envelope,pan_envelope,volume_byte,amiga_periods,enable_adpcm;
unsigned short rows;
unsigned int tmp[12],count,sample_header_size;
char header_tmp[512];
int count2;
int hH,hC;
	hXM = open(xm_file_name,0);
	if(hXM <= 0) return EFF_ERR_INPUT;
	/* Extract file name */
	cursor = xm_file_name+ustrlen(xm_file_name);
	while(cursor > xm_file_name){
		if(*(cursor-1) == '\\' || *(cursor-1) == '/') break;
		cursor--;
	}
	z_memset(file_name,sizeof(file_name));
	for(count = 0; count < sizeof(file_name)-1; count++){
		file_name[count] = *cursor++;
		if(!(*cursor)) break;
	}
	file_name[count+1] = 0;
	amiga_periods = 0;
	enable_adpcm = 0;
	/* VERIFY */
	f_read(&mod,sizeof(FMUSIC_MODULE));
	if(!((char)(mod.numchannels)) ||
		((unsigned char)(mod.numchannels)) > 64 ||
		!mod.numpatternsmem ||
		!mod.numorders ||
		mod.numorders > 256){
		bad_file_format = 1;
		goto next;
	}
	if(!(mod.flags&1)) amiga_periods = 1;
	/* Seek to patterndata */
	f_seek(60+mod.mixer_samplespertick,0);
	z_memset((char*)effects,40);
	z_memset(special_effects,sizeof(special_effects));
	volume_byte = 0;
	/* Unpack and read patterns */
	for(count=0; count < mod.numpatternsmem; count++){
		if(f_read(pat,9) != 9){
			bad_file_format = 1;
			goto next;
		}
		rows = *(unsigned short*)(pat+5);
		if(*(unsigned short*)(pat+7)){
			for(count2=0; count2 < rows*mod.numchannels; count2++){
				f_read(&skip,1);
				note.volume = 0;
				note.effect = 0;
				note.eparam = 0;
				if(skip&0x80){
					if(skip&1) f_read(pat,1);
					if(skip&2) f_read(pat,1);
					if(skip&4) f_read(&note.volume,1);
					if(skip&8) f_read(&note.effect,1);
					if(skip&16) f_read(&note.eparam,1);
				}else
					f_read(&note.number,4);
				if(note.volume) volume_byte = 1;
				/* Get normal effects */
				if((note.effect == FMUSIC_XM_ARPEGGIO && !note.eparam) ||
				  (note.effect == FMUSIC_XM_EXTRAFINEPORTA && (((note.eparam>>4) != 1) && ((note.eparam>>4) != 2))) ||
				  (!note.eparam &&
				   (note.effect == FMUSIC_XM_PORTAUP ||
				    note.effect == FMUSIC_XM_PORTADOWN ||
				    note.effect == FMUSIC_XM_MULTIRETRIG ||
				    note.effect == FMUSIC_XM_GLOBALVOLSLIDE))) continue;
				effects[note.effect] = 1;
				/* Get special effects */
				if(note.effect == FMUSIC_XM_SPECIAL){
					note.effect = note.eparam>>4;
					switch(note.effect){
						case FMUSIC_XM_RETRIG:
						case FMUSIC_XM_FINEPORTAUP:
						case FMUSIC_XM_FINEPORTADOWN:
						case FMUSIC_XM_FINEVOLUMESLIDEUP:
						case FMUSIC_XM_FINEVOLUMESLIDEDOWN:
							if(!(note.eparam&0xF)) break;
						default:
							special_effects[note.effect] = 1;
					}
				}
			}
		}
	}
	vibdepth = 0;
	vibrate = 0;
	vibtype = 0;
	volume_envelope = 0;
	pan_envelope = 0;
	/* Load instrument information */
	for(count=0;count<(unsigned int)mod.numinsts;count++){
		z_memset((char*)tmp,29);
		firstsampleoffset = f_read(tmp,29); /* instrument size & name & sample header size */
		/* Number of samples in this instrument */
		inst.numsamples = *((unsigned short*)((unsigned char*)tmp+27));
      		if(tmp[0] > 29 && inst.numsamples > 16){
			bad_file_format = 1;
			goto next;
		}
		skip_sample_bytes = tmp[0]-firstsampleoffset;
		if(tmp[0] >= 33 && inst.numsamples){
			*(unsigned int*)(inst.keymap-4) = 0;
			firstsampleoffset += f_read(inst.keymap-4,212);
			sample_header_size = *(unsigned int*)(inst.keymap-4);
			if(!sample_header_size || sample_header_size > 80){
				bad_file_format = 1;
				goto next;
			}
			firstsampleoffset = f_seek(tmp[0]-firstsampleoffset,1);
			if(inst.VIBdepth) vibdepth = 1;
			if(inst.VIBrate) vibrate = 1;
			if(inst.VIBtype == 1) vibtype = 1;
			if(inst.VOLtype & 1 && inst.VOLnumpoints >= 2) volume_envelope = 1;
			if(inst.PANtype & 1 && inst.PANnumpoints >= 2) pan_envelope = 1;
			skip_sample_bytes = 0;
			for(count2=0; count2<(int)inst.numsamples; count2++){
				FSOUND_SAMPLE sample[2];
				f_read(sample,18);
				if(sample[0].length & 0xFFC00000){
					bad_file_format = 1;
					goto next;
				}
				f_seek(sample_header_size-18,1);
				/* Type of sample */
				if(sample[0].Reserved == 0xAD){
					/* ADPCM */
					enable_adpcm = 1;
					skip_sample_bytes += 16+((sample[0].length+1)>>1);
				}else
					skip_sample_bytes += sample[0].length;
			}
			if(skip_sample_bytes & 0xFFC00000){
				bad_file_format = 1;
				goto next;
			}
		}
		f_seek(skip_sample_bytes,1);
	}
	if(!vibrate && !vibtype) vibdepth = 0;
	/* Generate header file */
	remove(szEff_inc);
	hH = creat(szEff_inc,0666);
	if(hH <= 0){
		close(hXM);
		return EFF_ERR_OUTPUT;
	}
	/* Store header */
	ustrcpy(header_tmp,"\n#ifndef _XMEFFECTS_H_\n#define _XMEFFECTS_H_\n\n// XM-file: ");
	ustrcat(header_tmp,file_name);
	ustrcat(header_tmp,"\n\n");
	write(hH,header_tmp,ustrlen(header_tmp));

	/* Erase file extension, if any */
	cursor = &file_name[sizeof(file_name)-1];
	while(cursor > file_name){
		if(*cursor == '.'){
			*cursor = 0;
			break;
		}
		cursor--;
	}
	/* Store special flags */
#define WR( var ) \
	write(hH,( var ? h_define : h_undef ),( var ? ustrlen( h_define ) : ustrlen( h_undef ) ) )

	WR( vibdepth );
	write(hH,equ3,sizeof(equ3)-1);

	WR( volume_envelope );
	write(hH,equ4,sizeof(equ4)-1);

	WR( pan_envelope );
	write(hH,equ5,sizeof(equ5)-1);

	WR( volume_byte );
	write(hH,equ6,sizeof(equ6)-1);

	WR( amiga_periods );
	write(hH,equ7,sizeof(equ7)-1);

	/* Store normal effects */
	for(count = 0; count <= FMUSIC_XM_EXTRAFINEPORTA; count++){
		if(count == FMUSIC_XM_SPECIAL || count == FMUSIC_XM_I ||
		   count == FMUSIC_XM_J || count == FMUSIC_XM_M ||
		   count == FMUSIC_XM_N || count == FMUSIC_XM_O ||
		   count == FMUSIC_XM_Q || count == FMUSIC_XM_S ||
		   count == FMUSIC_XM_U || count == FMUSIC_XM_V ||
		   count == FMUSIC_XM_W || count == FMUSIC_XM_Y ||
		   count == FMUSIC_XM_Z) continue;

		if ( effects[count] ) ustrcpy(define,h_define);
		 else ustrcpy(define,h_undef);
		ustrcat(define,(char*)effect_names[count]);
		ustrcat(define,"\n");
		write(hH,define,ustrlen(define));

	}
	/* Store special effects */
	for(count = 0; count <= FMUSIC_XM_PATTERNDELAY; count++){
//		if(count == FMUSIC_XM_SETFILTER ||
//		   count == FMUSIC_XM_SETGLISSANDO ||
//		   count == FMUSIC_XM_FINEVOLUMESLIDEUP ||
//		   count == FMUSIC_XM_FINEVOLUMESLIDEDOWN) continue;
		if(count == FMUSIC_XM_SETFILTER ||
		   count == FMUSIC_XM_SETGLISSANDO) continue;

		if ( special_effects[count] ) ustrcpy(define,h_define);
		 else ustrcpy(define,h_undef);
		ustrcat(define,(char*)special_effect_names[count]);
		ustrcat(define,"\n");
		write(hH,define,ustrlen(define));
	}
	/* Store compound flags */

//	WR( ( effects[FMUSIC_XM_VIBRATO] || effects[FMUSIC_XM_VIBRATOVOLSLIDE] ) );
//	write(hH,equ8,sizeof(equ8)-1);

//	WR( ( effects[FMUSIC_XM_PORTATO] || effects[FMUSIC_XM_PORTATOVOLSLIDE] ) );
//	write(hH,equA,sizeof(equA)-1);

//	WR( ( volume_envelope || pan_envelope ) );
//	write(hH,equB,sizeof(equB)-1);

	ustrcpy(header_tmp,"\n#endif\n");
	write(hH,header_tmp,ustrlen(header_tmp));
	close(hH);
next:
	if(dump_cpp){
		count = ustrlen(file_name);
		if(count > 1016){
			/* Very looooong file path :-( */
			close(hXM);
			return EFF_ERR_OUTPUT;
		}
		ustrcpy((char*)effects,file_name);
		ustrcpy((char*)effects+count,".c");
		remove(effects);
		hC = creat(effects,0666);
		if(hC == -1){
			close(hXM);
			return EFF_ERR_OUTPUT;
		}
		/* Generate C/C++ dump */
		ustrcpy(out,"const char xm[");
		count2 = i2str(&out[14],f_seek(0,2));
		ustrcpy(&out[14+count2],"] = {\n");
		write(hC,out,count2+20);
		f_seek(0,0);
		umemcpy(pat,"     \0000x00,",sizeof(pat));
		while((count2 = read(hXM,effects,sizeof(effects))) > 0){
			for(count = 0; count < (unsigned int)count2; count++){
				/* BIN2HEX */
				skip = effects[count]>>4;
				if(skip < 10) skip += '0';
				else skip = 'A'+skip-10;
				pat[8] = skip;
				skip = effects[count]&0xF;
				if(skip < 10) skip += '0';
				else skip = 'A'+skip-10;
				pat[9] = skip;
				/* Header */
				if(count%16 == 0) ustrcpy(out,(char*)pat);
				ustrcat(out,(char*)&pat[6]);
				if(count%16 == 15){
					ustrcat(out,"\n");
					write(hC,out,86);
				}
			}
			if(count%16) write(hC,out,ustrlen(out));
		}
		lseek(hC,-1,1);
		write(hC,"\n   };\n",7);
		close(hC);
	}
	close(hXM);
	if(bad_file_format) return EFF_ERR_FORMAT;
	return EFF_OK;
}

const char msg1[] = " USAGE: eff [options] file\n"
			"         file - input file name\n"
			"         options:\n"
			"          -c - generate a C/C++ compatible\n"
			"               dump of input file\n";
//			"          -i - disable infoAPI:\n"
//			"                   uFMOD_GetStats, uFMOD_GetTitle\n"
//			"                   and uFMOD_GetTime\n"
//			"          -p - disable uFMOD_Pause, uFMOD_Resume\n"
//			"                   and XM_SUSPENDED\n"
//			"          -v - disable uFMOD_SetVolume\n"
//			"          -f - disable loading XM from file\n"
//			"          -l - disable XM_NOLOOP\n";
const char msg2_1[] = " Processing '";
const char msg2_2[] = "'...\n";
const char msg3[] = " Operation completed successfully!\n";
const char msg4[] = " Dump successfully created!\n";
const char err0[] = " -ERR: unable to create output file(s)\n";
const char err1[] = " -ERR: unable to read XM-file\n";
const char err2[] = " -ERR: unsupported file format\n";
const char err3[] = " -ERR: only one file at a time\n";
const char err4[] = " -ERR: input file name missing\n";

int main( int argc, char * argv[] ) {
int i;
char* arg;
char* filename = 0;
	/* Parse command line */
	for(i = 1;; i++){
		arg = argv[i];
		if(!arg){
			if(i == 1) goto prompt;
			break;
		}
		if(*arg == '-'){
			arg++;
			while(*arg){
				switch(*arg){
					case 'c':
						dump_cpp = 1;
						break;
					case 'i':
						disable_info_api = 1;
						break;
					case 'f':
						disable_xm_file = 1;
						break;
					case 'l':
						disable_noloop = 1;
						break;
					case 'p':
						disable_pauseresume = 1;
						break;
					case 'v':
						disable_volcontrol = 1;
						break;
					default:
						goto prompt;
				}
				arg++;
			}
		}else{
			if(!filename) filename = arg;
			else{
				/* Only one file! */
				write(1,err3,sizeof(err3)-1);
				goto prompt;
			}
		}
	}
	if(!filename){
		write(1,err4,sizeof(err4)-1);
		goto prompt;
	}else{
		write(1,msg2_1,sizeof(msg2_1)-1);
		write(1,filename,ustrlen(filename));
		write(1,msg2_2,sizeof(msg2_2)-1);
		/* Perform extraction */
		switch(extract_effects(filename)){
			case EFF_OK:
				write(1,msg3,sizeof(msg3)-1);
				break;
			case EFF_ERR_OUTPUT:
				write(1,err0,sizeof(err0)-1);
				break;
			case EFF_ERR_INPUT:
				write(1,err1,sizeof(err1)-1);
				break;
			default:
				/* Format error */
				if(dump_cpp)
					write(1,msg4,sizeof(msg4)-1);
				else
					write(1,err2,sizeof(err2)-1);
		}
	}
	_exit(0);
prompt:
	/* Usage: */
	write(1,msg1,sizeof(msg1)-1);
	_exit(1);
}
