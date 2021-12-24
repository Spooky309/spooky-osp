#include "disk.h"
#include "io.h"
#include "mem.h"
#include "command.h"
#include "string.h"
#include "term.h"

#define LBABITS 0x40

typedef struct bpb_c
{
	unsigned char jmp[3];
	unsigned char dosName[8];
	unsigned short bytesPerSector;
	unsigned char sectorsPerCluster;
	unsigned short reservedSectors;
	unsigned char fatCount;
	unsigned short maxRootDirEntries;
	unsigned short totalSectors;
	unsigned char mediaDescriptor;
	unsigned short sectorsPerFat;
	unsigned short sectorsPerTrack;
	unsigned short numHeads;
	unsigned long hiddenSectors;
	unsigned long bigTotalSectors;
	unsigned char driveNumber;
	unsigned char ntFlags;
	unsigned char dosSignature;
	unsigned long dosSerialNumber;
	unsigned char volumeName[11];
	unsigned char fileSystemID[8];
} __attribute__((packed)) bpb_t;

typedef struct direntry_c
{
	unsigned char name[11];
	unsigned char attrib;
	unsigned char o1;
	unsigned char o2;
	union
	{
		struct
		{
			unsigned short createTimeHours : 5;
			unsigned short createTimeMinutes : 6;
			unsigned short createTimeSeconds : 5; // multiply by two
		};
		unsigned short createTime;
	};
	union
	{
		struct
		{
			unsigned short createDateYear : 7;
			unsigned short createDateMonth : 4;
			unsigned short createDateDay : 5;
		};
		unsigned short createDate;
	};
	union
	{
		struct
		{
			unsigned short accessDateYear : 7;
			unsigned short accessDateMonth : 4;
			unsigned short accessDateDay : 5;
		};
		unsigned short accessDate;
	};
	unsigned short clusterHigh;
	union
	{
		struct
		{
			unsigned short modifyTimeHours : 5;
			unsigned short modifyTimeMinutes : 6;
			unsigned short modifyTimeSeconds : 5; // multiply by two
		};
		unsigned short modifyTime;
	};
	union
	{
		struct
		{
			unsigned short modifyDateYear : 7;
			unsigned short modifyDateMonth : 4;
			unsigned short modifyDateDay : 5;
		};
		unsigned short modifyDate;
	};
	unsigned short clusterLow;
	unsigned long fileSize;
} __attribute__((packed)) direntry_t;

