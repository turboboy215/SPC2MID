/*Arc System Works*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <complex.h>
#include "SNESArc.h"

FILE* spc, * mid;

long songTable;
long tablePtrLoc;
long songBase;
int i, j, k;
char outfile[1000000];
int songNum;
long songPtrs[8];
long songPtr;
int curVol;
int tempoVal;
int foundTable;
int versOverride;
int numSongs;
int songIndex;
int curInst;
int noTranspose;
int useRAMAddr;
int advJump;

char spcString[12];

int drvVers;

char* argParam;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

const char MagicBytes[4] = { 0x9C, 0x1C, 0x5D, 0xF5 };
const char MagicBytesShinGPX[7] = { 0x80, 0xA8, 0x66, 0x1C, 0x4D, 0x5D, 0x1F };
const char MagicBytesSunL[6] = { 0x2F, 0x14, 0x68, 0xFC, 0xD0, 0x0C };

char* tempPnt;
char OutFileBase[0x100];

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
unsigned short ReadBE16(unsigned char* Data);
void Write8B(unsigned char* buffer, unsigned int value);
void WriteBE32(unsigned char* buffer, unsigned long value);
void WriteBE24(unsigned char* buffer, unsigned long value);
void WriteBE16(unsigned char* buffer, unsigned int value);
unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
unsigned int WriteNoteEventOn(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
unsigned int WriteNoteEventOff(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value);
void song2mid(int songNum, long songPtr);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

/*Store big-endian pointer*/
unsigned short ReadBE16(unsigned char* Data)
{
	return (Data[0] << 8) | (Data[1] << 0);
}


void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst)
{
	int deltaValue;
	deltaValue = WriteDeltaTime(buffer, pos, delay);
	pos += deltaValue;

	if (firstNote == 1)
	{
		Write8B(&buffer[pos], 0xC0 | curChan);
		Write8B(&buffer[pos + 1], inst);
		Write8B(&buffer[pos + 2], 0);
		Write8B(&buffer[pos + 3], 0x90 | curChan);

		pos += 4;
	}

	Write8B(&buffer[pos], note);
	pos++;
	Write8B(&buffer[pos], curVol);
	pos++;

	deltaValue = WriteDeltaTime(buffer, pos, length);
	pos += deltaValue;

	Write8B(&buffer[pos], note);
	pos++;
	Write8B(&buffer[pos], 0);
	pos++;

	return pos;

}

unsigned int WriteNoteEventOn(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst)
{
	int deltaValue;
	deltaValue = WriteDeltaTime(buffer, pos, delay);
	pos += deltaValue;

	if (firstNote == 1)
	{
		Write8B(&buffer[pos], 0xC0 | curChan);

		Write8B(&buffer[pos + 1], inst);
		Write8B(&buffer[pos + 2], 0);

		Write8B(&buffer[pos + 3], 0x90 | curChan);

		pos += 4;
	}

	Write8B(&buffer[pos], note);
	pos++;
	Write8B(&buffer[pos], curVol);
	pos++;

	return pos;

}

unsigned int WriteNoteEventOff(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst)
{
	int deltaValue;

	deltaValue = WriteDeltaTime(buffer, pos, delay);
	pos += deltaValue;

	if (firstNote == 1)
	{
		Write8B(&buffer[pos], 0x90 | curChan);

		pos++;
	}

	Write8B(&buffer[pos], note);
	pos++;
	Write8B(&buffer[pos], 0);
	pos++;

	return pos;

}

int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value)
{
	unsigned char valSize;
	unsigned char* valData;
	unsigned int tempLen;
	unsigned int curPos;

	valSize = 0;
	tempLen = value;

	while (tempLen != 0)
	{
		tempLen >>= 7;
		valSize++;
	}

	valData = &buffer[pos];
	curPos = valSize;
	tempLen = value;

	while (tempLen != 0)
	{
		curPos--;
		valData[curPos] = 128 | (tempLen & 127);
		tempLen >>= 7;
	}

	valData[valSize - 1] &= 127;

	pos += valSize;

	if (value == 0)
	{
		valSize = 1;
	}
	return valSize;
}

