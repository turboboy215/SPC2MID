/*System Vision/Kaneko*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * mid;
long offset;
long songPtr;
int i, j, k;
char outfile[1000000];
int songNum;
long songPtrs[8];
int curVol;
int tempoVal;
int foundSong;

char spcString[12];

long switchPoint[400][4];
int switchNum = 0;

unsigned long chanPts[8];

int drvVers;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

char* tempPnt;
char OutFileBase[0x100];

const char MagicBytes[15] = { 0xDA, 0x3E, 0xDA, 0x40, 0xCD, 0x00, 0xE7, 0x3E, 0x3A, 0x3E, 0xC4, 0x36, 0xD0, 0x01, 0x6F };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
void Write8B(unsigned char* buffer, unsigned int value);
void WriteBE32(unsigned char* buffer, unsigned long value);
void WriteBE24(unsigned char* buffer, unsigned long value);
void WriteBE16(unsigned char* buffer, unsigned int value);
unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value);
void song2mid(int songNum, long songPtr);

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
	foundSong = 0;
	if (args < 2)
	{
		printf("Usage: SNESSV <spc>\n");
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

				/*Try to find the song*/

				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes, 15)) && foundSong != 1)
					{
						songPtr = (spcData[i - 4] * 0x100) + spcData[i - 1];
						printf("Found song at address 0x%04X!\n", songPtr);
						foundSong = 1;
						songNum = 1;
						song2mid(songNum, songPtr);
					}
				}

				if (foundSong == 1)
				{
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
					exit(0);
				}

			}
			else
			{
				fclose(spc);
				printf("ERROR: Invalid SPC file!\n");
				exit(1);
			}


		}
	}
}

