/*Sculptured Software (Berlioz)*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * mid;
int foundTable;
int curInst;
int curVol;
long tablePtrLoc;
long tableOffset;
int i, j;
char outfile[1000000];
int songNum;
long seqPtrs[16];
long songPtr;
long songOfs;
long patOfs;
long seqOfs;
long instOfs;
long melOfs;
int ctrl;
int rem;
int newSong;
int songBase;
int foundSong;
int selSong;
int fmtOverride;
int invalid;
int testNum;
int testNum2;
long testPtr;

char spcString[12];

int drvVers;

unsigned char* spcData;
unsigned char* midData;
unsigned char* ctrlMidData;

long midLength;

char* tempPnt;
char OutFileBase[0x100];

const unsigned char MagicBytesA[8] = { 0x8F, 0x00, 0x06, 0x1C, 0x2B, 0x06, 0x60, 0x96 };
const unsigned char MagicBytesB[8] = { 0x8F, 0x00, 0x0E, 0x1C, 0x2B, 0x0E, 0x60, 0x96 };
const unsigned char MagicBytesB2[8] = { 0x8F, 0x00, 0x07, 0x1C, 0x2B, 0x07, 0x60, 0x96 };
const unsigned char MagicBytesB3[8] = { 0x8F, 0x00, 0x0B, 0x1C, 0x2B, 0x0B, 0x60, 0x96 };
const unsigned char MagicBytesC[8] = { 0x8F, 0x00, 0x1F, 0x1C, 0x2B, 0x1F, 0x60, 0x96 };
const unsigned char MagicBytesC2[8] = { 0x8F, 0x00, 0x20, 0x1C, 0x2B, 0x20, 0x60, 0x96 };
const unsigned char MagicBytesC3[8] = { 0x8F, 0x00, 0x21, 0x1C, 0x2B, 0x21, 0x60, 0x96 };
const unsigned char FindSong[5] = { 0x0D, 0x6F, 0x32, 0x08, 0xE4 };
const unsigned char freqValsA[240] = { 0x00, 0x18, 0x2F, 0x47, 0x5F, 0x77, 0x8F, 0xA7,
0xBF, 0xD8, 0xF0, 0x08, 0x21, 0x39, 0x52, 0x6B,
0x83, 0x9C, 0xB5, 0xCE, 0xE7, 0x00, 0x19, 0x33, 
0x4C, 0x65, 0x7F, 0x98, 0xB2, 0xCC, 0xE5, 0xFF, 
0x19, 0x33, 0x4D, 0x67, 0x82, 0x9C, 0xB6, 0xD1, 
0xEB, 0x06, 0x20, 0x3B, 0x56, 0x71, 0x8C, 0xA7, 
0xC2, 0xDD, 0xF9, 0x14, 0x2F, 0x4B, 0x67, 0x82, 
0x9E, 0xBA, 0xD6, 0xF2, 0x0E, 0x2A, 0x46, 0x63, 
0x7F, 0x9C, 0xB8, 0xD5, 0xF2, 0x0F, 0x2B, 0x48, 
0x66, 0x83, 0xA0, 0xBD, 0xDB, 0xF8, 0x16, 0x34, 
0x51, 0x6F, 0x8D, 0xAB, 0xC9, 0xE7, 0x06, 0x24, 
0x43, 0x61, 0x80, 0x9E, 0xBD, 0xDC, 0xFB, 0x1A, 
0x39, 0x59, 0x78, 0x97, 0xB7, 0xD7, 0xF6, 0x16, 
0x36, 0x56, 0x76, 0x96, 0xB7, 0xD7, 0xF7, 0x18, 
0x39, 0x59, 0x7A, 0x9B, 0xBC, 0xDD, 0xFF, 0x20, 
0x41, 0x63, 0x84, 0xA6, 0xC8, 0xEA, 0x0C, 0x2E, 
0x50, 0x72, 0x95, 0xB7, 0xDA, 0xFC, 0x1F, 0x42, 
0x65, 0x88, 0xAB, 0xCF, 0xF2, 0x16, 0x39, 0x5D, 
0x81, 0xA5, 0xC9, 0xED, 0x11, 0x35, 0x5A, 0x7E, 
0xA3, 0xC8, 0xED, 0x12, 0x37, 0x5C, 0x81, 0xA6, 
0xCC, 0xF2, 0x17, 0x3D, 0x63, 0x89, 0xAF, 0xD6, 
0xFC, 0x22, 0x49, 0x70, 0x97, 0xBE, 0xE5, 0x0C, 
0x33, 0x5A, 0x82, 0xAA, 0xD1, 0xF9, 0x21, 0x49, 
0x71, 0x9A, 0xC2, 0xEB, 0x13, 0x3C, 0x65, 0x8E, 
0xB7, 0xE0, 0x0A, 0x33, 0x5D, 0x87, 0xB0, 0xDA, 
0x04, 0x2F, 0x59, 0x84, 0xAE, 0xD9, 0x04, 0x2F, 
0x5A, 0x85, 0xB0, 0xDC, 0x07, 0x33, 0x5F, 0x8B, 
0xB7, 0xE3, 0x0F, 0x3C, 0x68, 0x95, 0xC2, 0xEF, 
0x1C, 0x49, 0x77, 0xA4, 0xD2, 0x00, 0x2E, 0x5C, 
0x8A, 0xB8, 0xE7, 0x15, 0x44, 0x73, 0xA2, 0xD1 };
const unsigned char freqValsB[240] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x21, 0x21, 0x21, 0x21, 0x21,
0x21, 0x21, 0x21, 0x21, 0x21, 0x22, 0x22, 0x22,
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
0x24, 0x24, 0x24, 0x25, 0x25, 0x25, 0x25, 0x25,
0x25, 0x25, 0x25, 0x25, 0x26, 0x26, 0x26, 0x26,
0x26, 0x26, 0x26, 0x26, 0x26, 0x27, 0x27, 0x27,
0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x28, 0x28,
0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x29, 0x29,
0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x2A,
0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2B,
0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2C,
0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2D,
0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E,
0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F,
0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30,
0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31,
0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x32,
0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35,
0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36,
0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37,
0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A,
0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B,
0x3B, 0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3E, 0x3E, 0x3E,
0x3E, 0x3E, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F };

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
void song2mid1(int songNum, long ptr);
void song2mid2(int songNum, long ptr);
void song2mid3(int songNum, long ptr);

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
	foundSong = 0;
	curInst = 0;
	curVol = 100;
	drvVers = 1;
	ctrl = 0;
	fmtOverride = 0;

	if (args < 2)
	{
		printf("Usage: SNESSculpt <spc> <force version (optional)>\n");
		printf("Versions: 1, 2, 3, 4 (3 with 2-style pointers)\n");
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
				fmtOverride = 1;
				drvVers = strtol(argv[2], NULL, 16);

				if (drvVers > 4)
				{
					printf("ERROR: Invalid version number!\n");
					exit(1);
				}
				else if (drvVers == 3)
				{
					drvVers = 3;
					ctrl = 2;
				}
				else if (drvVers == 4)
				{
					drvVers = 3;
					ctrl = 1;
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

				if (fmtOverride != 1)
				{
					
				}

				/*Try to search the RAM for directory - Version 1*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesA, 8)) && foundTable != 1)
					{
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 1;
						}

					}
				}

				/*Version 2*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesB, 8)) && foundTable != 1)
					{
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 2;
						}
					}
				}

				/*Version 2 (alt - RoboCop vs. Terminator)*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesB2, 8)) && foundTable != 1)
					{
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 2;
						}
						ctrl = 1;
					}
				}

				/*Version 2 (alt - Bugs Bunny)*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesB3, 8)) && foundTable != 1)
					{
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 2;
						}
						ctrl = 2;
					}
				}

				/*Version 2 (alt - Japanese soccer game)/Version 3*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesC, 8)) && foundTable != 1)
					{
						/*
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						drvVers = 2;
						ctrl = 3;
						*/
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 3;
						}
						else if (drvVers == 2)
						{
							ctrl = 3;
						}

					}
				}

				/*Version 3 (alt - Boogerman)*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesC2, 8)) && foundTable != 1)
					{
						/*
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						drvVers = 2;
						ctrl = 3;
						*/
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 3;
						}

					}
				}

				/*Version 3 (alt - Doom)*/
				for (i = 0; i < ramSize; i++)
				{
					if ((!memcmp(&spcData[i], MagicBytesC3, 8)) && foundTable != 1)
					{
						/*
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						drvVers = 2;
						ctrl = 3;
						*/
						tablePtrLoc = i + 8;
						printf("Found pointer to directory at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&spcData[tablePtrLoc]);
						printf("Directory starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						if (fmtOverride != 1)
						{
							drvVers = 3;
						}

					}
				}

				if (foundTable == 1)
				{
					if (drvVers == 1)
					{
						/*Get pointers to directories*/
						instOfs = ReadLE16(&spcData[tableOffset + 2]);
						printf("Sounds (instruments): 0x%04X\n", instOfs);
						seqOfs = ReadLE16(&spcData[tableOffset + 16]);
						printf("Sequences: 0x%04X\n", seqOfs);
						patOfs = ReadLE16(&spcData[tableOffset + 22]);
						printf("Channel patterns: 0x%04X\n", patOfs);
						songOfs = ReadLE16(&spcData[tableOffset + 26]);
						printf("Songs: 0x%04X\n", songOfs);
						melOfs = ReadLE16(&spcData[tableOffset + 24]);
						printf("Channel melodies: 0x%04X\n\n", melOfs);

						songNum = 1;
						i = songOfs;

						if (songOfs < melOfs)
						{
							while (i < melOfs)
							{
								songPtr = ReadLE16(&spcData[i]);
								printf("Song %i: 0x%04X\n", songNum, songPtr);
								song2mid1(songNum, songPtr);
								i += 2;
								songNum++;
							}
						}
						else if (songOfs < instOfs)
						{
							while (i < instOfs)
							{
								songPtr = ReadLE16(&spcData[i]);
								printf("Song %i: 0x%04X\n", songNum, songPtr);
								song2mid1(songNum, songPtr);
								i += 2;
								songNum++;
							}
						}
					}

					else if (drvVers == 2)
					{
						/*Get pointers to directories*/
						instOfs = ReadLE16(&spcData[tableOffset + 2]);
						printf("Sounds (instruments): 0x%04X\n", instOfs);
						seqOfs = ReadLE16(&spcData[tableOffset + 16]);
						printf("Sequences: 0x%04X\n", seqOfs);
						songOfs = ReadLE16(&spcData[tableOffset + 26]);
						printf("Songs: 0x%04X\n", songOfs);


						songNum = 1;

						if (ctrl == 0)
						{
							i = 0x70;
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid2(songNum, songPtr);
						}
						else if (ctrl == 1)
						{
							i = 0x69;
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid2(songNum, songPtr);

						}
						else if (ctrl == 2)
						{
							i = 0x6D;
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid2(songNum, songPtr);
						}
						else if (ctrl == 3)
						{
							i = 0x8C;
							songPtr = ReadLE16(&spcData[i]);
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2mid2(songNum, songPtr);
						}



						/*
 						i = songOfs;
						while (i < seqOfs && i < (songOfs + 32))
						{
							songPtr = ReadLE16(&spcData[i]);

							if (songPtr == 0x0000 || songPtr == 0xFFFF)
							{
								printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
							}
							else if (spcData[songPtr] < 20)
							{
								printf("Song %i: 0x%04X\n", songNum, songPtr);
								song2mid2(songNum, songPtr);
							}
							else
							{
								printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
							}

							i += 2;
							songNum++;

						}*/
					}
					else if (drvVers == 3)
					{
						/*Get pointers to directories*/
						instOfs = ReadLE16(&spcData[tableOffset + 2]);
						printf("Sounds (instruments): 0x%04X\n", instOfs);
						seqOfs = ReadLE16(&spcData[tableOffset + 16]);
						printf("Sequences: 0x%04X\n", seqOfs);
						songOfs = ReadLE16(&spcData[tableOffset + 26]);
						printf("Songs: 0x%04X\n", songOfs);

						if (fmtOverride != 1)
						{
							if (seqOfs > 0x3000 || seqOfs == 0x0000)
							{
								ctrl = 2;
							}
							else
							{
								ctrl = 1;
							}
						}


						/*Try to search the RAM for song address*/
						for (i = 0; i < ramSize; i++)
						{
							if ((!memcmp(&spcData[i], FindSong, 5)) && foundSong != 1)
							{
								songBase = ReadLE16(&spcData[i + 21]);
							}
						}

						i = songOfs;
						songNum = 1;
						
						while (songNum <= 80)
						{
							songPtr = ReadLE16(&spcData[i]);

							if (songPtr == 0x0000 || songPtr == 0xFFFF || songPtr >= 0xFE00)
							{
								printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
							}
							else if (spcData[songPtr] < 21)
							{
								invalid = 0;
								if (ctrl == 1)
								{
									testNum = spcData[songPtr];

									if (testNum == 0)
									{
										invalid = 1;
									}
									for (j = 0; j < testNum; j++)
									{
										testNum2 = spcData[songPtr + 1 + j];
										if (testNum >= 0x80)
										{
											invalid = 1;
										}
										testPtr = ReadLE16(&spcData[seqOfs + (testNum2 * 2)]);

										if (spcData[testPtr] == 0x00)
										{
											invalid = 1;
										}
										else if (spcData[testPtr] < 0xE0)
										{
											invalid = 1;
										}
									}

									if (invalid != 1)
									{
										printf("Song %i: 0x%04X\n", songNum, songPtr);
										song2mid3(songNum, songPtr);
									}
									else if (invalid == 1)
									{
										printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
									}
								}
								else
								{
									testNum = spcData[songPtr];

									if (songNum == 49)
									{
										songNum = 49;
									}

									if (testNum == 0)
									{
										invalid = 1;
									}
									for (j = 0; j < testNum; j++)
									{
										testPtr = ReadLE16(&spcData[songPtr + 1 + (2 * j)]);

										if (testPtr < 0x1000)
										{
											invalid = 1;
										}
										if (testPtr >= 0xFE00)
										{
											invalid = 1;
										}
										if (spcData[testPtr] < 0xE0)
										{
											invalid = 1;
										}
									}

									if (invalid != 1)
									{
										printf("Song %i: 0x%04X\n", songNum, songPtr);
										song2mid3(songNum, songPtr);
									}
									else if (invalid == 1)
									{
										printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
									}
								}

							}
							else
							{
								printf("Song %i: 0x%04X (invalid, skipped)\n", songNum, songPtr);
							}

							i += 2;
							songNum++;
						}
					}

					fclose(spc);
					free(spcData);
					printf("The operation was successfully completed!\n");
					exit(0);
				}
				else
				{
					fclose(spc);
					free(spcData);
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

/*Convert the song data to MIDI (version 1)*/
void song2mid1(int songNum, long ptr)
{
	int activeChan[16];
	long spcPos = 0;
	long seqPos = 0;
	long melPos = 0;
	long patPos = 0;
	int curTrack = 0;
	int trackCnt = 4;
	int ticks = 120;
	int tempo = 190;
	int seqEnd = 0;
	int patEnd = 0;
	int melEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int transpose = 0;
	int dirNote = 0;
	unsigned char command[6];
	unsigned char patCommand[4];
	unsigned char melCommand[4];
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
	int repeat = 0;
	long repeatStart = 0;
	long tempPos = 0;
	int holdNote = 0;
	int curSeq = 0;
	int noteVal = 0;
	int curPat = 0;
	long noteBase = 0;
	int pitchVal = 0;
	int noteOn = 0;
	int prevNote = 0;
	int gain = 0;
	int pitchBend = 0;
	int tie = 0;
	int tieLen = 0;
	int tieNote = 0;

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

	for (j = 0; j < 16; j++)
	{
		seqPtrs[j] = 0;
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
		spcPos = ptr;
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


		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			seqPtrs[curTrack] = ReadLE16(&spcData[melOfs + (spcData[spcPos]) * 2]);
			spcPos++;
		}

		/*Process each channel's melody*/
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			firstNote = 1;
			holdNote = 0;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			seqEnd = 0;
			patEnd = 0;
			melEnd = 0;

			curNote = 0;
			curNoteLen = 0;
			noteOn = 0;

			transpose = 0;
			curVol = 100;

			gain = 0;
			pitchBend = 0;

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			melPos = seqPtrs[curTrack];

			while (melEnd == 0 && midPos < 48000 && ctrlDelay < 110000)
			{
				melCommand[0] = spcData[melPos];
				melCommand[1] = spcData[melPos + 1];
				melCommand[2] = spcData[melPos + 2];
				melCommand[3] = spcData[melPos + 3];

				/*End of melody*/
				if (melCommand[0] == 0x00)
				{
					melEnd = 1;
				}

				/*Go to loop*/
				else if (melCommand[0] == 0x02)
				{
					melEnd = 1;
				}

				/*Set loop position*/
				else if (melCommand[0] == 0x04)
				{
					melPos += 2;
				}

				/*Toggle channel mute*/
				else if (melCommand[0] == 0x06)
				{
					melPos += 2;
				}

				/*Play pattern*/
				else if (melCommand[0] == 0x08)
				{
					curPat = melCommand[1];

					patPos = ReadLE16(&spcData[patOfs + (curPat * 2)]);
					patEnd = 0;

					while (patEnd == 0)
					{
						patCommand[0] = spcData[patPos];
						patCommand[1] = spcData[patPos + 1];
						patCommand[2] = spcData[patPos + 2];
						patCommand[3] = spcData[patPos + 3];

						/*End of pattern*/
						if (patCommand[0] == 0x00 || patCommand[0] == 0x16)
						{
							patEnd = 1;
						}

						/*Play phrase*/
						else if (patCommand[0] == 0x02)
						{
							seqEnd = 0;
							curSeq = patCommand[1];
							seqPos = ReadLE16(&spcData[seqOfs + (curSeq * 2)]);
							holdNote = 0;
							pitchBend = 0;
							tie = 0;

							/*Get base note*/
							noteBase = (0x01A4 + ReadLE16(&spcData[seqPos])) & 0xFFFF;
							noteBase = (noteBase + transpose) & 0xFFFF;
							curNote = noteBase / 20;
							rem = noteBase % 20;

							if (rem >= 15)
							{
								curNote++;
							}

							if (curNote < 116)
							{
								curNote += 12;
							}
							seqPos += 2;

							seqEnd = 0;

							while (seqEnd == 0)
							{

								command[0] = spcData[seqPos];
								command[1] = spcData[seqPos + 1];
								command[2] = spcData[seqPos + 2];
								command[3] = spcData[seqPos + 3];
								command[4] = spcData[seqPos + 4];
								command[5] = spcData[seqPos + 5];

								/*Play note/etc.*/
								if (command[0] < 0xF0)
								{
									if (command[0] < 0x80)
									{
										noteVal = command[0] & 0x1F;


										switch (noteVal)
										{
										case 0x00:
											curNote -= 36;
											break;
										case 0x01:
											curNote -= 24;
											break;
										case 0x02:
											curNote -= 17;
											break;
										case 0x03:
											curNote -= 12;
											break;
										case 0x04:
											curNote -= 11;
											break;
										case 0x05:
											curNote -= 10;
											break;
										case 0x06:
											curNote -= 9;
											break;
										case 0x07:
											curNote -= 8;
											break;
										case 0x08:
											curNote -= 7;
											break;
										case 0x09:
											curNote -= 6;
											break;
										case 0x0A:
											curNote -= 5;
											break;
										case 0x0B:
											curNote -= 4;
											break;
										case 0x0C:
											curNote -= 3;
											break;
										case 0x0D:
											curNote -= 2;
											break;
										case 0x0E:
											curNote -= 1;
											break;
										case 0x0F:
											break;
										case 0x10:
											curNote += 1;
											break;
										case 0x11:
											curNote += 2;
											break;
										case 0x12:
											curNote += 3;
											break;
										case 0x13:
											curNote += 4;
											break;
										case 0x14:
											curNote += 5;
											break;
										case 0x15:
											curNote += 6;
											break;
										case 0x16:
											curNote += 7;
											break;
										case 0x17:
											curNote += 8;
											break;
										case 0x18:
											curNote += 9;
											break;
										case 0x19:
											curNote += 10;
											break;
										case 0x1A:
											curNote += 11;
											break;
										case 0x1B:
											curNote += 12;
											break;
										case 0x1C:
											curNote += 17;
											break;
										case 0x1D:
											curNote += 24;
											break;
										case 0x1E:
											curNote += 36;
											break;
										case 0x1F:
											curNote += 48;
											break;
										default:
											break;
										}
									}

									if (curNote < 0)
									{
										curNote = 1;
									}

									else if (curNote >= 128)
									{
										curNote = 30;
									}

									curNoteLen = command[1] * 8;

									/*Pitch bend*/
									if (command[0] >= 0x80)
									{

										if (holdNote == 1)
										{
											tempPos = WriteDeltaTime(midData, midPos, curDelay);
											midPos += tempPos;
											Write8B(&midData[midPos], (0xE0 | curTrack));
											Write8B(&midData[midPos + 1], 0);
											Write8B(&midData[midPos + 2], 0x40);
											curDelay = 0;
											firstNote = 1;
											midPos += 3;
										}
										curDelay += (command[1] * 8);
										seqPos += 2;
									}

									/*Play note*/
									else if (command[0] >= 0x20 && command[1] != 0x00)
									{
										if (holdNote == 1)
										{
											tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
											curDelay = 0;
											holdNote = 0;
											midPos = tempPos;
										}
										tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
										prevNote = curNote;
										curDelay = 0;
										firstNote = 0;
										holdNote = 1;
										midPos = tempPos;

										curDelay += curNoteLen;

										seqPos += 2;
									}

									/*No new note*/
									else
									{
										if (holdNote == 1)
										{
											tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
											curDelay = 0;
											holdNote = 0;
											midPos = tempPos;
										}
										if (command[1] != 0x00)
										{
											tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
											prevNote = curNote;
											curDelay = 0;
											firstNote = 0;
											holdNote = 1;
											midPos = tempPos;
											tieNote = curNote;
											tie = 1;
										}
										curDelay += curNoteLen;
										seqPos += 2;
									}
								}

								/*End of phrase*/
								else if (command[0] == 0xF0)
								{
									if (holdNote == 1)
									{
										tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
										curDelay = 0;
										holdNote = 0;
										midPos = tempPos;
									}
									seqEnd = 1;
									patPos += 2;
								}

								/*Set volume (relative)*/
								else if (command[0] == 0xF1)
								{
									curVol += (command[1] * 2);

									if (curVol == 0)
									{
										curVol = 1;
									}
									else if (curVol > 100)
									{
										curVol = 100;
									}
									seqPos += 2;
								}

								/*Set volume (absolute)*/
								else if (command[0] == 0xF2)
								{
									curVol = (gain + command[1]) * 2;

									if (curVol == 0)
									{
										curVol = 1;
									}
									else if (curVol > 100)
									{
										curVol = 100;
									}
									seqPos += 2;
								}

								/*Mute channel*/
								else if (command[0] == 0xF3)
								{
									if (holdNote == 1)
									{
										tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
										curDelay = 0;
										holdNote = 0;
										midPos = tempPos;
									}
									curDelay += (command[1] * 8);
									seqPos += 2;
								}

								/*Rest*/
								else if (command[0] == 0xF4)
								{
									if (holdNote == 1)
									{
										tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
										curDelay = 0;
										holdNote = 0;
										midPos = tempPos;
									}
									curDelay += (command[1] * 8);
									seqPos += 2;
								}

								/*Unknown command*/
								else
								{
									seqPos += 2;
								}
							}
						}

						/*Trigger another pattern?*/
						else if (patCommand[0] == 0x04)
						{
							printf("Warning: Unknown pattern command 04 at address 0x%04X!\n", patPos);
							patPos += 2;
						}


						/*Set channel tuning/transpose*/
						else if (patCommand[0] == 0x06)
						{
							transpose = ReadLE16(&spcData[patPos + 1]);
							patPos += 3;
						}

						/*Set instrument/sound*/
						else if (patCommand[0] == 0x08)
						{
							if (patCommand[1] != curInst)
							{
								curInst = patCommand[1];
								firstNote = 1;
							}
							patPos += 2;
						}

						/*Set channel pitch envelope*/
						else if (patCommand[0] == 0x0A)
						{
							patPos += 2;
						}

						/*Set channel panning (relative)?*/
						else if (patCommand[0] == 0x0C)
						{
							patPos += 2;
						}

						/*Set channel panning (absolute)?*/
						else if (patCommand[0] == 0x0E)
						{
							patPos += 3;
						}

						/*Set channel gain (relative?)*/
						else if (patCommand[0] == 0x10)
						{
							gain += patCommand[1];
							patPos += 2;
						}

						/*Set channel gain (absolute)*/
						else if (patCommand[0] == 0x12)
						{
							gain = patCommand[1];
							patPos += 2;
						}

						/*Play sound effect?*/
						else if (patCommand[0] == 0x14)
						{
							printf("Warning: Unknown pattern command 14 at address 0x%04X!\n", patPos);
							patPos += 2;
						}

						/*Unknown command*/
						else
						{
							patPos += 2;
						}
					}

					melPos += 2;
				}

				/*Set channel ID*/
				else if (melCommand[0] == 0x0A)
				{
					melPos += 2;
				}

				/*Re-map channel?*/
				else if (melCommand[0] == 0x0C)
				{
					melPos += 2;
				}

				/*Unknown command*/
				else
				{
					melPos += 2;
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

/*Convert the song data to MIDI (version 2)*/
void song2mid2(int songNum, long ptr)
{
	int activeChan[16];
	long spcPos = 0;
	long seqPos = 0;
	long melPos = 0;
	long patPos = 0;
	int curTrack = 0;
	int trackCnt = 4;
	int ticks = 120;
	int tempo = 190;
	int seqEnd = 0;
	int patEnd = 0;
	int melEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int transpose = 0;
	int dirNote = 0;
	unsigned char command[6];
	unsigned char patCommand[4];
	unsigned char melCommand[4];
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
	int repeat = 0;
	long repeatStart = 0;
	long tempPos = 0;
	int holdNote = 0;
	int curSeq = 0;
	int noteVal = 0;
	int curPat = 0;
	long noteBase = 0;
	int pitchVal = 0;
	int pitchAdd = 0;
	int noteOn = 0;
	int prevNote = 0;
	int gain = 0;
	int pitchBend = 0;
	long macros[16][12];
	int inMacro = 0;
	int absNote = 0;
	int tempVol = 0;
	float multiplier = 0;
	int rem;
	long insPtr = 0;

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

	for (j = 0; j < 16; j++)
	{
		seqPtrs[j] = 0;
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
		spcPos = ptr;
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

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			seqPtrs[curTrack] = ReadLE16(&spcData[seqOfs + (spcData[spcPos]) * 2]);
			spcPos++;
		}

		/*Process each channel's melody*/
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			firstNote = 1;
			holdNote = 0;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			seqEnd = 0;

			curNote = 48;
			curNoteLen = 0;
			noteOn = 0;

			transpose = 0;
			curVol = 100;
			tempVol = 100;

			gain = 0;
			pitchBend = 0;
			pitchVal = 1200;
			pitchAdd = 0;
			absNote = 0;
			inMacro = 0;
			multiplier = 1;

			for (j = 0; j < 16; j++)
			{
				macros[j][0] = -1;
				macros[j][1] = -1;
				macros[j][2] = -1;
				macros[j][3] = -1;
				macros[j][4] = -1;
				macros[j][5] = -1;
				macros[j][6] = -1;
				macros[j][7] = -1;
				macros[j][8] = -1;
				macros[j][9] = -1;
				macros[j][10] = -1;
				macros[j][11] = -1;
			}

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			seqPos = seqPtrs[curTrack];

			while (seqEnd == 0)
			{
				command[0] = spcData[seqPos];
				command[1] = spcData[seqPos + 1];
				command[2] = spcData[seqPos + 2];
				command[3] = spcData[seqPos + 3];
				command[4] = spcData[seqPos + 4];
				command[5] = spcData[seqPos + 5];

				while (seqPos == macros[inMacro][1])
				{
					macros[inMacro][10] = 0;
					if (macros[inMacro][2] > 1)
					{
						seqPos = macros[inMacro][0];
						macros[inMacro][2]--;
					}
					else
					{
						transpose = macros[inMacro][7];
						curVol = macros[inMacro][8];
						seqPos = macros[inMacro][6];
						inMacro--;
					}
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];
					command[4] = spcData[seqPos + 4];
					command[5] = spcData[seqPos + 5];
				}

				/*Play note/etc.*/
				if (command[0] < 0xF0)
				{
					if (absNote == 1)
					{
						curNote = command[0] + 33;
						if (holdNote == 1)
						{
							tempPos = WriteDeltaTime(midData, midPos, curDelay);
							midPos += tempPos;
							Write8B(&midData[midPos], (0xE0 | curTrack));
							Write8B(&midData[midPos + 1], 0);
							Write8B(&midData[midPos + 2], 0x40);
							curDelay = 0;
							firstNote = 1;
							midPos += 3;
						}
						curDelay += curNoteLen;
						ctrlDelay += curNoteLen;
						seqPos += 2;
					}
					else
					{
						if (command[0] < 0x80)
						{
							noteVal = command[0] & 0x1F;


							switch (noteVal)
							{
							case 0x00:
								curNote -= 36;
								pitchAdd = 0xFD30;
								break;
							case 0x01:
								curNote -= 24;
								pitchAdd = 0xFE20;
								break;
							case 0x02:
								curNote -= 17;
								pitchAdd = 0xFEAC;
								break;
							case 0x03:
								curNote -= 12;
								pitchAdd = 0xFF10;
								break;
							case 0x04:
								curNote -= 11;
								pitchAdd = 0xFF24;
								break;
							case 0x05:
								curNote -= 10;
								pitchAdd = 0xFF38;
								break;
							case 0x06:
								curNote -= 9;
								pitchAdd = 0xFF4C;
								break;
							case 0x07:
								curNote -= 8;
								pitchAdd = 0xFF60;
								break;
							case 0x08:
								curNote -= 7;
								pitchAdd = 0xFF74;
								break;
							case 0x09:
								curNote -= 6;
								pitchAdd = 0xFF88;
								break;
							case 0x0A:
								curNote -= 5;
								pitchAdd = 0xFF9C;
								break;
							case 0x0B:
								curNote -= 4;
								pitchAdd = 0xFFB0;
								break;
							case 0x0C:
								curNote -= 3;
								pitchAdd = 0xFFC4;
								break;
							case 0x0D:
								curNote -= 2;
								pitchAdd = 0xFFD8;
								break;
							case 0x0E:
								curNote -= 1;
								pitchAdd = 0xFFEC;
								break;
							case 0x0F:
								pitchAdd = 0x0000;
								break;
							case 0x10:
								curNote += 1;
								pitchAdd = 0x0014;
								break;
							case 0x11:
								curNote += 2;
								pitchAdd = 0x0028;
								break;
							case 0x12:
								curNote += 3;
								pitchAdd = 0x003C;
								break;
							case 0x13:
								curNote += 4;
								pitchAdd = 0x0050;
								break;
							case 0x14:
								curNote += 5;
								pitchAdd = 0x0064;
								break;
							case 0x15:
								curNote += 6;
								pitchAdd = 0x0078;
								break;
							case 0x16:
								curNote += 7;
								pitchAdd = 0x008C;
								break;
							case 0x17:
								curNote += 8;
								pitchAdd = 0x00A0;
								break;
							case 0x18:
								curNote += 9;
								pitchAdd = 0x00B4;
								break;
							case 0x19:
								curNote += 10;
								pitchAdd = 0x00C8;
								break;
							case 0x1A:
								curNote += 11;
								pitchAdd = 0x00DC;
								break;
							case 0x1B:
								curNote += 12;
								pitchAdd = 0x00F0;
								break;
							case 0x1C:
								curNote += 17;
								pitchAdd = 0x0154;
								break;
							case 0x1D:
								curNote += 24;
								pitchAdd = 0x01E0;
								break;
							case 0x1E:
								curNote += 36;
								pitchAdd = 0x02D0;
								break;
							case 0x1F:
								curNote += 48;
								pitchAdd = 0x03C0;
								break;
							default:
								pitchAdd = 0x0000;
								break;
							}

							pitchVal = (pitchVal + pitchAdd) & 0xFFFF;

							pitchVal = (pitchVal + transpose) & 0xFFFF;

							curNote = pitchVal / 20;

							rem = pitchVal % 20;

							if (rem >= 15)
							{
								curNote++;
							}

							transpose = 0;


							if (curNote < 0)
							{
								curNote = 1;
							}

							else if (curNote >= 128)
							{
								curNote = 30;
							}
						}

						curNoteLen = command[1] * 8;

						/*Pitch bend*/
						if (command[0] >= 0x80)
						{

							if (holdNote == 1)
							{
								tempPos = WriteDeltaTime(midData, midPos, curDelay);
								midPos += tempPos;
								Write8B(&midData[midPos], (0xE0 | curTrack));
								Write8B(&midData[midPos + 1], 0);
								Write8B(&midData[midPos + 2], 0x40);
								curDelay = 0;
								firstNote = 1;
								midPos += 3;
							}
							curDelay += (command[1] * 8);
							ctrlDelay += (command[1] * 8);
							seqPos += 2;
						}

						/*Play note*/
						else if (command[0] >= 0x20 && command[1] != 0x00)
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
							prevNote = curNote;
							curDelay = 0;
							firstNote = 0;
							holdNote = 1;
							midPos = tempPos;
							curDelay += curNoteLen;
							ctrlDelay += curNoteLen;
							seqPos += 2;
						}

						/*No new note*/
						else
						{
							if (holdNote == 1)
							{
								tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								curDelay = 0;
								holdNote = 0;
								midPos = tempPos;
							}
							if (command[1] != 0x00)
							{
								tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
								prevNote = curNote;
								curDelay = 0;
								firstNote = 0;
								holdNote = 1;
								midPos = tempPos;
							}
							curDelay += curNoteLen;
							ctrlDelay += curNoteLen;
							seqPos += 2;
						}
					}

				}

				/*End of phrase*/
				else if (command[0] == 0xF0)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					seqEnd = 1;
				}

				/*Set volume (relative)*/
				else if (command[0] == 0xF1)
				{
					multiplier = ReadLE16(&spcData[seqPos + 1]);
					multiplier /= 256;

					tempVol = tempVol * multiplier;

					if (tempVol > 0x7F)
					{
						tempVol = 0x7F;
					}

					curVol = tempVol + 0x19;

					if (curVol == 0)
					{
						curVol = 1;
					}
					else if (curVol > 100)
					{
						curVol = 100;
					}
					seqPos += 3;
				}

				/*Set volume (absolute)*/
				else if (command[0] == 0xF2)
				{
					tempVol = command[1];
					curVol = tempVol + 0x19;

					if (curVol == 0)
					{
						curVol = 1;
					}
					else if (curVol > 100)
					{
						curVol = 100;
					}
					seqPos += 2;
				}

				/*Mute channel*/
				else if (command[0] == 0xF3)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curDelay += (command[1] * 8);
					ctrlDelay += (command[1] * 8);
					seqPos += 2;
				}

				/*Rest*/
				else if (command[0] == 0xF4)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curDelay += (command[1] * 8);
					ctrlDelay += (command[1] * 8);
					seqPos += 2;
				}

				/*Set sound/instrument*/
				else if (command[0] == 0xF5)
				{
					if (inMacro > 0)
					{
						macros[inMacro][10]++;

						seqPos + 11 + macros[inMacro][5];
						if (curInst != spcData[macros[inMacro][9] + macros[inMacro][10]])
						{
							if (spcData[macros[inMacro][9] + macros[inMacro][10]] != 0xFF)
							{
								curInst = command[1];

								insPtr = ReadLE16(&spcData[instOfs + (curInst * 2)]);

								if (spcData[insPtr] >= 0x40)
								{
									absNote = 1;
								}
								firstNote = 1;
							}

						}
					}
					else if (inMacro == 0)
					{
						if (curInst != command[1])
						{
							curInst = command[1];

							insPtr = ReadLE16(&spcData[instOfs + (curInst * 2)]);

							if (spcData[insPtr] >= 0x40)
							{
								absNote = 1;
							}
							firstNote = 1;
						}
					}

					seqPos += 2;
				}

				/*Call phrase*/
				else if (command[0] == 0xF6)
				{
					inMacro++;

					if (inMacro >= 16)
					{
						seqEnd = 1;
					}
					else
					{
						/*Phrase start*/
						macros[inMacro][0] = ReadLE16(&spcData[seqPos + 1]);
						/*Phrase end*/
						macros[inMacro][1] = ReadLE16(&spcData[seqPos + 3]);
						/*Number of times*/
						macros[inMacro][2] = spcData[seqPos + 5];
						/*Transpose*/
						macros[inMacro][3] = ReadLE16(&spcData[seqPos + 6]);
						/*Volume scale*/
						macros[inMacro][4] = ReadLE16(&spcData[seqPos + 8]);
						/*Number of instrument changes*/
						macros[inMacro][5] = spcData[seqPos + 10];
						/*Return position*/
						macros[inMacro][6] = seqPos + 11 + macros[inMacro][5];

						/*Original transpose*/
						macros[inMacro][7] = transpose;
						/*Original volume*/
						macros[inMacro][8] = curVol;
						/*Instrument position*/
						macros[inMacro][9] = seqPos + 11;
						/*Current instrument*/
						macros[inMacro][10] = 0;

						seqPos = macros[inMacro][0];
						transpose = macros[inMacro][3];
						multiplier = macros[inMacro][4];
						multiplier /= 256;

						tempVol = tempVol * multiplier;

						if (curVol > 0x7F)
						{
							curVol = 0x7F;
						}

						curVol = tempVol + 0x19;

						if (curVol == 0)
						{
							curVol = 1;
						}
						else if (curVol > 100)
						{
							curVol = 100;
						}
					}


				}

				/*Set detune*/
				else if (command[0] == 0xF7 || command[0] == 0xF8)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					pitchVal = (ReadLE16(&spcData[seqPos + 1]) + 660) & 0xFFFF;
					pitchVal = (pitchVal + transpose) & 0xFFFF;

					if (pitchVal >= 0x8000)
					{
						pitchVal = (pitchVal + 0x00F0) & 0xFFFF;
					}

					curNote = pitchVal / 20;

					rem = pitchVal % 20;

					if (rem >= 15)
					{
						curNote++;
					}

					transpose = 0;
					curNoteLen = (command[3] * 8);

					if (command[3] != 0x00)
					{
						tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						prevNote = curNote;
						curDelay = 0;
						firstNote = 0;
						holdNote = 1;
						midPos = tempPos;
					}

					curDelay += curNoteLen;
					ctrlDelay += curNoteLen;
					seqPos += 4;
				}

				/*Go to loop*/
				else if (command[0] == 0xF9)
				{
					seqEnd = 1;
				}

				/*Set tempo*/
				else if (command[0] == 0xFA)
				{
					ctrlMidPos++;
					valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
					ctrlDelay = 0;
					ctrlMidPos += valSize;
					WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
					ctrlMidPos += 3;
					if (command[1] == 0x00)
					{
						tempo = 256 * 0.8;
					}
					else
					{
						tempo = command[1] * 0.8;
					}

					if (tempo < 2)
					{
						tempo = 190;
					}

					WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
					ctrlMidPos += 2;
					seqPos += 4;
				}

				/*Set panning*/
				else if (command[0] == 0xFB)
				{
					seqPos += 2;
				}

				/*No note retrigger?*/
				else if (command[0] == 0xFC)
				{
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

/*Convert the song data to MIDI (version 3)*/
void song2mid3(int songNum, long ptr)
{
	int activeChan[16];
	long spcPos = 0;
	long seqPos = 0;
	long melPos = 0;
	long patPos = 0;
	int curTrack = 0;
	int trackCnt = 4;
	int ticks = 120;
	int tempo = 190;
	int seqEnd = 0;
	int patEnd = 0;
	int melEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int transpose = 0;
	int dirNote = 0;
	unsigned char command[6];
	unsigned char patCommand[4];
	unsigned char melCommand[4];
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
	int repeat = 0;
	long repeatStart = 0;
	long tempPos = 0;
	int holdNote = 0;
	int curSeq = 0;
	int noteVal = 0;
	int curPat = 0;
	long noteBase = 0;
	int pitchVal = 0;
	int pitchAdd = 0;
	int noteOn = 0;
	int prevNote = 0;
	int gain = 0;
	int pitchBend = 0;
	long macros[16][12];
	int inMacro = 0;
	int absNote = 0;
	int tempVol = 0;
	float multiplier = 0;
	int rem;
	long insPtr = 0;
	int timeAmt = 0;
	int pbState = 0;

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

	for (j = 0; j < 16; j++)
	{
		seqPtrs[j] = 0;
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
		spcPos = ptr;
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

		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			if (ctrl == 1)
			{
				seqPtrs[curTrack] = ReadLE16(&spcData[seqOfs + (spcData[spcPos]) * 2]);
				spcPos++;
			}
			else
			{
				seqPtrs[curTrack] = ReadLE16(&spcData[spcPos]);
				spcPos += 2;
			}

		}

		/*Process each channel's melody*/
		for (curTrack = 0; curTrack < trackCnt; curTrack++)
		{
			firstNote = 1;
			holdNote = 0;
			/*Write MIDI chunk header with "MTrk"*/
			WriteBE32(&midData[midPos], 0x4D54726B);
			midPos += 8;
			midTrackBase = midPos;

			curDelay = 0;
			ctrlDelay = 0;
			seqEnd = 0;

			curNote = 48;
			curNoteLen = 0;
			noteOn = 0;

			transpose = 0;
			curVol = 100;
			tempVol = 100;

			gain = 0;
			pitchBend = 0;
			pitchVal = 1200;
			pitchAdd = 0;
			absNote = 0;
			inMacro = 0;
			multiplier = 1;
			timeAmt = 4;
			pbState = 0;

			for (j = 0; j < 16; j++)
			{
				macros[j][0] = -1;
				macros[j][1] = -1;
				macros[j][2] = -1;
				macros[j][3] = -1;
				macros[j][4] = -1;
				macros[j][5] = -1;
				macros[j][6] = -1;
				macros[j][7] = -1;
				macros[j][8] = -1;
				macros[j][9] = -1;
				macros[j][10] = -1;
				macros[j][11] = -1;
			}

			/*Calculate MIDI channel size*/
			trackSize = midPos - midTrackBase;
			WriteBE16(&midData[midTrackBase - 2], trackSize);

			seqPos = seqPtrs[curTrack];

			while (seqEnd == 0 && midPos < 48000 && ctrlDelay < 110000)
			{
				command[0] = spcData[seqPos];
				command[1] = spcData[seqPos + 1];
				command[2] = spcData[seqPos + 2];
				command[3] = spcData[seqPos + 3];
				command[4] = spcData[seqPos + 4];
				command[5] = spcData[seqPos + 5];

				while (seqPos == macros[inMacro][1])
				{
					macros[inMacro][10] = 0;
					if (macros[inMacro][2] > 1)
					{
						seqPos = macros[inMacro][0];
						macros[inMacro][2]--;
					}
					else
					{
						transpose = macros[inMacro][7];
						curVol = macros[inMacro][8];
						seqPos = macros[inMacro][6];
						inMacro--;
					}
					command[0] = spcData[seqPos];
					command[1] = spcData[seqPos + 1];
					command[2] = spcData[seqPos + 2];
					command[3] = spcData[seqPos + 3];
					command[4] = spcData[seqPos + 4];
					command[5] = spcData[seqPos + 5];
				}

				/*Play note (relative)*/
				if (command[0] <= 0x7F)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curNote = (command[0] / 8) - 8 + curNote;
					curNoteLen = ((command[0] & 0x07) + 1) * 8;
					pitchVal = 20 * curNote;
					curNote = pitchVal / 20;

					tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
					prevNote = curNote;
					curDelay = 0;
					firstNote = 0;
					holdNote = 1;
					midPos = tempPos;
					curDelay += curNoteLen;
					ctrlDelay += curNoteLen;

					seqPos++;
				}
				
				/*Play note (absolute)*/
				else if (command[0] >= 0x80 && command[0] <= 0xBF)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curNote = command[0] - 0x60;
					curNoteLen = (command[1]) * 8;
					pitchVal = ((20 * curNote) + transpose) & 0xFFFF;
					curNote = pitchVal / 20;
					rem = pitchVal % 20;
					if (rem >= 7)
					{
						curNote++;
					}

					tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
					prevNote = curNote;
					curDelay = 0;
					firstNote = 0;
					holdNote = 1;
					midPos = tempPos;
					curDelay += curNoteLen;
					ctrlDelay += curNoteLen;

					seqPos += 2;
				}

				/*Rest*/
				else if (command[0] >= 0xC0 && command[0] <= 0xDF)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curDelay += ((command[0] - 0xBF) * 8);
					seqPos++;
				}

				/*Set hardware detune*/
				else if (command[0] == 0xE0)
				{
					seqPos += 3;
				}

				/*Set amplitide articulation*/
				else if (command[0] == 0xE1)
				{
					seqPos += 2;
				}

				/*Set stereo articulation*/
				else if (command[0] == 0xE2)
				{
					seqPos += 2;
				}

				/*Set tone articulation*/
				else if (command[0] == 0xE3)
				{
					seqPos += 2;
				}

				/*Set pitch articulation*/
				else if (command[0] == 0xE4)
				{
					seqPos += 2;
				}

				/*Set channel allocation*/
				else if (command[0] == 0xE5)
				{
					seqPos += 4;
				}

				/*Set standard allocation*/
				else if (command[0] == 0xE6)
				{
					seqPos++;
				}

				/*Reset track*/
				else if (command[0] == 0xE7)
				{
					seqPos++;
				}

				/*Redirect track (disabled)*/
				else if (command[0] == 0xE8)
				{
					seqPos++;
				}

				/*Set effects mask*/
				else if (command[0] == 0xE9)
				{
					seqPos += 2;
				}

				/*Set release*/
				else if (command[0] == 0xEA)
				{
					seqPos++;
				}

				/*Set pitch bend state*/
				else if (command[0] == 0xEB)
				{
					pbState = 1;
					seqPos++;

					if (holdNote == 1)
					{
						tempPos = WriteDeltaTime(midData, midPos, curDelay);
						midPos += tempPos;
						Write8B(&midData[midPos], (0xE0 | curTrack));
						Write8B(&midData[midPos + 1], 0);
						Write8B(&midData[midPos + 2], 0x40);
						curDelay = 0;
						firstNote = 1;
						midPos += 3;
					}
					while (pbState == 1)
					{
						command[0] = spcData[seqPos];
						command[1] = spcData[seqPos + 1];
						command[2] = spcData[seqPos + 2];
						command[3] = spcData[seqPos + 3];
						command[4] = spcData[seqPos + 4];
						command[5] = spcData[seqPos + 5];

						if (command[0] == 0x00)
						{
							pbState = 0;
							seqPos++;
						}

						else if (command[0] == 0x01)
						{
							curDelay += (command[1] * 8);
							seqPos += 2;
						}

						else if (command[0] == 0x02)
						{
							seqPos++;
						}
						
						else if (command[0] == 0x03)
						{
							seqPos += 2;

						}

						else
						{
							tempByte = command[0] & 0x0F;
							curDelay += (tempByte * 8);
							seqPos++;
						}
					}
				}

				/*Set pitch bend and release*/
				else if (command[0] == 0xEC)
				{
					pbState = 1;
					seqPos++;

					if (holdNote == 1)
					{
						tempPos = WriteDeltaTime(midData, midPos, curDelay);
						midPos += tempPos;
						Write8B(&midData[midPos], (0xE0 | curTrack));
						Write8B(&midData[midPos + 1], 0);
						Write8B(&midData[midPos + 2], 0x40);
						curDelay = 0;
						firstNote = 1;
						midPos += 3;
					}
					while (pbState == 1)
					{
						command[0] = spcData[seqPos];
						command[1] = spcData[seqPos + 1];
						command[2] = spcData[seqPos + 2];
						command[3] = spcData[seqPos + 3];
						command[4] = spcData[seqPos + 4];
						command[5] = spcData[seqPos + 5];

						if (command[0] == 0x00)
						{
							pbState = 0;
							seqPos++;
						}

						else if (command[0] == 0x01)
						{
							curDelay += (command[1] * 8);
							seqPos += 2;
						}

						else if (command[0] == 0x02)
						{
							seqPos++;
						}

						else if (command[0] == 0x03)
						{
							seqPos += 2;

						}

						else
						{
							tempByte = command[0] & 0x0F;
							curDelay += (tempByte * 8);
							seqPos++;
						}
					}
				}

				/*Set velocity*/
				else if (command[0] == 0xED)
				{
					seqPos += 2;
				}

				/*Extended command*/
				else if (command[0] == 0xEE)
				{
					seqPos += 2;
				}

				/*Play absolute note*/
				else if (command[0] == 0xEF)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					curNote = command[1];
					pitchVal = ((20 * curNote) + transpose) & 0xFFFF;
					curNote = pitchVal / 20;
					curNoteLen = command[2] * 8;

					tempPos = WriteNoteEventOn(midData, midPos, curNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
					prevNote = curNote;
					curDelay = 0;
					firstNote = 0;
					holdNote = 1;
					midPos = tempPos;
					curDelay += curNoteLen;
					ctrlDelay += curNoteLen;
					seqPos += 3;
				}

				/*End of phrase*/
				else if (command[0] == 0xF0)
				{
					if (holdNote == 1)
					{
						tempPos = WriteNoteEventOff(midData, midPos, prevNote, curNoteLen, curDelay, firstNote, curTrack, curInst);
						curDelay = 0;
						holdNote = 0;
						midPos = tempPos;
					}
					seqEnd = 1;
				}

				/*Set volume (relative)*/
				else if (command[0] == 0xF1)
				{
					multiplier = ReadLE16(&spcData[seqPos + 1]);
					multiplier /= 256;

					tempVol = tempVol * multiplier;

					if (tempVol > 0x7F)
					{
						tempVol = 0x7F;
					}

					curVol = tempVol + 0x19;

					if (curVol == 0)
					{
						curVol = 1;
					}
					else if (curVol > 100)
					{
						curVol = 100;
					}
					seqPos += 3;
				}

				/*Set volume (absolute)*/
				else if (command[0] == 0xF2)
				{
					tempVol = command[1];
					curVol = tempVol + 0x19;

					if (curVol == 0)
					{
						curVol = 1;
					}
					else if (curVol > 100)
					{
						curVol = 100;
					}
					seqPos += 2;
				}

				/*Set priority*/
				else if (command[0] == 0xF3)
				{
					seqPos += 2;
				}

				/*Set tied note*/
				else if (command[0] == 0xF4)
				{
					seqPos++;
				}

				/*Set sound/instrument*/
				else if (command[0] == 0xF5)
				{
					if (inMacro > 0)
					{
						macros[inMacro][10]++;

						seqPos + 11 + macros[inMacro][5];
						if (curInst != spcData[macros[inMacro][9] + macros[inMacro][10]])
						{
							if (spcData[macros[inMacro][9] + macros[inMacro][10]] != 0xFF)
							{
								curInst = command[1];

								insPtr = ReadLE16(&spcData[instOfs + (curInst * 2)]);

								if (spcData[insPtr] >= 0x40)
								{
									absNote = 1;
								}
								firstNote = 1;
							}

						}
					}
					else if (inMacro == 0)
					{
						if (curInst != command[1])
						{
							curInst = command[1];

							insPtr = ReadLE16(&spcData[instOfs + (curInst * 2)]);

							if (spcData[insPtr] >= 0x40)
							{
								absNote = 1;
							}
							firstNote = 1;
						}
					}

					seqPos += 2;
				}

				/*Call phrase*/
				else if (command[0] == 0xF6)
				{
					inMacro++;

					if (inMacro >= 16)
					{
						seqEnd = 1;
					}
					else
					{
						/*Phrase start*/
						macros[inMacro][0] = ReadLE16(&spcData[seqPos + 1]);
						/*Phrase end*/
						macros[inMacro][1] = ReadLE16(&spcData[seqPos + 3]);
						/*Number of times*/
						macros[inMacro][2] = spcData[seqPos + 5];
						/*Transpose*/
						macros[inMacro][3] = ReadLE16(&spcData[seqPos + 6]);
						/*Volume scale*/
						macros[inMacro][4] = ReadLE16(&spcData[seqPos + 8]);
						/*Number of instrument changes*/
						macros[inMacro][5] = spcData[seqPos + 10];
						/*Return position*/
						macros[inMacro][6] = seqPos + 11 + macros[inMacro][5];

						/*Original transpose*/
						macros[inMacro][7] = transpose;
						/*Original volume*/
						macros[inMacro][8] = curVol;
						/*Instrument position*/
						macros[inMacro][9] = seqPos + 11;
						/*Current instrument*/
						macros[inMacro][10] = 0;

						seqPos = macros[inMacro][0];
						transpose = macros[inMacro][3];
						multiplier = macros[inMacro][4];
						multiplier /= 256;

						tempVol = tempVol * multiplier;

						if (curVol > 0x7F)
						{
							curVol = 0x7F;
						}

						curVol = tempVol + 0x19;

						if (curVol == 0)
						{
							curVol = 1;
						}
						else if (curVol > 100)
						{
							curVol = 100;
						}
					}


				}

				/*Set detune*/
				else if (command[0] == 0xF7)
				{
					seqPos += 3;
				}

				/*Set time - 32*/
				else if (command[0] == 0xF8)
				{
					curDelay += 256;
					seqPos++;
				}

				/*Go to loop*/
				else if (command[0] == 0xF9)
				{
					seqEnd = 1;
				}

				/*Set tempo*/
				else if (command[0] == 0xFA)
				{
					ctrlMidPos++;
					valSize = WriteDeltaTime(ctrlMidData, ctrlMidPos, ctrlDelay);
					ctrlDelay = 0;
					ctrlMidPos += valSize;
					WriteBE24(&ctrlMidData[ctrlMidPos], 0xFF5103);
					ctrlMidPos += 3;
					if (command[1] == 0x00)
					{
						tempo = 256 * 0.8;
					}
					else
					{
						tempo = command[1] * 0.8;
					}

					if (tempo < 2)
					{
						tempo = 190;
					}

					WriteBE24(&ctrlMidData[ctrlMidPos], 60000000 / tempo);
					ctrlMidPos += 2;
					seqPos += 2;
				}

				/*Set panning*/
				else if (command[0] == 0xFB)
				{
					seqPos += 2;
				}

				/*No note retrigger*/
				else if (command[0] == 0xFC)
				{
					seqPos++;
				}

				/*Set time - 64*/
				else if (command[0] == 0xFD)
				{
					curDelay += 512;
					seqPos++;
				}

				/*Set time - 128*/
				else if (command[0] == 0xFE)
				{
					curDelay += 1024;
					seqPos++;
				}

				/*Set time - 256*/
				else if (command[0] == 0xFF)
				{
					curDelay += 2048;
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