int main(int args, char* argv[])
{
	versOverride = 0;
	foundTable = 0;
	curInst = 0;
	drvVers = VER_NORMAL;
	noTranspose = 0;
	useRAMAddr = 0;
	advJump = 0;
	if (args < 2)
	{
		printf("Usage: SNESArc <spc> (optional tags:) -v = version, -t = disable transpose, -s = use song address from RAM, -j = advanced jump condition\n");
		return -1;
	}
	else
	{
		if ((spc = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			if (args > 2)
			{
				i = 2;
				while (i < args)
				{
					argParam = argv[i];
					if (strcmp(argParam, "v") == 0 || strcmp(argParam, "V") == 0)
					{
						if (i < args - 1)
						{
							drvVers = strtol(argv[i + 1], NULL, 16);
							if (drvVers != VER_BC && drvVers != VER_SHINGPX && drvVers != VER_NORMAL && drvVers != VER_SUNL)
							{
								printf("ERROR: Invalid version number!\n");
								exit(2);
							}
							versOverride = 1;
							i += 2;
						}
						else
						{
							printf("Usage: SNESArc <spc> (optional tags:) -v = version, -t = disable transpose, -s = use song address from RAM, -j = advanced jump condition\n");
							return -1;
						}

					}
					else if (strcmp(argParam, "t") == 0 || strcmp(argParam, "T") == 0)
					{
						noTranspose = 1;
						i++;
					}
					else if (strcmp(argParam, "s") == 0 || strcmp(argParam, "S") == 0)
					{
						useRAMAddr = 1;
						i++;
					}
					else if (strcmp(argParam, "j") == 0 || strcmp(argParam, "J") == 0)
					{
						advJump = 1;
						i++;
					}
					else
					{
						printf("Usage: SNESArc <spc> (optional tags:) -v = version, -t = disable transpose, -s = use song address from RAM, -j = advanced jump condition\n");
						return -1;
					}
				}


			}
			else
			{
				drvVers = VER_NORMAL;
			}
			fgets(spcString, 12, spc);
			if (!memcmp(spcString, "SNES-SPC700", 1))
			{
				fseek(spc, 0x100, SEEK_SET);
				/*Copy filename from argument - based on code by ValleyBell*/
				strcpy(OutFileBase, argv[1]);
				tempPnt = strrchr(OutFileBase, '.');
				if (tempPnt == NULL)
				{
					tempPnt = OutFileBase + strlen(OutFileBase);
				}
				*tempPnt = 0;

				spcData = (unsigned char*)malloc(ramSize);
				fread(spcData, 1, ramSize, spc);

				if (versOverride != 1)
				{
					/*Try to identify driver version*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesShinGPX, 7)))
						{
							drvVers = VER_SHINGPX;
						}
					}

					/*Try to identify driver version*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesSunL, 6)) && drvVers != VER_SHINGPX)
						{
							drvVers = VER_SUNL;
						}
					}
				}

				/*Find the song table*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes, 4)) && foundTable != 1)
					{
						tablePtrLoc = i + 4;
						songTable = ReadLE16(&spcData[tablePtrLoc]);
						printf("Found song table at address 0x%04X!\n", tablePtrLoc);
						printf("Song table: 0x%04X\n", songTable);
						foundTable = 1;
					}
				}

				if (foundTable == 1)
				{
					songNum = 1;
					i = songTable;

					if (useRAMAddr != 1)
					{
						while (spcData[ReadLE16(&spcData[i])] <= 8 && ReadLE16(&spcData[i]) >= 0x0100)
						{
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
							i += 2;
							songNum++;
						}
					}
					else
					{
						if (drvVers != VER_BC && drvVers != VER_SHINGPX)
						{
							songPtr = ReadLE16(&spcData[0x0004]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
						}
						else
						{
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
							i += 2;
							songNum++;
						}
					}


					free(spcData);
					fclose(spc);
					printf("The operation was successfully completed!\n");
					exit(0);
				}
				else
				{
					free(spcData);
					fclose(spc);
					printf("ERROR: Magic bytes not found!\n");
					exit(2);
				}
			}
			else
			{
				printf("ERROR: Invalid SPC file!\n");
				exit(1);
			}
		}
	}
}

/*Convert the song data to MIDI*/
void song2mid(int songNum, long songPtr)
{
	long spcPos = 0;
	long seqPos = 0;
	int curTrack = 0;
	int trackCnt = 8;
	int ticks = 120;
	int tempo = 120;
	int tempoVal;
	int k = 0;
	int seqEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int transpose = 0;
	int repeats[4][2];
	int repeatNum;
	unsigned char command[4];
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	int firstNote = 1;
	unsigned int midPos = 0;
	unsigned int ctrlMidPos = 0;
	long midTrackBase = 0;
	long ctrlMidTrackBase = 0;
	int valSize = 0;
	long trackSize = 0;
	int rest = 0;
	int tempByte = 0;
	int curDelay = 0;
	int ctrlDelay = 0;
	long masterDelay = 0;
	int macros[4][3];
	int inMacro = 0;
	long jumpAmt = 0;
	long tempPos = 0;
	int holdNote = 0;
	int tie = 0;
	int tieLen = 0;
	int prevLen = 0;
	long startPos = 0;
	int hasJumped = 0;
	int curInsts[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int curVols[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int curTransposes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	midPos = 0;
	ctrlMidPos = 0;


	midLength = 0x10000;
	midData = (unsigned char*)malloc(midLength);

	ctrlMidData = (unsigned char*)malloc(midLength);

	for (j = 0; j < midLength; j++)
	{
		midData[j] = 0;
		ctrlMidData[j] = 0;
	}

	sprintf(outfile, "%s_%01X.mid", OutFileBase, songNum);
	if ((mid = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file %s_%01X.mid!\n", OutFileBase, songNum);
		exit(2);
	}
	else
	{

		/*Get the number of tracks*/
		spcPos = songPtr;
		trackCnt = spcData[spcPos];
		spcPos++;

		/*Write MIDI header with "MThd"*/
		WriteBE32(&ctrlMidData[ctrlMidPos], 0x4D546864);
		WriteBE32(&ctrlMidData[ctrlMidPos + 4], 0x00000006);
		ctrlMidPos += 8;

		WriteBE16(&ctrlMidData[ctrlMidPos], 0x0001);
		WriteBE16(&ctrlMidData[ctrlMidPos + 2], trackCnt + 1);
		WriteBE16(&ctrlMidData[ctrlMidPos + 4], ticks);
		ctrlMidPos += 6;
		/*Write initial MIDI information for "control" track*/
		WriteBE32(&ctrlMidData[ctrlMidPos], 0x4D54726B);
		ctrlMidPos += 8;
		ctrlMidTrackBase = ctrlMidPos;

		/*Set channel name (blank)*/
		WriteDeltaTime(ctrlMidData, ctrlMidPos, 0);
		ctrlMidPos++;
		WriteBE16(&ctrlMidData[ctrlMidPos], 0xFF03);
		Write8B(&ctrlMidData[ctrlMidPos + 2], 0);
		ctrlMidPos += 2;

		/*Set initial tempo*/
		WriteDeltaTime(ctrlMidData, ctrlMidPos, 0);
		ctrlMidPos++;
		WriteBE32(&ctrlMidData[ctrlMidPos], 0xFF5103);
		ctrlMidPos += 4;

		WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
		ctrlMidPos += 3;

		/*Set time signature*/
		WriteDeltaTime(ctrlMidData, ctrlMidPos, 0);
		ctrlMidPos++;
		WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5804);
		ctrlMidPos += 3;
		WriteBE32(&ctrlMidData[ctrlMidPos], 0x04021808);
		ctrlMidPos += 4;

		/*Set key signature*/
		WriteDeltaTime(ctrlMidData, ctrlMidPos, 0);
		ctrlMidPos++;
		WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5902);
		ctrlMidPos += 4;

		switch (drvVers)
		{
		case VER_BC:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0x56;
			STATUS_PERCUSSION_NOTE_MIN = 0x57;
			STATUS_PERCUSSION_NOTE_MAX = 0x64;
			STATUS_NOTE_NODUR_MIN = 0x80;
			STATUS_NOTE_NODUR_MAX = 0xE4;

			EventMap[0x65] = EVENT_REST;
			EventMap[0x66] = EVENT_JUMP;
			EventMap[0x67] = EVENT_SUBROUTINE;
			EventMap[0x68] = EVENT_SUBROUTINE_END;
			EventMap[0x69] = EVENT_ECHO_PARAM;
			EventMap[0x6A] = EVENT_ECHO_ON;
			EventMap[0x6B] = EVENT_ECHO_OFF;
			EventMap[0x6C] = EVENT_END;
			EventMap[0x6D] = EVENT_VOLUME;
			EventMap[0x6E] = EVENT_PROG_CHANGE;
			EventMap[0x6F] = EVENT_REPEAT;
			EventMap[0x70] = EVENT_TRANSPOSE;
			EventMap[0x71] = EVENT_PITCH_SLIDE;
			EventMap[0x72] = EVENT_PITCH_LFO;
			EventMap[0x73] = EVENT_VIBRATO_OFF;
			EventMap[0x74] = EVENT_END;
			EventMap[0x75] = EVENT_MASTER_VOLUME;
			EventMap[0xE5] = EVENT_REST_NODUR;
			break;
		case VER_SHINGPX:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0x56;
			STATUS_PERCUSSION_NOTE_MIN = 0x57;
			STATUS_PERCUSSION_NOTE_MAX = 0x64;
			STATUS_NOTE_NODUR_MIN = 0x80;
			STATUS_NOTE_NODUR_MAX = 0xE4;

			EventMap[0x65] = EVENT_REST;
			EventMap[0x66] = EVENT_JUMP;
			EventMap[0x67] = EVENT_SUBROUTINE;
			EventMap[0x68] = EVENT_SUBROUTINE_END;
			EventMap[0x69] = EVENT_ECHO_PARAM;
			EventMap[0x6A] = EVENT_ECHO_ON;
			EventMap[0x6B] = EVENT_ECHO_OFF;
			EventMap[0x6C] = EVENT_END;
			EventMap[0x6D] = EVENT_VOLUME;
			EventMap[0x6E] = EVENT_PROG_CHANGE;
			EventMap[0x6F] = EVENT_REPEAT;
			EventMap[0x70] = EVENT_TRANSPOSE;
			EventMap[0x71] = EVENT_PITCH_SLIDE;
			EventMap[0x72] = EVENT_PITCH_LFO;
			EventMap[0x73] = EVENT_VIBRATO_OFF;
			EventMap[0x74] = EVENT_END;
			EventMap[0x75] = EVENT_MASTER_VOLUME;
			EventMap[0x76] = EVENT_VOLENV;
			EventMap[0x77] = EVENT_UNKNOWN0;
			EventMap[0x78] = EVENT_UNKNOWN0;
			EventMap[0x79] = EVENT_UNKNOWN2;
			EventMap[0xE5] = EVENT_REST_NODUR;
			break;
		case VER_SUNL:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0xCF;
			STATUS_PERCUSSION_NOTE_MIN = 0xD0;
			STATUS_PERCUSSION_NOTE_MAX = 0xDE;
			EventMap[0xDF] = EVENT_REST;
			EventMap[0xE0] = EVENT_JUMP;
			EventMap[0xE1] = EVENT_SUBROUTINE;
			EventMap[0xE2] = EVENT_SUBROUTINE_END;
			EventMap[0xE3] = EVENT_ECHO_PARAM;
			EventMap[0xE4] = EVENT_ECHO_ON;
			EventMap[0xE5] = EVENT_ECHO_OFF;
			EventMap[0xE6] = EVENT_END;
			EventMap[0xE7] = EVENT_VOLUME;
			EventMap[0xE8] = EVENT_PROG_CHANGE;
			EventMap[0xE9] = EVENT_REPEAT;
			EventMap[0xEA] = EVENT_TRANSPOSE;
			EventMap[0xEB] = EVENT_PITCH_SLIDE;
			EventMap[0xEC] = EVENT_PITCH_LFO;
			EventMap[0xED] = EVENT_VIBRATO_OFF;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			EventMap[0xEF] = EVENT_MASTER_VOLUME;
			EventMap[0xF0] = EVENT_UNKNOWN0;
			EventMap[0xF1] = EVENT_NOP;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_TEMPO;
			EventMap[0xF4] = EVENT_UNKNOWN0;
			EventMap[0xF5] = EVENT_TIE;
			EventMap[0xF6] = EVENT_UNKNOWN0;
			EventMap[0xF7] = EVENT_UNKNOWN0;
			EventMap[0xF8] = EVENT_ADSR;
			EventMap[0xF9] = EVENT_TUNING;
			EventMap[0xFA] = EVENT_NOISE_ADSR;
			EventMap[0xFB] = EVENT_NOISE_OFF;
			break;
		case VER_NORMAL:
			/*Fall-through*/
		default:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0xCF;
			STATUS_PERCUSSION_NOTE_MIN = 0xD0;
			STATUS_PERCUSSION_NOTE_MAX = 0xDE;
			EventMap[0xDF] = EVENT_REST;
			EventMap[0xE0] = EVENT_JUMP;
			EventMap[0xE1] = EVENT_SUBROUTINE;
			EventMap[0xE2] = EVENT_SUBROUTINE_END;
			EventMap[0xE3] = EVENT_ECHO_PARAM;
			EventMap[0xE4] = EVENT_ECHO_ON;
			EventMap[0xE5] = EVENT_ECHO_OFF;
			EventMap[0xE6] = EVENT_END;
			EventMap[0xE7] = EVENT_VOLUME;
			EventMap[0xE8] = EVENT_PROG_CHANGE;
			EventMap[0xE9] = EVENT_REPEAT;
			EventMap[0xEA] = EVENT_TRANSPOSE;
			EventMap[0xEB] = EVENT_PITCH_SLIDE;
			EventMap[0xEC] = EVENT_PITCH_LFO;
			EventMap[0xED] = EVENT_VIBRATO_OFF;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			EventMap[0xEF] = EVENT_MASTER_VOLUME;
			EventMap[0xF0] = EVENT_VOLENV;
			EventMap[0xF1] = EVENT_NOP;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_TEMPO;
			EventMap[0xF4] = EVENT_UNKNOWN0;
			EventMap[0xF5] = EVENT_TIE;
			EventMap[0xF6] = EVENT_UNKNOWN0;
			EventMap[0xF7] = EVENT_UNKNOWN0;
			EventMap[0xF8] = EVENT_ADSR;
			EventMap[0xF9] = EVENT_NOISE_ADSR;
			EventMap[0xFA] = EVENT_NOISE_OFF;
			EventMap[0xFB] = EVENT_PROGADD;
			EventMap[0xFC] = EVENT_TUNING;

		}

		/*Get the pointers to each channel's sequence data*/
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			songPtrs[curTrack] = ReadLE16(&spcData[spcPos]);
			curTransposes[curTrack] = (signed char)spcData[spcPos + 2];
			curInsts[curTrack] = spcData[spcPos + 3];
			curVols[curTrack] = spcData[spcPos + 4] | spcData[spcPos + 5];
			spcPos += 6;
		}

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			firstNote = 1;
			holdNote = 0;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			if (songPtrs[curTrack] != 0x0000)
			{
				seqPos = songPtrs[curTrack];
				curDelay = 0;
				ctrlDelay = 0;
				masterDelay = 0;
				seqEnd = 0;

				curNote = 0;
				curNoteLen = 0;
				inMacro = 0;
				if (noTranspose != 1)
				{
					transpose = curTransposes[curTrack];
				}
				else
				{
					transpose = 0;
				}

				curVol = (curVols[curTrack] << 1);
				
				if (curVol < 1)
				{
					curVol = 1;
				}
				if (curVol > 127)
				{
					curVol = 127;
				}
				
				holdNote = 0;
				curInst = curInsts[curTrack];
				tie = 0;
				tieLen = 0;

				for (j = 0; j < 4; j++)
				{
					repeats[j][0] = -1;
					repeats[j][1] = 0;
				}

				repeatNum = 0;

				for (j = 0; j < 4; j++)
				{
					macros[j][0] = 0;
					macros[j][1] = 0;
					macros[j][2] = 0;
				}

				inMacro = 0;
				hasJumped = 0;

				while (seqEnd == 0 && ctrlDelay < 110000)
				{

					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];

					if (command[0] >= STATUS_NOTE_MIN && command[0] <= STATUS_NOTE_MAX)
					{
						if (drvVers == VER_BC)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = command[0] - STATUS_NOTE_MIN + 24 + transpose;

							if (curNote < 0)
							{
								curNote = 0;
							}

							seqPos++;
							if (command[1] == 0x76)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							else
							{
								curNoteLen = command[1] * 4;
								seqPos++;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
						else if (drvVers == VER_SHINGPX)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = command[0] - STATUS_NOTE_MIN + 24 + transpose;

							if (curNote < 0)
							{
								curNote = 0;
							}

							seqPos++;
							if (command[1] == 0x79)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							else
							{
								curNoteLen = command[1] * 4;
								seqPos++;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
						else if (drvVers == VER_SUNL)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = (command[0] - 0x74) - STATUS_NOTE_MIN + 24 + transpose;

							if (curNote < 0)
							{
								curNote = 0;
							}

							seqPos++;

							if (spcData[seqPos] <= 0x74)
							{
								curNoteLen = spcData[seqPos] * 4;
								seqPos++;
							}
							else if (spcData[seqPos] == 0xFC)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
						else
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = (command[0] - 0x74) - STATUS_NOTE_MIN + 24 + transpose;

							if (curNote < 0)
							{
								curNote = 0;
							}
							seqPos++;

							if (spcData[seqPos] <= 0x74)
							{
								curNoteLen = spcData[seqPos] * 4;
								seqPos++;
							}
							else if (spcData[seqPos] == 0xFD)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
					}

					else if (command[0] >= STATUS_PERCUSSION_NOTE_MIN && command[0] <= STATUS_PERCUSSION_NOTE_MAX)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						if (drvVers == VER_BC)
						{
							curNote = command[0] - STATUS_PERCUSSION_NOTE_MIN + 24;
							seqPos++;
							if (command[1] == 0x76)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							else
							{
								curNoteLen = command[1] * 4;
								seqPos++;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
						else if (drvVers == VER_SHINGPX)
						{
							curNote = command[0] - STATUS_PERCUSSION_NOTE_MIN + 24;
							seqPos++;
							if (command[1] == 0x79)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							else
							{
								curNoteLen = command[1] * 4;
								seqPos++;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
						else if (drvVers == VER_SUNL)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[0] == 0xDA)
							{
								if (holdNote == 1)
								{
									tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
									curDelay = 0;
									holdNote = 0;
									midPos = tempPos;
								}
								if (command[1] <= 0x74)
								{
									curNoteLen = command[1] * 4;
									curDelay += curNoteLen;
									ctrlDelay += curNoteLen;
									masterDelay += curNoteLen;
									seqPos += 2;
								}
								else if (command[1] == 0xFC)
								{
									curNoteLen = ReadLE16(&spcData[seqPos + 2]) * 4;
									curDelay += curNoteLen;
									ctrlDelay += curNoteLen;
									masterDelay += curNoteLen;
									seqPos += 4;
								}
								else
								{
									curDelay += curNoteLen;
									ctrlDelay += curNoteLen;
									masterDelay += curNoteLen;
									seqPos++;
								}
							}
							else
							{
								curNote = command[0] - STATUS_PERCUSSION_NOTE_MIN + 24;
								seqPos++;

								if (spcData[seqPos] <= 0x74)
								{
									curNoteLen = spcData[seqPos] * 4;
									seqPos++;
								}
								else if (spcData[seqPos] == 0xFC)
								{
									curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
									seqPos += 3;
								}
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								firstNote = 0;
								midPos = tempPos;
								curDelay = curNoteLen;
								holdNote = 1;
							}

						}
						else
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = command[0] - STATUS_PERCUSSION_NOTE_MIN + 24;
							seqPos++;

							if (spcData[seqPos] <= 0x74)
							{
								curNoteLen = spcData[seqPos] * 4;
								seqPos++;
							}
							else if (spcData[seqPos] == 0xFD)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 1]) * 4;
								seqPos += 3;
							}
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = curNoteLen;
							holdNote = 1;
						}
					}

					else if (command[0] >= STATUS_NOTE_NODUR_MIN && command[0] <= STATUS_NOTE_NODUR_MAX && (drvVers == VER_SHINGPX || drvVers == VER_BC))
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						curNote = command[0] - STATUS_NOTE_NODUR_MIN + transpose + 24;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						firstNote = 0;
						midPos = tempPos;
						curDelay = curNoteLen;
						holdNote = 1;
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_UNKNOWN0 || EventMap[command[0]] == EVENT_NOP)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_UNKNOWN1)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_UNKNOWN2)
					{
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_REST)
					{
						if (drvVers == VER_BC)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								ctrlDelay += curNoteLen;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[1] == 0x76)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 2]) * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 4;
							}
							else
							{
								curNoteLen = command[1] * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 2;
							}
						}
						else if (drvVers == VER_SHINGPX)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								ctrlDelay += curNoteLen;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[1] == 0x79)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 2]) * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 4;
							}
							else
							{
								curNoteLen = command[1] * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 2;
							}
						}
						else if (drvVers == VER_SUNL)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[1] <= 0x74)
							{
								curNoteLen = command[1] * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 2;
							}
							else if (command[1] == 0xFC)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 2]) * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 4;
							}
							else
							{
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos++;
							}
						}
						else
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[1] <= 0x74)
							{
								curNoteLen = command[1] * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 2;
							}
							else if (command[1] == 0xFD)
							{
								curNoteLen = ReadLE16(&spcData[seqPos + 2]) * 4;
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos += 4;
							}
							else
							{
								curDelay += curNoteLen;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								seqPos++;
							}
						}

					}

					else if (EventMap[command[0]] == EVENT_JUMP)
					{
						if (advJump != 1)
						{
							if (ReadLE16(&spcData[seqPos + 1]) > seqPos)
							{
								seqPos = ReadLE16(&spcData[seqPos + 1]);
							}
							else
							{
								if (holdNote == 1)
								{
									tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
									curDelay = curNoteLen;
									ctrlDelay += curNoteLen;
									masterDelay += curNoteLen;
									holdNote = 0;
									midPos = tempPos;
								}
								seqEnd = 1;
							}
						}
						else
						{
							if (ReadLE16(&spcData[seqPos + 1]) > seqPos)
							{
								seqPos = ReadLE16(&spcData[seqPos + 1]);
							}
							else
							{
								if (hasJumped < 1)
								{
									seqPos = ReadLE16(&spcData[seqPos + 1]);
									hasJumped++;
								}
								else
								{
									seqEnd = 1;
								}
							}
						}

					}

					else if (EventMap[command[0]] == EVENT_SUBROUTINE)
					{
						if (inMacro >= 4)
						{
							seqEnd = 1;
						}
						else
						{
							inMacro++;
							macros[inMacro][0] = ReadLE16(&spcData[seqPos + 1]);
							macros[inMacro][1] = seqPos + 3;
							seqPos = macros[inMacro][0];
						}
					}

					else if (EventMap[command[0]] == EVENT_SUBROUTINE_END)
					{
						if (inMacro > 0)
						{
							seqPos = macros[inMacro][1];
							inMacro--;
						}
						else if (drvVers == VER_SHINGPX)
						{
							seqEnd = 1;
						}
						else
						{
							seqPos++;
						}
					}

					else if (EventMap[command[0]] == EVENT_ECHO_PARAM)
					{
						seqPos += 6;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_ON)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_END)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = curNoteLen;
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							holdNote = 0;
							midPos = tempPos;
						}
						seqEnd = 1;
					}

					else if (EventMap[command[0]] == EVENT_VOLUME)
					{
						
						curVol = ((command[1] | command[2]) << 1);

						if (curVol < 1)
						{
							curVol = 1;
						}
						if (curVol > 127)
						{
							curVol = 127;
						}						
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_PROG_CHANGE)
					{
						curInst = curInsts[curTrack];
						firstNote = 1;
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_REPEAT)
					{
						repeatNum = command[1];

						if (repeatNum > 4)
						{
							seqEnd = 1;
						}

						if (repeats[repeatNum][0] == -1)
						{
							repeats[repeatNum][0] = command[2];
							repeats[repeatNum][1] = ReadLE16(&spcData[seqPos + 3]);
						}
						else if (repeats[repeatNum][0] > 1)
						{
							repeats[repeatNum][0]--;
							seqPos = repeats[repeatNum][1];
						}
						else
						{
							repeats[repeatNum][0] = -1;
							seqPos += 5;
						}
					}

					else if (EventMap[command[0]] == EVENT_TRANSPOSE)
					{
						if (noTranspose != 1)
						{
							if (drvVers == VER_SHINGPX)
							{
								transpose = (signed char)command[1];
							}
							else
							{
								transpose += (signed char)command[1];
							}

						}
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_PITCH_SLIDE)
					{
						tempPos = WriteDeltaTime(midData, midPos, curDelay);
						midPos += tempPos;
						Write8B(&midData[midPos], (0xE0 | curTrack));
						Write8B(&midData[midPos + 1], 0);
						Write8B(&midData[midPos + 2], 0x40);
						Write8B(&midData[midPos + 3], 0);
						firstNote = 1;
						curDelay = 0;
						midPos += 3;
						seqPos += 4;
					}

					else if (EventMap[command[0]] == EVENT_PITCH_LFO)
					{
						seqPos += 4;
					}

					else if (EventMap[command[0]] == EVENT_VIBRATO_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_MASTER_VOLUME)
					{
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_VOLENV)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_TEMPO)
					{
						ctrlMidPos++;
						valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
						ctrlDelay = 0;
						ctrlMidPos += valSize;
						WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
						ctrlMidPos += 3;
						tempo = command[2] * 0.5;

						if (tempo < 2)
						{
							tempo = 120;
						}
						WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
						ctrlMidPos += 2;
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_TIE)
					{
						if (command[1] <= 0x74)
						{
							curNoteLen = command[1] * 4;
							curDelay += curNoteLen;
							ctrlDelay += (curNoteLen );
							masterDelay += curNoteLen;
							seqPos += 2;
						}
						else
						{
							curDelay += curNoteLen;
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							seqPos++;
						}
					}

					else if (EventMap[command[0]] == EVENT_ADSR)
					{
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_NOISE_ADSR)
					{
						seqPos += 4;
					}

					else if (EventMap[command[0]] == EVENT_NOISE_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_PROGADD)
					{
						curInst += (signed char)command[1];
						firstNote = 1;
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_TUNING)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_REST_NODUR)
					{
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos++;
					}

					/*Unknown command*/
					else
					{
						seqPos++;
					}
				}
			}

			/*End of track*/
			WriteBE32(&midData[midPos], 0xFF2F00);
			midPos += 4;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);
		}
		/*End of control track*/
		ctrlMidPos++;
		WriteBE32(&ctrlMidData[ctrlMidPos], 0xFF2F00);
		ctrlMidPos += 4;

		/*Calculate MIDI channel size*/
		trackSize = ctrlMidPos - ctrlMidTrackBase;
		WriteBE16(&ctrlMidData[ctrlMidTrackBase - 2], trackSize);
		fwrite(ctrlMidData, ctrlMidPos, 1, mid);
		fwrite(midData, midPos, 1, mid);
		free(midData);
		free(ctrlMidData);
		fclose(mid);
	}
}