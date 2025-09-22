/*Climax/Images Software (IMEDSNES)*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE *imed, * spc, * mod;
char outfile[1000000];
unsigned char* imedData;
unsigned char* modData;
unsigned char* spcData;
long patList;
long patDir;
long sampData;
int numInst;
long imedLength;
long modLength;
int songNum;
long songPtr;
int numSongs;
int fileExit;
int exitError;
int i, j;

char spcString[12];

char* tempPnt;
char OutFileBase[0x100];

/*Note frequency table*/
int noteVals[60] = { 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57,
53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30, 28 };

/*Our sample*/
const unsigned char sineWave[64] = { 0x00, 0x00, 0x00, 0x03, 0x06, 0x0A, 0x0E, 0x11, 0x15, 0x17, 0x1A,
0x1D, 0x20, 0x23, 0x27, 0x2A, 0x2C, 0x30, 0x33, 0x35, 0x38, 0x3B, 0x3E, 0x41, 0x43, 0x46, 0x4A, 0x4C,
0x4F, 0x52, 0x55, 0x58, 0x59, 0x59, 0x54, 0x4F, 0x4B, 0x48, 0x44, 0x41, 0x3E, 0x3B, 0x39, 0x37, 0x35,
0x33, 0x30, 0x2E, 0x2C, 0x29, 0x26, 0x22, 0x1F, 0x1B, 0x18, 0x12, 0x0B, 0x05, 0xFE, 0xF8, 0xF2, 0xEC,
0xE7, 0xE4 };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
static void WriteLE16(unsigned char* buffer, unsigned int value);
static void WriteLE24(unsigned char* buffer, unsigned long value);
static void WriteLE32(unsigned char* buffer, unsigned long value);
void imed2mod(long ptr);

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

static void WriteLE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0x00FF) >> 0;
	buffer[0x01] = (value & 0xFF00) >> 8;

	return;
}

static void WriteLE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0x0000FF) >> 0;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0xFF0000) >> 16;

	return;
}

static void WriteLE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0x000000FF) >> 0;
	buffer[0x01] = (value & 0x0000FF00) >> 8;
	buffer[0x02] = (value & 0x00FF0000) >> 16;
	buffer[0x03] = (value & 0xFF000000) >> 24;

	return;
}