bpb_t* pBPB;
direntry_t* fileTable;
unsigned char* fat;
void InitDisk()
{
	pBPB = malloc(sizeof(bpb_t));
	ReadSectors(0, 1, (void*)pBPB);
	puts("\nFAT12 Filesystem Info\nReserved Sectors: ");
    putint(pBPB->reservedSectors);
    puts("\nSectors Per FAT: ");
    putint(pBPB->sectorsPerFat);
    puts("\nNumber of FATs: ");
    putint(pBPB->fatCount);
    puts("\nMax Root Dir Entries: ");
    putint(pBPB->maxRootDirEntries);
	puts("\nTotal Disk Size: ");
	putint(pBPB->totalSectors * pBPB->bytesPerSector);
    writeLine(" Bytes\n");
	fileTable = malloc(pBPB->maxRootDirEntries * sizeof(direntry_t));
	register_command("ls", "list root dir files", &ListFiles);
	fat = malloc(512);
}
void ReadSectors(unsigned long LBA, unsigned char numSectors, void* dest)
{
	unsigned short* dPtr = (unsigned short*)dest;
	int runs = 15;
	while (((inb(0x1F7) & 0x80) != 0) || runs > 0)
	{
		runs--;
		// do nothing
	}
	outb(0x1F6, 0xE0 | LBABITS | ((LBA >> 24) & 0x0F));
	outb(0x1F1, 0);
	outb(0x1F2, numSectors);
	outb(0x1F3, (unsigned char)LBA);
	outb(0x1F4, (unsigned char)(LBA >> 8));
	outb(0x1F5, (unsigned char)(LBA >> 16));
	outb(0x1F7, 0x20);
	for (int r = 0; r < numSectors; r++)
	{
		runs = 15;
		while (((inb(0x1F7) & 0x08) == 0) || runs > 0)
		{
			runs--;
			// do nothing
		}
		for (int i = 0; i < 256; i++)
		{
			*dPtr = inw(0x1F0);
			dPtr += 1;
		}
	}
}
void LoadFileTo(unsigned short firstCluster, unsigned long fileSize, void* output)
{
	unsigned char* fileOutput = (unsigned char*)output;
	
	unsigned int first_fat_sector = pBPB->reservedSectors;
	unsigned short curCluster = firstCluster;
	unsigned int first_cluster_sector = (pBPB->fatCount * pBPB->sectorsPerFat) + ((pBPB->maxRootDirEntries * 32) / pBPB->bytesPerSector) - 1;
	
	while (curCluster < 0xFF8)
	{
		unsigned int sec = (curCluster * pBPB->sectorsPerCluster);
		
		ReadSectors(first_cluster_sector + sec, pBPB->sectorsPerCluster, fileOutput);
		
		fileOutput += pBPB->sectorsPerCluster * pBPB->bytesPerSector;
		
		unsigned int fat_offset = curCluster + (curCluster / 2);
		unsigned int fat_sector = first_fat_sector + (fat_offset / pBPB->bytesPerSector);
		unsigned int ent_offset = fat_offset % pBPB->bytesPerSector;
		ReadSectors(fat_sector, 1, fat);
		
		unsigned short table_value = *(unsigned short*)&fat[ent_offset]; // get the next cluster
		if (curCluster & 1)
		{
			table_value = table_value >> 4;
		}
		else
		{
			table_value = table_value & 0x0FFF;
		}
		curCluster = table_value;
	}
	*(((unsigned char*)output) + fileSize) = 0; // nullbyte at end
}
unsigned long LoadFile(const char* fileName, void** output)
{
	*output = 0;
	ReadSectors(pBPB->reservedSectors + (pBPB->sectorsPerFat * pBPB->fatCount), (pBPB->maxRootDirEntries * 32) / pBPB->bytesPerSector, (void*)fileTable);
	
	direntry_t* entry = (direntry_t*)fileTable;
	direntry_t* fileEntry = 0;
	while (entry->name[0] != 0)
	{
		if (strcmps(entry->name, (unsigned char*)fileName, 11))
		{
			fileEntry = entry;
			break;
		}
		entry += 1;
	}
	if (entry == 0)
	{
		writeLine("ERROR: File Not Found");
		return 0;
	}
	*output = malloc((((fileEntry->fileSize / 512) + 1) * 512) + 1);
	LoadFileTo(fileEntry->clusterLow, fileEntry->fileSize, *output);
	
	return (((fileEntry->fileSize / 512) + 1) * 512) + 1;
}

void ListFiles(void)
{
	writeLine("FILENAME    SIZE (BYTES)");
	ReadSectors(pBPB->reservedSectors + (pBPB->sectorsPerFat * pBPB->fatCount), (pBPB->maxRootDirEntries * 32) / pBPB->bytesPerSector, (void*)fileTable);
	direntry_t* entry = fileTable;
	while (entry->name[0] != 0)
	{
		int fnamechars = 0;
		for (int i = 0; i < 8; i++)
		{
			if (entry->name[i] == 0x20 || entry->name[i] == 0) break;
			putch(entry->name[i]);
			fnamechars++;
		}
		putch('.');
		for (int i = 8; i < 11; i++)
		{
			if (entry->name[i] == 0x20 || entry->name[i] == 0) break;
			putch(entry->name[i]);
			fnamechars++;
		}
		while (fnamechars < 11)
		{
			putch(' ');
			fnamechars++;
		}
		putint(entry->fileSize);
		putch('\n');
		entry += 1;
	}
}