/*Convert the song data to MIDI*/
void song2mid(int songNum, long songPtr)
{
	long romPos = 0;
	unsigned int midPos = 0;
	int trackCnt = 8;
	int curTrack = 0;
	long midTrackBase = 0;
	unsigned int curDelay = 0;
	int midChan = 0;
	int seqEnd = 0;
	int ticks = 120;
	int k = 0;
	long startPos = 0;

	unsigned int ctrlMidPos = 0;
	long ctrlMidTrackBase = 0;

	int valSize = 0;

	long trackSize = 0;

	unsigned int curNote = 0;
	int curNoteLen = 0;

	int tempByte = 0;
	long tempPos = 0;

	long tempo = 160;

	int curInst = 0;

	int repeat = 0;
	long repeatStart = 0;

	int playTimes = 1;

	unsigned long seqPos = 0;

	unsigned char command[8];

	signed int transpose = 0;
	signed int transpose2 = 0;

	int firstNote = 1;

	int timeVal = 0;

	int holdNote = 0;

	long ctrlDelay = 0;
	long masterDelay = 0;

	long songStart = 0;

	midPos = 0;
	ctrlMidPos = 0;

	int repeats[500][2];
	int repeatNum;
	long jumpAmt;
	


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
		/*Get the channel count and track pointers*/
		trackCnt = spcData[songPtr];
		songStart = songPtr + (trackCnt * 2) + 1;
		i = songPtr + 1;

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			songPtrs[curTrack] = ReadLE16(&spcData[i]) + songStart;
			i += 2;
		}

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

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			firstNote = 1;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			seqEnd = 0;
			curNote = 0;

			curNoteLen = 0;

			masterDelay = 0;

			switchNum = 0;
			curInst = 0;
			curVol = 100;
			repeat = -1;
			repeatNum = 0;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			seqPos = songPtrs[curTrack];

			while (seqEnd == 0 && midPos < 48000 && ctrlDelay < 110000)
			{
				command[0] = spcData[seqPos];
				command[1] = spcData[seqPos + 1];
				command[2] = spcData[seqPos + 2];
				command[3] = spcData[seqPos + 3];
				command[4] = spcData[seqPos + 4];
				command[5] = spcData[seqPos + 5];
				command[6] = spcData[seqPos + 6];
				command[7] = spcData[seqPos + 7];

				/*Play note*/
				if (command[0] <= 0x7F)
				{
					/*Rest*/
					if (command[0] == 0x00)
					{
						curNoteLen = command[1] * 2;
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
					}
					else
					{
						curNote = command[0] + 23;
						curNoteLen = command[1] * 2;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						tempPos = WriteNoteEvent(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						firstNote = 0;
						midPos = tempPos;
						curDelay = 0;
					}
					seqPos += 2;
				}

				/*Set variables?*/
				else if (command[0] == 0x80)
				{
					seqPos += 2;
				}

				/*Unknown command 81*/
				else if (command[0] == 0x81)
				{
					seqPos += 2;
				}

				/*Set volume*/
				else if (command[0] == 0x82)
				{
					curVol = command[1] * 0.5;

					if (curVol == 0)
					{
						curVol = 1;
					}
					seqPos += 2;
				}

				/*Unknown command 83*/
				else if (command[0] == 0x83)
				{
					seqPos += 8;
				}

				/*Set tempo*/
				else if (command[0] == 0x84)
				{
					ctrlMidPos++;
					valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
					ctrlDelay = 0;
					ctrlMidPos += valSize;
					WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
					ctrlMidPos += 3;
					if (command[1] > 1)
					{
						tempo = 65535 / command[1] / 8;
					}
					else
					{
						tempo = 150;
					}

					WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
					ctrlMidPos += 2;
					seqPos += 2;
				}

				/*Set instrument*/
				else if (command[0] == 0x85)
				{
					curInst = command[1];
					firstNote = 1;
					seqPos += 2;
				}

				/*Unknown command 86*/
				else if (command[0] == 0x86)
				{
					seqPos += 4;
				}

				/*Unknown command 87*/
				else if (command[0] == 0x87)
				{
					seqPos += 2;
				}

				/*Clear ASDR bit 7*/
				else if (command[0] == 0x88)
				{
					seqPos++;
				}

				/*Clear ASDR bits*/
				else if (command[0] == 0x89)
				{
					seqPos++;
				}

				/*Set vibrato parameters*/
				else if (command[0] == 0x8A)
				{
					seqPos += 4;
				}

				/*Set ASDR bit 6*/
				else if (command[0] == 0x8B)
				{
					seqPos++;
				}

				/*Set ASDR bits on*/
				else if (command[0] == 0x8C)
				{
					seqPos++;
				}

				/*Unknown command 8D*/
				else if (command[0] == 0x8D)
				{
					seqPos += 3;
				}

				/*Command 8E (No effect)*/
				else if (command[0] == 0x8E)
				{
					seqPos++;
				}

				/*Set ASDR bit 5*/
				else if (command[0] == 0x8F)
				{
					seqPos++;
				}

				/*Clear ASDR bits (v2)*/
				else if (command[0] == 0x90)
				{
					seqPos++;
				}

				/*Restart channel*/
				else if (command[0] == 0x91)
				{
					seqEnd = 1;
				}

				/*Repeat section*/
				else if (command[0] == 0x92)
				{
					jumpAmt = ReadLE16(&spcData[seqPos + 3]);
					if (repeats[repeatNum][0] == seqPos)
					{
						if (repeats[repeatNum][1] == -1)
						{
							repeats[repeatNum][1] = command[1];
						}
						else if (repeats[repeatNum][1] > 0)
						{
							repeats[repeatNum][1]--;
							seqPos = (seqPos + 5) - jumpAmt;
						}
						else
						{
							repeats[repeatNum][0] = -1;
							repeats[repeatNum][1] = -1;
							seqPos += 5;

							if (repeatNum > 0)
							{
								repeatNum--;
							}
						}
					}
					else
					{
						repeatNum++;
						repeats[repeatNum][0] = seqPos;
						repeats[repeatNum][1] = -1;
					}
				}

				/*Set ASDR bit 4*/
				else if (command[0] == 0x93)
				{
					seqPos++;
				}

				/*Set panning*/
				else if (command[0] == 0x94)
				{
					seqPos += 2;
				}

				/*Stop channel*/
				else if (command[0] == 0x95)
				{
					seqEnd = 1;
				}

				/*Start of song loop*/
				else if (command[0] == 0x96)
				{
					seqPos++;
				}

				/*Unknown command*/
				else
				{
					seqPos++;
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

		sprintf(outfile, "%s_%01X.mid", OutFileBase, songNum);
		fwrite(ctrlMidData, ctrlMidPos, 1, mid);
		fwrite(midData, midPos, 1, mid);
		free(midData);
		free(ctrlMidData);
		fclose(mid);
	}
}
