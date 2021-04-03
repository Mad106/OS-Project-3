#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define FAT32_MASK (((uint32_t) 0x0FFFFFFF))

/* Empty Cluster */
#define EMPTY ((uint32_t) 0x0000000)

/* End-of-Cluster mark */
#define EOC ((uint32_t) 0xFFFFFFFF)

#define BAD_CLUSTER ((uint32_t) 0xFFFFFF7)

/* BIOS  Parameter  Block (BPB) (most useful fields only) */
typedef struct
{
    /* Count of bytes per sector. This value may take on 
    only the following values: 512, 1024, 2048 or 4096. */
    uint16_t BPB_BytsPerSec;

    /* Number  of  sectors  per  allocation  unit.  This  value 
    must  be  a  power  of  2  that  is  greater  than  0.  The 
    legal values are 1, 2, 4, 8, 16, 32, 64, and 128. */
    uint8_t BPB_SecPerClus;

    /* Number of reserved sectors in the reserved region 
    of  the  volume  starting  at  the  first  sector  of  the 
    volume.  This  field  is  used  to  align  the  start  of  the 
    data  area  to  integral  multiples  of  the  cluster  size 
    with respect to the start of the partition/media. */
    uint16_t BPB_RsvdSecCnt;

    /* The count of FAT data structures on the volume */
    uint8_t BPB_NumFATs;

    /* This field is the new 32-bit total count of sectors on 
    the  volume.  This  count  includes  the  count  of  all 
    sectors in all four regions of the volume.  */
    uint32_t BPB_TotSec32;

    /* This field is the FAT32 32-bit count of sectors occupied 
    by one FAT. */
    uint32_t BPB_FATSz32;

    /* This is set to the cluster number of the first cluster of 
    the root directory.    
    This  value  should  be  2  or  the  first  usable  (not  bad) 
    cluster available thereafter. */
    uint32_t BPB_RootClus;
} BPB;

/* Directory Structure 
 * A FAT directory is a special file type. The directory serves as a container 
 * for other files and sub-directories. Directory contents (data) are a series
 * of 32 byte directory entries. Each directory entry in turn, typically 
 * represents a contained file or sub-directory directory. 
 */
typedef struct __attribute__((__packed__)) 
{
    char DIR_Name[11];
    uint8_t DIR_Attr;
    uint8_t reserved1[8];
    uint16_t DIR_FirstClusterHigh;
    uint8_t reserved2[4];
    uint16_t DIR_FirstClusterLow;
    uint32_t DIR_FileSize;
} DIR_Entry;


/* File/Directory Attributes */
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define SHORT_LEN 11

#endif
