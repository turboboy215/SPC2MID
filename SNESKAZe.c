/*KAZe/Meldac*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <complex.h>
#include "SNESKAZe.h"

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

char spcString[12];

int drvVers;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

const char MagicBytes[6] = { 0xF8, 0x07, 0xE4, 0x05, 0x60, 0x88 };

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
	if (args < 2)
	{
		printf("Usage: SNESKAZe <spc>\n");
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

				/*Find the song table*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes, 6)) && foundTable != 1)
					{
						songTable = spcData[ReadLE16(&spcData[i + 12])] + (spcData[ReadLE16(&spcData[i + 23])] * 0x100);
						songBase = spcData[i + 6] + (spcData[i + 17] * 0x100);
						printf("Found song table at address 0x%04X!\n", songTable);
						printf("Song base: 0x%04X\n", songBase);
						foundTable = 1;
					}
				}

				if (foundTable == 1)
				{
					songNum = 1;
					song2mid(songNum, songTable);
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
	int repeats[16][2];
	int repeatNum;
	int repeat1 = 0;
	int repeat1Start = 0;
	int repeat2 = 0;
	int repeat2Start = 0;
	int repeat3 = 0;
	int repeat3Start = 0;
	int repeat4 = 0;
	int repeat4Start = 0;
	int repeat5 = 0;
	int repeat5Start = 0;
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
	int inMacro = 0;
	long macro1Pos = 0;
	long macro1Ret = 0;
	long macro2Pos = 0;
	long macro2Ret = 0;
	long macro3Pos = 0;
	long macro3Ret = 0;
	long macro4Pos = 0;
	long macro4Ret = 0;
	long macro5Pos = 0;
	long macro5Ret = 0;
	long jumpAmt = 0;
	long tempPos = 0;
	int holdNote = 0;
	int tie = 0;
	long startPos = 0;

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
		case VER_STD:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0xC0;
			EventMap[0xC1] = EVENT_REST;
			EventMap[0xC2] = EVENT_ECHO_DELAY;
			EventMap[0xC3] = EVENT_ECHO_FEEDBACK;
			EventMap[0xC4] = EVENT_ECHO_FIR;
			EventMap[0xC5] = EVENT_ECHO_FIR;
			EventMap[0xC6] = EVENT_ECHO_FIR;
			EventMap[0xC7] = EVENT_ECHO_FIR;
			EventMap[0xC8] = EVENT_ECHO_FIR;
			EventMap[0xC9] = EVENT_ECHO_FIR;
			EventMap[0xCA] = EVENT_ECHO_FIR;
			EventMap[0xCB] = EVENT_ECHO_FIR;
			EventMap[0xCC] = EVENT_ASDR_MODE;
			EventMap[0xCD] = EVENT_GAIN_ENV_MODE;
			EventMap[0xCE] = EVENT_TIE_STATE;
			EventMap[0xCF] = EVENT_NOISE_ON;
			EventMap[0xD0] = EVENT_NOISE_OFF;
			EventMap[0xD1] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD2] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD3] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD4] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD5] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD6] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD7] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD8] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD9] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDA] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDB] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDC] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDD] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDE] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDF] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xE0] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xE1] = EVENT_ECHO_VOLUME_PAN;
			EventMap[0xE2] = EVENT_ECHO_VOLUME;
			EventMap[0xE3] = EVENT_ECHO_ON;
			EventMap[0xE4] = EVENT_ECHO_OFF;
			EventMap[0xE5] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE6] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE7] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE8] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE9] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEA] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEB] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEC] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xED] = EVENT_MASTER_VOLUME_PAN;
			EventMap[0xEE] = EVENT_MASTER_VOLUME;
			EventMap[0xEF] = EVENT_PAN;
			EventMap[0xF0] = EVENT_VOLUME;
			EventMap[0xF1] = EVENT_NOTE_ON_DELAY;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_NOP;
			EventMap[0xF4] = EVENT_NOP;
			EventMap[0xF5] = EVENT_REPEAT_START;
			EventMap[0xF6] = EVENT_REPEAT_END;
			EventMap[0xF7] = EVENT_SLUR_ON;
			EventMap[0xF8] = EVENT_SLUR_OFF;
			EventMap[0xF9] = EVENT_TRANSPOSE;
			EventMap[0xFA] = EVENT_TEMPO;
			EventMap[0xFB] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xFC] = EVENT_CALL;
			EventMap[0xFD] = EVENT_JUMP;
			EventMap[0xFE] = EVENT_RETURN;
			EventMap[0xFF] = EVENT_END;
		default:
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0xC0;
			EventMap[0xC1] = EVENT_REST;
			EventMap[0xC2] = EVENT_ECHO_DELAY;
			EventMap[0xC3] = EVENT_ECHO_FEEDBACK;
			EventMap[0xC4] = EVENT_ECHO_FIR;
			EventMap[0xC5] = EVENT_ECHO_FIR;
			EventMap[0xC6] = EVENT_ECHO_FIR;
			EventMap[0xC7] = EVENT_ECHO_FIR;
			EventMap[0xC8] = EVENT_ECHO_FIR;
			EventMap[0xC9] = EVENT_ECHO_FIR;
			EventMap[0xCA] = EVENT_ECHO_FIR;
			EventMap[0xCB] = EVENT_ECHO_FIR;
			EventMap[0xCC] = EVENT_ASDR_MODE;
			EventMap[0xCD] = EVENT_GAIN_ENV_MODE;
			EventMap[0xCE] = EVENT_TIE_STATE;
			EventMap[0xCF] = EVENT_NOISE_ON;
			EventMap[0xD0] = EVENT_NOISE_OFF;
			EventMap[0xD1] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD2] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD3] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD4] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD5] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD6] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD7] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD8] = EVENT_PROGCHANGE_FROM_TABLE;
			EventMap[0xD9] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDA] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDB] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDC] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDD] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDE] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xDF] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xE0] = EVENT_PROGTABLE_WRITE_2;
			EventMap[0xE1] = EVENT_ECHO_VOLUME_PAN;
			EventMap[0xE2] = EVENT_ECHO_VOLUME;
			EventMap[0xE3] = EVENT_ECHO_ON;
			EventMap[0xE4] = EVENT_ECHO_OFF;
			EventMap[0xE5] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE6] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE7] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE8] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xE9] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEA] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEB] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xEC] = EVENT_PROGTABLE_WRITE_3;
			EventMap[0xED] = EVENT_MASTER_VOLUME_PAN;
			EventMap[0xEE] = EVENT_MASTER_VOLUME;
			EventMap[0xEF] = EVENT_PAN;
			EventMap[0xF0] = EVENT_VOLUME;
			EventMap[0xF1] = EVENT_NOTE_ON_DELAY;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_NOP;
			EventMap[0xF4] = EVENT_NOP;
			EventMap[0xF5] = EVENT_REPEAT_START;
			EventMap[0xF6] = EVENT_REPEAT_END;
			EventMap[0xF7] = EVENT_SLUR_ON;
			EventMap[0xF8] = EVENT_SLUR_OFF;
			EventMap[0xF9] = EVENT_TRANSPOSE;
			EventMap[0xFA] = EVENT_TEMPO;
			EventMap[0xFB] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xFC] = EVENT_CALL;
			EventMap[0xFD] = EVENT_JUMP;
			EventMap[0xFE] = EVENT_RETURN;
			EventMap[0xFF] = EVENT_END;
		}

		/*Get the pointers to each channel's sequence data*/
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			songPtrs[curTrack] = ReadLE16(&spcData[songTable + (curTrack * 2)]);
			if (songPtrs[curTrack] != 0x0000)
			{
				songPtrs[curTrack] += songBase;
			}
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
				repeat1 = -1;
				repeat2 = -1;
				transpose = 0;
				curVol = 100;
				holdNote = 0;
				curInst = 0;
				tie = 0;

				for (j = 0; j < 16; j++)
				{
					repeats[j][0] = -1;
					repeats[j][1] = 0;
				}
				repeatNum = 0;

				while (seqEnd == 0 && midPos < 48000 && ctrlDelay < 110000)
				{
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];

					if (command[0] >= STATUS_NOTE_MIN && command[0] <= STATUS_NOTE_MAX)
					{
						if (tie == 0)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							curNote = command[0] + 24 + transpose;

							/*
							if (curNote >= 0x80)
							{
								curNote = 0;
							}
							*/
							curNoteLen = ((command[1] & 0x1F) + 1) * 16;

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
							tempByte = command[0] + 24 + transpose;

							if (curNote == tempByte)
							{
								curNoteLen = ((command[1] & 0x1F) + 1) * 16;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								curDelay += curNoteLen;
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
								curNote = command[0] + 24 + transpose;

								/*
								if (curNote >= 0x80)
								{
									curNote = 0;
								}
								*/
								curNoteLen = ((command[1] & 0x1F) + 1) * 16;

								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								firstNote = 0;
								midPos = tempPos;
								curDelay = curNoteLen;
								holdNote = 1;
							}

						}
						tie = 0;
						seqPos += 2;

					}

					else if (EventMap[command[0]] == EVENT_REST)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						curNoteLen = ((command[1] & 0x1F) + 1) * 16;
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos += 2;

					}

					else if (EventMap[command[0]] == EVENT_ECHO_DELAY)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_FEEDBACK)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_FIR)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_ASDR_MODE)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_GAIN_ENV_MODE)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_TIE_STATE)
					{
						tie = 1;
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_NOISE_ON)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_NOISE_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_PROGCHANGE_FROM_TABLE)
					{
						seqPos += 4;
					}

					else if (EventMap[command[0]] == EVENT_PROGTABLE_WRITE_2)
					{
						curInst = command[1];
						firstNote = 1;
						seqPos += 3;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_VOLUME_PAN)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_VOLUME)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_ECHO_ON)
					{
						seqPos++;
					}
					
					else if (EventMap[command[0]] == EVENT_ECHO_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_PROGTABLE_WRITE_3)
					{
						curInst = command[1];
						firstNote = 1;
						seqPos += 4;
					}

					else if (EventMap[command[0]] == EVENT_MASTER_VOLUME_PAN)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_MASTER_VOLUME)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_PAN)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_VOLUME)
					{
						curVol = (command[1] << 1) * 0.5;

						if (curVol == 0)
						{
							curVol = 1;
						}
						if (curVol > 127)
						{
							curVol = 127;
						}
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_NOTE_ON_DELAY)
					{
						curNoteLen = ((command[1] & 0x1F) + 1) * 16;
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_NOP || EventMap[command[0]] == EVENT_UNKNOWN0)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_REPEAT_START)
					{
						repeatNum++;
						repeats[repeatNum][0] = command[1];
						repeats[repeatNum][1] = seqPos + 2;
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_REPEAT_END)
					{
						if (repeats[repeatNum][0] > 1)
						{
							repeats[repeatNum][0]--;
							seqPos = repeats[repeatNum][1];
						}
						else
						{
							repeats[repeatNum][0] = -1;
							repeatNum--;
							seqPos++;
						}
					}

					else if (EventMap[command[0]] == EVENT_SLUR_ON)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_SLUR_OFF)
					{
						seqPos++;
					}

					else if (EventMap[command[0]] == EVENT_TRANSPOSE)
					{
						transpose = (signed char)command[1];
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
						tempo = command[1] * 4;
						WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
						ctrlMidPos += 2;
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_MASTER_VOLUME_FADE)
					{
						seqPos += 2;
					}

					else if (EventMap[command[0]] == EVENT_CALL)
					{
						if (inMacro == 0)
						{
							macro1Pos = (ReadLE16(&spcData[seqPos + 1]) + seqPos) & 0xFFFF;
							macro1Ret = seqPos + 3;
							seqPos = macro1Pos;
							inMacro = 1;
						}
						else if (inMacro == 1)
						{
							macro2Pos = (ReadLE16(&spcData[seqPos + 1]) + seqPos) & 0xFFFF;
							macro2Ret = seqPos + 3;
							seqPos = macro2Pos;
							inMacro = 2;
						}
						else if (inMacro == 2)
						{
							macro3Pos = (ReadLE16(&spcData[seqPos + 1]) + seqPos) & 0xFFFF;
							macro3Ret = seqPos + 3;
							seqPos = macro3Pos;
							inMacro = 3;
						}
						else if (inMacro == 3)
						{
							macro4Pos = (ReadLE16(&spcData[seqPos + 1]) + seqPos) & 0xFFFF;
							macro4Ret = seqPos + 3;
							seqPos = macro4Pos;
							inMacro = 4;
						}
						else if (inMacro == 4)
						{
							macro5Pos = (ReadLE16(&spcData[seqPos + 1]) + seqPos) & 0xFFFF;
							macro5Ret = seqPos + 3;
							seqPos = macro5Pos;
							inMacro = 5;
						}
						else
						{
							seqEnd = 1;
						}
					}

					else if (EventMap[command[0]] == EVENT_JUMP)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						seqEnd = 1;
					}

					else if (EventMap[command[0]] == EVENT_RETURN)
					{
						if (inMacro == 1)
						{
							seqPos = macro1Ret;
							inMacro = 0;
						}
						else if (inMacro == 2)
						{
							seqPos = macro2Ret;
							inMacro = 1;
						}
						else if (inMacro == 3)
						{
							seqPos = macro3Ret;
							inMacro = 2;
						}
						else if (inMacro == 4)
						{
							seqPos = macro4Ret;
							inMacro = 3;
						}
						else if (inMacro == 5)
						{
							seqPos = macro5Ret;
							inMacro = 4;
						}
						else
						{
							seqEnd = 1;
						}
					}

					else if (EventMap[command[0]] == EVENT_END)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						seqEnd = 1;
					}

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