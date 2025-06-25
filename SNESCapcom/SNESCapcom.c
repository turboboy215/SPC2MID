/*Capcom*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * mid;
long bank;
long offset;
long tablePtrLoc;
long tableOffset;
int i, j;
char outfile[1000000];
int songNum;
long seqPtrs[4];
long songPtr;
long firstPtr;
long bankAmt;
int foundTable = 0;
int curInst = 0;
int isSong = 1;
int curVol = 100;
char* argv2;

char spcString[12];

int drvVers;

char* tempPnt;
char OutFileBase[0x100];

long switchPoint[400][2];
int switchNum = 0;

int cvtSFX;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

const char MagicBytes1[3] = { 0x1C, 0x5D, 0xF5 };
const char MagicBytes2[3] = { 0x8D, 0x00, 0xDD };
const char MagicBytes3[6] = { 0xA3, 0x1C, 0x90, 0x02, 0xAB, 0xA3 };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
unsigned short ReadBE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value);
void song2mid(int songNum, long ptr);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

/*Convert little-endian pointer to big-endian*/
unsigned short ReadBE16(unsigned char* Data)
{
	return (Data[0] << 8) | (Data[1] << 0);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
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
		if (curChan != 3)
		{
			Write8B(&buffer[pos], 0xC0 | curChan);
		}
		else
		{
			Write8B(&buffer[pos], 0xC0 | curChan);

		}

		Write8B(&buffer[pos + 1], inst);
		Write8B(&buffer[pos + 2], 0);

		if (curChan != 3)
		{
			Write8B(&buffer[pos + 3], 0x90 | curChan);
		}
		else
		{
			Write8B(&buffer[pos + 3], 0x90 | curChan);

		}

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
	cvtSFX = 0;
	if (args < 2)
	{
		printf("Usage: SNESCapcom <spc> <flags (optional)>\n");
		printf("Flags: S = Convert SFX\n");
		return -1;
	}
	else
	{

		if (args >= 3)
		{
			argv2 = argv[2];
			if (strcmp(argv2, "s") == 0 || strcmp(argv2, "S") == 0)
			{
				cvtSFX = 1;
			}
		}
		if ((spc = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			/*Check for SPC file header*/
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
			}
			else
			{
				printf("ERROR: Invalid SPC file!\n");
				exit(1);
			}

			spcData = (unsigned char*)malloc(ramSize);
			fread(spcData, 1, ramSize, spc);

			/*Try to search the bank for song table loader (early driver)*/
			for (i = 0; i < ramSize; i++)
			{
				if ((!memcmp(&spcData[i], MagicBytes1, 3)) && foundTable != 1)
				{
					tablePtrLoc = i + 3;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&spcData[tablePtrLoc]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					foundTable = 1;
					drvVers = 1;
				}
			}

			/*Try to search the bank for song table loader (later driver)*/
			for (i = 0; i < ramSize; i++)
			{
				if ((!memcmp(&spcData[i], MagicBytes2, 3)) && foundTable != 1)
				{
					tablePtrLoc = i - 8;
					printf("Found pointer to song at address 0x%04x!\n", tablePtrLoc);
					songPtr = spcData[i - 5] + (spcData[i - 8] * 0x100);
					printf("Song starts at 0x%04x...\n", songPtr);
					foundTable = 1;
					drvVers = 2;
				}
			}

			if (foundTable == 1)
			{
				i = tableOffset;
				songNum = 1;
				isSong = 1;

				if (drvVers == 1)
				{
					songNum = 1;
					/*Check for "active song"*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes2, 3)))
						{
							songPtr = spcData[i - 5] + (spcData[i - 8] * 0x100);

							if (songPtr < 0x6000)
							{
								if (tableOffset != 0x2002)
								{
									printf("Song %i: 0x%04X\n", songNum, songPtr);
									song2mid(songNum, songPtr);
									songNum++;
								}
							}

							break;
						}
					}
					i = tableOffset + 1;
					if (ReadBE16(&spcData[tableOffset]) == 0x0000)
					{
						i++;
					}
					firstPtr = ReadBE16(&spcData[i]);

					if (i == 0x2004)
					{
						firstPtr = 0x2100;
					}

					while (i < firstPtr)
					{
						songPtr = ReadBE16(&spcData[i]);
						if (songPtr != 0x0000 && songPtr != 0xFFFF)
						{
							if (spcData[songPtr] == 0x00)
							{
								printf("Song %i: 0x%04X\n", songNum, songPtr);
								song2mid(songNum, songPtr);
							}
							else
							{
								if (cvtSFX == 1)
								{
									printf("Song %i: 0x%04X (SFX)\n", songNum, songPtr);
									song2mid(songNum, songPtr);
								}
								else
								{
									printf("Song %i: 0x%04X (SFX, skipped)\n", songNum, songPtr);
								}
							}
						}
						else
						{
							printf("Song %i: 0x%04X (empty, skipped)\n", songNum, songPtr);
						}

						i += 2;
						songNum++;
					}
				}

				else if (drvVers == 2)
				{
					songNum = 1;
					song2mid(songNum, songPtr);
					songNum++;

					/*Look for SFX/additional music table*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes3, 6)) && foundTable != 2)
						{
							tablePtrLoc = i - 4;
							tableOffset = spcData[i - 4] + (spcData[i - 1] * 0x100);
							printf("SFX/additional music table: 0x%04X\n", tableOffset);
							foundTable = 2;
						}
					}

					if (foundTable == 2)
					{
						i = tableOffset + 1;

						if (ReadBE16(&spcData[tableOffset]) == 0x0000)
						{
							i++;
						}
						firstPtr = ReadBE16(&spcData[i]);

						while (i < firstPtr)
						{
							songPtr = ReadBE16(&spcData[i]);
							if (songPtr != 0x0000 && songPtr != 0xFFFF)
							{
								if (spcData[songPtr] == 0x00)
								{
									printf("Song %i: 0x%04X\n", songNum, songPtr);
									song2mid(songNum, songPtr);
								}
								else
								{
									if (cvtSFX == 1)
									{
										printf("Song %i: 0x%04X (SFX)\n", songNum, songPtr);
										song2mid(songNum, songPtr);
									}
									else
									{
										printf("Song %i: 0x%04X (SFX, skipped)\n", songNum, songPtr);
									}
								}
							}
							else
							{
								printf("Song %i: 0x%04X (empty, skipped)\n", songNum, songPtr);
							}

							i += 2;
							songNum++;
						}
					}
				}

				free(spcData);
				fclose(spc);
				printf("The operation was successfully completed!\n");
				exit(0);
			}

			else
			{
				printf("ERROR: Magic bytes not found!\n");
				free(spcData);
				exit(2);
			}
		}
	}
}

void song2mid(int songNum, long ptr)
{
	unsigned char command[3];
	long seqPos = 0;
	unsigned int midPos = 0;
	long songPtrs[8];
	int trackCnt = 8;
	int curTrack = 0;
	long midTrackBase = 0;
	unsigned int ctrlMidPos = 0;
	long ctrlMidTrackBase = 0;
	unsigned int curDelay = 0;
	unsigned int ctrlDelay = 0;
	int ticks = 120;
	int k = 0;
	int seqEnd = 0;
	int octave = 0;
	int transpose1 = 0;
	int transpose2 = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int valSize = 0;
	long trackSize = 0;
	int lastNote = 0;
	int tempByte = 0;
	long tempPos = 0;
	long tempo = 150;
	int curInst = 0;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int flag4 = 0;
	int repeat1 = 0;
	int repeat2 = 0;
	int repeat3 = 0;
	int repeat4 = 0;
	long repeat1Pt = 0;
	long repeat2Pt = 0;
	long repeat3Pt = 0;
	long repeat4Pt = 0;
	long break1 = 0;
	long break2 = 0;
	long break3 = 0;
	long break4 = 0;
	long loopPt = 0;
	int firstNote = 1;
	int timeVal = 0;
	int triplet = 0;
	int connect = 0;
	int tripNotes = 0;
	int dotted = 0;
	int highOct = 0;
	unsigned char mask = 0;
	int maskArray[8];
	int subVal = 0;
	long oldPos = 0;
	int connExt = 0;
	long speedCtrl = 0;
	long masterDelay = 0;
	int prevNote = 0;

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
		/*Get channel sequence pointers*/
		songPtrs[0] = ReadBE16(&spcData[ptr + 1]);
		songPtrs[1] = ReadBE16(&spcData[ptr + 3]);
		songPtrs[2] = ReadBE16(&spcData[ptr + 5]);
		songPtrs[3] = ReadBE16(&spcData[ptr + 7]);
		songPtrs[4] = ReadBE16(&spcData[ptr + 9]);
		songPtrs[5] = ReadBE16(&spcData[ptr + 11]);
		songPtrs[6] = ReadBE16(&spcData[ptr + 13]);
		songPtrs[7] = ReadBE16(&spcData[ptr + 15]);

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

		switchNum = 0;

		for (switchNum = 0; switchNum < 400; switchNum++)
		{
			switchPoint[switchNum][0] = -1;
			switchPoint[switchNum][1] = 0;
		}

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
			lastNote = 0;
			curNoteLen = 0;
			repeat1 = -1;
			repeat2 = -1;
			repeat3 = -1;
			repeat4 = -1;
			highOct = 0;
			transpose1 = 0;
			transpose2 = 0;
			triplet = 0;
			connect = 0;
			dotted = 0;
			octave = 0;
			connExt = 0;
			speedCtrl = 0;
			curVol = 100;

			switchNum = 0;

			ctrlDelay = 0;
			masterDelay = 0;
			prevNote = -1;


			/*Add track header*/

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			if (songPtrs[curTrack] == 0)
			{
				seqEnd = 1;
			}
			else
			{
				seqPos = songPtrs[curTrack];
			}

			while (seqEnd == 0 && midPos < 48000 && ctrlDelay < 110000)
			{
				command[0] = spcData[seqPos];
				command[1] = spcData[seqPos + 1];
				command[2] = spcData[seqPos + 2];


				/*Check for global transpose*/
				if (curTrack != 0)
				{
					for (switchNum = 0; switchNum < 90; switchNum++)
					{
						if (masterDelay >= switchPoint[switchNum][0] && switchPoint[switchNum][0] != -1)
						{
							if (curTrack == 2)
							{
								curTrack = 2;
							}
							transpose2 = switchPoint[switchNum][1];
						}
					}
				}

				/*Triplets*/
				if (command[0] == 0x00)
				{
					if (triplet == 0)
					{
						triplet = 1;
					}
					else if (triplet == 1)
					{
						triplet = 0;
					}
					seqPos++;
				}

				/*Connect notes*/
				else if (command[0] == 0x01)
				{
					if (connect == 0)
					{
						connect = 1;
					}

					else if (connect == 1)
					{
						connect = 0;
					}
					seqPos++;
				}

				/*Dotted note*/
				else if (command[0] == 0x02)
				{
					dotted = 1;
					seqPos++;
				}

				/*Higher octave*/
				else if (command[0] == 0x03)
				{
					if (highOct == 0)
					{
						highOct = 1;
					}
					else if (highOct == 1)
					{
						highOct = 0;
					}

					seqPos++;
				}

				/*Channel flags*/
				else if (command[0] == 0x04)
				{

					mask = command[1];
					maskArray[7] = mask & 1;
					maskArray[6] = mask >> 1 & 1;
					maskArray[5] = mask >> 2 & 1;
					maskArray[4] = mask >> 3 & 1;
					maskArray[3] = mask >> 4 & 1;
					maskArray[2] = mask >> 5 & 1;
					maskArray[1] = mask >> 6 & 1;
					maskArray[0] = mask >> 7 & 1;

					if (maskArray[0] == 1)
					{
						connect = 1;
					}
					else if (maskArray[1] == 0)
					{
						connect = 0;
					}

					if (maskArray[1] == 1)
					{
						connect = 1;
					}
					else if (maskArray[1] == 0)
					{
						connect = 0;
					}

					if (maskArray[2] == 1)
					{
						triplet = 1;
					}
					else if (maskArray[2] == 0)
					{
						triplet = 0;
					}

					if (maskArray[3] == 1)
					{
						dotted = 1;
					}
					else if (maskArray[3] == 0)
					{
						dotted = 0;
					}

					if (maskArray[4] == 1)
					{
						highOct = 1;
					}
					else if (maskArray[4] == 0)
					{
						highOct = 0;
					}


					seqPos += 2;
				}

				/*Set tempo/speed*/
				else if (command[0] == 0x05)
				{
					speedCtrl = (ReadBE16(&spcData[seqPos + 1])) / 3.45;
					if (speedCtrl != tempo)
					{
						tempo = speedCtrl;
						ctrlMidPos++;
						valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
						ctrlDelay = 0;
						ctrlMidPos += valSize;
						WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
						ctrlMidPos += 3;
						if (tempo < 2)
						{
							tempo = 150;
						}
						WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
						ctrlMidPos += 2;
					}
					seqPos += 3;
				}

				/*Set note size*/
				else if (command[0] == 0x06)
				{
					seqPos += 2;
				}

				/*Set volume*/
				else if (command[0] == 0x07)
				{
					if (curTrack < 2)
					{
						if (command[1] >= 0x0F)
						{
							curVol = 100;
						}
						else if (command[1] <= 0x0E && command[1] >= 0x0B)
						{
							curVol = 90;
						}
						else if (command[1] == 0x0A)
						{
							curVol = 80;
						}
						else if (command[1] <= 0x09 && command[1] >= 0x07)
						{
							curVol = 70;
						}
						else if (command[1] == 0x06)
						{
							curVol = 50;
						}
						else if (command[1] == 0x05)
						{
							curVol = 40;
						}
						else if (command[1] == 0x04)
						{
							curVol = 30;
						}
						else if (command[1] == 0x03)
						{
							curVol = 20;
						}
						else if (command[1] == 0x02)
						{
							curVol = 10;
						}
						else if (command[1] == 0x01)
						{
							curVol = 5;
						}
						else if (command[1] == 0x00)
						{
							curVol = 0;
						}
					}

					else if (curTrack == 2)
					{
						if (command[1] >= 0x04)
						{
							curVol = 100;
						}

						else if (command[1] == 0x03)
						{
							curVol = 80;
						}

						else if (command[1] == 0x02)
						{
							curVol = 70;
						}

						else if (command[1] == 0x01)
						{
							curVol = 25;
						}

						else if (command[1] == 0x00)
						{
							curVol = 0;
						}
					}
					seqPos += 2;
				}

				/*Set vibrato*/
				else if (command[0] == 0x08)
				{
					curInst = command[1];
					if (curInst >= 0x80)
					{
						curInst = 0;
					}
					firstNote = 1;
					seqPos += 2;
				}

				/*Set octave*/
				else if (command[0] == 0x09)
				{
					if (command[1] < 8)
					{
						octave = command[1];
						if (octave == 7)
						{
							octave = 5;
						}
					}
					seqPos += 2;
				}

				/*Global transpose*/
				else if (command[0] == 0x0A)
				{
					if (curTrack == 0)
					{
						transpose2 = (signed char)command[1];
						switchPoint[switchNum][0] = masterDelay;
						switchPoint[switchNum][1] = transpose2;
						switchNum++;
					}
					seqPos += 2;
				}

				/*Transpose*/
				else if (command[0] == 0x0B)
				{
					transpose1 = (signed char)command[1];
					seqPos += 2;
				}

				/*Set tuning*/
				else if (command[0] == 0x0C)
				{
					seqPos += 2;
				}

				/*Pitch slide*/
				else if (command[0] == 0x0D)
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

				/*Repeat (1)*/
				else if (command[0] == 0x0E)
				{
					if (repeat1 == -1)
					{
						repeat1 = command[1];
						repeat1Pt = ReadBE16(&spcData[seqPos + 2]);
					}

					else if (repeat1 > 0)
					{
						seqPos = repeat1Pt;
						repeat1--;
					}
					else if (repeat1 == 0)
					{
						seqPos += 4;
						repeat1 = -1;
					}
				}

				/*Repeat (2)*/
				else if (command[0] == 0x0F)
				{
					if (repeat2 == -1)
					{
						repeat2 = command[1];
						repeat2Pt = ReadBE16(&spcData[seqPos + 2]);
					}

					else if (repeat2 > 0)
					{
						seqPos = repeat2Pt;
						repeat2--;
					}
					else if (repeat2 == 0)
					{
						seqPos += 4;
						repeat2 = -1;
					}
				}

				/*Repeat (3)*/
				else if (command[0] == 0x10)
				{
					if (repeat3 == -1)
					{
						repeat3 = command[1];
						repeat3Pt = ReadBE16(&spcData[seqPos + 2]);
					}

					else if (repeat3 > 0)
					{
						seqPos = repeat3Pt;
						repeat3--;
					}
					else if (repeat3 == 0)
					{
						seqPos += 4;
						repeat3 = -1;
					}
				}

				/*Repeat (4)*/
				else if (command[0] == 0x11)
				{
					if (repeat4 == -1)
					{
						repeat4 = command[1];
						repeat4Pt = ReadBE16(&spcData[seqPos + 2]);
					}

					else if (repeat4 > 0)
					{
						seqPos = repeat4Pt;
						repeat4--;
					}
					else if (repeat4 == 0)
					{
						seqPos += 4;
						repeat4 = -1;
					}
				}

				/*Break (1)*/
				else if (command[0] == 0x12)
				{
					if (repeat1 == 0)
					{
						mask = command[1];
						maskArray[7] = mask & 1;
						maskArray[6] = mask >> 1 & 1;
						maskArray[5] = mask >> 2 & 1;
						maskArray[4] = mask >> 3 & 1;
						maskArray[3] = mask >> 4 & 1;
						maskArray[2] = mask >> 5 & 1;
						maskArray[1] = mask >> 6 & 1;
						maskArray[0] = mask >> 7 & 1;

						if (maskArray[0] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[1] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[2] == 1)
						{
							triplet = 1;
						}
						else if (maskArray[2] == 0)
						{
							triplet = 0;
						}

						if (maskArray[3] == 1)
						{
							dotted = 1;
						}
						else if (maskArray[3] == 0)
						{
							dotted = 0;
						}

						if (maskArray[4] == 1)
						{
							highOct = 1;
						}
						else if (maskArray[4] == 0)
						{
							highOct = 0;
						}
						/*octave = mask & 7;*/

						break1 = ReadBE16(&spcData[seqPos + 2]);
						seqPos = break1;
						repeat1 = -1;
					}

					else
					{
						seqPos += 4;
					}

				}

				/*Break (2)*/
				else if (command[0] == 0x13)
				{
					if (repeat2 == 0)
					{
						mask = command[1];
						maskArray[7] = mask & 1;
						maskArray[6] = mask >> 1 & 1;
						maskArray[5] = mask >> 2 & 1;
						maskArray[4] = mask >> 3 & 1;
						maskArray[3] = mask >> 4 & 1;
						maskArray[2] = mask >> 5 & 1;
						maskArray[1] = mask >> 6 & 1;
						maskArray[0] = mask >> 7 & 1;

						if (maskArray[0] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[1] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[2] == 1)
						{
							triplet = 1;
						}
						else if (maskArray[2] == 0)
						{
							triplet = 0;
						}

						if (maskArray[3] == 1)
						{
							dotted = 1;
						}
						else if (maskArray[3] == 0)
						{
							dotted = 0;
						}

						if (maskArray[4] == 1)
						{
							highOct = 1;
						}
						else if (maskArray[4] == 0)
						{
							highOct = 0;
						}

						break2 = ReadBE16(&spcData[seqPos + 2]);
						seqPos = break2;
						repeat2 = -1;
					}

					else
					{
						seqPos += 4;
					}
				}

				/*Break (3)*/
				else if (command[0] == 0x14)
				{
					if (repeat3 == 0)
					{
						mask = command[1];
						maskArray[7] = mask & 1;
						maskArray[6] = mask >> 1 & 1;
						maskArray[5] = mask >> 2 & 1;
						maskArray[4] = mask >> 3 & 1;
						maskArray[3] = mask >> 4 & 1;
						maskArray[2] = mask >> 5 & 1;
						maskArray[1] = mask >> 6 & 1;
						maskArray[0] = mask >> 7 & 1;

						if (maskArray[0] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[1] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[2] == 1)
						{
							triplet = 1;
						}
						else if (maskArray[2] == 0)
						{
							triplet = 0;
						}

						if (maskArray[3] == 1)
						{
							dotted = 1;
						}
						else if (maskArray[3] == 0)
						{
							dotted = 0;
						}

						if (maskArray[4] == 1)
						{
							highOct = 1;
						}
						else if (maskArray[4] == 0)
						{
							highOct = 0;
						}

						break3 = ReadBE16(&spcData[seqPos + 2]);
						seqPos = break3;
						repeat3 = -1;
					}

					else
					{
						seqPos += 4;
					}
				}

				/*Break (4)*/
				else if (command[0] == 0x15)
				{
					if (repeat4 == 0)
					{
						mask = command[1];
						maskArray[7] = mask & 1;
						maskArray[6] = mask >> 1 & 1;
						maskArray[5] = mask >> 2 & 1;
						maskArray[4] = mask >> 3 & 1;
						maskArray[3] = mask >> 4 & 1;
						maskArray[2] = mask >> 5 & 1;
						maskArray[1] = mask >> 6 & 1;
						maskArray[0] = mask >> 7 & 1;

						if (maskArray[0] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[1] == 1)
						{
							connect = 1;
						}
						else if (maskArray[1] == 0)
						{
							connect = 0;
						}

						if (maskArray[2] == 1)
						{
							triplet = 1;
						}
						else if (maskArray[2] == 0)
						{
							triplet = 0;
						}

						if (maskArray[3] == 1)
						{
							dotted = 1;
						}
						else if (maskArray[3] == 0)
						{
							dotted = 0;
						}

						if (maskArray[4] == 1)
						{
							highOct = 1;
						}
						else if (maskArray[4] == 0)
						{
							highOct = 0;
						}

						break4 = ReadBE16(&spcData[seqPos + 2]);
						seqPos = break4;
						repeat4 = -1;
					}

					else
					{
						seqPos += 4;
					}
				}

				/*Loop point*/
				else if (command[0] == 0x16)
				{
					seqEnd = 1;
				}

				/*End of sequence*/
				else if (command[0] == 0x17)
				{
					seqEnd = 1;
				}

				/*Set duty*/
				else if (command[0] == 0x18)
				{
					seqPos += 2;
				}

				/*Set envelope*/
				else if (command[0] == 0x19)
				{
					seqPos += 2;
				}

				/*Set LFO parameters*/
				else if (command[0] == 0x1A)
				{
					seqPos += 3;
				}

				/*Set echo parameters*/
				else if (command[0] == 0x1B)
				{
					seqPos += 3;
				}

				/*Toggle echo on/off*/
				else if (command[0] == 0x1C)
				{
					seqPos += 2;
				}

				/*Set release rate*/
				else if (command[0] == 0x1D)
				{
					seqPos += 2;
				}

				/*Unknown command 1*/
				else if (command[0] == 0x1E)
				{
					seqPos += 2;
				}

				/*Unknown command 2*/
				else if (command[0] == 0x1F)
				{
					seqPos += 2;
				}

				/*Play note*/
				else if (command[0] >= 0x20)
				{
					if (command[0] >= 0x20 && command[0] < 0x40)
					{
						curNoteLen = 7;
						subVal = 0x20;
					}
					else if (command[0] >= 0x40 && command[0] < 0x60)
					{
						curNoteLen = 15;
						subVal = 0x40;
					}
					else if (command[0] >= 0x60 && command[0] < 0x80)
					{
						curNoteLen = 30;
						subVal = 0x60;
					}
					else if (command[0] >= 0x80 && command[0] < 0xA0)
					{
						curNoteLen = 60;
						subVal = 0x80;
					}
					else if (command[0] >= 0xA0 && command[0] < 0xC0)
					{
						curNoteLen = 120;
						subVal = 0xA0;
					}
					else if (command[0] >= 0xC0 && command[0] < 0xE0)
					{
						curNoteLen = 240;
						subVal = 0xC0;
					}
					else if (command[0] >= 0xE0)
					{
						curNoteLen = 480;
						subVal = 0xE0;
					}

					if (triplet == 1)
					{
						curNoteLen = curNoteLen * 2 / 3;
					}

					if (dotted == 1)
					{
						curNoteLen = curNoteLen * 1.5;
						dotted = 0;
					}

					else if (dotted == 2)
					{
						curNoteLen = curNoteLen * 1.5;
					}

					if (command[0] == 0x20 || command[0] == 0x40 || command[0] == 0x60 || command[0] == 0x80 || command[0] == 0xA0 || command[0] == 0xC0 || command[0] == 0xE0)
					{
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
					}
					else
					{
						if (connect == 0)
						{
							curNote = command[0] - subVal + 23 + (octave * 12) + transpose1 + transpose2;
							if (highOct == 1)
							{
								curNote += 24;
							}

							if (curNote > 24)
							{
								curNote -= 24;
							}

							curNoteLen += connExt;
							tempPos = WriteNoteEvent(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							curDelay = 0;
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							midPos = tempPos;
							connExt = 0;
						}
						else if (connect == 1)
						{
							curNote = command[0] - subVal + 23 + (octave * 12) + transpose1 + transpose2;
							if (highOct == 1)
							{
								curNote += 24;
							}

							if (curNote > 24)
							{
								curNote -= 24;
							}
							if (curNote != prevNote && prevNote != -1)
							{
								tempPos = WriteNoteEvent(midData, midPos, prevNote, connExt, curDelay, firstNote, curTrack, curInst);
								firstNote = 0;
								curDelay = 0;
								ctrlDelay += curNoteLen;
								masterDelay += curNoteLen;
								midPos = tempPos;
								connExt = 0;
							}

							connExt += curNoteLen;
						}
						prevNote = curNote;
					}
					seqPos++;
				}

				/*Unknown command*/
				else
				{
					seqPos += 2;
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