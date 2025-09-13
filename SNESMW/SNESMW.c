/*Martin Walker*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * mid;

long bank;
long offset;
long addTable;
long baseValue;
long tablePtrLoc;
long seqPtrList;
long seqPtrList2;
long seqData;
long songList;
int songPtr;
long patList;
long patData;
int i, j, k;
char outfile[1000000];
int songNum;
long songInfo[4];
long curSpeed;
long nextPtr;
long endPtr;
long bankAmt;
long nextBase;
int addValues[7];
int curPat;
int maskVal;
int curVol;
int tempoVal;

char spcString[12];

long switchPoint[400][4];
int switchNum = 0;

unsigned long seqList[500];
unsigned long chanPts[8];
int totalSeqs;

int drvVers;

unsigned static char* spcData;
unsigned static char* midData;
unsigned static char* ctrlMidData;

long midLength;

char* tempPnt;
char OutFileBase[0x100];

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
unsigned int WriteNoteEvent(unsigned char* buffer, unsigned int pos, unsigned int note, int length, int delay, int firstNote, int curChan, int inst);
int WriteDeltaTime(unsigned char* buffer, unsigned int pos, unsigned int value);
void song2mid(int songNum, long ptrs[8]);
void getTimeVals(int songNum, long ptrs[8]);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
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
	if (args < 2)
	{
		printf("Usage: SNESMW <spc>\n");
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

				/*Check for early driver version (Super James Pond NTSC)*/
				if (spcData[0x0500] == 0x20 && spcData[0x0501] == 0xCD && spcData[0x0502] == 0xFF && spcData[0x0503] == 0xBD)
				{
					drvVers = 0;
				}
				else
				{
					drvVers = 1;
				}



				/*Get RAM locations*/
				seqPtrList = ReadLE16(&spcData[0x0006]);
				seqData = ReadLE16(&spcData[0x0008]);
				patList = ReadLE16(&spcData[0x000A]);
				patData = ReadLE16(&spcData[0x000C]);
				songList = ReadLE16(&spcData[0x000E]);
				baseValue = ReadLE16(&spcData[0x0004]) - 0x10;
				endPtr = ReadLE16(&spcData[baseValue + 0x000A]) + songList;
				songNum = 1;
				songPtr = songList + (9 * songNum);

				printf("Sequence data pointer list: 0x%04X\n", seqPtrList);
				printf("Sequence data: 0x%04X\n", seqData);
				printf("Pattern list: 0x%04X\n", patList);
				printf("Pattern data: 0x%04X\n", patData);
				printf("Song list: 0x%04X\n", songList);
				printf("End of song data: 0x%04X\n", endPtr);

				songPtr = songList;

				while (songPtr < endPtr)
				{
					j = songPtr + 1;
					if (spcData[songPtr] < 0x80)
					{
						printf("Song %i: 0x%04X\n", songNum, songPtr);

						/*Get song pattern pointers*/
						for (i = 0; i < 8; i++)
						{
							curPat = spcData[j];

							if (curPat == 0xFF)
							{
								chanPts[i] = 0x0000;
							}
							else
							{
								chanPts[i] = ReadLE16(&spcData[patList + (2 * curPat)]) + patData;
							}

							printf("Song %i, channel %i: 0x%04X\n", songNum, (i + 1), chanPts[i]);
							j++;
						}
						getTimeVals(songNum, chanPts);
						song2mid(songNum, chanPts);
					}
					else
					{
						printf("Song %i (invalid, skipped): 0x%04X\n", songNum, songPtr);
					}

					songNum++;
					songPtr += 9;
				}



				free(spcData);
				fclose(spc);
				printf("The operation was successfully completed!\n");
				exit(0);
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
void song2mid(int songNum, long ptrs[8])
{
	long romPos = 0;
	unsigned int midPos = 0;
	int trackCnt = 8;
	int curTrack = 0;
	int curSeq = 0;
	long midTrackBase = 0;
	unsigned int curDelay = 0;
	int midChan = 0;
	int seqEnd = 0;
	int trackEnd = 0;
	int noteTrans = 0;
	int ticks = 120;
	int k = 0;
	long startPos = 0;

	unsigned int ctrlMidPos = 0;
	long ctrlMidTrackBase = 0;

	int valSize = 0;

	long trackSize = 0;

	unsigned int curNote = 0;
	int curNoteLen = 0;
	int lastNote = 0;

	int tempByte = 0;
	long tempPos = 0;

	long tempo = 160;

	int curInst = 0;

	int macRepeat = 0;
	long macStart = 0;
	long macEnd = 0;

	int playTimes = 1;

	unsigned long patPos = 0;
	unsigned long seqPos = 0;

	unsigned char command[4];

	signed int transpose = 0;
	signed int transpose2 = 0;

	int firstNote = 1;

	int timeVal = 0;

	int holdNote = 0;

	long ctrlDelay = 0;
	long masterDelay = 0;

	midPos = 0;
	ctrlMidPos = 0;
	switchNum = 0;

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

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			transpose = 0;
			transpose2 = 0;
			firstNote = 1;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			trackEnd = 0;

			curNote = 0;
			lastNote = 0;
			curNoteLen = 0;

			masterDelay = 0;

			switchNum = 0;
			curVol = 100;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);
			patPos = ptrs[curTrack];
			if (ptrs[curTrack] == 0)
			{
				trackEnd = 1;
			}

			while (trackEnd == 0)
			{
				/*Repeat macro*/
				if (spcData[patPos] > 0x40 && spcData[patPos] < 0x80)
				{
					macRepeat = spcData[patPos] - 0x40;
					macStart = patPos + 1;
					patPos = macStart;
				}

				else if (spcData[patPos] >= 0x80 && spcData[patPos] <= 0xFB)
				{
					playTimes = spcData[patPos];
					curSeq = spcData[patPos + 1];
					seqEnd = 0;
					patPos += 2;
				}

				/*Get current sequence and repeat times*/
				if (spcData[patPos] != 0xFF && spcData[patPos] != 0xFE && spcData[patPos] != 0xFD && spcData[patPos] != 0xFC && spcData[patPos] < 0x40)
				{
					playTimes = spcData[patPos];
					curSeq = spcData[patPos + 1];
					seqEnd = 0;
					patPos += 2;
				}
				/*End of repeat macro*/
				else if (spcData[patPos] == 0x40)
				{
					if (macRepeat > 1)
					{
						patPos = macStart;
						macRepeat--;
					}
					else
					{
						patPos++;
					}

				}

				/*End of track*/
				else if (spcData[patPos] == 0xFF || spcData[patPos] == 0xFE || spcData[patPos] == 0xFD || spcData[patPos] == 0xFC)
				{
					trackEnd = 1;
				}

				/*Go to the current sequence*/
				seqPos = ReadLE16(&spcData[seqPtrList + (2 * curSeq)]) + seqData;
				startPos = seqPos;

				if (drvVers == 1)
				{
					while (seqEnd == 0)
					{
						command[0] = spcData[seqPos];
						command[1] = spcData[seqPos + 1];
						command[2] = spcData[seqPos + 2];
						command[3] = spcData[seqPos + 3];

						for (switchNum = 0; switchNum < 90; switchNum++)
						{
							if (masterDelay >= switchPoint[switchNum][0] && switchPoint[switchNum][0] != -1)
							{
								maskVal = switchPoint[switchNum][2];
								if (curTrack == 7)
								{
									maskVal = maskVal &= 0x80;
								}
								else if (curTrack == 6)
								{
									maskVal = maskVal &= 0x40;
								}
								else if (curTrack == 5)
								{
									maskVal = maskVal &= 0x20;
								}
								else if (curTrack == 4)
								{
									maskVal = maskVal &= 0x10;
								}
								else if (curTrack == 3)
								{
									maskVal = maskVal &= 0x08;
								}
								else if (curTrack == 2)
								{
									maskVal = maskVal &= 0x04;
								}
								else if (curTrack == 1)
								{
									maskVal = maskVal &= 0x02;
								}
								else if (curTrack == 0)
								{
									maskVal = maskVal &= 0x01;
								}
								else
								{
									maskVal = maskVal &= 0x80;
								}

								if (maskVal != 0x00)
								{
									if (switchPoint[switchNum][1] == 0x00)
									{
										tempoVal = switchPoint[switchNum][3];
										if (tempo != tempoVal)
										{
											ctrlMidPos++;
											valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
											ctrlDelay = 0;
											ctrlMidPos += valSize;
											WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
											ctrlMidPos += 3;
											tempo = switchPoint[switchNum][3] * 1.2;
											WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
											ctrlMidPos += 2;
										}
									}

									else if (switchPoint[switchNum][1] == 0x01)
									{
										transpose2 = switchPoint[switchNum][3];
									}

									else if (switchPoint[switchNum][1] == 0x02)
									{
										curVol = switchPoint[switchNum][3] * 0.5;
									}

									else if (switchPoint[switchNum][1] == 0x03)
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
									}


									if (curTrack == 7)
									{
										switchPoint[switchNum][2] &= 0x7F;
									}
									else if (curTrack == 6)
									{
										switchPoint[switchNum][2] &= 0xBF;
									}
									else if (curTrack == 5)
									{
										switchPoint[switchNum][2] &= 0xDF;
									}
									else if (curTrack == 4)
									{
										switchPoint[switchNum][2] &= 0xEF;
									}
									else if (curTrack == 3)
									{
										switchPoint[switchNum][2] &= 0xF7;
									}
									else if (curTrack == 2)
									{
										switchPoint[switchNum][2] &= 0xFB;
									}
									else if (curTrack == 1)
									{
										switchPoint[switchNum][2] &= 0xFD;
									}
									else if (curTrack == 0)
									{
										switchPoint[switchNum][2] &= 0xFE;
									}
									else
									{
										switchPoint[switchNum][2] &= 0x7F;
									}
								}
							}
						}

						/*Change instrument*/
						if (command[0] == 0x80)
						{
							curInst = command[1];
							firstNote = 1;
							seqPos += 2;
						}
						/*Rest*/
						else if (command[0] == 0x81)
						{
							curDelay += (command[1] * 10);
							ctrlDelay += (command[1] * 10);
							masterDelay += (command[1] * 10);
							seqPos += 2;
						}
						/*Set tempo*/
						else if (command[0] == 0x82)
						{
							tempoVal = command[1] * 1.2;
							if (tempoVal != tempo)
							{
								ctrlMidPos++;
								valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
								ctrlDelay = 0;
								ctrlMidPos += valSize;
								WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
								ctrlMidPos += 3;
								tempo = command[1] * 1.2;
								WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
								ctrlMidPos += 2;
							}
							seqPos += 2;

						}
						/*Set tempo (multiple channels)*/
						else if (command[0] == 0x83)
						{
							seqPos += 4;

						}
						/*Set channel gain*/
						else if (command[0] == 0x84)
						{
							seqPos += 2;
						}
						/*Set release*/
						else if (command[0] == 0x85)
						{
							seqPos += 2;
						}
						/*Set attack*/
						else if (command[0] == 0x86)
						{
							seqPos += 2;
						}
						/*Set panpot?*/
						else if (command[0] == 0x87)
						{
							seqPos += 2;
						}
						/*Set pan*/
						else if (command[0] == 0x88)
						{
							seqPos += 2;
						}
						/*Set detune*/
						else if (command[0] == 0x89)
						{
							seqPos += 2;
						}
						/*Set channel transpose*/
						else if (command[0] == 0x8A)
						{
							transpose = (signed char)command[1];

							if (transpose > 24 && curNote >= 96)
							{
								transpose = 0;
							}
							seqPos += 2;
						}
						/*Set transpose (multiple channels)*/
						else if (command[0] == 0x8B)
						{
							seqPos += 4;
						}
						/*Set channel volume*/
						else if (command[0] == 0x8C)
						{
							curVol = command[1] * 0.5;
							seqPos += 2;
						}
						/*Command 8D*/
						else if (command[0] == 0x8D)
						{
							seqPos += 4;
						}
						/*Command 8E*/
						else if (command[0] == 0x8E)
						{
							seqPos += 2;
						}
						/*Command 8F*/
						else if (command[0] == 0x8F)
						{
							seqPos += 4;
						}
						/*Command 90*/
						else if (command[0] == 0x90)
						{
							seqPos += 2;
						}
						/*Command 91*/
						else if (command[0] == 0x91)
						{
							seqPos += 2;
						}
						/*Pitch bend (multiple channels)*/
						else if (command[0] == 0x92)
						{
							seqPos += 4;
						}
						/*Command 93 (pitch bend 2?)*/
						else if (command[0] == 0x93)
						{
							seqPos += 4;
						}
						/*Disable pitch bend*/
						else if (command[0] == 0x94)
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
						/*Set echo volume*/
						else if (command[0] == 0x95)
						{
							seqPos += 4;
						}
						/*Set echo delay*/
						else if (command[0] == 0x96)
						{
							seqPos += 4;
						}
						/*Disable echo*/
						else if (command[0] == 0x97)
						{
							seqPos += 2;
						}
						/*Command 98*/
						else if (command[0] == 0x98)
						{
							seqPos += 4;
						}
						/*Command 99*/
						else if (command[0] == 0x99)
						{
							seqPos += 4;
						}
						/*Command 9A*/
						else if (command[0] == 0x9A)
						{
							seqPos += 4;
						}
						/*Set vibrato*/
						else if (command[0] == 0x9B)
						{
							seqPos += 2;
						}
						/*Set delay*/
						else if (command[0] == 0x9C)
						{
							seqPos += 2;
						}
						/*Set channel volume (multiple channels)*/
						else if (command[0] == 0x9D)
						{
							seqPos += 4;
						}
						/*End of sequence*/
						else if (command[0] == 0xFF)
						{
							if (playTimes > 1)
							{
								seqPos = startPos;
								playTimes--;
							}
							else
							{
								seqEnd = 1;
							}
						}
						else if (command[0] >= 0x80)
						{
							seqPos += 2;
						}
						else
						{
							curNote = command[0] + transpose + transpose2;
							curNoteLen = command[1] * 10;
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEvent(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = 0;
							seqPos += 2;
						}
					}
				}
				else
				{
					while (seqEnd == 0)
					{
						command[0] = spcData[seqPos];
						command[1] = spcData[seqPos + 1];
						command[2] = spcData[seqPos + 2];
						command[3] = spcData[seqPos + 3];

						for (switchNum = 0; switchNum < 90; switchNum++)
						{
							if (masterDelay >= switchPoint[switchNum][0] && switchPoint[switchNum][0] != -1)
							{
								maskVal = switchPoint[switchNum][2];
								if (curTrack == 7)
								{
									maskVal = maskVal &= 0x80;
								}
								else if (curTrack == 6)
								{
									maskVal = maskVal &= 0x40;
								}
								else if (curTrack == 5)
								{
									maskVal = maskVal &= 0x20;
								}
								else if (curTrack == 4)
								{
									maskVal = maskVal &= 0x10;
								}
								else if (curTrack == 3)
								{
									maskVal = maskVal &= 0x08;
								}
								else if (curTrack == 2)
								{
									maskVal = maskVal &= 0x04;
								}
								else if (curTrack == 1)
								{
									maskVal = maskVal &= 0x02;
								}
								else if (curTrack == 0)
								{
									maskVal = maskVal &= 0x01;
								}
								else
								{
									maskVal = maskVal &= 0x80;
								}

								if (maskVal != 0x00)
								{
									if (switchPoint[switchNum][1] == 0x00)
									{
										tempoVal = switchPoint[switchNum][3];
										if (tempo != tempoVal)
										{
											ctrlMidPos++;
											valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
											ctrlDelay = 0;
											ctrlMidPos += valSize;
											WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
											ctrlMidPos += 3;
											tempo = switchPoint[switchNum][3] * 3.5;
											WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
											ctrlMidPos += 2;
										}
									}

									else if (switchPoint[switchNum][1] == 0x01)
									{
										transpose2 = switchPoint[switchNum][3];
									}

									else if (switchPoint[switchNum][1] == 0x02)
									{
										curVol = switchPoint[switchNum][3] * 0.5;
									}

									else if (switchPoint[switchNum][1] == 0x03)
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
									}


									if (curTrack == 7)
									{
										switchPoint[switchNum][2] &= 0x7F;
									}
									else if (curTrack == 6)
									{
										switchPoint[switchNum][2] &= 0xBF;
									}
									else if (curTrack == 5)
									{
										switchPoint[switchNum][2] &= 0xDF;
									}
									else if (curTrack == 4)
									{
										switchPoint[switchNum][2] &= 0xEF;
									}
									else if (curTrack == 3)
									{
										switchPoint[switchNum][2] &= 0xF7;
									}
									else if (curTrack == 2)
									{
										switchPoint[switchNum][2] &= 0xFB;
									}
									else if (curTrack == 1)
									{
										switchPoint[switchNum][2] &= 0xFD;
									}
									else if (curTrack == 0)
									{
										switchPoint[switchNum][2] &= 0xFE;
									}
									else
									{
										switchPoint[switchNum][2] &= 0x7F;
									}
								}
							}
						}

						/*Change instrument*/
						if (command[0] == 0x80)
						{
							curInst = command[1];
							firstNote = 1;
							seqPos += 2;
						}

						/*Set gain*/
						else if (command[0] == 0x81)
						{
							seqPos += 2;
						}

						/*Set gain (multiple channels)*/
						else if (command[0] == 0x82)
						{
							seqPos += 5;
						}

						/*Command 83*/
						else if (command[0] == 0x83)
						{
							seqPos += 4;
						}

						/*Command 84*/
						else if (command[0] == 0x84)
						{
							seqPos += 2;
						}

						/*Command 85*/
						else if (command[0] == 0x85)
						{
							seqPos += 2;
						}

						/*Set global volume*/
						else if (command[0] == 0x86)
						{
							seqPos += 5;
						}

						/*Set channel volume (multiple channels)*/
						else if (command[0] == 0x87)
						{
							seqPos += 4;
						}

						/*Set tempo*/
						else if (command[0] == 0x88)
						{
							tempoVal = command[1] * 1.2;
							if (tempoVal != tempo)
							{
								ctrlMidPos++;
								valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
								ctrlDelay = 0;
								ctrlMidPos += valSize;
								WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
								ctrlMidPos += 3;
								tempo = command[1] * 1.2;
								WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
								ctrlMidPos += 2;
							}
							seqPos += 2;
						}

						/*Set tempo (multiple channels)*/
						else if (command[0] == 0x89)
						{
							seqPos += 4;
						}

						/*Set channel transpose*/
						else if (command[0] == 0x8A)
						{
							transpose = (signed char)command[1];
							seqPos += 2;
						}

						/*Set transpose (multiple channels)*/
						else if (command[0] == 0x8B)
						{
							seqPos += 4;
						}

						/*Disable transpose*/
						else if (command[0] == 0x8C)
						{
							seqPos += 2;
						}

						/*Set channel volume*/
						else if (command[0] == 0x8D)
						{
							seqPos += 2;
						}

						/*Command 8E*/
						else if (command[0] == 0x8E)
						{
							seqPos += 4;
						}

						/*Command 8F (No effect)*/
						else if (command[0] == 0x8F)
						{
							seqPos += 2;
						}

						/*Command 90*/
						else if (command[0] == 0x90)
						{
							seqPos += 2;
						}

						/*Pitch bend*/
						else if (command[0] == 0x91)
						{
							seqPos += 2;
						}

						/*Command 92*/
						else if (command[0] == 0x92)
						{
							seqPos += 2;
						}

						/*Disable pitch bend*/
						else if (command[0] == 0x93)
						{
							seqPos += 2;
						}

						/*Set detune*/
						else if (command[0] == 0x94)
						{
							seqPos += 2;
						}

						/*Set echo delay*/
						else if (command[0] == 0x95)
						{
							seqPos += 4;
						}

						/*Disable echo*/
						else if (command[0] == 0x96)
						{
							seqPos += 2;
						}

						/*Set echo volume*/
						else if (command[0] == 0x97)
						{
							seqPos += 4;
						}

						/*Command 98*/
						else if (command[0] == 0x98)
						{
							seqPos += 4;
						}

						/*End of sequence*/
						else if (command[0] == 0xFF)
						{
							if (playTimes > 1)
							{
								seqPos = startPos;
								playTimes--;
							}
							else
							{
								seqEnd = 1;
							}
						}

						else if (command[0] >= 0x80)
						{
							seqPos += 2;
						}

						/*Play note*/
						else
						{
							curNote = command[0] + transpose + transpose2 + 36;
							curNoteLen = command[1] * 10;
							ctrlDelay += curNoteLen;
							masterDelay += curNoteLen;
							tempPos = WriteNoteEvent(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							firstNote = 0;
							midPos = tempPos;
							curDelay = 0;
							seqPos += 2;
						}
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

		sprintf(outfile, "%s_%01X.mid", OutFileBase, songNum);
		fwrite(ctrlMidData, ctrlMidPos, 1, mid);
		fwrite(midData, midPos, 1, mid);
		fclose(mid);
	}
}

void getTimeVals(int songNum, long ptrs[8])
{
	long romPos = 0;
	int trackCnt = 8;
	int curTrack = 0;
	int curSeq = 0;
	unsigned int curDelay = 0;
	int midChan = 0;
	int seqEnd = 0;
	int trackEnd = 0;
	int noteTrans = 0;
	int ticks = 120;
	int k = 0;
	long startPos = 0;

	unsigned int curNote = 0;
	int curNoteLen = 0;
	int lastNote = 0;

	int tempByte = 0;
	long tempPos = 0;

	long tempo = 160;

	int curInst = 0;

	int macRepeat = 0;
	long macStart = 0;
	long macEnd = 0;

	int playTimes = 1;

	unsigned long patPos = 0;
	unsigned long seqPos = 0;

	unsigned char command[4];

	signed int transpose = 0;
	signed int transpose2 = 0;

	int firstNote = 1;

	int timeVal = 0;

	int holdNote = 0;

	long ctrlDelay = 0;
	long masterDelay = 0;

	switchNum = 0;

	for (switchNum = 0; switchNum < 400; switchNum++)
	{
		switchPoint[switchNum][0] = -1;
		switchPoint[switchNum][1] = 0;
		switchPoint[switchNum][2] = 0;
		switchPoint[switchNum][3] = 0;
	}

	for (curTrack = 0; curTrack < trackCnt; curTrack++)
	{
		curDelay = 0;
		ctrlDelay = 0;
		trackEnd = 0;

		curNote = 0;
		lastNote = 0;
		curNoteLen = 0;

		masterDelay = 0;

		switchNum = 0;

		patPos = ptrs[curTrack];
		if (ptrs[curTrack] == 0)
		{
			trackEnd = 1;
		}


		while (trackEnd == 0)
		{
			if (spcData[patPos] > 0x40 && spcData[patPos] < 0x80)
			{
				macRepeat = spcData[patPos] - 0x40;
				macStart = patPos + 1;
				patPos = macStart;
			}

			else if (spcData[patPos] >= 0x80 && spcData[patPos] <= 0xFB)
			{
				playTimes = spcData[patPos];
				curSeq = spcData[patPos + 1];
				seqEnd = 0;
				patPos += 2;
			}

			/*Get current sequence and repeat times*/
			if (spcData[patPos] != 0xFF && spcData[patPos] != 0xFE && spcData[patPos] != 0xFD && spcData[patPos] != 0xFC && spcData[patPos] < 0x40)
			{
				playTimes = spcData[patPos];
				curSeq = spcData[patPos + 1];
				seqEnd = 0;
				patPos += 2;
			}
			/*End of repeat macro*/
			else if (spcData[patPos] == 0x40)
			{
				if (macRepeat > 1)
				{
					patPos = macStart;
					macRepeat--;
				}
				else
				{
					patPos++;
				}

			}

			/*End of track*/
			else if (spcData[patPos] == 0xFF || spcData[patPos] == 0xFE || spcData[patPos] == 0xFD || spcData[patPos] == 0xFC)
			{
				trackEnd = 1;
			}

			/*Go to the current sequence*/
			seqPos = ReadLE16(&spcData[seqPtrList + (2 * curSeq)]) + seqData;
			startPos = seqPos;

			if (drvVers == 1)
			{
				while (seqEnd == 0)
				{
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];

					/*Change instrument*/
					if (command[0] == 0x80)
					{
						seqPos += 2;
					}
					/*Rest*/
					else if (command[0] == 0x81)
					{
						curDelay += (command[1] * 10);
						ctrlDelay += (command[1] * 10);
						masterDelay += (command[1] * 10);
						seqPos += 2;
					}
					/*Set tempo*/
					else if (command[0] == 0x82)
					{
						seqPos += 2;
					}
					/*Set tempo (multiple channels)*/
					else if (command[0] == 0x83)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 0;
						switchPoint[switchNum][2] = command[1];
						switchPoint[switchNum][3] = command[2];
						switchNum++;
						seqPos += 4;
					}

					/*Set channel gain*/
					else if (command[0] == 0x84)
					{
						seqPos += 2;
					}

					/*Set release*/
					else if (command[0] == 0x85)
					{
						seqPos += 2;
					}

					/*Set attack*/
					else if (command[0] == 0x86)
					{
						seqPos += 2;
					}

					/*Set panpot?*/
					else if (command[0] == 0x87)
					{
						seqPos += 2;
					}

					/*Set pan*/
					else if (command[0] == 0x88)
					{
						seqPos += 2;
					}

					/*Set detune*/
					else if (command[0] == 0x89)
					{
						seqPos += 2;
					}

					/*Set channel transpose*/
					else if (command[0] == 0x8A)
					{
						seqPos += 2;
					}

					/*Set channel transpose (multiple channels)*/
					else if (command[0] == 0x8B)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 1;
						switchPoint[switchNum][2] = command[1];
						switchPoint[switchNum][3] = command[2];
						seqPos += 4;
						switchNum++;
					}
					/*Set channel volume*/
					else if (command[0] == 0x8C)
					{
						seqPos += 2;
					}
					/*Command 8D*/
					else if (command[0] == 0x8D)
					{
						seqPos += 4;
					}
					/*Command 8E*/
					else if (command[0] == 0x8E)
					{
						seqPos += 2;
					}
					/*Command 8F*/
					else if (command[0] == 0x8F)
					{
						seqPos += 4;
					}
					/*Command 90*/
					else if (command[0] == 0x90)
					{
						seqPos += 2;
					}
					/*Command 91*/
					else if (command[0] == 0x91)
					{
						seqPos += 2;
					}
					/*Pitch bend (multiple channels)*/
					else if (command[0] == 0x92)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 3;
						switchPoint[switchNum][2] = command[1];
						switchPoint[switchNum][3] = command[2];
						seqPos += 4;
						switchNum++;
					}
					/*Command 93 (pitch bend 2?)*/
					else if (command[0] == 0x93)
					{
						seqPos += 4;
					}
					/*Disable pitch bend*/
					else if (command[0] == 0x94)
					{
						seqPos += 2;
					}
					/*Set echo volume*/
					else if (command[0] == 0x95)
					{
						seqPos += 4;
					}
					/*Set echo delay*/
					else if (command[0] == 0x96)
					{
						seqPos += 4;
					}
					/*Disable echo*/
					else if (command[0] == 0x97)
					{
						seqPos += 2;
					}
					/*Command 98*/
					else if (command[0] == 0x98)
					{
						seqPos += 4;
					}
					/*Command 99*/
					else if (command[0] == 0x99)
					{
						seqPos += 4;
					}
					/*Command 9A*/
					else if (command[0] == 0x9A)
					{
						seqPos += 4;
					}
					/*Set vibrato*/
					else if (command[0] == 0x9B)
					{
						seqPos += 2;
					}
					/*Set delay*/
					else if (command[0] == 0x9C)
					{
						seqPos += 2;
					}
					/*Set channel volume (multiple channels)*/
					else if (command[0] == 0x9D)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 2;
						switchPoint[switchNum][2] = command[1];
						switchPoint[switchNum][3] = command[2];
						switchNum++;
						seqPos += 4;
					}
					/*End of sequence*/
					else if (command[0] == 0xFF)
					{
						if (playTimes > 1)
						{
							seqPos = startPos;
							playTimes--;
						}
						else
						{
							seqEnd = 1;
						}
					}
					/*Play note*/
					else
					{
						curNoteLen = command[1] * 10;
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos += 2;
					}
				}
			}
			else
			{
				while (seqEnd == 0)
				{
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];

					/*Change instrument*/
					if (command[0] == 0x80)
					{
						curInst = command[1];
						firstNote = 1;
						seqPos += 2;
					}

					/*Set gain*/
					else if (command[0] == 0x81)
					{
						seqPos += 2;
					}

					/*Set gain (multiple channels)*/
					else if (command[0] == 0x82)
					{
						seqPos += 5;
					}

					/*Command 83*/
					else if (command[0] == 0x83)
					{
						seqPos += 4;
					}

					/*Command 84*/
					else if (command[0] == 0x84)
					{
						seqPos += 2;
					}

					/*Command 85*/
					else if (command[0] == 0x85)
					{
						seqPos += 2;
					}

					/*Set global volume*/
					else if (command[0] == 0x86)
					{
						seqPos += 5;
					}

					/*Set channel volume (multiple channels)*/
					else if (command[0] == 0x87)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 2;
						switchPoint[switchNum][2] = command[2];
						switchPoint[switchNum][3] = command[3];
						switchNum++;
						seqPos += 4;
					}

					/*Set tempo*/
					else if (command[0] == 0x88)
					{
						seqPos += 2;
					}

					/*Set tempo (multiple channels)*/
					else if (command[0] == 0x89)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 0;
						switchPoint[switchNum][2] = command[2];
						switchPoint[switchNum][3] = command[3];
						seqPos += 4;
						switchNum++;
					}

					/*Set channel transpose*/
					else if (command[0] == 0x8A)
					{
						transpose = (signed char)command[1];
						seqPos += 2;
					}

					/*Set transpose (multiple channels)*/
					else if (command[0] == 0x8B)
					{
						switchPoint[switchNum][0] = curDelay;
						switchPoint[switchNum][1] = 1;
						switchPoint[switchNum][2] = command[2];
						switchPoint[switchNum][3] = command[3];
						switchNum++;
						seqPos += 4;
					}

					/*Disable transpose*/
					else if (command[0] == 0x8C)
					{
						seqPos += 2;
					}

					/*Set channel volume*/
					else if (command[0] == 0x8D)
					{
						seqPos += 2;
					}

					/*Command 8E*/
					else if (command[0] == 0x8E)
					{
						seqPos += 4;
					}

					/*Command 8F (No effect)*/
					else if (command[0] == 0x8F)
					{
						seqPos += 2;
					}

					/*Command 90*/
					else if (command[0] == 0x90)
					{
						seqPos += 2;
					}

					/*Pitch bend*/
					else if (command[0] == 0x91)
					{
						seqPos += 2;
					}

					/*Command 92*/
					else if (command[0] == 0x92)
					{
						seqPos += 2;
					}

					/*Disable pitch bend*/
					else if (command[0] == 0x93)
					{
						seqPos += 2;
					}

					/*Set detune*/
					else if (command[0] == 0x94)
					{
						seqPos += 2;
					}

					/*Set echo delay*/
					else if (command[0] == 0x95)
					{
						seqPos += 4;
					}

					/*Disable echo*/
					else if (command[0] == 0x96)
					{
						seqPos += 2;
					}

					/*Set echo volume*/
					else if (command[0] == 0x97)
					{
						seqPos += 4;
					}

					/*Command 98*/
					else if (command[0] == 0x98)
					{
						seqPos += 4;
					}

					/*End of sequence*/
					else if (command[0] == 0xFF)
					{
						if (playTimes > 1)
						{
							seqPos = startPos;
							playTimes--;
						}
						else
						{
							seqEnd = 1;
						}
					}

					else if (command[0] >= 0x80)
					{
						seqPos += 2;
					}

					/*Play note*/
					else
					{
						curNoteLen = command[1] * 10;
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						masterDelay += curNoteLen;
						seqPos += 2;
					}
				}
			}
		}
	}
}
