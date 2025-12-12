/*Nintendo (N-SPC)*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <complex.h>
#include "SNESNint.h"

#define ramSize 65536

FILE* spc, * mid;

long bank;
long offset;
long songTable;
long baseValue;
long tablePtrLoc;
long seqPtrList;
long seqPtrList2;
long seqData;
long songList;
int songPtr;
int songIndex;
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
int curVols[8];
int curVolVels[8];
int tempoVal;
int foundTable;
int foundLen;
int versOverride;
int ptrOverride;
int numOverride;
int chanOverride;
int numChan;
int volTrack = 0;
long lenTab;
long velTab;
int numSongs;
int addVal;
int qtFix;
int tableFix;
long maxLen;

char *stringPtr;

char spcString[12];

long switchPoint[400][4];
int switchNum = 0;

unsigned long seqList[500];
unsigned long chanPts[8];
int totalSeqs;

int drvVers;

char* argParam;

unsigned char* spcData;
unsigned char* midData;
unsigned char* multiMidData[8];
unsigned char* ctrlMidData;

long midLength;

const char MagicBytes1[3] = { 0x1C, 0x5D, 0xF5 };
const char MagicBytes2[3] = { 0x1C, 0xFD, 0xF6 };
const char MagicBytes3[3] = { 0x1C, 0xFD, 0xF7 };
const char MagicBytes4[7] = { 0xE8, 0x00, 0xC4, 0x00, 0xC4, 0x04, 0x8D };
const char MagicBytes5[3] = { 0x0D, 0x5D, 0xF5 };
const char MagicBytes6[4] = { 0x1C, 0xF0, 0x15, 0x5D };
const char MagicBytes7[8] = { 0xE4, 0x1A, 0x48, 0xFF, 0x0E, 0x46, 0x00, 0x6F };
const char MagicBytes8[4] = { 0xDA, 0x23, 0x3A, 0x23 };
const char MagicBytes9[4] = { 0x2D, 0xDD, 0x80, 0xA8 };
const char MagicBytes10[4] = { 0xFD, 0xAE, 0x6F, 0xE5 };
const char MagicBytes11[3] = { 0x28, 0x1F, 0xC4 };
const char MagicBytes12[14] = { 0xC4, 0x06, 0x1C, 0x5D, 0xF5, 0xFE };
const char MagicBytes13[4] = { 0x9C, 0x1C, 0xFD, 0xF6 };
const char MagicBytesDE[5] = { 0x05, 0xEE, 0xD0, 0x01, 0x6F };
const char MagicBytesIS[4] = { 0x1C, 0xF0, 0x0D, 0x5D };
const char MagicBytesKon[4] = { 0xD6, 0xE1, 0x02, 0x2D };
const char MagicBytesSSR[3] = { 0x15, 0x5D, 0xF5 };
const char MagicBytesArg1[6] = { 0x00, 0x6F, 0xCD, 0x0E, 0x8F, 0x80 };
const char MagicBytesArg2[11] = { 0xE4, 0x05, 0xC4, 0x04, 0x1C, 0xF0, 0x14, 0x9C, 0xFD, 0xE8, 0x00 };
const char MagicBytesKS[2] = { 0xE8, 0x01 };
const char MagicBytesTOSE[5] = { 0x8D, 0x5D, 0xE8, 0x1B, 0xC4 };
const char MagicBytesTOSE2[5] = { 0x8D, 0x5D, 0xE8, 0x1A, 0xC4 };
const char MagicBytesTaito[2] = { 0x1C, 0xF0 };
const char MagicBytesOutback[6] = { 0xC4, 0x04, 0xE5, 0x00, 0x17, 0xEC };
const char MagicBytesDQ6[3] = { 0x08, 0xEE, 0xF6 };
const char MagicBytesOgre[4] = { 0xE4, 0xCC, 0x68, 0xFF };
const char MagicBytesTO[5] = { 0xE4, 0xBC, 0x9C, 0x1C, 0xFD };
const char MagicBytesCPBRAiN[9] = { 0x8F, 0x00, 0xC2, 0xE8, 0x03, 0xC4, 0x07, 0x8F, 0xFF };
const char MagicBytesHanari[9] = { 0xE8, 0x01, 0x2F, 0x02, 0xE8, 0x02, 0xC4, 0x04, 0x3F};
const char MagicBytesDDW[4] = { 0xC4, 0x04, 0x5D, 0xF5 };
const char MagicBytesBLCB[7] = { 0xDA, 0x40, 0x8F, 0x02, 0x0C, 0x8D, 0x00 };
const char MagicBytesSAI2[6] = { 0xC4, 0x0D, 0x9C, 0x1C, 0xFD, 0xF6 };
const char MagicBytesMDH[3] = { 0x1C, 0xFD, 0xF6 };
const char MagicBytesMIHO[9] = { 0xE8, 0x01, 0xC4, 0x7F, 0xC4, 0x7E, 0x6F, 0x5D, 0xF5 };
const char MagicBytesSDD[5] = { 0x2F, 0xF9, 0xCB, 0x0B, 0x4D };
const char MagicBytesMSQ[8] = { 0x8F, 0x00, 0x18, 0x68, 0x80, 0x90, 0x0A, 0xE8 };
const char MagicBytes7th[6] = { 0xFD, 0xAE, 0x6F, 0x1C, 0xFD, 0xF6 };
const char MagicBytesFH[5] = { 0xC5, 0xA4, 0x03, 0x2D, 0xE5 };
const char MagicBytesTHG[8] = { 0x68, 0xB6, 0xB0, 0x5F, 0x68, 0x10, 0x90, 0x0B };
const char MagicBytesSDF1[10] = { 0xF5, 0x02, 0x20, 0xFD, 0xF5, 0x01, 0x20, 0xDA, 0xE4, 0xF5 };
const char MagicBytesKDCS[10] = { 0x1A, 0x05, 0x3F, 0x4B, 0x0F, 0xE4, 0x25, 0x1C, 0x5D, 0xF5 };
const char MagicBytesBetop[5] = { 0xF0, 0x0A, 0x60, 0x98, 0x11};
const char MagicBytesLen[4] = { 0x2D, 0x9F, 0x28, 0x07 };
const char MagicBytesLenTOSE[4] = { 0x2D, 0x28, 0x0F, 0xFD };
const char MagicBytesLenDQ6[5] = { 0x2D, 0x9F, 0x28, 0x0F, 0xFD };
const char MagicBytesLenSada[5] = { 0x5C, 0x5C, 0x5C, 0x5C, 0x1C };
const char MagicBytesLenSting[3] = { 0x28, 0x70, 0x9F };
const char MagicBytesLenFH[5] = { 0x28, 0x70, 0x9F, 0x5D, 0xF5 };
const char MagicBytesLenAS[5] = { 0x2D, 0x28, 0xF0, 0x9F, 0xFD };
const unsigned int FHLens[32] = {
	0x0001,0x0002,0x0004,0x0006,0x0008,0x000C,0x0010,0x0018,
	0x0020,0x0030,0x0040,0x0060,0x0080,0x00C0,0x0100,0x0180,
	0x0200,0x0300,0x0400,0x0600,0x0800,0x0C00,0x0003,0x0015,
	0x0024,0x002A,0x002B,0x0055,0x0056,0x00AA,0x00AC,0x01E0
};

const char VCMDLensSBL3[32] = { 0x02, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x02,
0x03, 0x02, 0x02, 0x04, 0x01, 0x02, 0x03, 0x04,
0x02, 0x04, 0x04, 0x01, 0x02, 0x04, 0x01, 0x04,
0x04, 0x04, 0x02, 0x01, 0x02, 0x03, 0x04, 0x01 };

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
	Write8B(&buffer[pos], curVols[volTrack]);
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
	Write8B(&buffer[pos], curVols[volTrack]);
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
	ptrOverride = 0;
	numOverride = 0;
	maxLen = 48000;
	foundTable = 0;
	foundLen = 0;
	qtFix = 0;
	tableFix = 0;
	if (args < 2)
	{
		printf("Usage: SNESNint <spc> (optional tags:) -v = version, -s = song table, -i = index, -c = number of channels\n");
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
				if (!(args % 2))
				{
					for (i = 2; i < args; i += 2)
					{
						argParam = argv[i];
						if (strcmp(argParam, "v") == 0 || strcmp(argParam, "V") == 0)
						{
							drvVers = strtol(argv[i + 1], NULL, 16);
							versOverride = 1;
						}
						else if (strcmp(argParam, "s") == 0 || strcmp(argParam, "S") == 0)
						{
							songTable = strtol(argv[i + 1], NULL, 16);
							ptrOverride = 1;
							foundTable = 1;
						}
						else if (strcmp(argParam, "i") == 0 || strcmp(argParam, "I") == 0)
						{
							songIndex = strtol(argv[i + 1], NULL, 16);
							numOverride = 1;
						}
						else if (strcmp(argParam, "c") == 0 || strcmp(argParam, "C") == 0)
						{
							numChan = strtol(argv[i + 1], NULL, 16);
							chanOverride = 1;
						}
						else if (strcmp(argParam, "l") == 0 || strcmp(argParam, "L") == 0)
						{
							maxLen = strtod(argv[i + 1], &stringPtr);
							if (maxLen > 48000)
							{
								maxLen = 48000;
								printf("Maximum length reduced to 48000 to avoid memory overflow.\n");
							}
						}
						else
						{
							printf("ERROR: Invalid argument!\n");
							exit(1);
						}
					}
				}
				else
				{
					printf("Usage: SNESNint <spc> (optional tags:) -v = version, -s = song table, -i = index, -c = number of channels, -l = maximum length of song\n");
					return -1;
				}
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

				if (ptrOverride != 1)
				{

					/*Harvest Moon*/
					if (drvVers == VER_JAM)
					{
						songTable = ReadLE16(&spcData[0xAC]);
						if (songTable == 0x0000)
						{
							/*Song Master*/
							songTable = ReadLE16(&spcData[0xD9]);
						}
						printf("Found song table at address 0x%04X!\n", songTable);

						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
					}

					if (drvVers == VER_KS)
					{
						/*Super Bonk*/
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesKS, 2)) && foundTable != 1)
							{
								if (spcData[i - 4] == 0xD4 && spcData[i - 6] == 0xDB)
								{
									songTable = ReadLE16(&spcData[spcData[i - 3]]);
									printf("Found song table at address 0x%04X!\n", songTable);
									foundTable = 1;

									if (versOverride != 1)
									{
										drvVers = VER_KS;
									}

									songIndex = 0;
								}
							}
						}
					}

					/*Jimmy Houston's Bass Tournament U.S.A.*/
					if (drvVers == VER_NEXUS)
					{
						songTable = ReadLE16(&spcData[0xC7]);
						printf("Found song table at address 0x%04X!\n", songTable);
						foundTable = 1;
						songIndex = 0;
					}

					/*VS Collection*/
					if (drvVers == VER_SADA)
					{
						songIndex = 0;
						if (ReadLE16(&spcData[0x1C00]) == 0x1C08)
						{
							songTable = 0x1CAB;
						}
						/*SD Ultra Battle*/
						else if (ReadLE16(&spcData[0x2400]) == 0x2408)
						{
							songTable = 0x24AB;
						}
						printf("Found song table at address 0x%04X!\n", songTable);
						foundTable = 1;
					}

					/*Fire Emblem 3*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesIS, 4)) && foundTable != 1 && (drvVers == VER_FE3 || drvVers == VER_FE4 || drvVers == VER_TA))
						{
							songTable = ReadLE16(&spcData[i + 5]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);

							if (numOverride != 1)
							{
								songIndex = 1;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_FE3;
							}

						}
					}

					/*Yoshi's Safari*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesTOSE, 5)) && foundTable != 1)
						{
							if (spcData[i + 6] == 0x3F && spcData[i + 19] != 0x00)
							{
								songTable = (spcData[i + 19] * 0x0100) + spcData[i + 16];
								printf("Found song table at address 0x%04X!\n", songTable);
								if (numOverride != 1)
								{
									songIndex = 0;
								}

								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_YSFR;
								}
							}
						}
					}

					/*Dragon Ball Z: Hyper Dimension*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesTOSE2, 5)) && foundTable != 1)
						{
							if (spcData[i + 6] == 0x3F && spcData[i + 19] != 0x00)
							{
								songTable = (spcData[i + 19] * 0x0100) + spcData[i + 16];
								printf("Found song table at address 0x%04X!\n", songTable);
								if (numOverride != 1)
								{
									songIndex = 0;
								}

								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_SBL3;
								}
							}
						}
					}

					/*Puzzle Bobble*/
					if (drvVers == VER_AISYSTEM)
					{
						songTable = ReadLE16(&spcData[0x14]);
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}

						printf("Song index: %02X\n", songIndex);
						foundTable = 1;

					}

					/*Try to search the bank for song table loader - common variant*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes1, 3)) && foundTable != 1)
						{
							if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 11] == 0x8F)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;
								printf("Found song table at address 0x%04X!\n", songTable);


								if (numOverride != 1)
								{
									if (spcData[i - 2] == 0xC4)
									{
										songIndex = spcData[spcData[i - 1]];
									}
									/*Yoshi's Cookie*/
									else if (spcData[i - 15] == 0xC4)
									{
										songIndex = spcData[spcData[i - 14]];
									}
									/*American Gladiators*/
									else if (spcData[i - 11] == 0xC4)
									{
										songIndex = spcData[spcData[i - 10]];
									}
									/*Super Game Boy*/
									else if (spcData[i - 10] == 0xC4)
									{
										songIndex = spcData[spcData[i - 9]];
										if (songIndex == 0)
										{
											songIndex = spcData[0x00];
											if (songIndex == 0)
											{
												songIndex = 1;
											}
										}
									}
									else
									{
										songIndex = spcData[0x04];
									}
								}

								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_STD;
								}
							}

							/*Yoshi's Island*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 7] == 0x03 && spcData[i + 10] == 0x6F)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;
								printf("Found song table at address 0x%04X!\n", songTable);
								if (numOverride != 1)
								{
									songIndex = spcData[spcData[i + 9]];
								}
								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_STD;
								}
							}

							/*Clock Tower*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 16] == 0xCD)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;
								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_HUMAN;
								}

								if (numOverride != 1)
								{
									songIndex = 0;
								}
							}

							/*AeroFighters*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 13] == 0x8F)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;
								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;

								if (numOverride != 1)
								{
									songIndex = 0;
								}
								printf("Song index: %02X\n", songIndex);
							}

							/*Esparks*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 11] == 0x8D && spcData[i - 5] == 0xC5)
							{

								songTable = ReadLE16(&spcData[i + 3]) - 1;

								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;

								if (numOverride != 1)
								{
									songIndex = 1;
								}
								printf("Song index: %02X\n", songIndex);
							}

							/*Melfand Stories*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 11] == 0x8D && drvVers == VER_MELFAND)
							{

								songTable = ReadLE16(&spcData[i + 18]) - 1;

								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;

								if (numOverride != 1)
								{
									songIndex = 1;
								}
								printf("Song index: %02X\n", songIndex);
							}

							/*Youkai Busters*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && drvVers == VER_MELFAND)
							{
								songTable = ReadLE16(&spcData[i + 16]) - 1;

								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;

								if (numOverride != 1)
								{
									songIndex = 1;
								}
								printf("Song index: %02X\n", songIndex);
							}

							/*F-1 Grand Prix Part 3*/
							else if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && drvVers == VER_F1GP3)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;

								printf("Found song table at address 0x%04X!\n", songTable);
								foundTable = 1;

								if (numOverride != 1)
								{
									songIndex = 0;
								}
								printf("Song index: %02X\n", songIndex);
							}

						}
					}

					/*Super Mario World*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes2, 3)) && foundTable != 1)
						{
							if (spcData[i + 5] == 0xC4 && spcData[i + 6] == 0x40)
							{
								songTable = ReadLE16(&spcData[i + 3]);
								printf("Found song table at address 0x%04X!\n", songTable);
								if (numOverride != 1)
								{
									songIndex = spcData[spcData[i - 4]];
								}
								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_SMW;
								}
							}
							else if (spcData[i + 5] == 0xC4 && spcData[i + 6] == 0x04 && drvVers == VER_SNKK)
							{
								songTable = ReadLE16(&spcData[i + 3]);
								printf("Found song table at address 0x%04X!\n", songTable);
								if (numOverride != 1)
								{
									songIndex = 1;
								}
								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_SNKK;
								}
							}
						}
					}

					/*The Death and Return of Superman*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes3, 3)) && foundTable != 1 && drvVers != VER_JPARK && drvVers != VER_SAI2 && drvVers != VER_MDH && spcData[i + 11] == 0x8F)
						{
							songTable = spcData[i + 3];
							printf("Found song table at address 0x%04X!\n", songTable);


							if (numOverride != 1)
							{
								songIndex = spcData[spcData[i - 2]];
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_SUPERMAN;
							}
						}
					}

					/*The Daze Before Christmas*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes4, 7)) && foundTable != 1)
						{
							songTable = ReadLE16(&spcData[i + 16]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);

							if (numOverride != 1)
							{
								songIndex = spcData[spcData[i + 5]];
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_STD;
							}

						}
					}

					/*Top Gear*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytes5, 3)) && foundTable != 1)
						{
							if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 11] == 0x8F)
							{
								songTable = ReadLE16(&spcData[i + 3]) - 1;
								printf("Found song table at address 0x%04X!\n", songTable);

								if (numOverride != 1)
								{
									songIndex = spcData[spcData[i - 3]];
								}

								printf("Song index: %02X\n", songIndex);
								foundTable = 1;
								if (versOverride != 1)
								{
									drvVers = VER_STD;
								}
							}

						}

					}

					/*Herakles no Eikou 3/4*/
					for (i = 0; i < ramSize; i++)
					{
						if ((!memcmp(&spcData[i], MagicBytesDE, 5)) && foundTable != 1)
						{
							songTable = spcData[i - 25] * 0x100;
							printf("Found song table at address 0x%04X!\n", songTable);

							if (numOverride != 1)
							{
								if (spcData[i - 31] == 0xC4)
								{
									songIndex = spcData[spcData[i - 30]];
								}
								else
								{
									songIndex = 7;
								}
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_DATAEAST;
							}

						}
					}

				}

				/*Astérix & Obélix*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes6, 4)) && foundTable != 1)
					{
						if (spcData[i + 7] == 0xE8 && spcData[i - 2] == 0xC4)
						{
							songTable = ReadLE16(&spcData[i + 13]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);

							if (numOverride != 1)
							{
								songIndex = spcData[spcData[i - 1]];
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_STD;
							}
						}
					}
				}

				/*Gradius 3*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesKon, 4)) && foundTable != 1)
					{
						if (spcData[i + 7] == 0x3F || spcData[i + 9] == 0x3F)
						{
							songTable = ReadLE16(&spcData[i - 12]);
							printf("Found song table at address 0x%04X!\n", songTable);

							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_KONAMI;
							}
						}

					}
				}

				/*Sunset Riders*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesSSR, 3)) && foundTable != 1)
					{
						if (spcData[i + 5] == 0xFD && spcData[i + 9] == 0xDA && spcData[i + 11] == 0x8F)
						{
							songTable = ReadLE16(&spcData[i + 3]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 1;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_SSR;
							}
						}

					}
				}

				/*ActRaiser*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes7, 8)) && foundTable != 1)
					{
						while (ReadLE16(&spcData[i]) != 0x04C4 && i >= 0)
						{
							i--;
						}
						if (spcData[i + 9] == 0xF5)
						{
							songTable = ReadLE16(&spcData[i + 10]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = spcData[0x04];
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_QUINTET;
							}
						}
						/*ActRaiser 2*/
						else if (spcData[i + 12] == 0xE5)
						{
							songTable = ReadLE16(&spcData[i + 13]);
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_QUINTET;
								qtFix = 1;
							}
						}
						else
						{
							/*Workaround for hang*/
							i = ramSize;
						}

					}
				}

				/*Terranigma*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes8, 3)) && foundTable != 1)
					{
						if (spcData[i + 8] == 0x8F)
						{
							songTable = ReadLE16(&spcData[i - 5]);
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_QUINTET;
								qtFix = 1;
							}
						}
					}
				}

				/*Sonic Blast Man*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesTaito, 2)) && foundTable != 1)
					{
						if (spcData[i + 3] == 0x5D)
						{
							songTable = ReadLE16(&spcData[i + 5]) - 1;
							if (drvVers == VER_RC3)
							{
								songTable += 2;
							}
							else if (drvVers == VER_JPARK)
							{
								songTable += 6;
							}
							else if (drvVers == VER_KINGARTHUR)
							{
								songTable = ReadLE16(&spcData[i + 19]) - 1;
								numChan = 6;
							}
							else if (drvVers == VER_VORTEX)
							{
								if (spcData[i + 12] == 0xF5)
								{
									songTable = ReadLE16(&spcData[i + 13]);
									numChan = 6;
								}
								else
								{
									songTable = ReadLE16(&spcData[0x06]);
								}
							}

							if (songTable == 0xE3D1 && ReadLE16(&spcData[songTable + 10]) == 0x0000)
							{
								songTable = 0x1816;
							}
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								if (spcData[i - 5] == 0xC4)
								{
									songIndex = spcData[spcData[i - 4]];
								}
								/*RoboCop 3*/
								else if (spcData[i - 6] == 0xC4 && spcData[i - 1] == 0x1C)
								{
									songIndex = spcData[spcData[i - 5]];
								}
								/*Populous 2*/
								else
								{
									songIndex = spcData[spcData[i - 3]];
								}

							}

							if (drvVers == VER_JPARK)
							{
								songIndex = 1;
							}

							/*Kyuuyaku Megami Tensei*/
							if (drvVers == VER_HECT && numOverride != 1)
							{
								songIndex = 1;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_TAITO;
							}
						}
					}
				}

				/*Space Football*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesArg1, 6)) && foundTable != 1)
					{
						if (spcData[i - 2] == 0x0E)
						{
							songTable = ReadLE16(&spcData[i - 17]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 1;
							}

							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_SFB;
								numChan = 5;
							}
						}
					}
				}

				/*Vortex*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesArg2, 11)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[0x06]);
						printf("Found song table at address 0x%04X!\n", songTable);

						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_VORTEX;
							numChan = 8;
						}
					}
				}

				/*Kindai Mahjong Special*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesOutback, 6)) && foundTable != 1)
					{
						songTable = 0x1700;
						printf("Found song table at address 0x%04X!\n", songTable);

						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_OUTBACK;
						}
					}
				}

				/*Dragon Quest 6*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesDQ6, 3)) && foundTable != 1)
					{
						if (spcData[i - 3] == 0x03 && spcData[i - 2] == 0xD5)
						{
							songTable = ReadLE16(&spcData[i + 3]);
						}
						printf("Found song table at address 0x%04X!\n", songTable);

						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_DQ6;
						}
					}
				}

				/*Ogre Battle*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesOgre, 4)) && foundTable != 1)
					{
						if (spcData[i + 6] == 0x68)
						{
							songTable = ReadLE16(&spcData[i + 30]);

							if (ReadLE16(&spcData[0x1E00]) == 0x0301)
							{
								songTable = 0x1E01;
							}

							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								if (songTable == 0x1E01)
								{
									songIndex = 0;
								}
								else
								{
									songIndex = 1;
								}

							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_OGRE;
							}
						}
					}
				}

				/*Tactics Ogre*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesTO, 5)) && foundTable != 1)
					{
						if (spcData[i + 8] == 0xE5)
						{
							songTable = ReadLE16(&spcData[i + 9]);
							songTable = ReadLE16(&spcData[songTable]);

							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;

							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_TO;
							}
						}
					}
				}

				/*Ys 4*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes9, 4)) && foundTable != 1)
					{
						if (spcData[i - 2] == 0xDA)
						{
							baseValue = spcData[(i + 4)] * 0x100;
							songTable = ReadLE16(&spcData[spcData[i + 8]]) + baseValue;

							if (songTable > 0xF000)
							{
								songTable = 0xD000;
							}

							printf("Found song table at address 0x%04X!\n", songTable);
							songIndex = 0;
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_CUBE;
							}
						}

					}
				}

				/*Okamoto Ayako to Match Play Golf*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesCPBRAiN, 9)) && foundTable != 1)
					{
						if (spcData[i - 1] == 0x10)
						{
							songTable = ReadLE16(&spcData[0x04]);

							printf("Found song table at address 0x%04X!\n", songTable);
							songIndex = 0;
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_CPBRAIN;
							}
						}

					}
				}

				/*Super Valis IV*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesHanari, 9)) && foundTable != 1)
					{
						if (spcData[i + 11] == 0x3F)
						{
							songTable = spcData[i + 15] + (spcData[i + 17] * 0x100);
							printf("Found song table at address 0x%04X!\n", songTable);
							songIndex = 0;
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_HANARI;
							}
						}
					}
				}

				/*Doomsday Warrior*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesDDW, 4)) && foundTable != 1)
					{
						if (spcData[i - 2] == 0xE8)
						{
							songTable = ReadLE16(&spcData[i + 4]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							songIndex = 1;
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_DDW;
							}
						}
					}
				}

				/*Bill Laimbeer's Combat Basketball*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesBLCB, 7)) && foundTable != 1)
					{
						if (spcData[i - 3] == 0xF5)
						{
							songTable = ReadLE16(&spcData[i - 2]);
							printf("Found song table at address 0x%04X!\n", songTable);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_BLCB;
							}
						}
					}
				}

				/*Super Adventure Island 2*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesSAI2, 6)) && foundTable != 1)
					{
						if (spcData[i + 8] == 0xC4)
						{
							songTable = ReadLE16(&spcData[i + 6]);
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_SAI2;
							}
						}
					}
				}

				/*Momotarou Densetsu Happy*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesMDH, 3)) && foundTable != 1 && drvVers == VER_MDH)
					{
						if (spcData[i + 5] == 0xC4)
						{
							songTable = ReadLE16(&spcData[i + 3]);
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_MDH;
							}
						}
					}
				}

				/*Super Tekkyu Fight!*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes10, 4)) && foundTable != 1)
					{
						if (spcData[i + 7] == 0xE5)
						{
							songTable = ReadLE16(&spcData[i + 4]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_STD;
							}
						}
					}
				}

				/*Super Double Dragon*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesSDD, 5)) && foundTable != 1)
					{
						if (spcData[i + 7] == 0x1C)
						{
							songTable = ReadLE16(&spcData[i + 12]) - 1;
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_SDD;
							}
						}
					}
				}

				/*Hatayama Hatch no Pro Yakyuu News! Jitsumei Han*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesMSQ, 8)) && foundTable != 1)
					{
						songTable = 0x2400;
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_MSQ;
						}
					}
				}

				/*Gegege no Kitarou*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes11, 3)) && foundTable != 1)
					{
						if (spcData[i - 3] == 0x5F)
						{
							while (spcData[i] != 0x8F)
							{
								i++;
							}

							songTable = spcData[i + 1] + (spcData[i + 4] * 0x100);
							printf("Found song table at address 0x%04X!\n", songTable);
							if (numOverride != 1)
							{
								songIndex = 0;
							}
							printf("Song index: %02X\n", songIndex);
							foundTable = 1;
							if (versOverride != 1)
							{
								drvVers = VER_STD;
								tableFix = 1;
							}
						}

					}
				}


				/*Nichibutsu Arcade Classics*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes12, 6)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[i + 48]);
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = spcData[0x04];
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_STD;
						}

					}
				}

				/*The 7th Saga*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes7th, 6)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[i + 6]);
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_7TH;
						}

					}
				}

				/*Flying Hero*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesFH, 5)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[i + 5]);
						songTable = ReadLE16(&spcData[songTable]);
						printf("Found song table at address 0x%04X!\n", songTable);
						songIndex = 0;
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_FH;
						}

					}
				}

				/*Treasure Hunter G*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesTHG, 8)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[0x3A00]);
						printf("Found song table at address 0x%04X!\n", songTable);
						songIndex = 0;
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_THG;
						}

					}
				}

				/*SD F-1 Grand Prix*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesSDF1, 10)) && foundLen != 1)
					{
						songTable = ReadLE16(&spcData[i + 1]) - 1;
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_SDF1;
						}

					}
				}

				/*Kiteretsu Daihyakka*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesKDCS, 10)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[i + 10]) - 1;
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_KDCS;
						}

					}
				}

				/*Appleseed*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytes13, 4)) && foundTable != 1)
					{
						songTable = ReadLE16(&spcData[i + 4]);
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 1;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_STD;
						}

					}
				}

				/*Ball Bullet Gun*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesBetop, 5)) && foundTable != 1)
					{
						songTable = spcData[i - 9] + (spcData[i - 6] * 0x100);
						printf("Found song table at address 0x%04X!\n", songTable);
						if (numOverride != 1)
						{
							songIndex = 0;
						}
						printf("Song index: %02X\n", songIndex);
						foundTable = 1;
						if (versOverride != 1)
						{
							drvVers = VER_BETOP;
						}

					}
				}

				if (foundTable == 1)
				{
					songNum = 1;

					if (ptrOverride == 1 && numOverride != 1)
					{
						printf("Song index not specified; using value of 1.\n");
						songIndex = 1;
					}

					/*Try to search the bank for note lengths and velocity*/
					if (drvVers == VER_YSFR || drvVers == VER_SBL3)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenTOSE, 4)) && foundLen != 1)
							{
								lenTab = ReadLE16(&spcData[i + 17]);
								velTab = ReadLE16(&spcData[i + 5]);
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else if (drvVers == VER_DQ6)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenDQ6, 5)) && foundLen != 1)
							{
								lenTab = ReadLE16(&spcData[i + 6]);
								velTab = ReadLE16(&spcData[i + 16]);
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else if (drvVers == VER_OGRE)
					{
						lenTab = 0x176F;
						velTab = 0x1777;
						printf("Found note and velocity tables!\n");
						printf("Note lengths: 0x%04X\n", lenTab);
						printf("Velocity values: 0x%04X\n", velTab);
						foundLen = 1;
					}
					else if (drvVers == VER_TO)
					{
						lenTab = 0x217B;
						velTab = 0x218B;
						printf("Found note and velocity tables!\n");
						printf("Note lengths: 0x%04X\n", lenTab);
						printf("Velocity values: 0x%04X\n", velTab);
						foundLen = 1;
					}
					else if (drvVers == VER_SADA)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenSada, 5)) && foundLen != 1)
							{
								lenTab = i - 8;
								velTab = i - 8;
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else if (drvVers == VER_MSQ)
					{
						lenTab = 0x07E8;
						velTab = 0x07F0;
						printf("Found note and velocity tables!\n");
						printf("Note lengths: 0x%04X\n", lenTab);
						printf("Velocity values: 0x%04X\n", velTab);
						foundLen = 1;
					}
					else if (drvVers == VER_MELFAND)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenSting, 3)) && foundLen != 1)
							{
								lenTab = ReadLE16(&spcData[i + 5]);
								velTab = ReadLE16(&spcData[i + 17]);
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else if (drvVers == VER_FH)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenFH, 5)) && foundLen != 1)
							{
								lenTab = ReadLE16(&spcData[i + 5]);
								velTab = ReadLE16(&spcData[i + 22]);
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else if (drvVers == VER_THG)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLenSting, 3)) && foundLen != 1)
							{
								lenTab = ReadLE16(&spcData[i + 5]);
								velTab = lenTab + 8;
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}
					}
					else
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], MagicBytesLen, 4)) && foundLen != 1)
							{
								if (drvVers == VER_SSR)
								{
									lenTab = ReadLE16(&spcData[i + 14]);
									velTab = ReadLE16(&spcData[i + 24]);
								}
								else if (drvVers == VER_AISYSTEM)
								{
									lenTab = ReadLE16(&spcData[0x5A]);
									velTab = ReadLE16(&spcData[0x5A]) + 8;
								}
								else if (drvVers == VER_CUBE)
								{
									lenTab = ReadLE16(&spcData[i + 6]);
									velTab = ReadLE16(&spcData[i + 15]);
								}
								else
								{
									lenTab = ReadLE16(&spcData[i + 6]);
									velTab = ReadLE16(&spcData[i + 16]);
								}
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
								foundLen = 1;

							}
						}

					}



					if (foundLen != 1)
					{
						if (drvVers != VER_FE4 && drvVers != VER_LEMMINGS && drvVers != VER_RC3 && drvVers != VER_JPARK && drvVers != VER_DQ6 && drvVers != VER_CPBRAIN && drvVers != VER_HANARI && drvVers != VER_DDW && drvVers != VER_7TH)
						{
							for (i = 0; i < ramSize; i++)
							{
								if ((!memcmp(&spcData[i], MagicBytesLenAS, 5)))
								{
									lenTab = ReadLE16(&spcData[i + 6]);
									velTab = ReadLE16(&spcData[i + 16]);
									foundLen = 1;
								}
							}

							if (foundLen == 1)
							{
								printf("Found note and velocity tables!\n");
								printf("Note lengths: 0x%04X\n", lenTab);
								printf("Velocity values: 0x%04X\n", velTab);
							}
							else
							{
								printf("Warning: Note and velocity tables not found!\n");
							}

						}
					}
					if (drvVers == VER_SUPERMAN)
					{
						songPtr = ReadLE16(&spcData[songTable]);
						songPtr = ReadLE16(&spcData[songPtr + ((songIndex - 1) * 2)]);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_HUMAN)
					{
						songNum = 1;
						numSongs = ReadLE16(&spcData[songTable - 2]);
						songIndex = 1;

						while (songNum <= numSongs)
						{
							songPtr = ReadLE16(&spcData[songTable + ((songIndex - 1) * 2)]);
							printf("Song %i address: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
							songIndex++;
							songNum++;
						}
					}
					else if (drvVers == VER_KONAMI)
					{
						songNum = 1;
						songPtr = ReadLE16(&spcData[0x40]);
						if (songPtr == 0x3C02 && ReadLE16(&spcData[0x3C00]) < 0x3C00)
						{
							addVal = 0x0C00;
						}
						else
						{
							addVal = 0x0000;
						}
						songPtr -= 2;
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_QUINTET && qtFix == 1)
					{
						songPtr = ReadLE16(&spcData[songTable]);
						songPtr = ReadLE16(&spcData[songPtr]);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_YSFR)
					{
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], VCMDLensSBL3, 32)))
							{
								drvVers = VER_SBL3;
								break;
							}

						}
						songPtr = ReadLE16(&spcData[songTable + (songIndex * 2)]);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_JAM || drvVers == VER_AISYSTEM || drvVers == VER_KS || drvVers == VER_CUBE || drvVers == VER_CPBRAIN || drvVers == VER_HANARI)
					{
						songPtr = songTable;
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (tableFix == 1)
					{
						songPtr = songTable;
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_DQ6)
					{
						songPtr = (spcData[songTable + songIndex]) + (spcData[songTable + 0x0C + songIndex] * 0x100);
						printf("Song address: 0x%04X\n", songPtr);
						baseValue = ReadLE16(&spcData[songPtr]) + songPtr;
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_BLCB)
					{
						songPtr = ReadLE16(&spcData[0x40]);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else if (drvVers == VER_FH)
					{
						songNum = 1;
						endPtr = ReadLE16(&spcData[songTable]);
						i = songTable + 2;

						while (i < endPtr)
						{
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i address: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
							i += 2;
							songNum++;
						}
					}
					else if (drvVers == VER_THG)
					{
						songNum = 1;
						endPtr = ReadLE16(&spcData[songTable + 2]);
						i = songTable + 2;

						while (i < endPtr)
						{
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i address: 0x%04X\n", songNum, songPtr);
							song2mid(songNum, songPtr);
							i += 2;
							songNum++;
						}
					}
					else if (drvVers == VER_BETOP)
					{
						songPtr = songTable + (songIndex * 17);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
					}
					else
					{
						songPtr = ReadLE16(&spcData[songTable + (songIndex * 2)]);
						printf("Song address: 0x%04X\n", songPtr);
						song2mid(songNum, songPtr);
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
					printf("ERROR: Unable to find song table!\n");
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
	long romPos = 0;
	unsigned int midPos = 0;
	unsigned int midPosM[8];
	int trackCnt = 8;
	int curTrack = 0;
	int curSeq = 0;
	long midTrackBase = 0;
	unsigned int curDelay = 0;
	int curDelays[8];
	int prevDelays[8];
	int midChan = 0;
	int seqEnd = 0;
	int seqsEnd[8];
	int trackEnd = 0;
	int noteTrans = 0;
	int ticks = 120;
	int k = 0;
	long startPos = 0;
	long songPtrs[8];
	unsigned int noteDurs[8];
	unsigned int noteVels[8];

	unsigned int ctrlMidPos = 0;
	long ctrlMidTrackBase = 0;

	int valSize = 0;

	long trackSize = 0;
	long trackSizes[8];
	long ctrlTrackSize = 0;

	unsigned int curNote = 0;
	unsigned int curNotes[8];
	unsigned int prevNotes[8];
	int curNoteLen = 0;
	int curNoteLens[8];
	int lastNote = 0;

	int tempByte = 0;
	long tempPos = 0;

	long tempo = 160;
	float fadeTempo;

	int curInst = 0;
	int curInsts[8];

	int macRepeat = 0;
	long macStart = 0;
	long macEnd = 0;

	int playTimes = 1;

	unsigned long patPos = 0;
	unsigned long seqPos = 0;
	unsigned long patPosM[8];

	unsigned char command[4];
	unsigned char patCommand[4];

	signed int transpose = 0;
	signed int transpose2 = 0;
	signed int transposes[8];

	int firstNote = 1;
	int firstNotes[8];

	int timeVal = 0;

	int holdNote = 0;
	int holdNotes[8];

	long ctrlDelay = 0;
	long ctrlDelays[8];
	long masterDelay = 0;
	long masterDelays[8];

	int patRepeat = 0;
	long patJump = 0;

	int inMacro = 0;
	int inMacroM[8];
	long macro1Pos = 0;
	long macro1Ret = 0;
	long macro1PosM[8];
	long macro1RetM[8];
	int macro1Times = 0;
	int macro1TimesM[8];
	long macro2PosM[8];
	long macro2RetM[8];
	int macro2Times = 0;
	int macro2TimesM[8];
	long macro3PosM[8];
	long macro3RetM[8];
	int macro3Times = 0;
	int macro3TimesM[8];
	long macro4PosM[8];
	long macro4RetM[8];
	int macro4Times = 0;
	int macro4TimesM[8];

	int playTrack = 0;

	int seqPosM[8];

	long chanLens[8];
	int ticksLeft;

	int seqTime;
	long seqsTime[8];

	int percBase;
	int percInsts[8];
	int percNotes[8];

	int minTick;

	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;

	int repeatTimes[8];
	int repeatStart[8];
	int repeatVol[8];
	int repeatTune[8];
	int repeatEnd[8];

	int noPercNotes;

	int jumpPos[8];
	int jumpRet[8];
	int inJump[8];

	int repeats[8][16][4];
	int repeatLevels[8];

	long songBase;
	int multiplier[8];

	if (chanOverride == 1)
	{
		trackCnt = numChan;
	}
	else if (drvVers == VER_SFB)
	{
		trackCnt = 5;
	}
	else if (drvVers == VER_KINGARTHUR)
	{
		trackCnt = 6;
	}
	else if (drvVers == VER_VORTEX && numChan == 6)
	{
		trackCnt = 6;
	}
	else if (drvVers == VER_CPBRAIN)
	{
		trackCnt = 6;
	}
	else if (drvVers == VER_MIHO)
	{
		trackCnt = 7;
	}
	else if (drvVers == VER_SDD)
	{
		trackCnt = 6;
	}
	else
	{
		trackCnt = 8;
	}


	for (curTrack = 0; curTrack < trackCnt; curTrack++)
	{
		midPosM[curTrack] = 0;
	}
	int playNotes[8];
	int tempDelays[8];
	float noteDurVals[8];
	float noteVelVals[8];
	int finalTempo;
	int tempoTicks;
	int tempoDiff;
	float fadeAmt;
	int fading;
	float temp1;
	float temp2;

	int asyncEnd = 0;

	int ISParams[256][4];

	int ISTranspose[7] = { -24, -12, -1, 0, 1, 12, 24 };

	const unsigned char FE3Vals[64] = { 0x00, 0x0C, 0x19, 0x26, 0x33, 0x3F, 0x4C, 0x59,
										0x66, 0x72, 0x75, 0x77, 0x70, 0x7C, 0x7F, 0x82,
										0x84, 0x87, 0x89, 0x8C, 0x8E, 0x91, 0x93, 0x96,
										0x99, 0x9B, 0x9E, 0xA0, 0xA3, 0xA5, 0xA8, 0xAA,
										0xAD, 0xAF, 0xB2, 0xB5, 0xB7, 0xBA, 0xBC, 0xBF,
										0xC1, 0xC4, 0xC6, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3,
										0xD6, 0xD8, 0xDB, 0xDD, 0xE0, 0xE2, 0xE5, 0xE8,
										0xEA, 0xED, 0xEF, 0xF2, 0xF4, 0xF7, 0xF9, 0xFC };
	const unsigned char FE4Vals[64] = { 0x19, 0x26, 0x33, 0x3F, 0x4C, 0x59, 0x66, 0x6D,
										0x70, 0x72, 0x75, 0x77, 0x70, 0x7C, 0x7F, 0x82,
										0x84, 0x87, 0x89, 0x8C, 0x8E, 0x91, 0x93, 0x96,
										0x99, 0x9B, 0x9E, 0xA0, 0xA3, 0xA5, 0xA8, 0xAA,
										0xAD, 0xAF, 0xB2, 0xB5, 0xB7, 0xBA, 0xBC, 0xBF,
										0xC1, 0xC4, 0xC6, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3,
										0xD6, 0xD8, 0xDB, 0xDD, 0xE0, 0xE2, 0xE5, 0xE8,
										0xEA, 0xED, 0xEF, 0xF2, 0xF4, 0xF7, 0xF9, 0xFC };

	int customPerc;
	int customLens;

	ctrlMidPos = 0;

	switchNum = 0;

	noPercNotes = 0;

	midLength = 0x10000;
	midData = (unsigned char*)malloc(midLength);

	ctrlMidData = (unsigned char*)malloc(midLength);

	for (j = 0; j < 8; j++)
	{
		multiMidData[j] = (unsigned char*)malloc(midLength);
	}

	ctrlMidData = (unsigned char*)malloc(midLength);

	for (j = 0; j < midLength; j++)
	{
		for (k = 0; k < 8; k++)
		{
			multiMidData[k][j] = 0;
		}

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

		/*Now retrieve version information...*/

		switch (drvVers)
		{
		case VER_STD:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP2;
			EventMap[0xFC] = EVENT_MUTE;
			EventMap[0xFD] = EVENT_FAST_FORWARD_ON;
			EventMap[0xFE] = EVENT_FAST_FORWARD_OFF;
			break;

		case VER_SMW:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC5;
			STATUS_PERCUSSION_NOTE_MIN = 0xD0;
			STATUS_PERCUSSION_NOTE_MAX = 0xD9;

			EventMap[0xC6] = EVENT_TIE;
			EventMap[0xC7] = EVENT_REST;

			EventMap[0xDA] = EVENT_PROG_CHANGE;
			EventMap[0xDB] = EVENT_PAN;
			EventMap[0xDC] = EVENT_PAN_FADE;
			EventMap[0xDD] = EVENT_PITCH_SLIDE;
			EventMap[0xDE] = EVENT_VIBRATO_ON;
			EventMap[0xDF] = EVENT_VIBRATO_OFF;
			EventMap[0xE0] = EVENT_MASTER_VOLUME;
			EventMap[0xE1] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE2] = EVENT_TEMPO;
			EventMap[0xE3] = EVENT_TEMPO_FADE;
			EventMap[0xE4] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE5] = EVENT_TREMOLO_ON;
			EventMap[0xE6] = EVENT_TREMOLO_OFF;
			EventMap[0xE7] = EVENT_VOLUME;
			EventMap[0xE8] = EVENT_VOLUME_FADE;
			EventMap[0xE9] = EVENT_CALL;
			EventMap[0xEA] = EVENT_VIBRATO_FADE;
			EventMap[0xEB] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xEC] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xED] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xEE] = EVENT_TUNING;
			EventMap[0xEF] = EVENT_ECHO_ON;
			EventMap[0xF0] = EVENT_ECHO_OFF;
			EventMap[0xF1] = EVENT_ECHO_PARAM;
			EventMap[0xF2] = EVENT_ECHO_VOLUME;
			break;

		case VER_BUBSY:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_JUMP;
			EventMap[0xFD] = EVENT_CLD_DISTORT_ON;
			EventMap[0xFE] = EVENT_CLD_DISTORT_OFF;
			EventMap[0xFF] = EVENT_UNKNOWN2;
			asyncEnd = 1;
			break;

		case VER_SUPERMAN:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_JUMP;
			EventMap[0xFD] = EVENT_UNKNOWN2;
			EventMap[0xFE] = EVENT_NOP;
			EventMap[0xFF] = EVENT_UNKNOWN3;
			asyncEnd = 1;
			break;

		case VER_HUMAN:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP2;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_JUMP;
			EventMap[0xFE] = EVENT_UNKNOWN0;
			EventMap[0xFF] = EVENT_UNKNOWN3;
			break;

		case VER_DATAEAST:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			EventMap[0xFF] = EVENT_DE_SUB_COMMAND;
			break;

		case VER_KONAMI:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;
			
			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_KON_PITCH3;
			EventMap[0xE5] = EVENT_KON_REPEAT_START;
			EventMap[0xE6] = EVENT_KON_REPEAT_END;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_NOP;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_KON_ECHO_OFF;
			EventMap[0xF6] = EVENT_KON_ECHO_OFF;
			EventMap[0xF7] = EVENT_KON_ECHO_OFF;
			EventMap[0xF8] = EVENT_KON_ECHO_OFF;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_NOP1;
			EventMap[0xFB] = EVENT_KON_ENVELOPE;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			break;

		case VER_SSR:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_SSR_CALL;
			EventMap[0xFD] = EVENT_SSR_REPEAT;
			EventMap[0xFE] = EVENT_SSR_RETURN;
			break;

		case VER_SFA2:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN2;
			EventMap[0xFC] = EVENT_UNKNOWN2;
			EventMap[0xFD] = EVENT_NOP1;
			EventMap[0xFE] = EVENT_NOP1;
			break;

		case VER_QUINTET:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			EventMap[0xFF] = EVENT_QT_ASDR;
			break;

		case VER_MARV:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_MARV_ASDR;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_PROG_CHANGE;
			EventMap[0xFE] = EVENT_NOP;
			break;

		case VER_FE3:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD5;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xD6] = EVENT_PROG_CHANGE;
			EventMap[0xD7] = EVENT_PAN;
			EventMap[0xD8] = EVENT_PAN_FADE;
			EventMap[0xD9] = EVENT_VIBRATO_ON;
			EventMap[0xDA] = EVENT_VIBRATO_OFF;
			EventMap[0xDB] = EVENT_MASTER_VOLUME;
			EventMap[0xDC] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xDD] = EVENT_TEMPO;
			EventMap[0xDE] = EVENT_TEMPO_FADE;
			EventMap[0xDF] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE0] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE1] = EVENT_TREMOLO_ON;
			EventMap[0xE2] = EVENT_TREMOLO_OFF;
			EventMap[0xE3] = EVENT_VOLUME;
			EventMap[0xE4] = EVENT_VOLUME_FADE;
			EventMap[0xE5] = EVENT_CALL;
			EventMap[0xE6] = EVENT_VIBRATO_FADE;
			EventMap[0xE7] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE8] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE9] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xEA] = EVENT_TUNING;
			EventMap[0xEB] = EVENT_ECHO_ON;
			EventMap[0xFC] = EVENT_ECHO_OFF;
			EventMap[0xED] = EVENT_ECHO_PARAM;
			EventMap[0xEE] = EVENT_ECHO_VOLUME;
			EventMap[0xEF] = EVENT_PITCH_SLIDE;
			EventMap[0xF0] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xF1] = EVENT_IS_ECHO_ON;
			EventMap[0xF2] = EVENT_IS_ECHO_OFF;
			EventMap[0xF3] = EVENT_IS_LEGATO_ON;
			EventMap[0xF4] = EVENT_IS_LEGATO_OFF;
			EventMap[0xF5] = EVENT_IS_CA;
			EventMap[0xF6] = EVENT_IS_APU_WRITE;
			EventMap[0xF7] = EVENT_IS_JUMP_CONDITIONAL;
			EventMap[0xF8] = EVENT_IS_JUMP;
			EventMap[0xF9] = EVENT_IS_EVENT_F9;
			EventMap[0xFA] = EVENT_IS_VOICE_DEF;
			EventMap[0xFB] = EVENT_IS_VOICE_SET;
			EventMap[0xFC] = EVENT_IS_ASDR;
			EventMap[0xFD] = EVENT_IS_DUR_GAIN;
			break;

		case VER_FE4:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD9;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xDA] = EVENT_PROG_CHANGE;
			EventMap[0xDB] = EVENT_PAN;
			EventMap[0xDC] = EVENT_PAN_FADE;
			EventMap[0xDD] = EVENT_VIBRATO_ON;
			EventMap[0xDE] = EVENT_VIBRATO_OFF;
			EventMap[0xDF] = EVENT_MASTER_VOLUME;
			EventMap[0xE0] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE1] = EVENT_TEMPO;
			EventMap[0xE2] = EVENT_TEMPO_FADE;
			EventMap[0xE3] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE4] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE5] = EVENT_TREMOLO_ON;
			EventMap[0xE6] = EVENT_TREMOLO_OFF;
			EventMap[0xE7] = EVENT_VOLUME;
			EventMap[0xE8] = EVENT_VOLUME_FADE;
			EventMap[0xE9] = EVENT_CALL;
			EventMap[0xEA] = EVENT_VIBRATO_FADE;
			EventMap[0xEB] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xEC] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xED] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xEE] = EVENT_TUNING;
			EventMap[0xEF] = EVENT_ECHO_ON;
			EventMap[0xF0] = EVENT_ECHO_OFF;
			EventMap[0xF1] = EVENT_ECHO_PARAM;
			EventMap[0xF2] = EVENT_ECHO_VOLUME;
			EventMap[0xF3] = EVENT_PITCH_SLIDE;
			EventMap[0xF4] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xF5] = EVENT_IS_ECHO_ON;
			EventMap[0xF6] = EVENT_IS_ECHO_OFF;
			EventMap[0xF7] = EVENT_IS_GAIN;
			EventMap[0xF8] = EVENT_IS_GAIN;
			EventMap[0xF9] = EVENT_IS_GAIN_RELEASE;
			EventMap[0xFA] = EVENT_IS_VOICE_DEF;
			EventMap[0xFB] = EVENT_IS_VOICE_SET;
			EventMap[0xFC] = EVENT_IS_EVENT_FC;
			EventMap[0xFD] = EVENT_IS_SUB_COMMAND;
			break;

		case VER_TA:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD9;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xDA] = EVENT_PROG_CHANGE;
			EventMap[0xDB] = EVENT_PAN;
			EventMap[0xDC] = EVENT_PAN_FADE;
			EventMap[0xDD] = EVENT_VIBRATO_ON;
			EventMap[0xDE] = EVENT_VIBRATO_OFF;
			EventMap[0xDF] = EVENT_MASTER_VOLUME;
			EventMap[0xE0] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE1] = EVENT_TEMPO;
			EventMap[0xE2] = EVENT_TEMPO_FADE;
			EventMap[0xE3] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE4] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE5] = EVENT_TREMOLO_ON;
			EventMap[0xE6] = EVENT_TREMOLO_OFF;
			EventMap[0xE7] = EVENT_VOLUME;
			EventMap[0xE8] = EVENT_VOLUME_FADE;
			EventMap[0xE9] = EVENT_CALL;
			EventMap[0xEA] = EVENT_VIBRATO_FADE;
			EventMap[0xEB] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xEC] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xED] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xEE] = EVENT_TUNING;
			EventMap[0xEF] = EVENT_ECHO_ON;
			EventMap[0xF0] = EVENT_ECHO_OFF;
			EventMap[0xF1] = EVENT_ECHO_PARAM;
			EventMap[0xF2] = EVENT_ECHO_VOLUME;
			EventMap[0xF3] = EVENT_PITCH_SLIDE;
			EventMap[0xF4] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xF5] = EVENT_IS_ECHO_ON;
			EventMap[0xF6] = EVENT_IS_ECHO_OFF;
			EventMap[0xF7] = EVENT_IS_ASDR;
			EventMap[0xF8] = EVENT_IS_DUR_GAIN;
			EventMap[0xF9] = EVENT_IS_DUR;
			EventMap[0xFA] = EVENT_IS_VOICE_DEF;
			EventMap[0xFB] = EVENT_IS_VOICE_SET;
			EventMap[0xFC] = EVENT_IS_EVENT_FC;
			EventMap[0xFD] = EVENT_IS_SUB_COMMAND;
			break;

		case VER_SUNSOFT:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_SS_ECHO_ON;
			EventMap[0xFC] = EVENT_SS_ECHO_OFF;
			EventMap[0xFD] = EVENT_SS_ASDR;
			EventMap[0xFE] = EVENT_SS_GLOBAL_SONG_VOLUME;
			break;

		case VER_LEMMINGS:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP1;
			EventMap[0xFC] = EVENT_UNKNOWN0;
			EventMap[0xFD] = EVENT_UNKNOWN0;
			EventMap[0xFE] = EVENT_UNKNOWN0;
			break;

		case VER_YSFR:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_TOSE_APU_WRITE;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			break;

		case VER_SBL3:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_TOSE_NOISE;
			EventMap[0xFD] = EVENT_UNKNOWN2;
			EventMap[0xFE] = EVENT_UNKNOWN3;
			EventMap[0xFF] = EVENT_NOP;
			break;

		case VER_TAITO:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_UNKNOWN0;
			EventMap[0xFD] = EVENT_UNKNOWN0;
			EventMap[0xFE] = EVENT_UNKNOWN0;
			EventMap[0xFF] = EVENT_UNKNOWN1;
			break;

		case VER_RC3:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_ECHO_ON;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN1;
			EventMap[0xFC] = EVENT_PROG_CHANGE;
			EventMap[0xFD] = EVENT_UNKNOWN3;
			EventMap[0xFE] = EVENT_OCEAN_GAIN_ON;
			EventMap[0xFF] = EVENT_OCEAN_GAIN_OFF;
			break;

		case VER_JPARK:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			noPercNotes = 1;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCA] = EVENT_PROG_CHANGE;
			EventMap[0xCB] = EVENT_PAN;
			EventMap[0xCC] = EVENT_PAN_FADE;
			EventMap[0xCD] = EVENT_VIBRATO_ON;
			EventMap[0xCE] = EVENT_VIBRATO_OFF;
			EventMap[0xCF] = EVENT_MASTER_VOLUME;
			EventMap[0xD0] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xD1] = EVENT_TEMPO;
			EventMap[0xD2] = EVENT_TEMPO_FADE;
			EventMap[0xD3] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xD4] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD5] = EVENT_TREMOLO_ON;
			EventMap[0xD6] = EVENT_TREMOLO_OFF;
			EventMap[0xD7] = EVENT_VOLUME;
			EventMap[0xD8] = EVENT_VOLUME_FADE;
			EventMap[0xD9] = EVENT_UNKNOWN1;
			EventMap[0xDA] = EVENT_VIBRATO_FADE;
			EventMap[0xDB] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xDC] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xDD] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xDE] = EVENT_TUNING;
			EventMap[0xDF] = EVENT_ECHO_ON;
			EventMap[0xE0] = EVENT_ECHO_OFF;
			EventMap[0xE1] = EVENT_ECHO_PARAM;
			EventMap[0xE2] = EVENT_ECHO_VOLUME;
			EventMap[0xE3] = EVENT_PITCH_SLIDE;
			EventMap[0xE4] = EVENT_NOP1;
			EventMap[0xE5] = EVENT_NOP;
			EventMap[0xE6] = EVENT_PROG_CHANGE;
			EventMap[0xE7] = EVENT_UNKNOWN4;
			EventMap[0xE8] = EVENT_OCEAN_GAIN_ON;
			EventMap[0xE9] = EVENT_OCEAN_GAIN_OFF;
			EventMap[0xEA] = EVENT_UNKNOWN1;
			EventMap[0xEB] = EVENT_UNKNOWN1;
			EventMap[0xEC] = EVENT_UNKNOWN1;
			EventMap[0xED] = EVENT_UNKNOWN0;
			break;

		case VER_FFS:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN2;
			EventMap[0xFC] = EVENT_MONOLITH_DEC_POS;
			EventMap[0xFD] = EVENT_MONOLITH_SET_INS_PARAMETER;
			EventMap[0xFE] = EVENT_MONOLITH_ECHO_OFF;
			break;

		case VER_SFB:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP1;
			EventMap[0xFC] = EVENT_NOP;
			break;

		case VER_KINGARTHUR:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xCD;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCE] = EVENT_PROG_CHANGE;
			EventMap[0xCF] = EVENT_PAN;
			EventMap[0xD0] = EVENT_PAN_FADE;
			EventMap[0xD1] = EVENT_VIBRATO_ON;
			EventMap[0xD2] = EVENT_VIBRATO_OFF;
			EventMap[0xD3] = EVENT_MASTER_VOLUME;
			EventMap[0xD4] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xD5] = EVENT_TEMPO;
			EventMap[0xD6] = EVENT_TEMPO_FADE;
			EventMap[0xD7] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xD8] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD9] = EVENT_ARGONAUT_ASDR;
			EventMap[0xDA] = EVENT_ARGONAUT_ASDR_ENV;
			EventMap[0xDB] = EVENT_VOLUME;
			EventMap[0xDC] = EVENT_VOLUME_FADE;
			EventMap[0xDD] = EVENT_CALL;
			EventMap[0xDE] = EVENT_VIBRATO_FADE;
			EventMap[0xDF] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE0] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE2] = EVENT_TUNING;
			EventMap[0xE3] = EVENT_ECHO_ON;
			EventMap[0xE4] = EVENT_ECHO_OFF;
			EventMap[0xE5] = EVENT_ECHO_PARAM;
			EventMap[0xE6] = EVENT_ECHO_VOLUME;
			EventMap[0xE7] = EVENT_PITCH_SLIDE;
			EventMap[0xE8] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xE9] = EVENT_UNKNOWN1;
			EventMap[0xEA] = EVENT_UNKNOWN1;
			EventMap[0xEB] = EVENT_ARGONAUT_FIR;
			EventMap[0xEC] = EVENT_UNKNOWN2;
			EventMap[0xED] = EVENT_UNKNOWN0;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			EventMap[0xEF] = EVENT_UNKNOWN2;
			break;

		case VER_VORTEX:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xCD;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCE] = EVENT_PROG_CHANGE;
			EventMap[0xCF] = EVENT_PAN;
			EventMap[0xD0] = EVENT_PAN_FADE;
			EventMap[0xD1] = EVENT_VIBRATO_ON;
			EventMap[0xD2] = EVENT_VIBRATO_OFF;
			EventMap[0xD3] = EVENT_MASTER_VOLUME;
			EventMap[0xD4] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xD5] = EVENT_TEMPO;
			EventMap[0xD6] = EVENT_TEMPO_FADE;
			EventMap[0xD7] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xD8] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD9] = EVENT_ARGONAUT_ASDR;
			EventMap[0xDA] = EVENT_ARGONAUT_ASDR_ENV;
			EventMap[0xDB] = EVENT_VOLUME;
			EventMap[0xDC] = EVENT_VOLUME_FADE;
			EventMap[0xDD] = EVENT_CALL;
			EventMap[0xDE] = EVENT_VIBRATO_FADE;
			EventMap[0xDF] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE0] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE2] = EVENT_TUNING;
			EventMap[0xE3] = EVENT_ECHO_ON;
			EventMap[0xE4] = EVENT_ECHO_OFF;
			EventMap[0xE5] = EVENT_ECHO_PARAM;
			EventMap[0xE6] = EVENT_ECHO_VOLUME;
			EventMap[0xE7] = EVENT_PITCH_SLIDE;
			EventMap[0xE8] = EVENT_UNKNOWN0;
			EventMap[0xE9] = EVENT_ARGONAUT_FIR;
			EventMap[0xEA] = EVENT_UNKNOWN0;
			EventMap[0xEB] = EVENT_UNKNOWN0;
			EventMap[0xEC] = EVENT_UNKNOWN0;
			EventMap[0xED] = EVENT_UNKNOWN1;
			EventMap[0xEE] = EVENT_UNKNOWN1;
			EventMap[0xEF] = EVENT_UNKNOWN3;
			EventMap[0xF0] = EVENT_UNKNOWN0;
			break;

		case VER_JAM:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD7;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xDC] = EVENT_UNKNOWN0;
			EventMap[0xDD] = EVENT_UNKNOWN0;
			EventMap[0xDE] = EVENT_UNKNOWN0;
			EventMap[0xDF] = EVENT_UNKNOWN0;
			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP2;
			EventMap[0xFC] = EVENT_MUTE;
			EventMap[0xFD] = EVENT_FAST_FORWARD_ON;
			EventMap[0xFE] = EVENT_FAST_FORWARD_OFF;
			break;

		case VER_ACC:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDA;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xDB] = EVENT_NOP;
			EventMap[0xDC] = EVENT_ACC_ASDR;
			EventMap[0xDD] = EVENT_ACC_ASDR_ENV;
			EventMap[0xDE] = EVENT_ACC_ASDR_ENV_TABLE_1;
			EventMap[0xDF] = EVENT_ACC_ASDR_ENV_TABLE_2;
			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP1;
			EventMap[0xFC] = EVENT_UNKNOWN0;
			EventMap[0xFD] = EVENT_UNKNOWN0;
			EventMap[0xFE] = EVENT_ACC_ASDR_ENV_CLEAR;
			break;

		case VER_KOEI:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_KOEI_REPEAT_START;
			EventMap[0xFC] = EVENT_KOEI_REPEAT_END;
			EventMap[0xFD] = EVENT_KOEI_REPEAT_JUMP;
			break;

		case VER_AISYSTEM:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xDB;
			STATUS_PERCUSSION_NOTE_MIN = 0xDC;
			STATUS_PERCUSSION_NOTE_MAX = 0xE3;


			EventMap[0xE3] = EVENT_UNKNOWN1;
			EventMap[0xE4] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE5] = EVENT_TEMPO;
			EventMap[0xE6] = EVENT_TEMPO_FADE;
			EventMap[0xE7] = EVENT_MASTER_VOLUME;
			EventMap[0xE8] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE9] = EVENT_REST;
			EventMap[0xEA] = EVENT_TIE;
			EventMap[0xEB] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEC] = EVENT_PAN;
			EventMap[0xED] = EVENT_PAN_FADE;
			EventMap[0xEE] = EVENT_PROG_CHANGE;
			EventMap[0xEF] = EVENT_VIBRATO_ON;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_VIBRATO_OFF;
			EventMap[0xF2] = EVENT_TREMOLO_ON;
			EventMap[0xF3] = EVENT_TREMOLO_OFF;
			EventMap[0xF4] = EVENT_VOLUME;
			EventMap[0xF5] = EVENT_VOLUME_FADE;
			EventMap[0xF6] = EVENT_UNKNOWN3;
			EventMap[0xF7] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF8] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF9] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xFA] = EVENT_ECHO_ON;
			EventMap[0xFB] = EVENT_ECHO_VOLUME;
			EventMap[0xFC] = EVENT_ECHO_PARAM;
			EventMap[0xFD] = EVENT_ECHO_OFF;
			EventMap[0xFE] = EVENT_TUNING;
			EventMap[0xFF] = EVENT_AISYSTEM_RESET_PROG;
			asyncEnd = 2;
			break;

		case VER_KS:
		case VER_NEXUS:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD4;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xD5] = EVENT_PROG_CHANGE;
			EventMap[0xD6] = EVENT_CALL;
			EventMap[0xD7] = EVENT_VOLUME;
			EventMap[0xD8] = EVENT_KS_ASDR;
			EventMap[0xD9] = EVENT_KS_FIR1;
			EventMap[0xDA] = EVENT_KS_FIR2;
			EventMap[0xDB] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xDC] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xDD] = EVENT_KS_LOOP_START;
			EventMap[0xDE] = EVENT_KS_LOOP_END;
			EventMap[0xDF] = EVENT_TEMPO;
			EventMap[0xE0] = EVENT_PITCH_SLIDE;
			EventMap[0xE1] = EVENT_VIBRATO_ON;
			EventMap[0xE2] = EVENT_VIBRATO_OFF;
			EventMap[0xE3] = EVENT_VIBRATO_FADE;
			EventMap[0xE4] = EVENT_MASTER_VOLUME;
			EventMap[0xE5] = EVENT_TUNING;
			EventMap[0xE6] = EVENT_KS_VELOCITY0;
			EventMap[0xE7] = EVENT_KS_VELOCITY1;
			EventMap[0xE8] = EVENT_KS_VELOCITY2;
			EventMap[0xE9] = EVENT_KS_VELOCITY3;
			EventMap[0xEA] = EVENT_KS_VELOCITY4;
			EventMap[0xEB] = EVENT_KS_VELOCITY5;
			EventMap[0xEC] = EVENT_KS_VELOCITY6;
			EventMap[0xED] = EVENT_KS_VELOCITY7;
			EventMap[0xEE] = EVENT_KS_VELOCITY8;
			EventMap[0xEF] = EVENT_KS_VELOCITY9;
			EventMap[0xF0] = EVENT_KS_VELOCITYA;
			EventMap[0xF1] = EVENT_KS_VELOCITYB;
			EventMap[0xF2] = EVENT_KS_VELOCITYC;
			EventMap[0xF3] = EVENT_KS_VELOCITYD;
			EventMap[0xF4] = EVENT_KS_VELOCITYE;
			EventMap[0xF5] = EVENT_KS_VELOCITYF;
			EventMap[0xF6] = EVENT_ECHO_ON;
			EventMap[0xF7] = EVENT_ECHO_OFF;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_KS_ECHO_ON;
			EventMap[0xFA] = EVENT_KS_ECHO_OFF;
			EventMap[0xFB] = EVENT_KS_FIR_FILTER;
			EventMap[0xFC] = EVENT_UNKNOWN0;
			EventMap[0xFE] = EVENT_KS_END;
			asyncEnd = 2;
			break;

		case VER_OUTBACK:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN1;
			EventMap[0xFC] = EVENT_UNKNOWN1;
			break;

		case VER_HECT:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_NOP;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFF] = EVENT_HECT_SUB_COMMAND;
			break;

		case VER_DQ6:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xCF;
			noPercNotes = 1;

			EventMap[0xD0] = EVENT_TIE;
			EventMap[0xD1] = EVENT_REST;

			EventMap[0xD2] = EVENT_HB_SLUR_ON;
			EventMap[0xD3] = EVENT_HB_SLUR_OFF;
			EventMap[0xD4] = EVENT_PROG_CHANGE;
			EventMap[0xD5] = EVENT_NOP;
			EventMap[0xD6] = EVENT_PAN;
			EventMap[0xD7] = EVENT_PAN_FADE;
			EventMap[0xD8] = EVENT_VIBRATO_ON;
			EventMap[0xD9] = EVENT_VIBRATO_FADE;
			EventMap[0xDA] = EVENT_VIBRATO_OFF;
			EventMap[0xDB] = EVENT_MASTER_VOLUME;
			EventMap[0xDC] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xDD] = EVENT_TEMPO;
			EventMap[0xDE] = EVENT_UNKNOWN1;
			EventMap[0xDF] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE0] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE1] = EVENT_TREMOLO_ON;
			EventMap[0xE2] = EVENT_TREMOLO_OFF;
			EventMap[0xE3] = EVENT_VOLUME;
			EventMap[0xE4] = EVENT_VOLUME_FADE;
			EventMap[0xE5] = EVENT_UNKNOWN3;
			EventMap[0xE6] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE7] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE8] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE9] = EVENT_TUNING;
			EventMap[0xEA] = EVENT_ECHO_VOLUME;
			EventMap[0xEB] = EVENT_ECHO_PARAM;
			EventMap[0xEC] = EVENT_UNKNOWN3;
			EventMap[0xED] = EVENT_HB_ECHO_OFF;
			EventMap[0xEE] = EVENT_HB_ECHO_ON;
			EventMap[0xEF] = EVENT_HB_ECHO_FIR;
			EventMap[0xF0] = EVENT_HB_ASDR;
			EventMap[0xF1] = EVENT_HB_DUR_VEL;
			EventMap[0xF2] = EVENT_HB_JUMP;
			EventMap[0xF3] = EVENT_HB_CALL;
			EventMap[0xF4] = EVENT_HB_RETURN;
			EventMap[0xF5] = EVENT_HB_NOISE_ON;
			EventMap[0xF6] = EVENT_HB_NOISE_OFF;
			EventMap[0xF7] = EVENT_HB_NOISE_CLOCK;
			EventMap[0xF8] = EVENT_UNKNOWN0;
			EventMap[0xF9] = EVENT_HB_SUB_COMMAND;
			break;

		case VER_OGRE:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_NOP2;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_NOP2;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_NOP1;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xF5] = EVENT_NOP3;
			EventMap[0xF6] = EVENT_QUEST_APU_WRITE;
			EventMap[0xF7] = EVENT_NOP3;
			EventMap[0xF8] = EVENT_QUEST_APU_WRITE;
			EventMap[0xF9] = EVENT_NOP;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN2;
			EventMap[0xFC] = EVENT_QUEST_APU_WRITE;
			EventMap[0xFD] = EVENT_UNKNOWN4;
			EventMap[0xFE] = EVENT_UNKNOWN2;
			break;

		case VER_TO:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xD7;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xD8] = EVENT_QUEST_PARAMETERS;
			EventMap[0xD9] = EVENT_QUEST_CMD_VOICE;
			EventMap[0xDA] = EVENT_QUEST_JUMP;
			EventMap[0xDB] = EVENT_QUEST_ASDR_GAIN;
			EventMap[0xDC] = EVENT_QUEST_PITCH;
			EventMap[0xDD] = EVENT_QUEST_ASDR;
			EventMap[0xDE] = EVENT_QUEST_LONG_NOTE;
			EventMap[0xDF] = EVENT_UNKNOWN3;
			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_QUEST_REPEAT_START;
			EventMap[0xEC] = EVENT_QUEST_REPEAT_END;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_QUEST_GAIN;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_QUEST_ECHO_START_DELAY;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_QUEST_ECHO_START_DELAY;
			EventMap[0xFD] = EVENT_QUEST_SUB_COMMAND;
			EventMap[0xFE] = EVENT_UNKNOWN2;
			EventMap[0xFF] = EVENT_UNKNOWN1;
			break;

		case VER_CUBE:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			EventMap[0xFF] = EVENT_NOP;
			break;

		case VER_VIRGIN:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xCA;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCB] = EVENT_PROG_CHANGE;
			EventMap[0xCC] = EVENT_VIRGIN_NOTE_LEN;
			EventMap[0xCD] = EVENT_PAN;
			EventMap[0xCE] = EVENT_VIRGIN_PAN_PARAM;
			EventMap[0xCF] = EVENT_PAN_FADE;
			EventMap[0xD0] = EVENT_VIBRATO_OFF;
			EventMap[0xD1] = EVENT_VIBRATO_ON;
			EventMap[0xD2] = EVENT_VIRGIN_VIBRATO_PARAM1;
			EventMap[0xD3] = EVENT_VIRGIN_VIBRATO_PARAM2;
			EventMap[0xD4] = EVENT_VIRGIN_VIBRATO_PARAM3;
			EventMap[0xD5] = EVENT_VIBRATO_FADE;
			EventMap[0xD6] = EVENT_MASTER_VOLUME;
			EventMap[0xD7] = EVENT_VIRGIN_MASTER_VOLUME_PARAM;
			EventMap[0xD8] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xD9] = EVENT_TEMPO;
			EventMap[0xDA] = EVENT_VIRGIN_TEMPO_FADE_PARAM;
			EventMap[0xDB] = EVENT_TEMPO_FADE;
			EventMap[0xDC] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xDD] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xDE] = EVENT_TREMOLO_ON;
			EventMap[0xDF] = EVENT_TREMOLO_OFF;
			EventMap[0xE0] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE2] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE3] = EVENT_VOLUME;
			EventMap[0xE4] = EVENT_VIRGIN_VOLUME_FADE_PARAM;
			EventMap[0xE5] = EVENT_VOLUME_FADE;
			EventMap[0xE6] = EVENT_TUNING;
			EventMap[0xE7] = EVENT_UNKNOWN0;
			EventMap[0xE8] = EVENT_UNKNOWN1;
			EventMap[0xE9] = EVENT_VIRGIN_ECHO_VOLUME_PARAM1;
			EventMap[0xEA] = EVENT_VIRGIN_ECHO_VOLUME_PARAM2;
			EventMap[0xEB] = EVENT_VIRGIN_ECHO_PARAM1;
			EventMap[0xEC] = EVENT_ECHO_OFF;
			EventMap[0xED] = EVENT_VIRGIN_ECHO_PARAM2;
			EventMap[0xEE] = EVENT_UNKNOWN1;
			EventMap[0xEF] = EVENT_UNKNOWN1;
			EventMap[0xF0] = EVENT_UNKNOWN1;
			EventMap[0xF1] = EVENT_UNKNOWN0;
			EventMap[0xF2] = EVENT_UNKNOWN0;
			EventMap[0xF3] = EVENT_UNKNOWN1;
			EventMap[0xF4] = EVENT_UNKNOWN1;
			EventMap[0xF5] = EVENT_UNKNOWN1;
			EventMap[0xF6] = EVENT_UNKNOWN1;
			EventMap[0xF7] = EVENT_PITCH_SLIDE;
			EventMap[0xF8] = EVENT_UNKNOWN1;
			EventMap[0xF9] = EVENT_UNKNOWN1;
			EventMap[0xFA] = EVENT_CALL;
			EventMap[0xFB] = EVENT_UNKNOWN1;
			break;

		case VER_CPBRAIN:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_NOP;
			EventMap[0xE6] = EVENT_NOP;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_NOP;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_CPBRAIN_SUB_COMMAND;
			break;

		case VER_HANARI:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			break;

		case VER_SADA:
			STATUS_END = 0xFF;
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0x54;
			noPercNotes = 1;

			EventMap[0x56] = EVENT_TIE;
			EventMap[0x55] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_SADA_ASDR;
			EventMap[0xEC] = EVENT_SADA_PITCH;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_NOP;
			EventMap[0xEF] = EVENT_NOP;
			EventMap[0xF0] = EVENT_VIBRATO_ON;
			EventMap[0xF1] = EVENT_SADA_ECHO1;
			EventMap[0xF2] = EVENT_SADA_ECHO2;
			EventMap[0xF3] = EVENT_SADA_ECHO3;
			EventMap[0xF4] = EVENT_SADA_QUANTIZATION;
			EventMap[0xF9] = EVENT_SADA_SLUR;
			EventMap[0xFA] = EVENT_UNKNOWN0;
			EventMap[0xFB] = EVENT_SADA_REPEAT_START;
			EventMap[0xFC] = EVENT_JUMP;
			EventMap[0xFE] = EVENT_SADA_REPEAT_END;
			asyncEnd = 2;
			break;

		case VER_DDW:
			STATUS_END = 0xFF;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDD;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xDE] = EVENT_PROG_CHANGE;
			EventMap[0xDF] = EVENT_PAN;
			EventMap[0xE0] = EVENT_PAN_FADE;
			EventMap[0xE1] = EVENT_VIBRATO_ON;
			EventMap[0xE2] = EVENT_VIBRATO_OFF;
			EventMap[0xE3] = EVENT_TREMOLO_ON;
			EventMap[0xE4] = EVENT_TREMOLO_OFF;
			EventMap[0xE5] = EVENT_VOLUME;
			EventMap[0xE6] = EVENT_VOLUME_FADE;
			EventMap[0xE7] = EVENT_CALL;
			EventMap[0xE8] = EVENT_VIBRATO_FADE;
			EventMap[0xE9] = EVENT_PITCH_SLIDE;
			EventMap[0xEA] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xEB] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xEC] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xED] = EVENT_TUNING;
			EventMap[0xEE] = EVENT_MASTER_VOLUME;
			EventMap[0xEF] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xF0] = EVENT_TEMPO;
			EventMap[0xF1] = EVENT_TEMPO_FADE;
			EventMap[0xF2] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xF3] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xF4] = EVENT_ECHO_ON;
			EventMap[0xF5] = EVENT_ECHO_OFF;
			EventMap[0xF6] = EVENT_ECHO_PARAM;
			EventMap[0xF7] = EVENT_ECHO_VOLUME;
			EventMap[0xF8] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xF9] = EVENT_UNKNOWN2;
			EventMap[0xFA] = EVENT_UNKNOWN0;
			EventMap[0xFB] = EVENT_UNKNOWN0;
			EventMap[0xFC] = EVENT_UNKNOWN0;
			EventMap[0xFD] = EVENT_UNKNOWN0;
			break;

		case VER_BLCB:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDD;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;
			EventMap[0xDE] = EVENT_MAKE1_REPEAT_START,
			EventMap[0xDF] = EVENT_MAKE1_REPEAT_END,
			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_MAKE1_CALL;
			EventMap[0xFC] = EVENT_MAKE1_RETURN;
			EventMap[0xFD] = EVENT_MAKE1_SONG_LOOP;
			EventMap[0xFE] = EVENT_MAKE1_SUB_COMMAND;
			asyncEnd = 2;
			break;

		case VER_SAI2:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_NOP;
			EventMap[0xEC] = EVENT_NOP;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_NOP;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_MAKE2_ASDR;
			break;

		case VER_MDH:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_UNKNOWN1;
			EventMap[0xEC] = EVENT_MAKE2_VOLUME_ADD;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_NOP;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_PITCH_SLIDE;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_MAKE2_ASDR;
			EventMap[0xFC] = EVENT_MAKE2_TUNING2;
			EventMap[0xFD] = EVENT_MAKE2_SPEED;
			EventMap[0xFE] = EVENT_MAKE2_SUB_COMMAND;
			EventMap[0xFF] = EVENT_MAKE2_SUB_COMMAND;
			break;

		case VER_MIHO:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_MIHO_CALL;
			EventMap[0xFC] = EVENT_JUMP;
			EventMap[0xFD] = EVENT_MIHO_RETURN;
			EventMap[0xFE] = EVENT_MIHO_REPEAT;
			break;

		case VER_SDD:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE9] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_TREMOLO_ON;
			EventMap[0xEB] = EVENT_TREMOLO_OFF;
			EventMap[0xEC] = EVENT_VOLUME;
			EventMap[0xED] = EVENT_VOLUME_FADE;
			EventMap[0xEE] = EVENT_CALL;
			EventMap[0xEF] = EVENT_VIBRATO_FADE;
			EventMap[0xF0] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF3] = EVENT_TUNING;
			EventMap[0xF4] = EVENT_UNKNOWN3;
			EventMap[0xF5] = EVENT_MAKE2_ASDR;
			EventMap[0xF6] = EVENT_ECHO_ON;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_PITCH_SLIDE;
			break;

		case VER_SNKK:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xBD;
			STATUS_PERCUSSION_NOTE_MIN = 0xC0;
			STATUS_PERCUSSION_NOTE_MAX = 0xCF;

			EventMap[0xBE] = EVENT_TIE;
			EventMap[0xBF] = EVENT_REST;

			EventMap[0xD0] = EVENT_NOP;
			EventMap[0xD1] = EVENT_PROG_CHANGE;
			EventMap[0xD2] = EVENT_PAN;
			EventMap[0xD3] = EVENT_PAN;
			EventMap[0xD4] = EVENT_PAN_FADE;
			EventMap[0xD5] = EVENT_MINT_VIBRATO;
			EventMap[0xD6] = EVENT_MASTER_VOLUME;
			EventMap[0xD7] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xD8] = EVENT_TEMPO;
			EventMap[0xD9] = EVENT_TEMPO_FADE;
			EventMap[0xDA] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xDB] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xDC] = EVENT_TREMOLO_ON;
			EventMap[0xDD] = EVENT_TREMOLO_OFF;
			EventMap[0xDE] = EVENT_VOLUME;
			EventMap[0xDF] = EVENT_VOLUME_FADE;
			EventMap[0xE0] = EVENT_VIBRATO_FADE;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE4] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE5] = EVENT_TUNING;
			EventMap[0xE6] = EVENT_UNKNOWN0;
			EventMap[0xE7] = EVENT_ECHO_OFF;
			EventMap[0xE8] = EVENT_ECHO_PARAM;
			EventMap[0xE9] = EVENT_NOP;
			EventMap[0xEA] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xEB] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xEC] = EVENT_NOP;
			EventMap[0xED] = EVENT_JUMP;
			EventMap[0xEE] = EVENT_MINT_CALL;
			EventMap[0xEF] = EVENT_MINT_REPEAT_START;
			EventMap[0xF0] = EVENT_MINT_REPEAT_END;
			EventMap[0xF1] = EVENT_MINT_NOTE_ADD;
			EventMap[0xF2] = EVENT_MINT_NOTE_ADD_TUNING;
			EventMap[0xF3] = EVENT_UNKNOWN0;
			EventMap[0xF4] = EVENT_UNKNOWN0;
			EventMap[0xF5] = EVENT_UNKNOWN1;
			EventMap[0xF6] = EVENT_UNKNOWN3;
			break;

		case VER_MSQ:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_UNKNOWN1;
			EventMap[0xEC] = EVENT_UNKNOWN0;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_UNKNOWN1;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_UNKNOWN2;
			EventMap[0xF9] = EVENT_UNKNOWN1;
			EventMap[0xFA] = EVENT_UNKNOWN1;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_NOP;
			EventMap[0xFE] = EVENT_NOP;
			EventMap[0xFF] = EVENT_NOP;
			asyncEnd = 2;
			break;

		case VER_7TH:
			STATUS_END = 0xFF;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDE;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xDF] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_MASTER_VOLUME;
			EventMap[0xE2] = EVENT_VOLUME;
			EventMap[0xE3] = EVENT_VOLUME;
			EventMap[0xE4] = EVENT_NOP;
			EventMap[0xE5] = EVENT_PAN;
			EventMap[0xE6] = EVENT_PAN_FADE;
			EventMap[0xE7] = EVENT_TREMOLO_ON;
			EventMap[0xE8] = EVENT_TREMOLO_OFF;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_UNKNOWN1;
			EventMap[0xEC] = EVENT_TUNING;
			EventMap[0xED] = EVENT_VIBRATO_ON;
			EventMap[0xEE] = EVENT_VIBRATO_OFF;
			EventMap[0xEF] = EVENT_PITCH_SLIDE;
			EventMap[0xF0] = EVENT_NOP;
			EventMap[0xF1] = EVENT_NOP;
			EventMap[0xF2] = EVENT_NOP;
			EventMap[0xF3] = EVENT_ECHO_ON;
			EventMap[0xF4] = EVENT_ECHO_OFF;
			EventMap[0xF5] = EVENT_ECHO_PARAM;
			EventMap[0xF6] = EVENT_NOP;
			EventMap[0xF7] = EVENT_TEMPO;
			EventMap[0xF8] = EVENT_TEMPO_FADE;
			EventMap[0xF9] = EVENT_CALL;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN1;
			asyncEnd = 1;
			break;

		case VER_MELFAND:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_UNKNOWN1;
			EventMap[0xE6] = EVENT_STING_QUANTIZATION;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_UNKNOWN2;
			EventMap[0xF0] = EVENT_UNKNOWN0;
			EventMap[0xF1] = EVENT_STING_PITCH_SLIDE;
			EventMap[0xF2] = EVENT_STING_SLUR_ON;
			EventMap[0xF3] = EVENT_STING_SLUR_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_UNKNOWN0;
			EventMap[0xFA] = EVENT_JUMP;
			EventMap[0xFB] = EVENT_STING_CALL;
			EventMap[0xFC] = EVENT_STING_RETURN;
			EventMap[0xFD] = EVENT_STING_REPEAT_START;
			EventMap[0xFE] = EVENT_STING_REPEAT_END;
			EventMap[0xFF] = EVENT_UNKNOWN2;
			break;

		case VER_FH:
			STATUS_END = 0xFF;
			STATUS_NOTE_LEN_MIN = 0x80;
			STATUS_NOTE_LEN_MAX = 0x9F;
			STATUS_NOTE_MIN = 0x00;
			STATUS_NOTE_MAX = 0x7F;
			noPercNotes = 1;

			EventMap[0xFF] = EVENT_TIE;
			EventMap[0x0C] = EVENT_REST;

			EventMap[0xE0] = EVENT_TEMPO;
			EventMap[0xE1] = EVENT_STING_CHAN_MASK;
			EventMap[0xE2] = EVENT_PROG_CHANGE;
			EventMap[0xE3] = EVENT_STING_QUANTIZATION;
			EventMap[0xE4] = EVENT_VOLUME;
			EventMap[0xE5] = EVENT_TUNING;
			EventMap[0xE6] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE7] = EVENT_STING_PITCH_SLIDE;
			EventMap[0xE8] = EVENT_JUMP;
			EventMap[0xE9] = EVENT_STING_CALL;
			EventMap[0xEA] = EVENT_STING_RETURN;
			EventMap[0xEB] = EVENT_STING_REPEAT_START;
			EventMap[0xEC] = EVENT_STING_REPEAT_END;
			EventMap[0xED] = EVENT_UNKNOWN1;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			EventMap[0xEF] = EVENT_UNKNOWN1;
			EventMap[0xF0] = EVENT_PAN;
			EventMap[0xF1] = EVENT_PAN_FADE;
			EventMap[0xF2] = EVENT_STING_SLUR_ON;
			EventMap[0xF3] = EVENT_STING_SLUR_OFF;
			EventMap[0xF4] = EVENT_NOP;
			EventMap[0xF5] = EVENT_NOP;
			EventMap[0xF6] = EVENT_NOP;
			EventMap[0xF7] = EVENT_NOP;
			EventMap[0xF8] = EVENT_NOP;
			EventMap[0xF9] = EVENT_NOP;
			EventMap[0xFA] = EVENT_NOP;
			EventMap[0xFB] = EVENT_NOP;
			EventMap[0xFC] = EVENT_NOP;
			EventMap[0xFD] = EVENT_UNKNOWN0;
			EventMap[0xFE] = EVENT_STING_FADE_OUT;
			EventMap[0xFF] = EVENT_END;
			break;

		case VER_THG:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0xFF;
			STATUS_NOTE_LEN_MAX = 0xFF;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCB;
			STATUS_PERCUSSION_NOTE_MAX = 0xCF;

			EventMap[0xC8] = EVENT_STING_NOTE_LEN;
			EventMap[0xC9] = EVENT_TIE;
			EventMap[0xCA] = EVENT_REST;

			EventMap[0xD0] = EVENT_PROG_CHANGE;
			EventMap[0xD1] = EVENT_MASTER_VOLUME;
			EventMap[0xD2] = EVENT_STING_ASDR1;
			EventMap[0xD3] = EVENT_STING_ASDR2;
			EventMap[0xD4] = EVENT_STING_GAIN;
			EventMap[0xD5] = EVENT_STING_ASDR_GAIN;
			EventMap[0xD6] = EVENT_STING_QUANTIZATION;
			EventMap[0xD7] = EVENT_STING_PITCH_ENV;
			EventMap[0xD8] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xD9] = EVENT_TEMPO;
			EventMap[0xDA] = EVENT_TEMPO_FADE;
			EventMap[0xDB] = EVENT_VOLUME;
			EventMap[0xDC] = EVENT_VOLUME_FADE;
			EventMap[0xDD] = EVENT_TREMOLO_ON;
			EventMap[0xDE] = EVENT_UNKNOWN2;
			EventMap[0xDF] = EVENT_UNKNOWN0;
			EventMap[0xE0] = EVENT_TREMOLO_OFF;
			EventMap[0xE1] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xE2] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xE3] = EVENT_TUNING;
			EventMap[0xE4] = EVENT_VIBRATO_ON;
			EventMap[0xE5] = EVENT_VIBRATO_OFF;
			EventMap[0xE6] = EVENT_STING_PITCH;
			EventMap[0xE7] = EVENT_STING_GET_SLUR;
			EventMap[0xE8] = EVENT_STING_SLUR_ON;
			EventMap[0xE9] = EVENT_STING_SLUR_OFF;
			EventMap[0xEA] = EVENT_STING_ECHO1;
			EventMap[0xEB] = EVENT_STING_ECHO2;
			EventMap[0xEC] = EVENT_STING_FIR;
			EventMap[0xED] = EVENT_STING_ECHO_VOL;
			EventMap[0xEE] = EVENT_STING_ECHO_ON;
			EventMap[0xEF] = EVENT_STING_ECHO_OFF;
			EventMap[0xF0] = EVENT_UNKNOWN0;
			EventMap[0xF1] = EVENT_UNKNOWN2;
			EventMap[0xF2] = EVENT_STING_NOISE_OFF;
			EventMap[0xF3] = EVENT_STING_PMOD;
			EventMap[0xF4] = EVENT_STING_RESET_PITCH;
			EventMap[0xF5] = EVENT_JUMP;
			EventMap[0xF6] = EVENT_STING_CALL;
			EventMap[0xF7] = EVENT_STING_RETURN;
			EventMap[0xF8] = EVENT_STING_REPEAT_START;
			EventMap[0xF9] = EVENT_STING_REPEAT_END;
			break;

		case VER_F1GP3:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			noPercNotes = 1;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCA] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xCB] = EVENT_TEMPO;
			EventMap[0xCC] = EVENT_TEMPO_FADE;
			EventMap[0xCD] = EVENT_MASTER_VOLUME;
			EventMap[0xCE] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xCF] = EVENT_ECHO_ON;
			EventMap[0xD0] = EVENT_ECHO_PARAM;
			EventMap[0xD1] = EVENT_ECHO_VOLUME;
			EventMap[0xD2] = EVENT_ECHO_OFF;
			EventMap[0xD3] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD4] = EVENT_PAN;
			EventMap[0xD5] = EVENT_PAN_FADE;
			EventMap[0xD6] = EVENT_PROG_CHANGE;
			EventMap[0xD7] = EVENT_VIBRATO_ON;
			EventMap[0xD8] = EVENT_VIBRATO_FADE;
			EventMap[0xD9] = EVENT_VIBRATO_OFF;
			EventMap[0xDA] = EVENT_TREMOLO_ON;
			EventMap[0xDB] = EVENT_TREMOLO_OFF;
			EventMap[0xDC] = EVENT_VOLUME;
			EventMap[0xDD] = EVENT_VOLUME_FADE;
			EventMap[0xDE] = EVENT_PITCH_SLIDE;
			EventMap[0xDF] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE0] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE2] = EVENT_TUNING;
			EventMap[0xE3] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xE4] = EVENT_CALL;
			EventMap[0xE5] = EVENT_UNKNOWN0;
			EventMap[0xE6] = EVENT_UNKNOWN0;
			EventMap[0xE7] = EVENT_CALL;
			break;

		case VER_SDF1:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			noPercNotes = 1;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCA] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xCB] = EVENT_TEMPO;
			EventMap[0xCC] = EVENT_TEMPO_FADE;
			EventMap[0xCD] = EVENT_MASTER_VOLUME;
			EventMap[0xCE] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xCF] = EVENT_ECHO_ON;
			EventMap[0xD0] = EVENT_ECHO_PARAM;
			EventMap[0xD1] = EVENT_ECHO_VOLUME;
			EventMap[0xD2] = EVENT_ECHO_OFF;
			EventMap[0xD3] = EVENT_UNKNOWN1;
			EventMap[0xD4] = EVENT_UNKNOWN1;
			EventMap[0xD5] = EVENT_UNKNOWN0;
			EventMap[0xD6] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD7] = EVENT_PAN;
			EventMap[0xD8] = EVENT_PAN_FADE;
			EventMap[0xD9] = EVENT_PROG_CHANGE;
			EventMap[0xDA] = EVENT_VIBRATO_ON;
			EventMap[0xDB] = EVENT_VIBRATO_OFF;
			EventMap[0xDC] = EVENT_VIBRATO_FADE;
			EventMap[0xDD] = EVENT_TREMOLO_ON;
			EventMap[0xDE] = EVENT_TREMOLO_OFF;
			EventMap[0xDF] = EVENT_VOLUME;
			EventMap[0xE0] = EVENT_VOLUME_FADE;
			EventMap[0xE1] = EVENT_PITCH_SLIDE;
			EventMap[0xE2] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE3] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE4] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE5] = EVENT_TUNING;
			EventMap[0xE6] = EVENT_CALL;
			EventMap[0xE7] = EVENT_NOP1;
			EventMap[0xE8] = EVENT_UNKNOWN1;
			EventMap[0xE9] = EVENT_UNKNOWN1;
			EventMap[0xEA] = EVENT_NOP;
			EventMap[0xEB] = EVENT_UNKNOWN1;
			EventMap[0xEC] = EVENT_UNKNOWN1;
			EventMap[0xED] = EVENT_UNKNOWN1;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			EventMap[0xEF] = EVENT_UNKNOWN0;
			EventMap[0xF0] = EVENT_UNKNOWN0;
			break;

		case VER_KDCS:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			noPercNotes = 1;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xCA] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xCB] = EVENT_TEMPO;
			EventMap[0xCC] = EVENT_TEMPO_FADE;
			EventMap[0xCD] = EVENT_MASTER_VOLUME;
			EventMap[0xCE] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xCF] = EVENT_ECHO_ON;
			EventMap[0xD0] = EVENT_ECHO_PARAM;
			EventMap[0xD1] = EVENT_ECHO_VOLUME;
			EventMap[0xD2] = EVENT_ECHO_OFF;
			EventMap[0xD3] = EVENT_UNKNOWN0;
			EventMap[0xD4] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xD5] = EVENT_PAN;
			EventMap[0xD6] = EVENT_PAN_FADE;
			EventMap[0xD7] = EVENT_PROG_CHANGE;
			EventMap[0xD8] = EVENT_VIBRATO_ON;
			EventMap[0xD9] = EVENT_VIBRATO_OFF;
			EventMap[0xDA] = EVENT_VIBRATO_FADE;
			EventMap[0xDB] = EVENT_TREMOLO_ON;
			EventMap[0xDC] = EVENT_TREMOLO_OFF;
			EventMap[0xDD] = EVENT_VOLUME;
			EventMap[0xDE] = EVENT_VOLUME_FADE;
			EventMap[0xDF] = EVENT_PITCH_SLIDE;
			EventMap[0xE0] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xE1] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xE2] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xE3] = EVENT_TUNING;
			EventMap[0xE4] = EVENT_CALL;
			EventMap[0xE5] = EVENT_NOP1;
			EventMap[0xE6] = EVENT_UNKNOWN1;
			EventMap[0xE7] = EVENT_UNKNOWN1;
			EventMap[0xE8] = EVENT_NOP;
			EventMap[0xE9] = EVENT_UNKNOWN1;
			EventMap[0xEA] = EVENT_UNKNOWN1;
			EventMap[0xEB] = EVENT_UNKNOWN0;
			EventMap[0xEC] = EVENT_UNKNOWN0;
			EventMap[0xED] = EVENT_UNKNOWN0;
			EventMap[0xEE] = EVENT_UNKNOWN0;
			break;

		case VER_BETOP:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x63;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0x64] = EVENT_BETOP_JUMP;
			EventMap[0x65] = EVENT_BETOP_CALL;
			EventMap[0x66] = EVENT_BETOP_RETURN;
			EventMap[0x67] = EVENT_BETOP_REPEAT_START;
			EventMap[0x68] = EVENT_BETOP_REPEAT_END;
			EventMap[0x69] = EVENT_UNKNOWN0;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_UNKNOWN1;
			asyncEnd = 2;
			break;

		default:
			STATUS_END = 0x00;
			STATUS_NOTE_LEN_MIN = 0x01;
			STATUS_NOTE_LEN_MAX = 0x7F;
			STATUS_NOTE_MIN = 0x80;
			STATUS_NOTE_MAX = 0xC7;
			STATUS_PERCUSSION_NOTE_MIN = 0xCA;
			STATUS_PERCUSSION_NOTE_MAX = 0xDF;

			EventMap[0xC8] = EVENT_TIE;
			EventMap[0xC9] = EVENT_REST;

			EventMap[0xE0] = EVENT_PROG_CHANGE;
			EventMap[0xE1] = EVENT_PAN;
			EventMap[0xE2] = EVENT_PAN_FADE;
			EventMap[0xE3] = EVENT_VIBRATO_ON;
			EventMap[0xE4] = EVENT_VIBRATO_OFF;
			EventMap[0xE5] = EVENT_MASTER_VOLUME;
			EventMap[0xE6] = EVENT_MASTER_VOLUME_FADE;
			EventMap[0xE7] = EVENT_TEMPO;
			EventMap[0xE8] = EVENT_TEMPO_FADE;
			EventMap[0xE9] = EVENT_GLOBAL_TRANSPOSE;
			EventMap[0xEA] = EVENT_LOCAL_TRANSPOSE;
			EventMap[0xEB] = EVENT_TREMOLO_ON;
			EventMap[0xEC] = EVENT_TREMOLO_OFF;
			EventMap[0xED] = EVENT_VOLUME;
			EventMap[0xEE] = EVENT_VOLUME_FADE;
			EventMap[0xEF] = EVENT_CALL;
			EventMap[0xF0] = EVENT_VIBRATO_FADE;
			EventMap[0xF1] = EVENT_PITCH_ENVELOPE_TO;
			EventMap[0xF2] = EVENT_PITCH_ENVELOPE_FROM;
			EventMap[0xF3] = EVENT_PITCH_ENVELOPE_OFF;
			EventMap[0xF4] = EVENT_TUNING;
			EventMap[0xF5] = EVENT_ECHO_ON;
			EventMap[0xF6] = EVENT_ECHO_OFF;
			EventMap[0xF7] = EVENT_ECHO_PARAM;
			EventMap[0xF8] = EVENT_ECHO_VOLUME;
			EventMap[0xF9] = EVENT_PITCH_SLIDE;
			EventMap[0xFA] = EVENT_PERCUSSION_PATCH_BASE;
			EventMap[0xFB] = EVENT_NOP2;
			EventMap[0xFC] = EVENT_MUTE;
			EventMap[0xFD] = EVENT_FAST_FORWARD_ON;
			EventMap[0xFE] = EVENT_FAST_FORWARD_OFF;
			break;
		}

		trackEnd = 0;
		seqEnd = 0;
		patRepeat = -1;
		playTrack = 0;
		transpose2 = 0;
		ctrlDelay = 0;
		fading = 0;
		ticksLeft = 0;
		percBase = 0;
		customPerc = 0;

		if (drvVers == VER_FE4)
		{
			customLens = 1;
		}
		else
		{
			customLens = 0;
		}


		if (drvVers == VER_FE3 || drvVers == VER_FE4 || drvVers == VER_TA)
		{
			for (j = 0; j < 256; j++)
			{
				ISParams[j][0] = 0;
				ISParams[j][1] = 127;
				ISParams[j][2] = 0;
				ISParams[j][3] = 0;
			}
		}

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			midPosM[curTrack] = 0;
			transposes[curTrack] = 0;
			curDelays[curTrack] = 0;
			ctrlDelays[curTrack] = 0;
			masterDelays[curTrack] = 0;
			firstNotes[curTrack] = 1;
			holdNotes[curTrack] = 0;
			curVols[curTrack] = 127;
			curVolVels[curTrack] = 256;
			noteDurs[curTrack] = 256;
			playNotes[curTrack] = 0;
			percNotes[curTrack] = 0;
			repeatStart[curTrack] = 0;
			repeatTimes[curTrack] = -1;
			repeatVol[curTrack] = 0;
			repeatTune[curTrack] = 0;
			repeatEnd[curTrack] = 0;
			multiplier[curTrack] = 1;
			if (drvVers == VER_RC3 || drvVers == VER_JPARK)
			{
				patPosM[curTrack] = 0x0000;
			}

			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&multiMidData[curTrack][midPosM[curTrack]], 0x4D54726B);
			midPosM[curTrack] += 8;
			curNoteLens[curTrack] = 0;
			curDelays[curTrack] = 0;
			seqEnd = 0;
			midTrackBase = midPosM[curTrack];
			curInsts[curTrack] = 0;

			/*Calculate MIDI channel size*/
			trackSizes[curTrack] = midPosM[curTrack] - midTrackBase;
			WriteBE16(&multiMidData[curTrack][midTrackBase - 2], trackSizes[curTrack]);
		}
		patPos = songPtr;
		seqTime = 0;

		if (trackCnt < 8)
		{
			for (curTrack = trackCnt; curTrack < 8; curTrack++)
			{
				songPtrs[curTrack] = 0x0000;
				seqsEnd[curTrack] = 1;
			}
		}

		while (trackEnd == 0)
		{
			patCommand[0] = spcData[patPos];
			patCommand[1] = spcData[patPos + 1];
			patCommand[2] = spcData[patPos + 2];
			patCommand[3] = spcData[patPos + 3];

			if (drvVers == VER_YSFR || drvVers == VER_SBL3)
			{
				/*End of track*/
				if (patCommand[0] == 0x00 && patCommand[1] == 0x00)
				{
					playTrack = 0;
					trackEnd = 1;
				}

				/*Play phrase multiple times*/
				else if (patCommand[1] == 0x00 && patCommand[0] >= 0x01 && patCommand[0] <= 0xFE)
				{
					if (patRepeat == -1)
					{
						patRepeat = patCommand[0];
						patJump = ReadLE16(&spcData[patPos + 2]);
						playTrack = 0;
					}
					else if (patRepeat > 0)
					{
						patRepeat--;
						patPos = patJump;
						playTrack = 0;
					}
					else
					{
						patRepeat = -1;
						playTrack = 0;
						patPos += 4;
					}
				}

				/*Always jump*/
				else if (patCommand[0] == 0xFF)
				{
					trackEnd = 1;
					playTrack = 0;
				}

				/*Play phrase*/
				else
				{
					playTrack = 1;
					curSeq = ReadLE16(&spcData[patPos]);
					if (drvVers == VER_KONAMI)
					{
						curSeq += addVal;
					}
					romPos = curSeq;
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
						if (drvVers == VER_KONAMI && songPtrs[curTrack] >= 0x0100)
						{
							songPtrs[curTrack] += addVal;
						}
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]);
						if (drvVers == VER_KONAMI && seqPosM[curTrack] >= 0x0100)
						{
							seqPosM[curTrack] += addVal;
						}
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;
						if (songPtrs[curTrack] < 0x0100)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
					patPos += 2;

				}
			}
			else if (drvVers == VER_RC3 || drvVers == VER_JPARK)
			{
				romPos = songPtr;
				for (curTrack = 0; curTrack < 8; curTrack++)
				{
					songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
					if (songPtrs[curTrack] < 0x0100)
					{
						seqsEnd[curTrack] = 1;
					}
					else
					{
						seqsEnd[curTrack] = 0;

						patPosM[curTrack] = songPtrs[curTrack];
						seqPosM[curTrack] = ReadLE16(&spcData[patPosM[curTrack]]);
					}
					romPos += 2;
				}
				playTrack = 1;
			}
			else if (drvVers == VER_SNKK)
			{
				playTrack = 1;
				curSeq = songPtr + 1;
				romPos = curSeq;
				if (seqEnd == 1)
				{
					trackEnd = 1;
				}
				else
				{
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = 0;
						seqPosM[curTrack] = 0;
					}
					while (spcData[romPos] != 0xFF)
					{
						curTrack = spcData[romPos];
						if (curTrack >= trackCnt)
						{
							curTrack = 0;
						}
						songPtrs[curTrack] = ReadLE16(&spcData[romPos + 1]) + romPos + 1;
						seqPosM[curTrack] = ReadLE16(&spcData[romPos + 1]) + romPos + 1;
						seqsTime[curTrack] = 0;
						romPos += 3;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;

						for (j = 0; j < 16; j++)
						{
							repeats[curTrack][0][0] = -1;
							repeats[curTrack][0][1] = 0;
							repeats[curTrack][0][2] = 0;
							repeats[curTrack][0][3] = 0;
						}
						repeatLevels[curTrack] = 0;
					}
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						if (songPtrs[curTrack] == 0)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
				}
			}
			else if (drvVers == VER_JAM)
			{
				romPos = songPtr;

				/*End of track*/
				if (patCommand[0] == 0x00 && patCommand[1] == 0x00)
				{
					playTrack = 0;
					trackEnd = 1;
				}

				/*Song loop?*/
				else if (patCommand[0] == 0xFF)
				{
					playTrack = 0;
					trackEnd = 1;
				}

				else if (patCommand[0] == 0x00)
				{
					playTrack = 0;
					patPos += 2;
				}

				/*Play phrase*/
				else
				{
					playTrack = 1;
					curSeq = ReadLE16(&spcData[patPos]) + songPtr;
					romPos = curSeq;
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]) + songPtr;
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]) + songPtr;
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;
						if (songPtrs[curTrack] == songPtr)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
					patPos += 2;
				}
			}
			else if (drvVers == VER_KS || drvVers == VER_NEXUS || drvVers == VER_CPBRAIN || drvVers == VER_SADA)
			{
				playTrack = 1;
				curSeq = songTable;
				romPos = curSeq;
				if (seqEnd == 1)
				{
					trackEnd = 1;
				}
				else
				{
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]);
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;

						if (drvVers == VER_CPBRAIN && (romPos - 2) >= songPtrs[0] && curTrack > 0)
						{
							songPtrs[curTrack] = 0x0000;
							seqPosM[curTrack] = 0x0000;
						}
						for (j = 0; j < 16; j++)
						{
							repeats[curTrack][0][0] = -1;
							repeats[curTrack][0][1] = 0;
							repeats[curTrack][0][2] = 0;
							repeats[curTrack][0][3] = 0;
						}
						repeatLevels[curTrack] = 0;

						if (songPtrs[curTrack] == 0)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
				}

			}
			else if (drvVers == VER_DQ6)
			{
				playTrack = 1;
				romPos = songPtr + 2;
				if (seqEnd == 1)
				{
					trackEnd = 1;
				}
				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					songPtrs[curTrack] = ReadLE16(&spcData[romPos]) + songPtr;
					seqPosM[curTrack] = ReadLE16(&spcData[romPos]) + songPtr;
					seqsTime[curTrack] = 0;
					romPos += 2;
					seqsEnd[curTrack] = 0;
					inMacroM[curTrack] = 0;
					minTick = 0;
					repeatTimes[curTrack] = -1;
					inJump[curTrack] = 0;
					if (songPtrs[curTrack] == songPtr || romPos >= baseValue)
					{
						songPtrs[curTrack] = 0x0000;
						seqPosM[curTrack] = 0x0000;
						seqsEnd[curTrack] = 1;
					}
				}
				seqEnd = 0;
			}
			else if (drvVers == VER_BLCB)
			{
				playTrack = 1;
				romPos = songPtr + 1;
				if (seqEnd == 1)
				{
					trackEnd = 1;
				}
				else
				{
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]);
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;

						for (j = 0; j < 16; j++)
						{
							repeats[curTrack][0][0] = -1;
							repeats[curTrack][0][1] = 0;
							repeats[curTrack][0][2] = 0;
							repeats[curTrack][0][3] = 0;
						}
						repeatLevels[curTrack] = 0;

						if (songPtrs[curTrack] == 0)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
				}
			}
			else if (drvVers == VER_MELFAND)
			{
				playTrack = 1;
				romPos = songPtr;
				asyncEnd = 2;

				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					songPtrs[curTrack] = 0;
					seqPosM[curTrack] = 0;
				}
				while (spcData[romPos] != 0x00)
				{
					switch (spcData[romPos + 1])
					{
					case 0x01:
						curTrack = 0;
						break;
					case 0x02:
						curTrack = 1;
						break;
					case 0x04:
						curTrack = 2;
						break;
					case 0x08:
						curTrack = 3;
						break;
					case 0x10:
						curTrack = 4;
						break;
					case 0x20:
						curTrack = 5;
						break;
					case 0x40:
						curTrack = 6;
						break;
					case 0x80:
						curTrack = 7;
						break;
					default:
						curTrack = 0;
						break;
					}
					songPtrs[curTrack] = ReadLE16(&spcData[romPos + 2]);
					seqPosM[curTrack] = ReadLE16(&spcData[romPos + 2]);
					seqsTime[curTrack] = 0;
					romPos += 4;
					seqsEnd[curTrack] = 0;
					inMacroM[curTrack] = 0;
					minTick = 0;
					repeatTimes[curTrack] = -1;
					inJump[curTrack] = 0;

					for (j = 0; j < 16; j++)
					{
						repeats[curTrack][0][0] = -1;
						repeats[curTrack][0][1] = 0;
						repeats[curTrack][0][2] = 0;
						repeats[curTrack][0][3] = 0;
					}

					repeatLevels[curTrack] = 0;

				}
				
				for (curTrack = 0; curTrack < 8; curTrack++)
				{
					if (songPtrs[curTrack] == 0)
					{
						seqsEnd[curTrack] = 1;
					}
				}
			}
			else if (drvVers == VER_FH)
			{
				playTrack = 1;
				romPos = songPtr;
				asyncEnd = 2;

				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					songPtrs[curTrack] = 0;
					seqPosM[curTrack] = 0;
				}
				while (spcData[romPos] != 0xFF)
				{
					curTrack = spcData[romPos];
					if (curTrack > 7)
					{
						curTrack = 0;
					}
					songPtrs[curTrack] = ReadLE16(&spcData[romPos + 1]);
					seqPosM[curTrack] = ReadLE16(&spcData[romPos + 1]);
					seqsTime[curTrack] = 0;
					romPos += 3;
					seqsEnd[curTrack] = 0;
					inMacroM[curTrack] = 0;
					minTick = 0;
					repeatTimes[curTrack] = -1;
					inJump[curTrack] = 0;
					transposes[curTrack] = 0;

					for (j = 0; j < 16; j++)
					{
						repeats[curTrack][0][0] = -1;
						repeats[curTrack][0][1] = 0;
						repeats[curTrack][0][2] = 0;
						repeats[curTrack][0][3] = 0;
					}

					repeatLevels[curTrack] = 0;

				}

				for (curTrack = 0; curTrack < 8; curTrack++)
				{
					if (songPtrs[curTrack] == 0)
					{
						seqsEnd[curTrack] = 1;
					}
				}
			}
			else if (drvVers == VER_THG)
			{
				playTrack = 1;
				romPos = songPtr;
				asyncEnd = 2;

				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					songPtrs[curTrack] = 0;
					seqPosM[curTrack] = 0;
				}
				while (ReadLE16(&spcData[romPos]) != 0x0000)
				{
					switch (spcData[romPos + 1])
					{
					case 0x01:
						curTrack = 0;
						break;
					case 0x02:
						curTrack = 1;
						break;
					case 0x04:
						curTrack = 2;
						break;
					case 0x08:
						curTrack = 3;
						break;
					case 0x10:
						curTrack = 4;
						break;
					case 0x20:
						curTrack = 5;
						break;
					case 0x40:
						curTrack = 6;
						break;
					case 0x80:
						curTrack = 7;
						break;
					default:
						curTrack = 0;
						break;
					}
					songPtrs[curTrack] = ReadLE16(&spcData[romPos + 2]);
					seqPosM[curTrack] = ReadLE16(&spcData[romPos + 2]);
					seqsTime[curTrack] = 0;
					romPos += 4;
					seqsEnd[curTrack] = 0;
					inMacroM[curTrack] = 0;
					minTick = 0;
					repeatTimes[curTrack] = -1;
					inJump[curTrack] = 0;

					for (j = 0; j < 16; j++)
					{
						repeats[curTrack][0][0] = -1;
						repeats[curTrack][0][1] = 0;
						repeats[curTrack][0][2] = 0;
						repeats[curTrack][0][3] = 0;
					}

					repeatLevels[curTrack] = 0;

				}

				for (curTrack = 0; curTrack < 8; curTrack++)
				{
					if (songPtrs[curTrack] == 0)
					{
						seqsEnd[curTrack] = 1;
					}
				}
			}
			else if (drvVers == VER_BETOP)
			{
				playTrack = 1;
				curSeq = songPtr + 1;
				romPos = curSeq;
				if (seqEnd == 1)
				{
					trackEnd = 1;
				}
				else
				{
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]);
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;
						for (j = 0; j < 16; j++)
						{
							repeats[curTrack][0][0] = -1;
							repeats[curTrack][0][1] = 0;
							repeats[curTrack][0][2] = 0;
							repeats[curTrack][0][3] = 0;
						}
						repeatLevels[curTrack] = 0;

						if (songPtrs[curTrack] == 0)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
				}

				}
			else
			{
				/*End of track*/
				if (patCommand[0] == 0x00 && patCommand[1] == 0x00)
				{
					playTrack = 0;
					trackEnd = 1;
				}

				/*Play phrase multiple times*/
				else if (patCommand[0] >= 0x01 && patCommand[0] <= 0x7F && patCommand[1] == 0x00)
				{
					if (patRepeat == -1)
					{
						patRepeat = patCommand[0];
						patJump = ReadLE16(&spcData[patPos + 2]);
						playTrack = 0;
					}
					else if (patRepeat > 0)
					{
						patRepeat--;
						patPos = patJump;
						playTrack = 0;
					}
					else
					{
						patRepeat = -1;
						playTrack = 0;
						patPos += 4;
					}
				}

				/*Fast forward on*/
				else if (patCommand[0] == 0x80 && patCommand[1] == 0x00)
				{
					patPos += 2;
					playTrack = 0;
				}

				/*Fast forward off*/
				else if (patCommand[0] == 0x81 && patCommand[1] == 0x00)
				{
					patPos += 2;
					playTrack = 0;
				}

				/*Always jump*/
				else if (patCommand[0] >= 0x82 && patCommand[1] == 0x00)
				{
					trackEnd = 1;
					playTrack = 0;
				}

				/*Play phrase*/
				else
				{
					playTrack = 1;
					curSeq = ReadLE16(&spcData[patPos]);
					if (drvVers == VER_KONAMI)
					{
						curSeq += addVal;
					}
					else if (drvVers == VER_CUBE)
					{
						curSeq -= baseValue;
						curSeq += songPtr;
					}
					if (drvVers == VER_VIRGIN)
					{
						numChan = spcData[patPos + 2];
					}
					romPos = curSeq;
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						songPtrs[curTrack] = ReadLE16(&spcData[romPos]);
						if (drvVers == VER_KONAMI && songPtrs[curTrack] >= 0x0100)
						{
							songPtrs[curTrack] += addVal;
						}
						else if (drvVers == VER_CUBE && seqPosM[curTrack] >= 0x0100)
						{
							songPtrs[curTrack] -= baseValue;
							songPtrs[curTrack] += songPtr;
						}
						else if (drvVers == VER_VIRGIN && curTrack >= numChan)
						{
							songPtrs[curTrack] = 0x0000;
						}
						seqPosM[curTrack] = ReadLE16(&spcData[romPos]);
						if (drvVers == VER_KONAMI && seqPosM[curTrack] >= 0x0100)
						{
							seqPosM[curTrack] += addVal;
						}
						else if (drvVers == VER_CUBE && seqPosM[curTrack] >= 0x0100)
						{
							seqPosM[curTrack] -= baseValue;
							seqPosM[curTrack] += songPtr;
						}
						else if (drvVers == VER_VIRGIN && curTrack >= numChan)
						{
							seqPosM[curTrack] = 0x0000;
						}
						seqsTime[curTrack] = 0;
						romPos += 2;
						seqsEnd[curTrack] = 0;
						inMacroM[curTrack] = 0;
						minTick = 0;
						repeatTimes[curTrack] = -1;
						inJump[curTrack] = 0;
						if (songPtrs[curTrack] < 0x0100)
						{
							seqsEnd[curTrack] = 1;
						}
					}
					seqEnd = 0;
					patPos += 2;
					if (drvVers == VER_VIRGIN)
					{
						patPos++;
					}
				}
			}

			if (playTrack == 1)
			{
				while (seqEnd == 0)
				{
					if (seqsEnd[0] == 1 && seqsEnd[1] == 1 && seqsEnd[2] == 1 && seqsEnd[3] == 1 && seqsEnd[4] == 1 && seqsEnd[5] == 1 && seqsEnd[6] == 1 && seqsEnd[7] == 1)
					{
						seqEnd = 1;
						if (drvVers == VER_RC3 || drvVers == VER_JPARK || drvVers == VER_KS || drvVers == VER_NEXUS || drvVers == VER_DQ6 || drvVers == VER_CPBRAIN || drvVers == VER_SADA || drvVers == VER_SNKK || drvVers == VER_MELFAND || drvVers == VER_FH || drvVers == VER_THG)
						{
							trackEnd = 1;
						}
					}
					for (curTrack = 0; curTrack < trackCnt; curTrack++)
					{
						if (songPtrs[curTrack] >= 0x0100 && seqEnd == 0)
						{
							while (seqTime >= masterDelays[curTrack] && seqsEnd[curTrack] == 0)
							{
								if (midPosM[curTrack] >= maxLen)
								{
									seqEnd = 1;
									break;
								}

								command[0] = spcData[seqPosM[curTrack]];
								command[1] = spcData[seqPosM[curTrack] + 1];
								command[2] = spcData[seqPosM[curTrack] + 2];
								command[3] = spcData[seqPosM[curTrack] + 3];

								if (command[0] == STATUS_END)
								{
									if (drvVers == VER_RC3 || drvVers == VER_JPARK)
									{
										patPosM[curTrack] += 2;
										/*End of song*/
										if (spcData[(patPosM[curTrack] + 1)] == 0x00)
										{
											seqEnd = 1;
											seqTime = masterDelays[curTrack];
											trackEnd = 1;
											break;
										}
										/*Restart all patterns*/
										else if (spcData[(patPosM[curTrack] + 1)] == 0xFF)
										{
											seqEnd = 1;
											seqTime = masterDelays[curTrack];
											trackEnd = 1;
											break;
										}
										/*Jump to pattern loop*/
										else if (spcData[(patPosM[curTrack] + 1)] == 0x01)
										{
											seqsEnd[curTrack] = 1;
										}
										/*Play phrase*/
										else
										{
											seqPosM[curTrack] = ReadLE16(&spcData[patPosM[curTrack]]);
										}

									}
									else
									{
										if (inMacroM[curTrack] == 0)
										{
											if ((asyncEnd == 1 && curTrack != 0) || asyncEnd == 2)
											{
												seqsEnd[curTrack] = 1;
											}
											else
											{
												seqEnd = 1;

												seqTime = masterDelays[curTrack];
												break;
											}

										}
										else
										{
											if (drvVers == VER_TO || drvVers == VER_SNKK)
											{
												if (inMacroM[curTrack] == 1)
												{
													if (macro1TimesM[curTrack] > 1)
													{
														seqPosM[curTrack] = macro1PosM[curTrack];
														macro1TimesM[curTrack]--;
													}
													else
													{
														inMacroM[curTrack] = 0;
														seqPosM[curTrack] = macro1RetM[curTrack];
														macro1TimesM[curTrack] = -1;
													}
												}
												else if (inMacroM[curTrack] == 2)
												{
													if (macro2TimesM[curTrack] > 1)
													{
														seqPosM[curTrack] = macro2PosM[curTrack];
														macro2TimesM[curTrack]--;
													}
													else
													{
														inMacroM[curTrack] = 1;
														seqPosM[curTrack] = macro2RetM[curTrack];
														macro2TimesM[curTrack] = -1;
													}
												}
												else if (inMacroM[curTrack] == 3)
												{
													if (macro3TimesM[curTrack] > 1)
													{
														seqPosM[curTrack] = macro3PosM[curTrack];
														macro3TimesM[curTrack]--;
													}
													else
													{
														inMacroM[curTrack] = 2;
														seqPosM[curTrack] = macro3RetM[curTrack];
														macro3TimesM[curTrack] = -1;
													}
												}
												else if (inMacroM[curTrack] == 4)
												{
													if (macro4TimesM[curTrack] > 1)
													{
														seqPosM[curTrack] = macro4PosM[curTrack];
														macro4TimesM[curTrack]--;
													}
													else
													{
														inMacroM[curTrack] = 3;
														seqPosM[curTrack] = macro4RetM[curTrack];
														macro4TimesM[curTrack] = -1;
													}
												}
												else
												{
													seqsEnd[curTrack] = 1;
												}

											}
											else
											{
												if (macro1TimesM[curTrack] > 1)
												{
													seqPosM[curTrack] = macro1PosM[curTrack];
													macro1TimesM[curTrack]--;
												}
												else
												{
													inMacroM[curTrack] = 0;
													seqPosM[curTrack] = macro1RetM[curTrack];
													macro1TimesM[curTrack] = -1;
												}
											}


										}
									}


								}

								else if (command[0] >= STATUS_NOTE_LEN_MIN && command[0] <= STATUS_NOTE_LEN_MAX && drvVers != VER_SADA)
								{
									if (drvVers == VER_FE3 || drvVers == VER_FE4 || drvVers == VER_TA)
									{
										if (command[1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}

										else
										{
											if (customLens == 0)
											{
												curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;

												lowNibble = (command[1] >> 4);
												highNibble = (command[1] & 15);

												noteDurs[curTrack] = spcData[lenTab + lowNibble];
												noteVels[curTrack] = spcData[velTab + highNibble];

												seqPosM[curTrack] += 2;
											}
											else
											{
												curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;

												seqPosM[curTrack]++;

												while (spcData[seqPosM[curTrack]] < 0x80 && seqPosM[curTrack] < 0xFFFF)
												{
													if (spcData[seqPosM[curTrack]] <= 0x3F)
													{
														if (drvVers == VER_FE3)
														{
															noteDurs[curTrack] = FE3Vals[spcData[seqPosM[curTrack]]];
														}
														else
														{
															noteDurs[curTrack] = FE4Vals[spcData[seqPosM[curTrack]]];
														}

														seqPosM[curTrack]++;
													}
													else if (spcData[seqPosM[curTrack]] >= 0x40 && spcData[seqPosM[curTrack]] <= 0x7F)
													{
														lowNibble = spcData[seqPosM[curTrack]] & 0x3F;
														if (drvVers == VER_FE3)
														{
															noteVels[curTrack] = FE3Vals[lowNibble];
														}
														else
														{
															noteVels[curTrack] = FE4Vals[lowNibble];
														}

														seqPosM[curTrack]++;
													}
												}
											}

										}
									}
									else if (drvVers == VER_LEMMINGS)
									{
										if (spcData[seqPosM[curTrack] + 1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											noteDurs[curTrack] = command[1];
											noteDurs[curTrack] = (command[1] << 1) + (command[1] >> 1) + (command[1] & 1);
											seqPosM[curTrack] += 2;
											if (command[2] <= 0x7F)
											{
												noteVels[curTrack] = command[2] << 1;
												seqPosM[curTrack]++;
											}
										}
									}
									else if (drvVers == VER_RC3 || drvVers == VER_JPARK)
									{
										if (spcData[seqPosM[curTrack] + 1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											noteDurs[curTrack] = 255;
											noteVels[curTrack] = command[1] << 4;
											seqPosM[curTrack] += 2;
										}
									}
									else if (drvVers == VER_CPBRAIN)
									{
										curNoteLens[curTrack] = 0;
										noteDurs[curTrack] = 255;
										noteVels[curTrack] = 255;

										while (spcData[seqPosM[curTrack]] >= 0x01 && spcData[seqPosM[curTrack]] <= 0x7F)
										{
											curNoteLens[curTrack] += (spcData[seqPosM[curTrack]] - STATUS_NOTE_LEN_MIN + 1);
											seqPosM[curTrack]++;
										}
										curNoteLens[curTrack] *= 10;
									}
									else if (drvVers == VER_HANARI)
									{
										if (spcData[seqPosM[curTrack] + 1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											noteDurs[curTrack] = command[1];
											tempByte = command[1] >> 1;
											noteDurs[curTrack] << 1;
											noteDurs[curTrack] += tempByte;
											seqPosM[curTrack] += 2;
											if (command[2] <= 0x7F)
											{
												noteVels[curTrack] = command[2] << 1;
												seqPosM[curTrack]++;
											}
										}
									}
									else if (drvVers == VER_DDW)
									{
										if (spcData[seqPosM[curTrack] + 1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											noteDurs[curTrack] = command[1];
											seqPosM[curTrack] += 2;
											if (command[2] <= 0x7F)
											{
												noteVels[curTrack] = command[2] << 1;
												seqPosM[curTrack]++;
											}
										}
									}
									else if (drvVers == VER_7TH)
									{
										if (spcData[seqPosM[curTrack] + 1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
											noteDurs[curTrack] = command[1];
											seqPosM[curTrack] += 2;
											noteVels[curTrack] = 255;
											if (command[2] <= 0x7F)
											{
												seqPosM[curTrack]++;
											}
										}
									}
									else if (drvVers == VER_MELFAND || drvVers == VER_THG)
									{
										curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
										seqPosM[curTrack]++;
									}
									else if (drvVers == VER_FH)
									{
										curNoteLens[curTrack] = FHLens[command[0] - STATUS_NOTE_LEN_MIN] * 5;
										seqPosM[curTrack]++;

										while ((spcData[seqPosM[curTrack]] >= STATUS_NOTE_LEN_MIN) && (spcData[seqPosM[curTrack]] <= STATUS_NOTE_LEN_MAX))
										{
											curNoteLens[curTrack] += (FHLens[spcData[seqPosM[curTrack]] - STATUS_NOTE_LEN_MIN] * 5);
											seqPosM[curTrack]++;
										}
									}
									else if (drvVers == VER_BETOP)
									{
										curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;
										seqPosM[curTrack]++;
									}
									else
									{
										if (command[1] >= 0x80)
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;

											if (drvVers == VER_TO)
											{
												if (command[0] == 0x7F)
												{
													curNoteLens[curTrack] = 960;
												}
												else if (command[0] == 0x7E)
												{
													curNoteLens[curTrack] = 720;
												}
											}


											if (drvVers == VER_AISYSTEM)
											{
												curNoteLens[curTrack] *= 5;
											}

											seqPosM[curTrack]++;
										}
										else
										{
											curNoteLens[curTrack] = (command[0] - STATUS_NOTE_LEN_MIN + 1) * 5;

											if (drvVers == VER_MDH)
											{
												curNoteLens[curTrack] *= multiplier[curTrack];
											}


											if (drvVers == VER_TO)
											{
												if (command[0] == 0x7F)
												{
													curNoteLens[curTrack] = 960;
												}
												else if (command[0] == 0x7E)
												{
													curNoteLens[curTrack] = 720;
												}
											}
											if (drvVers == VER_AISYSTEM)
											{
												curNoteLens[curTrack] *= 5;
											}

											lowNibble = (command[1] >> 4);
											highNibble = (command[1] & 15);

											noteDurs[curTrack] = spcData[lenTab + lowNibble];
											noteVels[curTrack] = spcData[velTab + highNibble];

											seqPosM[curTrack] += 2;
										}
									}


								}

								else if (command[0] >= STATUS_NOTE_MIN && command[0] <= STATUS_NOTE_MAX && EventMap[command[0]] != EVENT_REST)
								{
									if (drvVers == VER_SADA)
									{
										if (holdNotes[curTrack] == 1)
										{
											tempDelays[curTrack] = curDelays[curTrack];
											curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
											if (percNotes[curTrack] == 1)
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
											}
											else
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
											}
											percNotes[curTrack] = 0;
											curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
											holdNotes[curTrack] = 0;
											midPosM[curTrack] = tempPos;
										}
										if (transposes[curTrack] < -36 && command[0] < 0x90)
										{
											transposes[curTrack] = -36;
										}

										curNotes[curTrack] = command[0] - STATUS_NOTE_MIN + transposes[curTrack] + transpose2 + 23;

										if (drvVers == VER_KONAMI)
										{
											curNotes[curTrack] += repeatTune[curTrack];
										}

										ctrlDelays[curTrack] += curNoteLens[curTrack];
										masterDelays[curTrack] += curNoteLens[curTrack];
										noteVelVals[curTrack] = noteVels[curTrack];
										noteVelVals[curTrack] = noteVelVals[curTrack] / 256;
										noteDurVals[curTrack] = noteDurs[curTrack];
										noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
										noteVelVals[curTrack] = noteVels[curTrack];
										noteVelVals[curTrack] = noteVelVals[curTrack] / 256;
										curVols[curTrack] = curVolVels[curTrack] * noteVelVals[curTrack] * 0.5;
										if (drvVers == VER_KONAMI)
										{
											curVols[curTrack] += (repeatVol[curTrack] / 2);
										}
										if (curVols[curTrack] == 0)
										{
											curVols[curTrack] = 1;
										}
										if (curVols[curTrack] > 127)
										{
											curVols[curTrack] = 127;
										}

										volTrack = curTrack;
										curNoteLens[curTrack] = (spcData[seqPosM[curTrack] + 1] & 0x7F) * 5;
										tempPos = WriteNoteEventOn(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
										firstNotes[curTrack] = 0;
										midPosM[curTrack] = tempPos;
										curDelays[curTrack] = curNoteLens[curTrack];
										holdNotes[curTrack] = 1;
										seqPosM[curTrack] += 2;
									}
									else if (drvVers == VER_FH && command[0] >= 0xA0)
									{
										seqPosM[curTrack]++;
									}
									else
									{
										if (holdNotes[curTrack] == 1)
										{
											tempDelays[curTrack] = curDelays[curTrack];
											curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
											if (drvVers == VER_DDW || drvVers == VER_7TH)
											{
												curDelays[curTrack] = noteDurs[curTrack] * 5;
												if (curDelays[curTrack] > tempDelays[curTrack])
												{
													curDelays[curTrack] = tempDelays[curTrack];
												}
											}
											if (percNotes[curTrack] == 1)
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
											}
											else
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
											}
											percNotes[curTrack] = 0;
											curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
											holdNotes[curTrack] = 0;
											midPosM[curTrack] = tempPos;
										}
										if (transposes[curTrack] < -36 && command[0] < 0x90)
										{
											transposes[curTrack] = -36;
										}

										curNotes[curTrack] = command[0] - STATUS_NOTE_MIN + transposes[curTrack] + transpose2 + 24;
										if (drvVers == VER_KONAMI)
										{
											curNotes[curTrack] += repeatTune[curTrack];
										}

										if (drvVers == VER_FH)
										{
											lowNibble = (command[0] >> 4);
											highNibble = (command[0] & 15);

											curNotes[curTrack] = ((lowNibble * 12) + highNibble) + 24;
										}

										ctrlDelays[curTrack] += curNoteLens[curTrack];
										masterDelays[curTrack] += curNoteLens[curTrack];
										noteVelVals[curTrack] = noteVels[curTrack];
										noteVelVals[curTrack] = noteVelVals[curTrack] / 256;
										noteDurVals[curTrack] = noteDurs[curTrack];
										noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
										noteVelVals[curTrack] = noteVels[curTrack];
										noteVelVals[curTrack] = noteVelVals[curTrack] / 256;
										curVols[curTrack] = curVolVels[curTrack] * noteVelVals[curTrack] * 0.5;
										if (drvVers == VER_KONAMI)
										{
											curVols[curTrack] += (repeatVol[curTrack] / 2);
										}
										if (curVols[curTrack] == 0)
										{
											curVols[curTrack] = 1;
										}
										if (curVols[curTrack] > 127)
										{
											curVols[curTrack] = 127;
										}

										volTrack = curTrack;
										tempPos = WriteNoteEventOn(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
										firstNotes[curTrack] = 0;
										midPosM[curTrack] = tempPos;
										curDelays[curTrack] = curNoteLens[curTrack];
										holdNotes[curTrack] = 1;
										seqPosM[curTrack]++;
									}

								}

								else if (command[0] >= STATUS_PERCUSSION_NOTE_MIN && command[0] <= STATUS_PERCUSSION_NOTE_MAX && noPercNotes != 1)
								{
									if (drvVers == VER_AISYSTEM)
									{
										if (holdNotes[curTrack] == 1)
										{
											noteDurVals[curTrack] = noteDurs[curTrack];
											noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
											tempDelays[curTrack] = curDelays[curTrack];
											curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
											if (percNotes[curTrack] == 1)
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
											}
											else
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
											}
											percNotes[curTrack] = 0;
											curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
											holdNotes[curTrack] = 0;
											midPosM[curTrack] = tempPos;
										}
										curDelays[curTrack] += curNoteLens[curTrack];
										ctrlDelays[curTrack] += curNoteLens[curTrack];
										masterDelays[curTrack] += curNoteLens[curTrack];
										seqPosM[curTrack]++;
									}
									else
									{
										firstNote = 1;
										percInsts[curTrack] = curInsts[curTrack] - percBase;
										percNotes[curTrack] = 1;

										if (holdNotes[curTrack] == 1)
										{
											tempDelays[curTrack] = curDelays[curTrack];
											curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
											if (percNotes[curTrack] == 1)
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
											}
											else
											{
												tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
											}
											percNotes[curTrack] = 0;
											curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
											holdNotes[curTrack] = 0;
											midPosM[curTrack] = tempPos;
										}

										curNotes[curTrack] = command[0] - STATUS_PERCUSSION_NOTE_MIN + transpose + transpose2 + 24;
										ctrlDelays[curTrack] += curNoteLens[curTrack];
										masterDelays[curTrack] += curNoteLens[curTrack];
										noteDurVals[curTrack] = noteDurs[curTrack];
										noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
										noteVelVals[curTrack] = noteVels[curTrack];
										noteVelVals[curTrack] = noteVelVals[curTrack] / 256;
										curVols[curTrack] = curVolVels[curTrack] * noteVelVals[curTrack] * 0.5;
										if (drvVers == VER_KONAMI)
										{
											curVols[curTrack] += (repeatVol[curTrack] / 2);
										}
										if (curVols[curTrack] < 1)
										{
											curVols[curTrack] = 1;
										}
										if (curVols[curTrack] > 127)
										{
											curVols[curTrack] = 127;
										}
										volTrack = curTrack;
										tempPos = WriteNoteEventOn(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
										firstNotes[curTrack] = 0;
										midPosM[curTrack] = tempPos;
										curDelays[curTrack] = curNoteLens[curTrack];
										holdNotes[curTrack] = 1;
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_TIE)
								{
									if (drvVers == VER_SADA)
									{
										curNoteLens[curTrack] = (spcData[seqPosM[curTrack] + 1] & 0x7F) * 5;
										seqPosM[curTrack]++;
									}

									if (drvVers == VER_THG)
									{
										curNoteLens[curTrack] = (command[1] * 5);
										curDelays[curTrack] += (command[1] * 5);
										ctrlDelays[curTrack] += (command[1] * 5);
										masterDelays[curTrack] += (command[1] * 5);
										seqPosM[curTrack] += 2;
									}
									else
									{
										curDelays[curTrack] += curNoteLens[curTrack];
										ctrlDelays[curTrack] += curNoteLens[curTrack];
										masterDelays[curTrack] += curNoteLens[curTrack];
										seqPosM[curTrack]++;
									}

								}

								else if (EventMap[command[0]] == EVENT_REST)
								{
									if (holdNotes[curTrack] == 1)
									{
										noteDurVals[curTrack] = noteDurs[curTrack];
										noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
										tempDelays[curTrack] = curDelays[curTrack];
										curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
										if (percNotes[curTrack] == 1)
										{
											tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
										}
										else
										{
											tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
										}
										percNotes[curTrack] = 0;
										curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
										holdNotes[curTrack] = 0;
										midPosM[curTrack] = tempPos;
									}
									if (drvVers == VER_SADA)
									{
										curNoteLens[curTrack] = (spcData[seqPosM[curTrack] + 1] & 0x7F) * 5;
										seqPosM[curTrack]++;
									}
									curDelays[curTrack] += curNoteLens[curTrack];
									ctrlDelays[curTrack] += curNoteLens[curTrack];
									masterDelays[curTrack] += curNoteLens[curTrack];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_PROG_CHANGE)
								{
									curInsts[curTrack] = command[1];
									firstNotes[curTrack] = 1;
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_PAN)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_PAN_FADE)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack] += 3;
									}


								}

								else if (EventMap[command[0]] == EVENT_VIBRATO_ON)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else if (drvVers == VER_SADA)
									{
										seqPosM[curTrack] += 3;
									}
									else if (drvVers == VER_7TH)
									{
										seqPosM[curTrack] += 5;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_VIBRATO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_MASTER_VOLUME)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MASTER_VOLUME_FADE)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack] += 3;
									}
								}

								else if (EventMap[command[0]] == EVENT_TEMPO)
								{
									fading = 0;
									tempoVal = command[1] * 5;
									if (tempoVal != tempo)
									{
										ctrlMidPos++;
										valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
										ctrlDelay = 0;
										ctrlMidPos += valSize;
										WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
										ctrlMidPos += 3;
										tempo = command[1] * 5;
										if (drvVers == VER_MDH || drvVers == VER_BETOP)
										{
											tempo *= 0.5;
										}
										else if (drvVers == VER_MSQ)
										{
											tempo *= 0.2;
										}
										else if (drvVers == VER_MELFAND || drvVers == VER_THG)
										{
											tempo *= 0.45;
										}
										if (tempo < 2)
										{
											tempo = 160;
										}
										WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
										ctrlMidPos += 2;
									}
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_TEMPO_FADE)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										tempoTicks = command[1] * 5;
										finalTempo = command[2] * 5;
										tempoDiff = finalTempo - tempo;
										temp1 = finalTempo;
										temp2 = tempoTicks;
										fadeAmt = temp1 / temp2;
										fading = 1;
										fadeTempo = tempo;
										ticksLeft = 0;
										seqPosM[curTrack] += 3;
									}
								}


								else if (EventMap[command[0]] == EVENT_GLOBAL_TRANSPOSE)
								{
									transpose2 = (signed char)command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_LOCAL_TRANSPOSE)
								{
									transposes[curTrack] = (signed char)command[1];

									if (drvVers == VER_THG && command[1] == 0x8C)
									{
										transposes[curTrack] = 0;
									}
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_TREMOLO_ON)
								{
									if (drvVers == VER_TO)
									{
										seqPosM[curTrack] += 2;
									}
									else if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack]++;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_TREMOLO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_VOLUME)
								{
									curVolVels[curTrack] = command[1];
									if (curVolVels[curTrack] == 0x00)
									{
										curVolVels[curTrack] = 0x01;
									}

									seqPosM[curTrack] += 2;
									if (drvVers == VER_SADA)
									{
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_VOLUME_FADE)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack] += 3;
									}

								}

								else if (EventMap[command[0]] == EVENT_CALL)
								{
									if (drvVers == VER_TO)
									{
										if (inMacroM[curTrack] == 0)
										{
											macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
											macro1TimesM[curTrack] = spcData[seqPosM[curTrack] + 3];
											macro1RetM[curTrack] = seqPosM[curTrack] + 4;
											inMacroM[curTrack] = 1;
											seqPosM[curTrack] = macro1PosM[curTrack];
										}
										else if (inMacroM[curTrack] == 1)
										{
											macro2PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
											macro2TimesM[curTrack] = spcData[seqPosM[curTrack] + 3];
											macro2RetM[curTrack] = seqPosM[curTrack] + 4;
											inMacroM[curTrack] = 2;
											seqPosM[curTrack] = macro2PosM[curTrack];
										}
										else if (inMacroM[curTrack] == 2)
										{
											macro3PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
											macro3TimesM[curTrack] = spcData[seqPosM[curTrack] + 3];
											macro3RetM[curTrack] = seqPosM[curTrack] + 4;
											inMacroM[curTrack] = 3;
											seqPosM[curTrack] = macro3PosM[curTrack];
										}
										else if (inMacroM[curTrack] == 3)
										{
											macro4PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
											macro4TimesM[curTrack] = spcData[seqPosM[curTrack] + 3];
											macro4RetM[curTrack] = seqPosM[curTrack] + 4;
											inMacroM[curTrack] = 4;
											seqPosM[curTrack] = macro4PosM[curTrack];
										}
										else
										{
											seqsEnd[curTrack] = 1;
										}
									}
									else
									{
										if (inMacroM[curTrack] == 0)
										{
											macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);

											if (drvVers == VER_KONAMI)
											{
												macro1PosM[curTrack] += addVal;
											}

											if (drvVers == VER_JAM)
											{
												macro1PosM[curTrack] += songPtr;
											}

											if (drvVers == VER_CUBE)
											{
												macro1PosM[curTrack] -= baseValue;
												macro1PosM[curTrack] += songPtr;
											}
											macro1TimesM[curTrack] = spcData[seqPosM[curTrack] + 3];

											if ((drvVers == VER_SAI2 || drvVers == VER_MDH) && macro1TimesM[curTrack] == 0x00)
											{
												seqsEnd[curTrack] = 1;
											}

											/*
											if (macro1TimesM[curTrack] == 0xFA && drvVers == VER_KS)
											{
												macro1TimesM[curTrack] = 1;
											}
											*/
											macro1RetM[curTrack] = seqPosM[curTrack] + 4;
											if (drvVers == VER_NEXUS)
											{
												macro1TimesM[curTrack] = 1;
												macro1RetM[curTrack]--;
											}
											inMacroM[curTrack] = 1;
											seqPosM[curTrack] = macro1PosM[curTrack];
										}
										else
										{
											seqsEnd[curTrack] = 1;
											/*
											seqEnd = 1;
											if (patCommand[1] != 0x00 && patRepeat == -1)
											{
												patPos += 2;
											}
											*/
										}
									}
								}

								else if (EventMap[command[0]] == EVENT_VIBRATO_FADE)
								{
									if (drvVers == VER_TO)
									{
										seqPosM[curTrack] += 3;
									}
									else
									{
										seqPosM[curTrack] += 2;
									}
								}

								else if (EventMap[command[0]] == EVENT_PITCH_ENVELOPE_TO)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_PITCH_ENVELOPE_FROM)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else if (drvVers == VER_THG)
									{
										seqPosM[curTrack] += 3;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_PITCH_ENVELOPE_OFF)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_TUNING)
								{
									if (drvVers == VER_DDW)
									{
										seqPosM[curTrack] += 4;
									}
									else
									{
										seqPosM[curTrack] += 2;
									}
								}

								else if (EventMap[command[0]] == EVENT_ECHO_ON)
								{
									seqPosM[curTrack] += 4;
								}

								else if (EventMap[command[0]] == EVENT_ECHO_OFF)
								{
									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack]++;
									}

								}

								else if (EventMap[command[0]] == EVENT_ECHO_PARAM)
								{
									if (drvVers == VER_TO)
									{
										if (command[1] >= 0x80)
										{
											seqPosM[curTrack] += 3;
										}
										else if (command[3] == 0x04)
										{
											seqPosM[curTrack] += 12;
										}
										else
										{
											seqPosM[curTrack] += 4;
										}
									}
									else if (drvVers == VER_SNKK)
									{
										seqPosM[curTrack] += 5;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_ECHO_VOLUME)
								{
									if (drvVers == VER_DQ6)
									{
										seqPosM[curTrack] += 3;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_PITCH_SLIDE)
								{

									tempPos = WriteDeltaTime(multiMidData[curTrack], midPosM[curTrack], curDelays[curTrack]);
									midPosM[curTrack] += tempPos;
									Write8B(&multiMidData[curTrack][midPosM[curTrack]], (0xE0 | curTrack));
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 1], 0);
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 2], 0x40);
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 3], 0);
									curDelays[curTrack] = 0;
									firstNotes[curTrack] = 1;
									midPosM[curTrack] += 3;

									if (drvVers == VER_VIRGIN)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_PERCUSSION_PATCH_BASE)
								{
									percBase = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_NOP || EventMap[command[0]] == EVENT_UNKNOWN0)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_NOP1 || EventMap[command[0]] == EVENT_UNKNOWN1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_NOP2 || EventMap[command[0]] == EVENT_UNKNOWN2)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_NOP3 || EventMap[command[0]] == EVENT_UNKNOWN3)
								{
									seqPosM[curTrack] += 4;
								}

								else if (EventMap[command[0]] == EVENT_UNKNOWN4)
								{
									seqPosM[curTrack] += 5;
								}

								else if (EventMap[command[0]] == EVENT_MUTE)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_FAST_FORWARD_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_FAST_FORWARD_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_JUMP)
								{
									seqsEnd[curTrack] = 1;
								}

								/*Data East events*/
								else if (EventMap[command[0]] == EVENT_DE_SUB_COMMAND)
								{
									seqPosM[curTrack] += 2;
								}

								/*Konami events*/

								else if (EventMap[command[0]] == EVENT_KON_PITCH3)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_KON_REPEAT_START)
								{
 									repeatStart[curTrack] = seqPosM[curTrack] + 1;
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KON_REPEAT_END)
								{
									if (repeatTimes[curTrack] == -1)
									{
										repeatTimes[curTrack] = command[1];
										repeatVol[curTrack] = (signed char)command[2];
										repeatTune[curTrack] = ((signed char)command[3]) / 16;
									}
									else if (repeatTimes[curTrack] > 1)
									{
										repeatTimes[curTrack]--;
										seqPosM[curTrack] = repeatStart[curTrack];
									}
									else
									{
										repeatTimes[curTrack] = -1;
										repeatVol[curTrack] = 0;
										repeatTune[curTrack] = 0;
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_KON_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KON_ENVELOPE)
								{
									seqPosM[curTrack] += 4;
								}

								/*Sunset Riders events*/

								else if (EventMap[command[0]] == EVENT_SSR_CALL)
								{
									jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack]] + 1);
									jumpRet[curTrack] = seqPosM[curTrack] + 3;
									inJump[curTrack] = 1;
									seqPosM[curTrack] = jumpPos[curTrack];
								}

								else if (EventMap[command[0]] == EVENT_SSR_REPEAT)
								{
									if (repeatTimes[curTrack] == -1)
									{
										repeatTimes[curTrack] = command[1];
										repeatStart[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 2]);
									}
									else if (repeatTimes[curTrack] > 0)
									{
										repeatTimes[curTrack]--;
										seqPosM[curTrack] = repeatStart[curTrack];
									}
									else
									{
										repeatTimes[curTrack] = -1;
										seqPosM[curTrack] += 4;
									}

								}

								else if (EventMap[command[0]] == EVENT_SSR_RETURN)
								{
									if (inJump[curTrack] == 1)
									{
										inJump[curTrack] = 0;
										seqPosM[curTrack] = jumpRet[curTrack];
									}
									else
									{
										seqPosM[curTrack]++;
									}
								}

								/*Quintet events*/

								else if (EventMap[command[0]] == EVENT_QT_ASDR)
								{
									seqPosM[curTrack] += 4;
								}

								/*Marvelous events*/

								else if (EventMap[command[0]] == EVENT_MARV_ASDR)
								{
									curInsts[curTrack] = command[1];
									firstNotes[curTrack] = 1;
									seqPosM[curTrack] += 4;
								}

								/*Intelligent Systems events*/

								else if (EventMap[command[0]] == EVENT_IS_ECHO_ON || EventMap[command[0]] == EVENT_SS_ECHO_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_IS_ECHO_OFF || EventMap[command[0]] == EVENT_SS_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_IS_LEGATO_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_IS_LEGATO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_IS_CA)
								{
									/*Wait for APU port*/
									if (command[1] < 0xF0)
									{
										;
									}
									/*Set flag*/
									else
									{
										tempByte = command[1] & 7;
										if (tempByte == 0)
										{
											if (customPerc == 0)
											{
												customPerc = 1;
											}
											else
											{
												customPerc = 0;
											}

										}
										else if (tempByte == 7)
										{
											if (customLens == 0)
											{
												customLens = 1;
											}
											else
											{
												customLens = 0;
											}

										}
									}
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_IS_APU_WRITE)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_IS_JUMP_CONDITIONAL)
								{
									jumpPos[curTrack] = command[1];
									seqPosM[curTrack] += jumpPos[curTrack] + 2;

								}

								else if (EventMap[command[0]] == EVENT_IS_JUMP)
								{
									jumpPos[curTrack] = command[1];
									seqPosM[curTrack] += jumpPos[curTrack] + 2;
								}

								else if (EventMap[command[0]] == EVENT_IS_EVENT_F9)
								{
									seqPosM[curTrack] += 37;
								}

								else if (EventMap[command[0]] == EVENT_IS_EVENT_FC)
								{
									seqPosM[curTrack] += 2;
									seqPosM[curTrack] += ((command[1] & 15) + 1) * 3;
								}

								else if (EventMap[command[0]] == EVENT_IS_VOICE_DEF)
								{
									lowNibble = command[1] & 0x3F;
									if (command[1] >= 0 && command[1] <= 0x7F)
									{
										seqPosM[curTrack] += 2;
										for (k = 0; k < lowNibble; k++)
										{
											ISParams[k][0] = spcData[seqPosM[curTrack]];
											ISParams[k][1] = spcData[seqPosM[curTrack] + 1];
											ISParams[k][2] = spcData[seqPosM[curTrack] + 2];
											ISParams[k][3] = spcData[seqPosM[curTrack] + 3];
											seqPosM[curTrack] += 4;
										}

									}
									else
									{
										curInsts[curTrack] = command[1];
										firstNotes[curTrack] = 1;
										seqPosM[curTrack] += 8;
									}

								}

								else if (EventMap[command[0]] == EVENT_IS_VOICE_SET)
								{

									curInsts[curTrack] = ISParams[command[1]][0];
									firstNotes[curTrack] = 1;
									curVolVels[curTrack] = ISParams[command[1]][1];
									if (curVolVels[curTrack] == 0x00)
									{
										curVolVels[curTrack] = 0x01;
									}
									if (drvVers == VER_FE3)
									{
										transpose = (ISParams[command[1]][3] >> 4) - 1;

										if (transpose < 0 || transpose > 6)
										{
											transpose = 3;
										}
										transposes[curTrack] = ISTranspose[transpose];
									}
									else
									{
										transposes[curTrack] = (signed char)ISParams[command[1]][3];
									}

									
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_IS_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_IS_DUR_GAIN)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_IS_GAIN)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_IS_GAIN_RELEASE)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_IS_SUB_COMMAND)
								{
									if (drvVers == VER_FE4)
									{
										seqPosM[curTrack] += 3;
									}
									else
									{
										/*Unknown*/
										if (command[1] == 0x00)
										{
											seqPosM[curTrack] += 4;
										}
										/*Use customized percussion table on*/
										else if (command[1] == 0x01)
										{
											seqPosM[curTrack] += 3;
										}
										/*Use customized percussion table off*/
										else if (command[1] == 0x02)
										{
											seqPosM[curTrack] += 3;
										}
										/*Legato on*/
										else if (command[1] == 0x03)
										{
											seqPosM[curTrack] += 3;
										}
										/*Legato off*/
										else if (command[1] == 0x04)
										{
											seqPosM[curTrack] += 3;
										}
										/*Unknown*/
										else if (command[1] == 0x05)
										{
											seqPosM[curTrack] += 3;
										}
										else
										{
											seqPosM[curTrack] += 3;
										}
									}
								}

								/*Sunsoft events*/

								else if (EventMap[command[0]] == EVENT_SS_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_SS_GLOBAL_SONG_VOLUME)
								{
									seqPosM[curTrack] += 2;
								}

								/*TOSE events*/

								else if (EventMap[command[0]] == EVENT_TOSE_APU_WRITE)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_TOSE_NOISE)
								{
									seqPosM[curTrack] += 2;
								}

								/*Ocean events*/

								else if (EventMap[command[0]] == EVENT_OCEAN_GAIN_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_OCEAN_GAIN_OFF)
								{
									seqPosM[curTrack]++;
								}

								/*Monolith events*/

								else if (EventMap[command[0]] == EVENT_MONOLITH_GOTO)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_MONOLITH_DEC_POS)
								{
									seqPosM[curTrack]--;
								}

								else if (EventMap[command[0]] == EVENT_MONOLITH_SET_INS_PARAMETER)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MONOLITH_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								/*Argonaut Software events*/

								else if (EventMap[command[0]] == EVENT_ARGONAUT_ASDR)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_ARGONAUT_ASDR_ENV)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_ARGONAUT_FIR)
								{
									seqPosM[curTrack] += 2;
								}

								/*Advanced Communication events*/

								else if (EventMap[command[0]] == EVENT_ACC_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_ACC_ASDR_ENV)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_ACC_ASDR_ENV_TABLE_1)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_ACC_ASDR_ENV_TABLE_2)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_ACC_ASDR_ENV_CLEAR)
								{
									seqPosM[curTrack]++;
								}

								/*KOEI events*/
								
								else if (EventMap[command[0]] == EVENT_KOEI_REPEAT_START)
								{
									repeatStart[curTrack] = seqPosM[curTrack] + 1;
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KOEI_REPEAT_END)
								{
									if (repeatTimes[curTrack] == -1)
									{
										repeatTimes[curTrack] = command[1];
										repeatEnd[curTrack] = seqPosM[curTrack];
									}
									else if (repeatTimes[curTrack] > 1)
									{
										repeatTimes[curTrack]--;
										seqPosM[curTrack] = repeatStart[curTrack];
									}
									else
									{
										repeatTimes[curTrack] = -1;
										seqPosM[curTrack] += 2;
									}
								}

								else if (EventMap[command[0]] == EVENT_KOEI_REPEAT_JUMP)
								{
									if (repeatTimes[curTrack] == 1)
									{
										seqPosM[curTrack] = repeatEnd[curTrack];
									}
									else
									{
										seqPosM[curTrack]++;
									}
								}

								/*Aisystem Tokyo events*/

								else if (EventMap[command[0]] == EVENT_AISYSTEM_RESET_PROG)
								{
									seqPosM[curTrack]++;
								}

								/*Kennosuke Suemura events*/

								else if (EventMap[command[0]] == EVENT_KS_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_KS_FIR1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_KS_FIR2)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_LOOP_START)
								{
									if (drvVers == VER_KS)
									{
										seqPosM[curTrack]++;
									}
									else
									{
										repeatLevels[curTrack]++;
										repeats[curTrack][repeatLevels[curTrack]][0] = seqPosM[curTrack] + 2;
										repeats[curTrack][repeatLevels[curTrack]][1] = command[1];
										seqPosM[curTrack] += 2;
									}

								}

								else if (EventMap[command[0]] == EVENT_KS_LOOP_END)
								{
									if (drvVers == VER_KS)
									{
										seqsEnd[curTrack] = 1;
									}
									else
									{
										if (repeats[curTrack][repeatLevels[curTrack]][1] == 0xFF)
										{
											seqsEnd[curTrack] = 1;
										}
										else if (repeats[curTrack][repeatLevels[curTrack]][1] > 1)
										{
											repeats[curTrack][repeatLevels[curTrack]][1]--;
											seqPosM[curTrack] = repeats[curTrack][repeatLevels[curTrack]][0];
										}
										else
										{
											repeats[curTrack][repeatLevels[curTrack]][1] = -1;
											if (repeatLevels[curTrack] > 0)
											{
												repeatLevels[curTrack]--;
											}
											seqPosM[curTrack]++;
										}

									}

								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY0)
								{
									noteVels[curTrack] = spcData[velTab];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY1)
								{
									noteVels[curTrack] = spcData[velTab + 1];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY2)
								{
									noteVels[curTrack] = spcData[velTab + 2];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY3)
								{
									noteVels[curTrack] = spcData[velTab + 3];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY4)
								{
									noteVels[curTrack] = spcData[velTab + 4];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY5)
								{
									noteVels[curTrack] = spcData[velTab + 5];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY6)
								{
									noteVels[curTrack] = spcData[velTab + 6];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY7)
								{
									noteVels[curTrack] = spcData[velTab + 7];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY8)
								{
									noteVels[curTrack] = spcData[velTab + 8];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITY9)
								{
									noteVels[curTrack] = spcData[velTab + 9];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYA)
								{
									noteVels[curTrack] = spcData[velTab + 10];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYB)
								{
									noteVels[curTrack] = spcData[velTab + 11];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYC)
								{
									noteVels[curTrack] = spcData[velTab + 12];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYD)
								{
									noteVels[curTrack] = spcData[velTab + 13];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYE)
								{
									noteVels[curTrack] = spcData[velTab + 14];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_VELOCITYF)
								{
									noteVels[curTrack] = spcData[velTab + 15];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_ECHO_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_KS_FIR_FILTER)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_KS_END)
								{
									seqsEnd[curTrack] = 1;
									break;
								}

								/*Hect events*/

								else if (EventMap[command[0]] == EVENT_HECT_SUB_COMMAND)
								{
									/*Set attack rate*/
									if (command[1] == 0x00)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set decay rate*/
									else if (command[1] == 0x01)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set sustain rate*/
									else if (command[1] == 0x02)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set release rate*/
									else if (command[1] == 0x03)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set sustain level*/
									else if (command[1] == 0x04)
									{ 
										seqPosM[curTrack] += 3;
									}

									/*Go to macro*/
									else if (command[1] == 0x05)
									{
										if (inMacroM[curTrack] == 0)
										{
											macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 2]);
											macro1RetM[curTrack] = seqPosM[curTrack] + 4;
											seqPosM[curTrack] = macro1PosM[curTrack];
											inMacroM[curTrack] = 1;
										}
										else
										{
											seqsEnd[curTrack] = 1;
										}
									}

									/*Return from macro*/
									else if (command[1] == 0x06)
									{
										if (inMacroM[curTrack] == 1)
										{
											inMacroM[curTrack] = 0;
											seqPosM[curTrack] = macro1RetM[curTrack];
										}
										else
										{
											seqPosM[curTrack] += 3;
										}
									}

									/*Jump to position*/
									else if (command[1] == 0x07)
									{
										jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 2]);

										if (jumpPos[curTrack] < seqPosM[curTrack])
										{
											seqsEnd[curTrack] = 1;
										}
										else
										{
											seqPosM[curTrack] = jumpPos[curTrack];
										}
									}

									/*Repeat the following section*/
									else if (command[1] == 0x08)
									{
										repeatTimes[curTrack] = command[2];
										seqPosM[curTrack] += 3;
									}

									/*End of repeat section*/
									else if (command[1] == 0x09)
									{
										if (repeatTimes[curTrack] > 1)
										{
											repeatTimes[curTrack]--;
											repeatStart[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 2]);
											seqPosM[curTrack] = repeatStart[curTrack];
										}
										else
										{
											repeatTimes[curTrack] = -1;
											seqPosM[curTrack] += 4;
										}
									}

									/*Unknown sub-command 0A*/
									else if (command[1] == 0x0A)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set pan value from table (1)*/
									else if (command[1] == 0x0B)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set pan value from table (2)*/
									else if (command[1] == 0x0C)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set reverb*/
									else if (command[1] == 0x0D)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set surround volume*/
									else if (command[1] == 0x0E)
									{
										seqPosM[curTrack] += 3;
									}

									/*Subtract volume*/
									else if (command[1] == 0x0F)
									{
										seqPosM[curTrack] += 4;
									}

									/*Unknown sub-command*/
									else
									{
										seqPosM[curTrack] += 3;
									}
								}

								/*Heart Beat events*/

								else if (EventMap[command[0]] == EVENT_HB_SLUR_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_SLUR_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_ECHO_VOLUME)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_HB_ECHO_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_ECHO_FIR)
								{
									seqPosM[curTrack] += 9;
								}

								else if (EventMap[command[0]] == EVENT_HB_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_HB_DUR_VEL)
								{
									lowNibble = (command[1] >> 4);
									highNibble = (command[1] & 15);

									noteDurs[curTrack] = spcData[lenTab + lowNibble];
									noteVels[curTrack] = spcData[velTab + highNibble];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_HB_JUMP)
								{
									jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtr;

									if (jumpPos[curTrack] < seqPosM[curTrack])
									{
										seqsEnd[curTrack] = 1;
									}
									else
									{
										seqPosM[curTrack] = jumpPos[curTrack];
									}
								}

								else if (EventMap[command[0]] == EVENT_HB_CALL)
								{
									if (inMacroM[curTrack] == 0)
									{
										macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtr;
										macro1RetM[curTrack] = seqPosM[curTrack] + 3;
										seqPosM[curTrack] = macro1PosM[curTrack];
										inMacroM[curTrack] = 1;
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_HB_RETURN)
								{
									if (inMacroM[curTrack] == 1)
									{
										inMacroM[curTrack] = 0;
										seqPosM[curTrack] = macro1RetM[curTrack];
									}
									else
									{
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_HB_NOISE_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_NOISE_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_HB_NOISE_CLOCK)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_HB_SUB_COMMAND)
								{
									/*Repeat the following section*/
									if (command[1] == 0x00)
									{
										repeatTimes[curTrack] = command[2];
										seqPosM[curTrack] += 3;
									}

									/*End of repeat section*/
									else if (command[1] == 0x01)
									{
										if (repeatTimes[curTrack] > 1)
										{
											repeatTimes[curTrack]--;
											repeatStart[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 2]) + songPtr;
											seqPosM[curTrack] = repeatStart[curTrack];
										}
										else
										{
											repeatTimes[curTrack] = -1;
											seqPosM[curTrack] += 4;
										}
									}

									/*Unknown sub-command 02*/
									else if (command[1] == 0x02)
									{
										seqPosM[curTrack] += 2;
									}

									/*Set attack rate*/
									else if (command[1] == 0x03)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set decay rate*/
									else if (command[1] == 0x04)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set sustain level*/
									else if (command[1] == 0x05)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set release rate*/
									else if (command[1] == 0x06)
									{
										seqPosM[curTrack] += 3;
									}

									/*Set sustain rate*/
									else if (command[1] == 0x07)
									{
										seqPosM[curTrack] += 3;
									}

									/*Unknown sub-command 08*/
									else if (command[1] == 0x08)
									{
										seqPosM[curTrack] += 2;
									}

									/*Phase reverse surround on/off*/
									else if (command[1] == 0x09)
									{
										seqPosM[curTrack] += 4;
									}
								}

								/*Quest events*/

								else if (EventMap[command[0]] == EVENT_QUEST_APU_WRITE)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_CMD_VOICE)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_JUMP)
								{
									jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);

									if (jumpPos[curTrack] < seqPosM[curTrack])
									{
										seqsEnd[curTrack] = 1;
									}
									else
									{
										seqPosM[curTrack] = jumpPos[curTrack];
									}
								}

								else if (EventMap[command[0]] == EVENT_QUEST_ASDR_GAIN)
								{
									seqPosM[curTrack] += 4;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_PITCH)
								{
									seqPosM[curTrack] += 4;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_ASDR)
								{
									seqPosM[curTrack] += 6;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_LONG_NOTE)
								{
									curNoteLens[curTrack] = ((command[1] - STATUS_NOTE_LEN_MIN + 1) + (command[2] * 0x100)) * 5;

									if (command[2] == 0x7F)
									{
										curNoteLens[curTrack] = (0xC0 + (command[2] * 0x100)) * 5;
									}
									else if (command[2] == 0x7E)
									{
										curNoteLens[curTrack] = (0x90 + (command[2] * 0x100)) * 5;
									}

									if (command[3] <= 0x7F)
									{
										lowNibble = (command[3] >> 4);
										highNibble = (command[3] & 15);

										noteDurs[curTrack] = spcData[lenTab + lowNibble];
										noteVels[curTrack] = spcData[velTab + highNibble];

										seqPosM[curTrack] += 4;
									}
									else
									{
										seqPosM[curTrack] += 3;
									}


								}

								else if (EventMap[command[0]] == EVENT_QUEST_REPEAT_START)
								{
									repeatTimes[curTrack] = command[1];
									repeatStart[curTrack] = seqPosM[curTrack] + 2;
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_REPEAT_END)
								{
									if (repeatTimes[curTrack] > 1)
									{
										repeatTimes[curTrack]--;
										seqPosM[curTrack] = repeatStart[curTrack];
									}
									else
									{
										repeatTimes[curTrack] = -1;
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_QUEST_GAIN)
								{
									if (command[1] == 0x00)
									{
										seqPosM[curTrack] += 2;
									}
									else
									{
										if (command[2] != 0x00)
										{
											seqPosM[curTrack]++;
										}
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_QUEST_ECHO_START_DELAY)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_QUEST_SUB_COMMAND)
								{
									/*Set pitch/SRCN/ASDR*/
									if (command[1] == 0xFF)
									{
										seqPosM[curTrack] += 8;
									}
									/*Set SRCN/ASDR*/
									else if (command[1] == 0xFE)
									{
										seqPosM[curTrack] += 6;
									}
									/*Set ASDR*/
									else if (command[1] == 0xFD)
									{
										seqPosM[curTrack] += 5;
									}
									/*Set pitch*/
									else if (command[1] == 0xFC)
									{
										seqPosM[curTrack] += 4;
									}
									else
									{
										seqPosM[curTrack] += 8;
									}
								}

								/*Virgin events*/

								else if (EventMap[command[0]] == EVENT_VIRGIN_NOTE_LEN)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_PAN_PARAM)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_VIBRATO_PARAM1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_VIBRATO_PARAM2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_VIBRATO_PARAM3)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_MASTER_VOLUME_PARAM)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_TEMPO_FADE_PARAM)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_VOLUME_FADE_PARAM)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_ECHO_VOLUME_PARAM1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_ECHO_VOLUME_PARAM2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_ECHO_PARAM1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_VIRGIN_ECHO_PARAM2)
								{
									seqPosM[curTrack] += 2;
								}

								/*CP.BRAiN events*/

								else if (EventMap[command[0]] == EVENT_CPBRAIN_SUB_COMMAND)
								{
									/*Jump to position*/
									if (command[1] == 0x00)
									{
										jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack]] + 2) + songPtr + (curTrack * 2);

										if (jumpPos[curTrack] > seqPosM[curTrack])
										{
											seqPosM[curTrack] = jumpPos[curTrack];
										}
										else
										{
											seqsEnd[curTrack] = 1;
										}
									}

									/*Go to macro*/
									else if (command[1] == 0x01)
									{
										jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack]] + 2) + songPtr + (curTrack * 2);

										if (inMacroM[curTrack] == 0)
										{
											macro1PosM[curTrack] = jumpPos[curTrack];
											macro1RetM[curTrack] = seqPosM[curTrack] + 4;
											inMacroM[curTrack] = 1;
											seqPosM[curTrack] = macro1PosM[curTrack];
										}
										else
										{
											seqsEnd[curTrack] = 1;
										}
									}

									/*Return from macro*/
									else if (command[1] == 0x02)
									{
										if (inMacroM[curTrack] == 1)
										{
											inMacroM[curTrack] = 0;
											seqPosM[curTrack] = macro1RetM[curTrack];
										}
										else
										{
											seqPosM[curTrack] += 2;
										}
									}

									/*Repeat the following section*/
									else if (command[1] == 0x03)
									{
										repeatStart[curTrack] = seqPosM[curTrack] + 3;
										repeatTimes[curTrack] = command[2];
										seqPosM[curTrack] += 3;
									}

									/*End of repeat section*/
									else if (command[1] == 0x04)
									{
										if (repeatTimes[curTrack] > 1)
										{
											repeatTimes[curTrack]--;
											seqPosM[curTrack] = repeatStart[curTrack];
										}
										else
										{
											repeatTimes[curTrack] = -1;
											seqPosM[curTrack] += 2;
										}
									}

									/*Conditional jump*/
									else if (command[1] == 0x05)
									{
										jumpPos[curTrack] = ReadLE16(&spcData[seqPosM[curTrack]] + 2) + songPtr + (curTrack * 2);

										if (jumpPos[curTrack] > seqPosM[curTrack])
										{
											seqPosM[curTrack] = jumpPos[curTrack];
										}
										else
										{
											seqsEnd[curTrack] = 1;
										}
									}

									/*Unknown sub-command*/
									else
									{
										seqPosM[curTrack] += 2;
									}
								}

								/*Kyouhei Sada events*/

								else if (EventMap[command[0]] == EVENT_SADA_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_SADA_PITCH)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_SADA_ECHO1)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_SADA_ECHO2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_SADA_ECHO3)
								{
									seqPosM[curTrack] += 4;
								}

								else if (EventMap[command[0]] == EVENT_SADA_QUANTIZATION)
								{
									noteDurs[curTrack] = spcData[lenTab + command[1]];
									noteVels[curTrack] = spcData[lenTab + command[2]];
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_SADA_SLUR)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_SADA_REST_REPEAT)
								{
									if (holdNotes[curTrack] == 1)
									{
										noteDurVals[curTrack] = noteDurs[curTrack];
										noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
										tempDelays[curTrack] = curDelays[curTrack];
										curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
										if (percNotes[curTrack] == 1)
										{
											tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
										}
										else
										{
											tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
										}
										percNotes[curTrack] = 0;
										curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
										holdNotes[curTrack] = 0;
										midPosM[curTrack] = tempPos;
									}
									curDelays[curTrack] += curNoteLens[curTrack];
									ctrlDelays[curTrack] += curNoteLens[curTrack];
									masterDelays[curTrack] += curNoteLens[curTrack];
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_SADA_REPEAT_START)
								{
									repeatStart[curTrack] = seqPosM[curTrack] + 1;
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_SADA_REPEAT_END)
								{
									if (repeatTimes[curTrack] == -1)
									{
										repeatTimes[curTrack] = command[1];
									}
									else if (repeatTimes[curTrack] > 1)
									{
										repeatTimes[curTrack]--;
										seqPosM[curTrack] = repeatStart[curTrack];
									}
									else
									{
										repeatTimes[curTrack] = -1;
										seqPosM[curTrack] += 2;
									}
								}

								/*Make Software (early)? events*/

								else if (EventMap[command[0]] == EVENT_MAKE1_REPEAT_START)
								{
									repeatLevels[curTrack]++;
									repeats[curTrack][repeatLevels[curTrack]][0] = seqPosM[curTrack] + 2;
									repeats[curTrack][repeatLevels[curTrack]][1] = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MAKE1_REPEAT_END)
								{
									if (repeats[curTrack][repeatLevels[curTrack]][1] == 0xFF)
									{
										seqsEnd[curTrack] = 1;
									}
									else if (repeats[curTrack][repeatLevels[curTrack]][1] > 1)
									{
										repeats[curTrack][repeatLevels[curTrack]][1]--;
										seqPosM[curTrack] = repeats[curTrack][repeatLevels[curTrack]][0];
									}
									else
									{
										repeats[curTrack][repeatLevels[curTrack]][1] = -1;
										if (repeatLevels[curTrack] > 0)
										{
											repeatLevels[curTrack]--;
										}
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_MAKE1_CALL)
								{
									if (inMacroM[curTrack] == 0)
									{
										macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro1RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro1PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 1)
									{
										macro2PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro2RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro2PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										macro3PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro3RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro3PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										macro4PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro4RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 4;
										seqPosM[curTrack] = macro4PosM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_MAKE1_RETURN)
								{
									if (inMacroM[curTrack] == 1)
									{
										inMacroM[curTrack] = 0;
										seqPosM[curTrack] = macro1RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro2RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro3RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 4)
									{
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro4RetM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_MAKE1_SONG_LOOP)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_MAKE1_SUB_COMMAND)
								{
									/*Go to song loop*/
									if (ReadLE16(&spcData[seqPosM[curTrack] + 1]) == 0x0000)
									{
										seqsEnd[curTrack] = 1;
									}
									/*Unknown sub-command 1*/
									else if (ReadLE16(&spcData[seqPosM[curTrack] + 1]) == 0x0001)
									{
										seqPosM[curTrack] += 3;
									}
									/*Unknown sub-command 2*/
									else if (ReadLE16(&spcData[seqPosM[curTrack] + 1]) == 0x0002)
									{
										seqPosM[curTrack] += 3;
									}
									/*Unknown sub-command 3*/
									else if (ReadLE16(&spcData[seqPosM[curTrack] + 1]) == 0x0003)
									{
										seqPosM[curTrack] += 3;
									}
									/*Unknown sub-command*/
									else
									{
										seqPosM[curTrack]++;
									}
								}

								/*Make Software events*/

								else if (EventMap[command[0]] == EVENT_MAKE2_ASDR)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_MAKE2_VOLUME_ADD)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MAKE2_TUNING2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MAKE2_SPEED)
								{
									multiplier[curTrack] = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MAKE2_SUB_COMMAND)
								{
									/*Unknown sub-command 00*/
									if (command[1] == 0x00)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 01*/
									else if (command[1] == 0x01)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 02*/
									else if (command[1] == 0x02)
									{
										if (command[2] == 0x00)
										{
											seqPosM[curTrack] += 2;
										}
										else
										{
											seqPosM[curTrack] += 4;
										}
									}

									/*Unknown sub-command 03*/
									else if (command[1] == 0x03)
									{
										seqPosM[curTrack] += 3;
									}

									/*Unknown sub-command 04*/
									if (command[1] == 0x04)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 05*/
									if (command[1] == 0x05)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 06*/
									if (command[1] == 0x06)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 07*/
									if (command[1] == 0x07)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 08*/
									if (command[1] == 0x08)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command 09*/
									if (command[1] == 0x09)
									{
										seqPosM[curTrack] += 2;
									}

									/*Unknown sub-command*/
									else
									{
										seqPosM[curTrack] += 2;
									}
								}

								/*MIHO events*/

								else if (EventMap[command[0]] == EVENT_MIHO_CALL)
								{
									if (inMacroM[curTrack] == 0)
									{
										macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro1RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro1PosM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_MIHO_RETURN)
								{
									if (inMacroM[curTrack] == 1)
									{
										inMacroM[curTrack] = 0;
										seqPosM[curTrack] = macro1RetM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_MIHO_REPEAT)
								{
									if (command[1] == 0)
									{
										if (repeatTimes[curTrack] > 0)
										{
											repeatTimes[curTrack]--;
											seqPosM[curTrack] = repeatStart[curTrack];
										}
										else
										{
											repeatTimes[curTrack] = -1;
											seqPosM[curTrack] += 2;
										}
									}
									else
									{
										repeatTimes[curTrack] = command[1];
										repeatStart[curTrack] = seqPosM[curTrack] + 2;
										seqPosM[curTrack] += 2;
									}
								}

								/*MINT events*/

								else if (EventMap[command[0]] == EVENT_MINT_VIBRATO)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MINT_ECHO_VOL)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MINT_REPEAT_START)
								{
									repeatLevels[curTrack]++;
									repeats[curTrack][repeatLevels[curTrack]][0] = seqPosM[curTrack] + 2;
									repeats[curTrack][repeatLevels[curTrack]][1] = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_MINT_REPEAT_END)
								{
									if (repeats[curTrack][repeatLevels[curTrack]][1] == 0xFF)
									{
										seqsEnd[curTrack] = 1;
									}
									else if (repeats[curTrack][repeatLevels[curTrack]][1] > 1)
									{
										repeats[curTrack][repeatLevels[curTrack]][1]--;
										seqPosM[curTrack] = repeats[curTrack][repeatLevels[curTrack]][0];
									}
									else
									{
										repeats[curTrack][repeatLevels[curTrack]][1] = -1;
										if (repeatLevels[curTrack] > 1)
										{
											repeatLevels[curTrack]--;
										}
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_MINT_CALL || EventMap[command[0]] == EVENT_STING_CALL)
								{
									if (inMacroM[curTrack] == 0)
									{
										macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro1RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro1PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 1)
									{
										macro2PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro2RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro2PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										macro3PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro3RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro3PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										macro4PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]);
										macro4RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 4;
										seqPosM[curTrack] = macro4PosM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								/*Sting events*/
								else if (EventMap[command[0]] == EVENT_STING_CHAN_MASK)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_QUANTIZATION)
								{
									lowNibble = (command[1] >> 4);
									highNibble = (command[1] & 15);

									noteDurs[curTrack] = spcData[lenTab + lowNibble];
									noteVels[curTrack] = spcData[velTab + highNibble];

									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_VIBRATO)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_STING_VOL_ENV1)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_VOL_ENV2)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_FADE_OUT)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_PITCH_SLIDE)
								{
									tempPos = WriteDeltaTime(multiMidData[curTrack], midPosM[curTrack], curDelays[curTrack]);
									midPosM[curTrack] += tempPos;
									Write8B(&multiMidData[curTrack][midPosM[curTrack]], (0xE0 | curTrack));
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 1], 0);
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 2], 0x40);
									Write8B(&multiMidData[curTrack][midPosM[curTrack] + 3], 0);
									curDelays[curTrack] = 0;
									firstNotes[curTrack] = 1;
									midPosM[curTrack] += 3;
									if (drvVers == VER_FH)
									{
										curDelays[curTrack] += (curNoteLens[curTrack]);
										seqPosM[curTrack] += 3;
									}
									else
									{
										curDelays[curTrack] += ((command[1]) * 5);
										seqPosM[curTrack] += 4;
									}
								}

								else if (EventMap[command[0]] == EVENT_STING_SLUR_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_SLUR_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_RETURN)
								{
									if (inMacroM[curTrack] == 1)
									{
										inMacroM[curTrack] = 0;
										seqPosM[curTrack] = macro1RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro2RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro3RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 4)
									{
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro4RetM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_STING_REPEAT_START)
								{
									repeatLevels[curTrack]++;
									repeats[curTrack][repeatLevels[curTrack]][0] = seqPosM[curTrack] + 2;
									repeats[curTrack][repeatLevels[curTrack]][1] = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_REPEAT_END)
								{
									if (repeats[curTrack][repeatLevels[curTrack]][1] == 0xFF)
									{
										seqsEnd[curTrack] = 1;
									}
									else if (repeats[curTrack][repeatLevels[curTrack]][1] > 1)
									{
										repeats[curTrack][repeatLevels[curTrack]][1]--;
										seqPosM[curTrack] = repeats[curTrack][repeatLevels[curTrack]][0];
									}
									else
									{
										repeats[curTrack][repeatLevels[curTrack]][1] = -1;
										if (repeatLevels[curTrack] > 1)
										{
											repeatLevels[curTrack]--;
										}
										seqPosM[curTrack]++;
									}
								}

								else if (EventMap[command[0]] == EVENT_STING_ASDR1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_ASDR2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_GAIN)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_ASDR_GAIN)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_PITCH_ENV)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_PITCH)
								{
									seqPosM[curTrack] += 3;
								}

								else if (EventMap[command[0]] == EVENT_STING_GET_SLUR)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_ECHO1)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_ECHO2)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_FIR)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_ECHO_VOL)
								{
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_STING_ECHO_ON)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_ECHO_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_NOISE_OFF)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_PMOD)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_RESET_PITCH)
								{
									seqPosM[curTrack]++;
								}

								else if (EventMap[command[0]] == EVENT_STING_NOTE_LEN)
								{
									curNoteLens[curTrack] = command[1] * 5;
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_BETOP_JUMP)
								{
									seqsEnd[curTrack] = 1;
								}

								else if (EventMap[command[0]] == EVENT_BETOP_CALL)
								{
									if (inMacroM[curTrack] == 0)
									{
										macro1PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtrs[0];
										macro1RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro1PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 1)
									{
										macro2PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtrs[0];
										macro2RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro2PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										macro3PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtrs[0];
										macro3RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro3PosM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										macro4PosM[curTrack] = ReadLE16(&spcData[seqPosM[curTrack] + 1]) + songPtrs[0];
										macro4RetM[curTrack] = seqPosM[curTrack] + 3;
										inMacroM[curTrack] = 4;
										seqPosM[curTrack] = macro4PosM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_BETOP_RETURN)
								{
									if (inMacroM[curTrack] == 1)
									{
										inMacroM[curTrack] = 0;
										seqPosM[curTrack] = macro1RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 2)
									{
										inMacroM[curTrack] = 1;
										seqPosM[curTrack] = macro2RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 3)
									{
										inMacroM[curTrack] = 2;
										seqPosM[curTrack] = macro3RetM[curTrack];
									}
									else if (inMacroM[curTrack] == 4)
									{
										inMacroM[curTrack] = 3;
										seqPosM[curTrack] = macro4RetM[curTrack];
									}
									else
									{
										seqsEnd[curTrack] = 1;
									}
								}

								else if (EventMap[command[0]] == EVENT_BETOP_REPEAT_START)
								{
									repeatLevels[curTrack]++;
									repeats[curTrack][repeatLevels[curTrack]][0] = seqPosM[curTrack] + 2;
									repeats[curTrack][repeatLevels[curTrack]][1] = command[1];
									seqPosM[curTrack] += 2;
								}

								else if (EventMap[command[0]] == EVENT_BETOP_REPEAT_END)
								{
									if (repeats[curTrack][repeatLevels[curTrack]][1] == 0xFF)
									{
										seqsEnd[curTrack] = 1;
									}
									else if (repeats[curTrack][repeatLevels[curTrack]][1] > 1)
									{
										repeats[curTrack][repeatLevels[curTrack]][1]--;
										seqPosM[curTrack] = repeats[curTrack][repeatLevels[curTrack]][0];
									}
									else
									{
										repeats[curTrack][repeatLevels[curTrack]][1] = -1;
										if (repeatLevels[curTrack] > 0)
										{
											repeatLevels[curTrack]--;
										}
										seqPosM[curTrack]++;
									}
									}

								else if (EventMap[command[0]] == EVENT_BETOP_QUANTIZE)
								{
									lowNibble = (command[1] >> 4);
									highNibble = (command[1] & 15);

									noteDurs[curTrack] = spcData[lenTab + lowNibble];
									noteVels[curTrack] = spcData[velTab + highNibble];

									seqPosM[curTrack] += 2;
							}

								else
								{
									seqPosM[curTrack]++;
								}

							}

							if (seqsEnd[curTrack] == 1)
							{
								if (holdNotes[curTrack] == 1)
								{
									noteDurVals[curTrack] = noteDurs[curTrack];
									noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
									tempDelays[curTrack] = curDelays[curTrack];
									curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
									if (percNotes[curTrack] == 1)
									{
										tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
									}
									else
									{
										tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
									}
									percNotes[curTrack] = 0;
									curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
									holdNotes[curTrack] = 0;
									midPosM[curTrack] = tempPos;
								}
								curDelays[curTrack] += 5;
								masterDelays[curTrack] += 5;
							}
						
						}
					}

					if (fading == 1 && ticksLeft < tempoTicks)
					{
						fadeTempo += fadeAmt;
						ctrlMidPos++;
						valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
						ctrlDelay = 0;
						ctrlMidPos += valSize;
						WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
						ctrlMidPos += 3;
						if (fadeTempo < 2)
						{
							fadeTempo = 160;
						}
						WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / fadeTempo);
						ctrlMidPos += 2;
						ticksLeft += 5;
					}
					else if (fading == 1 && ticksLeft >= tempoTicks)
					{
						fading = 0;
					}
					seqTime += 5;
					ctrlDelay += 5;

				}

				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					if (songPtrs[curTrack] < 0x0100)
					{
						curDelays[curTrack] += (seqTime - masterDelays[curTrack]);
						masterDelays[curTrack] += (seqTime - masterDelays[curTrack]);
					}
				}

				for (curTrack = 0; curTrack < trackCnt; curTrack++)
				{
					if (masterDelays[curTrack] > seqTime)
					{
						curDelays[curTrack] -= (masterDelays[curTrack] - seqTime);
					}
					masterDelays[curTrack] = seqTime;
				}

			}

		}



		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			if (holdNotes[curTrack] == 1)
			{
				noteDurVals[curTrack] = noteDurs[curTrack];
				noteDurVals[curTrack] = noteDurVals[curTrack] / 256;
				tempDelays[curTrack] = curDelays[curTrack];
				curDelays[curTrack] = curDelays[curTrack] * noteDurVals[curTrack];
				if (percNotes[curTrack] == 1)
				{
					tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, percInsts[curTrack]);
				}
				else
				{
					tempPos = WriteNoteEventOff(multiMidData[curTrack], midPosM[curTrack], curNotes[curTrack], curNoteLens[curTrack], curDelays[curTrack], firstNotes[curTrack], curTrack, curInsts[curTrack]);
				}
				percNotes[curTrack] = 0;
				curDelays[curTrack] = tempDelays[curTrack] - curDelays[curTrack];
				holdNotes[curTrack] = 0;
				midPosM[curTrack] = tempPos;
			}
			/*End of track*/
			WriteBE32(&multiMidData[curTrack][midPosM[curTrack]], 0xFF2F00);
			midPosM[curTrack] += 4;
			firstNotes[curTrack] = 0;

			/*Calculate MIDI channel size*/
			trackSizes[curTrack] = midPosM[curTrack] - midTrackBase;
			WriteBE16(&multiMidData[curTrack][midTrackBase - 2], trackSizes[curTrack]);
		}
		/*End of control track*/
		ctrlMidPos++;
		WriteBE32(&ctrlMidData[ctrlMidPos], 0xFF2F00);
		ctrlMidPos += 4;

		/*Calculate MIDI channel size*/
		ctrlTrackSize = ctrlMidPos - ctrlMidTrackBase;
		WriteBE16(&ctrlMidData[ctrlMidTrackBase - 2], ctrlTrackSize);

		sprintf(outfile, "song%d.mid", songNum);
		fwrite(ctrlMidData, ctrlMidPos, 1, mid);
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			fwrite(multiMidData[curTrack], midPosM[curTrack], 1, mid);
		}

		free(multiMidData[0]);
		fclose(mid);
	}
}
