/*David Whittaker*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * mid;

long tablePtrLoc;
long tableOffset;
int songPtr;
int i, j, k;
char outfile[1000000];
int songNum;
int maskVal;
int curVol;
int tempoVal;
int foundTable;
int channels;
int chanOverride;
unsigned long firstPtr;
unsigned long songPtrs[8];

char spcString[12];

long switchPoint[400][4];
int switchNum = 0;

unsigned long seqList[500];
unsigned long chanPts[8];
int totalSeqs;

int drvVers;
int mjFix;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

char* tempPnt;
char OutFileBase[0x100];

const unsigned char MagicBytesA[8] = { 0xC4, 0x1B, 0xC4, 0x1C, 0xFC, 0xCD, 0x00, 0xF6 };
const unsigned char MagicBytesB[5] = { 0x05, 0xE8, 0x00, 0x5D, 0xD4};
const unsigned char MagicBytesB2[5] = { 0x07, 0xE8, 0x00, 0x5D, 0xD4 };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
void Write8B(unsigned char* buffer, unsigned int value);
void WriteBE32(unsigned char* buffer, unsigned long value);
void WriteBE24(unsigned char* buffer, unsigned long value);
void WriteBE16(unsigned char* buffer, unsigned int value);
unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
unsigned int WriteNoteEventOn(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
unsigned int WriteNoteEventOff(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value);
void song2mid(int songNum, unsigned long songPtrs[8]);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
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
	foundTable = 0;
	channels = 8;
	chanOverride = 0;
	mjFix = 0;
	if (args < 2)
	{
		printf("Usage: SNESDW <spc> <number of channels (optional)>\n");
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
			if (args >= 3)
			{
				channels = strtol(argv[2], NULL, 16);
				chanOverride = 1;
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
			}

			else
			{
				printf("ERROR: Invalid SPC file!\n");
				exit(1);
			}

			/*Try to search the bank for base table - early driver*/
			for (i = 0; i < ramSize; i++)
			{
				if ((!memcmp(&spcData[i], MagicBytesA, 8)) && foundTable == 0)
				{
					tablePtrLoc = i - 2;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&spcData[tablePtrLoc]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					if (chanOverride == 0)
					{
						channels = 8;
					}
					foundTable = 1;
					break;
				}
			}

			/*Try to search the bank for base table - common driver*/
			for (i = 0; i < ramSize; i++)
			{
				if ((!memcmp(&spcData[i], MagicBytesB, 5)) && foundTable == 0 && spcData[i + 6] == 0x3D)
				{
					tablePtrLoc = i - 4;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = spcData[tablePtrLoc] + (spcData[tablePtrLoc + 3] * 0x100);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					if (chanOverride == 0)
					{
						channels = 8;
					}
					foundTable = 1;
					break;
				}
			}

			/*Try to search the bank for base table - common driver (alt - Michael Jordan)*/
			for (i = 0; i < ramSize; i++)
			{
				if ((!memcmp(&spcData[i], MagicBytesB2, 5)) && foundTable == 0 && spcData[i + 6] == 0x3D)
				{
					tablePtrLoc = i - 4;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = spcData[tablePtrLoc] + (spcData[tablePtrLoc + 3] * 0x100);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					if (chanOverride == 0)
					{
						channels = 8;
					}
					foundTable = 1;
					mjFix = 1;
					break;
				}
			}

			if (foundTable == 1)
			{
				i = tableOffset;
				songNum = 1;
				firstPtr = ReadLE16(&spcData[i + 1]);

				while (i < (firstPtr - 2))
				{
					for (j = 0; j < 8; j++)
					{
						songPtrs[j] = 0x0000;
					}
					tempoVal = spcData[i];
					printf("Song %i tempo: %02X\n", songNum, tempoVal);

					for (j = 0; j < channels; j++)
					{
						songPtrs[j] = ReadLE16(&spcData[i + 1 + (2 * j)]);

						if (songPtrs[j] < firstPtr && songPtrs[j] != 0x0000)
						{
							firstPtr = songPtrs[j];
						}
						printf("Song %i channel %i: 0x%04X\n", songNum, (j + 1), songPtrs[j]);
					}
					song2mid(songNum, songPtrs);
					i += ((channels * 2) + 1);
					songNum++;
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
				exit(1);
			}
		}
	}
}

