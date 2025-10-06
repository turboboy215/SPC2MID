/*Krisalis (by Shaun Hollingworth)*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ramSize 65536

FILE* spc, * xm, * data;

long sfxPtr;
long musPtr;
long patList;
long patData;
int i, j, k;
char outfile[1000000];
int songNum;
int curPat;

char spcString[12];
char songTitle[32];

int drvVers;

unsigned char* spcData;
unsigned char* xmData;
unsigned char* endData;

long xmLength;

char* tempPnt;
char OutFileBase[0x100];

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
void Write8B(unsigned char* buffer, unsigned int value);
void WriteBE32(unsigned char* buffer, unsigned long value);
void WriteBE24(unsigned char* buffer, unsigned long value);
void WriteBE16(unsigned char* buffer, unsigned int value);
void WriteLE16(unsigned char* buffer, unsigned int value);
void WriteLE24(unsigned char* buffer, unsigned long value);
void WriteLE32(unsigned char* buffer, unsigned long value);
void song2xm(int songNum, long songPtr);

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

void WriteLE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0x00FF) >> 0;
	buffer[0x01] = (value & 0xFF00) >> 8;

	return;
}

void WriteLE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0x0000FF) >> 0;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0xFF0000) >> 16;

	return;
}

void WriteLE32(unsigned char* buffer, unsigned long value)
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
		printf("Usage: SNESKrisalis <spc>\n");
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

				sfxPtr = ReadLE16(&spcData[0x0012]);
				musPtr = ReadLE16(&spcData[0x0014]);

				printf("SFX module: 0x%04X\n", sfxPtr);
				printf("Music module: 0x%04X\n", musPtr);

				songNum = 1;
				song2xm(songNum, musPtr);

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

/*Convert the song data to XM*/
void song2xm(int songNum, long songPtr)
{
	int curPat = 0;
	long pattern[6];
	unsigned char command[3];
	long curPos = 0;
	int index = 0;
	int curSeq = 0;
	int numPats;
	int patRows = 64;
	long c1Pos = 0;
	long c2Pos = 0;
	long c3Pos = 0;
	long c4Pos = 0;
	long c5Pos = 0;
	long c6Pos = 0;
	long spcPos = 0;
	long xmPos = 0;
	int channels = 6;
	int defTicks = 6;
	int bpm = 150;
	long packPos = 0;
	long tempPos = 0;
	int rowsLeft = 0;
	int curChan = 0;
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	long patSize = 0;
	int curNote;
	int tempoVal;
	int remVal;
	int tempo;

	int emptyRows[6] = { 0, 0, 0, 0, 0, 0 };

	int l = 0;

	xmLength = 0x10000;
	xmData = ((unsigned char*)malloc(xmLength));

	for (l = 0; l < xmLength; l++)
	{
		xmData[l] = 0;
	}

	sprintf(outfile, "%s_%01X.xm", OutFileBase, songNum);


	if ((xm = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file %s_%01X.xm!\n", OutFileBase, songNum);
		exit(2);
	}
	else
	{
		/*Get some information*/
		patList = songPtr + 0x53;
		patData = ReadLE16(&spcData[songPtr + 0x51]);
		numPats = (patData - patList) / 6;

		spcPos = songPtr + 0x03;

		for (k = 0; k < 32; k++)
		{
			songTitle[k] = spcData[spcPos];
			spcPos++;
		}

		xmPos = 0;

		/*Write the header*/
		sprintf((char*)&xmData[xmPos], "Extended Module: ");
		xmPos += 17;

		sprintf((char*)&xmData[xmPos], songTitle);
		xmPos += 20;
		Write8B(&xmData[xmPos], 0x1A);
		xmPos++;
		sprintf((char*)&xmData[xmPos], "FastTracker v2.00   ");
		xmPos += 20;
		WriteBE16(&xmData[xmPos], 0x0401);
		xmPos += 2;

		/*Header size: 20 + number of patterns (256)*/
		WriteLE32(&xmData[xmPos], 276);
		xmPos += 4;

		/*Song length*/
		WriteLE16(&xmData[xmPos], numPats);
		xmPos += 2;

		/*Restart position*/
		WriteLE16(&xmData[xmPos], 0);
		xmPos += 2;

		/*Number of channels*/
		WriteLE16(&xmData[xmPos], channels);
		xmPos += 2;

		/*Number of patterns*/
		WriteLE16(&xmData[xmPos], numPats);
		xmPos += 2;

		/*Number of instruments*/
		WriteLE16(&xmData[xmPos], 32);
		xmPos += 2;

		/*Flags: Linear frequency*/
		WriteLE16(&xmData[xmPos], 1);
		xmPos += 2;

		/*Default tempo (ticks)*/
		WriteLE16(&xmData[xmPos], defTicks);
		xmPos += 2;

		/*Default tempo (BPM), always the same for our case*/
		WriteLE16(&xmData[xmPos], bpm);
		xmPos += 2;

		/*Pattern table*/
		for (l = 0; l < numPats; l++)
		{
			Write8B(&xmData[xmPos], l);
			xmPos++;
		}
		xmPos += (256 - l);

		spcPos = patList;

		for (curPat = 0; curPat < numPats; curPat++)
		{
			/*First, pattern header*/
			/*Pattern header length*/
			WriteLE32(&xmData[xmPos], 9);
			xmPos += 4;

			/*Packing type = 0*/
			Write8B(&xmData[xmPos], 0);
			xmPos++;

			/*Number of rows*/
			WriteLE16(&xmData[xmPos], patRows);
			xmPos += 2;

			/*Packed pattern data - fill in later*/
			packPos = xmPos;
			WriteLE16(&xmData[xmPos], 0);
			xmPos += 2;

			/*Now the actual pattern data...*/
			patSize = 0;

			for (index = 0; index < 6; index++)
			{
				pattern[index] = spcData[spcPos + index];
				emptyRows[index] = 0;
			}

			/*Get channel information*/
			c1Pos = ReadLE16(&spcData[patData + (2 * pattern[0])]);
			c2Pos = ReadLE16(&spcData[patData + (2 * pattern[1])]);
			c3Pos = ReadLE16(&spcData[patData + (2 * pattern[2])]);
			c4Pos = ReadLE16(&spcData[patData + (2 * pattern[3])]);
			c5Pos = ReadLE16(&spcData[patData + (2 * pattern[4])]);
			c6Pos = ReadLE16(&spcData[patData + (2 * pattern[5])]);
			spcPos += 6;

			for (rowsLeft = patRows; rowsLeft > 0; rowsLeft--)
			{
				for (curChan = 0; curChan < 6; curChan++)
				{
					if (curChan == 0)
					{
						command[0] = spcData[c1Pos];
						command[1] = spcData[c1Pos + 1];
						command[2] = spcData[c1Pos + 2];
					}
					else if (curChan == 1)
					{
						command[0] = spcData[c2Pos];
						command[1] = spcData[c2Pos + 1];
						command[2] = spcData[c2Pos + 2];
					}
					else if (curChan == 2)
					{
						command[0] = spcData[c3Pos];
						command[1] = spcData[c3Pos + 1];
						command[2] = spcData[c3Pos + 2];
					}
					else if (curChan == 3)
					{
						command[0] = spcData[c4Pos];
						command[1] = spcData[c4Pos + 1];
						command[2] = spcData[c4Pos + 2];
					}
					else if (curChan == 4)
					{
						command[0] = spcData[c5Pos];
						command[1] = spcData[c5Pos + 1];
						command[2] = spcData[c5Pos + 2];
					}
					else if (curChan == 5)
					{
						command[0] = spcData[c6Pos];
						command[1] = spcData[c6Pos + 1];
						command[2] = spcData[c6Pos + 2];
					}

					if (emptyRows[curChan] > 0)
					{
						Write8B(&xmData[xmPos], 0x80);
						emptyRows[curChan]--;
						xmPos++;
						patSize++;
					}
					else
					{
						/*1 empty row*/
						if (command[0] == 0x00 && command[1] == 0x00)
						{
							Write8B(&xmData[xmPos], 0x80);
							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}
							xmPos++;
							patSize++;
						}

						/*Multiple empty rows*/
						else if (command[1] == 0xFF)
						{
							Write8B(&xmData[xmPos], 0x80);
							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}

							emptyRows[curChan] = command[0];
							xmPos++;
							patSize++;
						}

						/*Set tempo*/
						else if (command[1] == 0xFA)
						{
							if (command[0] > 1)
							{
								tempo = (65535 / command[0] / 24) * 1.1;

								if (tempo < 32)
								{
									tempo = 32;
								}

								if (tempo > 255)
								{
									tempo = 255;
								}
							}
							else
							{
								tempo = 150;
							}
							Write8B(&xmData[xmPos], 0x98);
							Write8B(&xmData[xmPos + 1], 0x0F);
							Write8B(&xmData[xmPos + 2], tempo);

							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}
							xmPos += 3;
							patSize += 3;
						}

						/*Jump to position*/
						else if (command[1] == 0xFC)
						{
							if (command[0] == 0x00)
							{
								Write8B(&xmData[xmPos], 0x88);
								Write8B(&xmData[xmPos + 1], 0x0B);
								xmPos += 2;
								patSize += 2;
							}
							else
							{
								Write8B(&xmData[xmPos], 0x98);
								Write8B(&xmData[xmPos + 1], 0x0B);
								Write8B(&xmData[xmPos + 2], command[0]);
								xmPos += 3;
								patSize += 3;
							}

							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}
						}

						/*Pattern break*/
						else if (command[1] == 0xFE)
						{
							if (command[0] == 0x00)
							{
								Write8B(&xmData[xmPos], 0x88);
								Write8B(&xmData[xmPos + 1], 0x0D);
								xmPos += 2;
								patSize += 2;
							}
							else
							{
								Write8B(&xmData[xmPos], 0x98);
								Write8B(&xmData[xmPos + 1], 0x0D);
								Write8B(&xmData[xmPos + 2], command[0]);
								xmPos += 3;
								patSize += 3;
							}

							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}
						}

						/*Play note/rest*/
						else
						{
							curNote = (command[1] / 2);
							if (curNote >= 2)
							{
								curNote -= 2;
							}
							remVal = command[1] % 2;

							lowNibble = (command[0] >> 4);
							highNibble = (command[0] & 15);

							if (remVal == 0)
							{
								if (lowNibble != 0)
								{
									if (highNibble != 0)
									{
										if (highNibble <= 7)
										{
											Write8B(&xmData[xmPos], 0x9B);
											Write8B(&xmData[xmPos + 1], curNote);
											Write8B(&xmData[xmPos + 2], lowNibble);
											Write8B(&xmData[xmPos + 3], 0x01);
											Write8B(&xmData[xmPos + 4], highNibble);
											xmPos += 5;
											patSize += 5;
										}
										else
										{
											Write8B(&xmData[xmPos], 0x9B);
											Write8B(&xmData[xmPos + 1], curNote);
											Write8B(&xmData[xmPos + 2], lowNibble);
											Write8B(&xmData[xmPos + 3], 0x02);
											Write8B(&xmData[xmPos + 4], (highNibble - 7));
											xmPos += 5;
											patSize += 5;
										}

									}
									else
									{
										Write8B(&xmData[xmPos], 0x83);
										Write8B(&xmData[xmPos + 1], curNote);
										Write8B(&xmData[xmPos + 2], lowNibble);
										xmPos += 3;
										patSize += 3;
									}

								}
								else
								{
									if (highNibble != 0)
									{
										if (highNibble <= 7)
										{
											Write8B(&xmData[xmPos], 0x84);
											Write8B(&xmData[xmPos + 1], curNote);
											Write8B(&xmData[xmPos + 2], 0x01);
											Write8B(&xmData[xmPos + 3], highNibble);
											xmPos += 4;
											patSize += 4;
										}
										else
										{
											Write8B(&xmData[xmPos], 0x84);
											Write8B(&xmData[xmPos + 1], curNote);
											Write8B(&xmData[xmPos + 2], 0x02);
											Write8B(&xmData[xmPos + 3], (highNibble - 7));
											xmPos += 4;
											patSize += 4;
										}

									}
									else
									{
										Write8B(&xmData[xmPos], 0x81);
										Write8B(&xmData[xmPos + 1], curNote);
										xmPos += 2;
										patSize += 2;
									}


								}
							}
							else
							{
								if (lowNibble != 0)
								{
									Write8B(&xmData[xmPos], 0x83);
									Write8B(&xmData[xmPos + 1], 0x61);
									Write8B(&xmData[xmPos + 2], lowNibble);
									xmPos += 3;
									patSize += 3;
								}
								else
								{
									Write8B(&xmData[xmPos], 0x81);
									Write8B(&xmData[xmPos + 1], 0x61);
									xmPos += 2;
									patSize += 2;
								}

							}

							if (curChan == 0)
							{
								c1Pos += 2;
							}
							else if (curChan == 1)
							{
								c2Pos += 2;
							}
							else if (curChan == 2)
							{
								c3Pos += 2;
							}
							else if (curChan == 3)
							{
								c4Pos += 2;
							}
							else if (curChan == 4)
							{
								c5Pos += 2;
							}
							else if (curChan == 5)
							{
								c6Pos += 2;
							}

						}
					}

				}
			}

			WriteLE16(&xmData[packPos], patSize);
		}

		fwrite(xmData, xmPos, 1, xm);

		/*Add data to end of XM file*/
		if ((data = fopen("xmdata.dat", "rb")) == NULL)
		{
			printf("ERROR: Unable to open file xmdata.dat!\n");
			exit(1);
		}
		else
		{
			endData = ((unsigned char*)malloc(11744));
			fread(endData, 1, 11744, data);
			fwrite(endData, 11744, 1, xm);
			xmPos += 11744;
		}
		free(xmData);
		free(endData);
		fclose(xm);
	}
}
