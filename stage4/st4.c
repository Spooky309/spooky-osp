#include "term.h"
#include "st4.h"
#include "io.h"
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

typedef struct elfheader_c
{
	unsigned long magic;                        // 4
	unsigned char class;                        // 5
	unsigned char endian;                       // 6
	unsigned char version;						// 7
	unsigned char abi;							// 8
	unsigned char pad[8];						// 16
	unsigned short type;						// 18
	unsigned short isa;							// 20
	unsigned long eversion;						// 24
	unsigned long entry;						// 28
	unsigned long progheaderoff;				// 32
	unsigned long sectionheaderoff;				// 36
	unsigned long flags;						// 40
	unsigned short headersize;					// 42
	unsigned short progheaderentsize;			// 44
	unsigned short numprogheaderents;			// 46
	unsigned short sectionheaderentsize;		// 48
	unsigned short numsectionheaderents;		// 50
	unsigned short sectionheaderrootoff;		// 52
} __attribute__((packed)) elfheader_t;
#define ELF_MAGIC 0x464C457F

typedef struct elfpheader_c
{
	unsigned long type;
	unsigned long offset;
	unsigned long vaddr;
	unsigned long paddr;
	unsigned long filesz;
	unsigned long memsz;
	unsigned long flags;
	unsigned long align;
} __attribute__((packed)) elfpheader_t;

unsigned char* bpbBuffer = (void*)0xBFC04000;
bpb_t* pBpb;

#define LBABITS 0x40

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

int strcmps(unsigned char* a, unsigned char* b, unsigned long len)
{
	for (unsigned long i = 0; i < len; i++)
	{
		if (a[i] != b[i]) return 0;
	}
	return 1;
}

void stop(void)
{
	writeLine("\nNothing left to do. Halt.");
	while(1)
	{
		asm("hlt"); // stop until we receive some interrupt
	}
}

int st4main(void)
{
  initTerm();
  unsigned char* end = (void*)0xBFC10000;
  
  ReadSectors(0, 1, (void*)bpbBuffer);
  pBpb = (bpb_t*)bpbBuffer;
  unsigned char* rootDirectory = end + (pBpb->bytesPerSector * (pBpb->sectorsPerFat * pBpb->fatCount));
  unsigned char* fat = (unsigned char*)end;
  puts("FAT12 Filesystem Info\nReserved Sectors: ");
  putint(pBpb->reservedSectors);
  puts("\nSectors Per FAT: ");
  putint(pBpb->sectorsPerFat);
  puts("\nNumber of FATs: ");
  putint(pBpb->fatCount);
  puts("\nMax Root Dir Entries: ");
  putint(pBpb->maxRootDirEntries);
  putch('\n');
  ReadSectors(pBpb->reservedSectors + (pBpb->sectorsPerFat * pBpb->fatCount), (pBpb->maxRootDirEntries * 32) / pBpb->bytesPerSector, (void*)rootDirectory);
  direntry_t* entry = (direntry_t*)rootDirectory;
  writeLine("\nRoot Directory Listing:");
  direntry_t* kernEntry = 0;
  while (entry->name[0] != 0)
  {
	for (int i = 0; i < 8; i++)
	{
		if (entry->name[i] == 0x20 || entry->name[i] == 0) break;
		putch(entry->name[i]);
	}
	putch('.');
	for (int i = 8; i < 11; i++)
	{
		if (entry->name[i] == 0x20 || entry->name[i] == 0) break;
		putch(entry->name[i]);
	}
	putch(' ');
	putint(entry->fileSize);
	if (strcmps(entry->name, (unsigned char*)"KRNL    LIG", 11))
	{
		puts(" <-- KERNEL");
		kernEntry = entry;
	}
	putch('\n');
	entry += 1;
  }
  if (kernEntry == 0)
  {
	  writeLine("ERROR: KERN.LIG NOT FOUND");
	  stop();
  }
  writeLine("\nLoading Kernel to temp location. Reloc 0xC0000000");
  unsigned char* kernDest = (unsigned char*)entry; // we will load the kernel elf here
  unsigned char* kernPtr = kernDest;
  unsigned int first_fat_sector = pBpb->reservedSectors;
  unsigned short curCluster = kernEntry->clusterLow;
  // for some reason this calculation loses a sector, i don't know why, so i have to subtract 1 from it
  unsigned int first_cluster_sector = (pBpb->fatCount * pBpb->sectorsPerFat) + ((pBpb->maxRootDirEntries * 32) / pBpb->bytesPerSector) - 1;
  while (curCluster < 0xFF8)
  {
	  unsigned int sec = (curCluster * pBpb->sectorsPerCluster);
	  
	  ReadSectors(first_cluster_sector + sec, pBpb->sectorsPerCluster, kernPtr);

	  kernPtr += pBpb->sectorsPerCluster * pBpb->bytesPerSector;
	  
	  unsigned int fat_offset = curCluster + (curCluster / 2);
	  unsigned int fat_sector = first_fat_sector + (fat_offset / pBpb->bytesPerSector);
	  unsigned int ent_offset = fat_offset % pBpb->bytesPerSector;
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
  putch('\n');
  elfheader_t* eHeader = (elfheader_t*)kernDest;
  if (eHeader->magic != ELF_MAGIC)
  {
	  writeLine("ERROR: ELF_MAGIC BORKED");
	  puts("Expected ");
	  putintx(ELF_MAGIC);
	  puts(" got ");
	  putintx(eHeader->magic);
	  putch('\n');
	  stop();
  }
  puts("KRNL.LIG loaded at ");
  putintx((unsigned int)kernDest);
  putch('\n');
  
  
  for (int i = 0; i < eHeader->numprogheaderents; i++)
  {
	  elfpheader_t* ePHeader = (elfpheader_t*)(kernDest + eHeader->progheaderoff + (eHeader->progheaderentsize * i));
	  if (ePHeader->type != 1) continue; // 1 = LOAD
	  unsigned char* buf = (unsigned char*)ePHeader->vaddr; // get destination ptr
	  for (unsigned long b = 0; b < ePHeader->memsz; b++)
	  {
		  *(buf+b) = *(kernDest + ePHeader->offset + b); // copy from source to dest
	  }
  }
  typedef void (*ENTRYFUN)();
  ENTRYFUN _entry = (ENTRYFUN)eHeader->entry;
  _entry();
  stop();
  return 0;
}