/*Convert the song data to MIDI*/
void song2mid(int songNum, unsigned long songPtrs[8])
{
	int patPos = 0;
	int seqPos = 0;
	unsigned int midPos = 0;
	unsigned int ctrlMidPos = 0;
	long midTrackBase = 0;
	long ctrlMidTrackBase = 0;
	int trackCnt = channels;
	int ticks = 120;
	long curSeq = 0;
	long command[3];
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	int curTrack = 0;
	int endSeq = 0;
	int endChan = 0;
	int curTempo = tempoVal;
	int transpose = 0;
	int globalTranspose = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int curDelay = 0;
	int ctrlDelay = 0;
	int masterDelay = 0;
	long jumpPos = 0;
	int firstNote = 1;
	int timeVal = 0;
	int holdNote = 0;
	int lastNote = 0;

	int tempByte = 0;
	long tempPos = 0;

	long tempo = tempoVal * 3;

	int curInst = 0;

	int valSize = 0;

	long trackSize = 0;

	midPos = 0;
	ctrlMidPos = 0;

	switchNum = 0;

	for (switchNum = 0; switchNum < 400; switchNum++)
	{
		switchPoint[switchNum][0] = -1;
		switchPoint[switchNum][1] = 0;
	}

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
	if ((mid = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file song%d.mid!\n", songNum);
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

		for (curTrack = 0; curTrack < channels; curTrack++)
		{
			transpose = 0;
			globalTranspose = 0;
			firstNote = 1;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			endChan = 0;

			curNote = 0;
			lastNote = 0;
			curNoteLen = 0;

			masterDelay = 0;

			switchNum = 0;
			curVol = 100;
			holdNote = 0;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			/*Get pointers to each sequence from pattern*/
			endChan = 0;
			patPos = songPtrs[curTrack];

			if (songPtrs[curTrack] == 0x0000)
			{
				endChan = 1;
			}

			while (endChan == 0)
			{
				curSeq = ReadLE16(&spcData[patPos]);
				if (curSeq != 0)
				{
					seqPos = curSeq;
					endSeq = 0;
				}
				else
				{
					endChan = 1;
				}

				while (endSeq == 0)
				{
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];

					if (curTrack != 0)
					{
						for (switchNum = 0; switchNum < 400; switchNum++)
						{
							if (switchPoint[switchNum][0] == masterDelay)
							{
								globalTranspose = switchPoint[switchNum][1];
							}
						}
					}

					/*Play note*/
					if (command[0] <= 0x5F)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						curNote = command[0] + 24 + transpose + globalTranspose;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						firstNote = 0;
						midPos = tempPos;
						curDelay = curNoteLen;
						holdNote = 1;
						seqPos++;
					}

					/*Set channel speed/note length*/
					else if (command[0] >= 0x60 && command[0] <= 0x7F)
					{
						curNoteLen = 30 + (30 * (command[0] - 0x60));
						seqPos++;
					}

					/*Instant pitch change to note OR global volume*/
					else if (command[0] == 0xED)
					{
						if (mjFix == 1)
						{
							seqPos += 2;
						}
						else
						{
							seqPos++;
						}
					}

					/*Scale volume*/
					else if (command[0] == 0xEE)
					{
						seqPos += 3;
					}

					/*Set ASDR*/
					else if (command[0] == 0xEF)
					{
						seqPos += 3;
					}

					/*Set tuning*/
					else if (command[0] == 0xF0)
					{
						seqPos += 2;
					}

					/*Set pitch bend*/
					else if (command[0] == 0xF1)
					{
						tempPos = WriteDeltaTime(midData, midPos, curDelay);
						midPos += tempPos;
						Write8B(&midData[midPos], (0xE0 | curTrack));
						Write8B(&midData[midPos + 1], 0);
						Write8B(&midData[midPos + 2], 0x40);
						Write8B(&midData[midPos + 3], 0);
						curDelay = 0;
						firstNote = 1;
						midPos += 3;
						seqPos += 2;
					}

					/*Pitch envelope ID*/
					else if (command[0] == 0xF2)
					{
						seqPos += 2;
					}

					/*L/R voice volume*/
					else if (command[0] == 0xF3)
					{
						if (command[2] > command[1])
						{
							curVol = command[2] * 5;
						}
						else
						{
							curVol = command[1] * 5;
						}

						if (curVol >= 128)
						{
							curVol = 127;
						}

						seqPos += 3;
					}

					/*Change tempo*/
					else if (command[0] == 0xF4)
					{
						ctrlMidPos++;
						valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
						ctrlDelay = 0;
						ctrlMidPos += valSize;
						WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
						ctrlMidPos += 3;
						tempo = command[1] * 3;
						WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
						ctrlMidPos += 2;
						seqPos += 2;
					}

					/*Set channel loop point/end sequence*/
					else if (command[0] == 0xF5)
					{
						endSeq = 1;

						if (ReadLE16(&spcData[seqPos + 1]) < patPos)
						{
							endChan = 1;
						}
					}

					/*Command F6 (invalid)*/
					else if (command[0] == 0xF6)
					{
						seqPos++;
					}

					/*Command F7 (invalid)*/
					else if (command[0] == 0xF7)
					{
						seqPos++;
					}

					/*Rest*/
					else if (command[0] == 0xF8)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos++;
					}

					/*Hold note*/
					else if (command[0] == 0xF9)
					{
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos++;
					}

					/*Set instrument*/
					else if (command[0] == 0xFA)
					{
						curInst = command[1];
						firstNote = 1;
						seqPos += 2;
					}

					/*Transpose all channels*/
					else if (command[0] == 0xFB)
					{
						globalTranspose = (signed char)command[1];
						if (curTrack == 0)
						{
							switchPoint[switchNum][0] = masterDelay;
							switchPoint[switchNum][1] = globalTranspose;
							switchNum++;
						}
						seqPos += 2;
					}

					/*Transpose current channel*/
					else if (command[0] == 0xFC)
					{
						transpose = (signed char)command[1];
						seqPos += 2;
					}

					/*Command FD (Invalid)*/
					else if (command[0] == 0xFD)
					{
						seqPos++;
					}

					/*End of channel (no loop)*/
					else if (command[0] == 0xFE)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						endSeq = 1;
						endChan = 1;
					}

					/*End of sequence*/
					else if (command[0] == 0xFF)
					{
						if (holdNote == 1)
						{
							tempPos = WriteNoteEventOff(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							curDelay = 0;
							holdNote = 0;
							midPos = tempPos;
						}
						endSeq = 1;
					}

					/*Unknown command*/
					else
					{
						seqPos++;
					}
				}
				patPos += 2;
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

		sprintf(outfile, "%s_%01X.mid", OutFileBase, songNum);
		fwrite(ctrlMidData, ctrlMidPos, 1, mid);
		fwrite(midData, midPos, 1, mid);
		fclose(mid);
	}
}
