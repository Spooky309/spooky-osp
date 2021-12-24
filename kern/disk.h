#ifndef DISK_H
#define DISK_H

void InitDisk();
void ReadSectors(unsigned long LBA, unsigned char numSectors, void* dest);
unsigned long LoadFile(const char* fileName, void** output);
void ListFiles(void);

#endif // DISK_H