int main(int args, char* argv[])
{
	if (args < 2)
	{
		printf("Usage: SNESClimax <spc>\n");
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

				/*Get pointer to music data*/
				songPtr = ReadLE16(&spcData[0x0015]);
				printf("Song address: 0x%04X\n", songPtr);
				patList = ReadLE16(&spcData[songPtr + 0x0008]);
				printf("Pattern list: 0x%04X\n", patList);
				patDir = ReadLE16(&spcData[songPtr + 0x000A]);
				printf("Pattern directory: 0x%04X\n", patDir);
				sampData = ReadLE16(&spcData[songPtr + 0x000C]);
				printf("Sample list: 0x%04X\n", sampData);
				imed2mod(songPtr);


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

void imed2mod(long ptr)
{
	unsigned char mask[2];
	int curPat = 0;
	long curPtr;
	long nextPtr;
	unsigned char command[4];
	long imedPos = 0;
	long dataPos = 0;
	long modPos = 0;
	long tempPos = 0;
	int channels = 4;
	int curNote = 0;
	int totalPos = 0;
	int highestPos = 0;
	int curRow = 0;
	int i = 0;
	int tempVal;
	int maskArray[16];
	int curPattern[16];
	int timeVal = 0;
	int cellsLeft = 4;
	int patNumber;
	int numBeats;
	int curCell;
	int endPat;
	int shiftCnt;

	modLength = 0x100000;
	modData = ((unsigned char*)malloc(modLength));

	for (i = 0; i < modLength; i++)
	{
		modData[i] = 0;
	}

	sprintf(outfile, "%s_converted.mod", OutFileBase);
	if ((mod = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file %s_converted.mod!\n", OutFileBase);
		exit(2);
	}
	else
	{

		/*Write MOD header*/
		modPos = 0;
		sprintf((char*)&modData[modPos], "IMED2MOD conversion");
		modPos += 20;

		/*Write sample/instrument parameters*/
		for (i = 0; i < 31; i++)
		{
			sprintf((char*)&modData[modPos], "sinewave");
			modPos += 22;
			/*Sample length*/
			WriteBE16(&modData[modPos], 32);
			modPos += 2;
			/*Fine tune*/
			Write8B(&modData[modPos], 0);
			modPos++;
			/*Sample volume*/
			Write8B(&modData[modPos], 64);
			modPos++;
			/*Loop point start*/
			WriteBE16(&modData[modPos], 0);
			modPos += 2;
			/*Loop point end*/
			WriteBE16(&modData[modPos], 32);
			modPos += 2;
		}

		/*Get ready for the pattern table...*/
		imedPos = patList + songPtr;
		/*Number of song positions - fill out soon*/
		Write8B(&modData[modPos], 0);
		tempPos = modPos;
		modPos++;
		/*Set this byte to 127*/
		Write8B(&modData[modPos], 127);
		modPos++;
		/*Fill in the pattern table*/
		while (spcData[imedPos] != 0xFF)
		{
			Write8B(&modData[modPos], spcData[imedPos]);
			if (spcData[imedPos] > highestPos)
			{
				highestPos = spcData[imedPos];
			}
			totalPos++;
			modPos++;
			imedPos++;
		}

		/*Finally fill in the value for total number of positions*/
		Write8B(&modData[tempPos], totalPos);
		/*Skip to end of pattern data area*/
		modPos += 128 - totalPos;
		/*Put in the initials M.K.*/
		sprintf((char*)&modData[modPos], "M.K.");
		modPos += 4;

		/*Now for the actual pattern data...*/
		curPtr = patDir + songPtr;
		for (curPat = 0; curPat < highestPos + 1; curPat++)
		{
			curPtr = patDir + songPtr + ReadLE16(&spcData[patDir + songPtr + (curPat * 2)]);
			nextPtr = patDir + songPtr + ReadLE16(&spcData[patDir + songPtr + (curPat * 2)] + 2);
			imedPos = curPtr;
			curRow = 0;
			numBeats = 0;
			curCell = 0;

			while (curCell < 256)
			{
				command[0] = spcData[imedPos];
				command[1] = spcData[imedPos + 1];
				command[2] = spcData[imedPos + 2];
				command[3] = spcData[imedPos + 3];

				if (command[0] == 0xFF)
				{
					if (spcData[imedPos + 1] == 0xC5)
					{
						curRow = 0;
					}
					curRow = spcData[imedPos + 1] / 8;
					tempVal = spcData[imedPos + 1] % 8;

					if (((curRow * 4) + tempVal + curCell) >= 256)
					{
						curRow = (256 - curCell) / 4;
						tempVal = (256 - curCell) % 8;
					}

					for (j = 0; j < curRow; j++)
					{
						modPos += 16;
						curCell += 4;
					}
					if (tempVal >= 0x04)
					{
						for (i = 0; i < (tempVal - 4); i++)
						{
							modPos += 4;
							curCell++;
						}
					}
					else
					{
						for (i = 0; i < tempVal; i++)
						{
							modPos += 4;
							curCell++;
						}
					}

					imedPos += 2;
				}
				else
				{
					curPattern[0] = command[0];
					curPattern[1] = command[1];
					curPattern[2] = command[2];
					curPattern[3] = command[3];

					/*Transfer the current IMED module data to MOD*/

					/*Channel note*/
					if (curPattern[0] != 0)
					{
						curNote = noteVals[curPattern[0] - 1];
						WriteBE16(&modData[modPos], curNote);
					}
					else if (curPattern[0] == 0)
					{
						Write8B(&modData[modPos], 0);
					}
					modPos += 2;
					/*Channel instrument*/
					if (curPattern[1] != 0)
					{
						Write8B(&modData[modPos], (curPattern[1]) << 4);

						if (curPattern[1] >= 16)
						{
							tempVal = modData[modPos - 2];
							tempVal = tempVal | (curPattern[1] & 0xF0);
							Write8B(&modData[modPos - 2], tempVal);
						}
					}
					else if (curPattern[1] == 0)
					{
						Write8B(&modData[modPos], 0);
					}
					modPos++;
					/*Channel effect*/
					if (curPattern[2] != 0)
					{
						tempVal = modData[modPos - 1];
						tempVal = tempVal | curPattern[2];
						Write8B(&modData[modPos - 1], tempVal);
					}
					else if (curPattern[2] == 0)
					{
						;
					}
					/*Channel effect parameters*/
					if (curPattern[3] != 0)
					{
						Write8B(&modData[modPos], curPattern[3]);
					}
					else if (curPattern[3] == 0)
					{
						Write8B(&modData[modPos], 0);
					}
					modPos++;
					imedPos += 4;
					curCell++;
				}
			}
		}

		for (i = 0; i < 31; i++)
		{
			int j;
			for (j = 0; j < 64; j++)
			{
				Write8B(&modData[modPos + j], sineWave[j]);
			}
			modPos += 64;
		}
		fwrite(modData, modPos, 1, mod);
		fclose(mod);
	}


}
