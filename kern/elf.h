#ifndef ELF_H
#define ELF_H

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

typedef int(*PROGMAINFUN)(void);
PROGMAINFUN PutELF(void* elfBegin);

#endif // ELF_H