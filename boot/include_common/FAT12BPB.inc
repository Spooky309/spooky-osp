%ifndef _FAT_12_BPB_
%define _FAT_12_BPB_
dosName 					   db 'mkdosfs '   ; name, len 8
dosBytesPerSector    dw 512 			   ; Bytes Per Sector
dosSectorsPerCluster db 1   				 ; Sectors Per Cluster
dosReservedSectors   dw 1   			   ; Reserved Sectors
dosFatCount          db 2   				 ; FAT count
dosMaxRootDirEntries dw 512  				 ; Max Root Dir Entries
dosTotalSectors      dw 2880				 ; Total Sectors (2.8MiB)
dosMediaDescriptor   db 0xF0				 ; Media Descriptor (non-removable)
dosSectorsPerFat     dw 9 					 ; Sectors Per Fat (65KiB here)
dosSectorsPerTrack   dw 18					 ; Sectors Per Track
dosNumHeads          dw 2						 ; num heads
dosHiddenSectors     dd 0						 ; hidden sectors
dosBigTotalSectors   dd 0						 ; big total sectors (unused)
dosDriveNumber       db 0						 ; drive number (useless)
dosNTFlags           db 0						 ; NT flags (useless/could reuse)
dosSignature         db 0x29				 ; signature (??)
dosSerialNumber      dd 0xDEADBEEF	 ; serial number
dosVolumeName        db 'BOOT DISK  '; volume name
dosFileSystemID      db 'FAT12   '   ; some kind of fsid shit nobody cares about
%endif