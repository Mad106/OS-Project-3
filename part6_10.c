#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "fat32.h"

#define BUFFER_SIZE 256
#define MAX_OPENED_FILES 100

/* File mode */
typedef enum
{
    READ_ONLY,
    WRITE_ONLY,
    READ_AND_WRITE,
    WRITE_AND_READ
} FileMode;

/* To store opened file information */
typedef struct
{
    char *filename;
    FileMode mode;
    uint32_t filePointer;
    uint32_t firstCluster;
} FileEntry;

/* Image file */
FILE *fImage = NULL;
/* BIOS  Parameter  Block (BPB) */
BPB bpb;

/* Current working directory */
uint32_t CWD;

/* List of opened files */
FileEntry openedFiles[MAX_OPENED_FILES];

/* Prototypes */

int open_image(const char *fname);
int parse_command(char *command);

/* Commands */
int create_file(const char *fname);
int make_dir(const char *dname);
int move_file_or_dir(const char *from, const char *to);
int open_file(const char *fname, const char *mode);
int close_file(const char *fname);
int list_dir(const char *dname);
int change_dir(const char *dname);
int info();
int size_file(const char *fname);

/* helpers */
void find_free_space(uint32_t *usedClust, uint32_t *freeClust);
uint32_t allocate_free_cluster(void);
uint32_t find_dir(const char *dname);
int find_dir_entry(const char *name, DIR_Entry *entry, uint32_t *offset);
void list_dir2(uint32_t clust);
int create_file_in_cwd(const char *fname);
int create_dir_in_cwd(const char *dname);
void create_empty_dir(uint32_t dir, uint32_t parent);
void rename_file_or_dir(const char *from, const char *to);
int move_entry(DIR_Entry *entryFrom, uint32_t offFrom, DIR_Entry *entryTo, uint32_t offTo);
void long_to_short(const char *lname, char *sname);
void short_to_long(const char *sname, char *lname);
int compare_names(const char *lname, char *sname);

uint32_t fat_offset(uint32_t clust);
uint32_t cluster_to_data_offset(uint32_t clust);
uint32_t next_cluster(uint32_t clust);
uint32_t make_dword(uint16_t low, uint16_t high);

void initialize_files();
void free_files();
int find_opened_file(const char *fname);
int add_to_files(const char *filename, FileMode mode, uint32_t filePointer, uint32_t firstCluster);
int remove_from_files(char *filename);
FileMode str_to_mode(const char *mode);

/* The main */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./project3 <FAT32 image filename>\n");
        exit(EXIT_FAILURE);
    }

    open_image(argv[1]);
    initialize_files();
    info();

    /* command loop */
    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("> ");
        fflush(stdout);
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;
        if (parse_command(buffer) == 0)
            break;
    }

    free_files();
    fclose(fImage);

    return 0;
}

/* open fat32 image */
int open_image(const char *fname)
{
    fImage = fopen(fname, "r+b");
    if (!fImage)
    {
        fprintf(stderr, "Can't open %s\n", fname);
        exit(EXIT_FAILURE);
    }

    // read BPB
    fseek(fImage, 11, SEEK_SET);
    fread(&bpb.BPB_BytsPerSec, sizeof(bpb.BPB_BytsPerSec), 1, fImage);

    fseek(fImage, 13, SEEK_SET);
    fread(&bpb.BPB_SecPerClus, sizeof(bpb.BPB_SecPerClus), 1, fImage);

    fseek(fImage, 14, SEEK_SET);
    fread(&bpb.BPB_RsvdSecCnt, sizeof(bpb.BPB_RsvdSecCnt), 1, fImage);

    fseek(fImage, 16, SEEK_SET);
    fread(&bpb.BPB_NumFATs, sizeof(bpb.BPB_NumFATs), 1, fImage);

    fseek(fImage, 32, SEEK_SET);
    fread(&bpb.BPB_TotSec32, sizeof(bpb.BPB_TotSec32), 1, fImage);

    fseek(fImage, 36, SEEK_SET);
    fread(&bpb.BPB_FATSz32, sizeof(bpb.BPB_FATSz32), 1, fImage);

    fseek(fImage, 44, SEEK_SET);
    fread(&bpb.BPB_RootClus, sizeof(bpb.BPB_RootClus), 1, fImage);

    // set current workin dir
    CWD = bpb.BPB_RootClus;

    return 1;
}

/* parse and execute single command.
 * returns 1 on success, -1 on errors, 0 on "exit"
 */
int parse_command(char *command)
{
    char *cmd, *arg1, *arg2;

    cmd = strtok(command, " \t\n");
    if (strcmp(cmd, "creat") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            return create_file(arg1);
        }
    }
    else if (strcmp(cmd, "mkdir") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            return make_dir(arg1);
        }
    }
    else if (strcmp(cmd, "mv") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            arg2 = strtok(NULL, " \t\n");
            if (arg2)
            {
                return move_file_or_dir(arg1, arg2);
            }
        }
    }
    else if (strcmp(cmd, "open") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            arg2 = strtok(NULL, " \t\n");
            if (arg2)
            {
                return open_file(arg1, arg2);
            }
        }
    }
    else if (strcmp(cmd, "close") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            return close_file(arg1);
        }
    }
    else if (strcmp(cmd, "exit") == 0)
    {
        return 0;
    }
    else if (strcmp(cmd, "ls") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            return list_dir(arg1);
        }
        else
        {
            return list_dir(".");
        }
    }
    else if (strcmp(cmd, "cd") == 0)
    {
        arg1 = strtok(NULL, " \t\n");
        if (arg1)
        {
            return change_dir(arg1);
        }
    }
    else if (strcmp(cmd, "info") == 0)
    {
        return info();
    }
	else if (strcmp(cmd, "size") == 0)
	{
		arg1 = strtok(NULL, "\t\n");
		if(arg1)
		{
			return size_file(arg1);
		}
	}

    printf("Invalid command\n");
    return -1;
}

/* "creat" command */
int create_file(const char *fname)
{
    if (create_file_in_cwd(fname) == 1)
    {
        uint32_t usedClust, freeClust;
        find_free_space(&usedClust, &freeClust);
        printf("%s created: %u bytes used, %u bytes remaining\n",
               fname,
               usedClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus,
               freeClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus);
        return 1;
    }
    return -1;
}

/* "mkdir" command */
int make_dir(const char *dname)
{
    if (create_dir_in_cwd(dname) == 1)
    {
        uint32_t usedClust, freeClust;
        find_free_space(&usedClust, &freeClust);
        printf("%s created: %u bytes used, %u bytes remaining\n",
               dname,
               usedClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus,
               freeClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus);
        return 1;
    }
    return -1;
}

/* "mv" command */
int move_file_or_dir(const char *from, const char *to)
{
    uint32_t offFrom, offTo;
    DIR_Entry entryFrom, entryTo;
    if (!find_dir_entry(from, &entryFrom, &offFrom))
    {
        fprintf(stderr, "%s does not exist.\n", from);
        return -1;
    }

    if (!find_dir_entry(to, &entryTo, &offTo))
    {
        rename_file_or_dir(from, to);
    }
    else
    {
        if (!(entryTo.DIR_Attr & ATTR_DIRECTORY) && entryFrom.DIR_Attr & ATTR_DIRECTORY)
        {
            fprintf(stderr, "Cannot move directory: invalid destination argument.\n");
            return -1;
        }
        if (!(entryTo.DIR_Attr & ATTR_DIRECTORY) && !(entryFrom.DIR_Attr & ATTR_DIRECTORY))
        {
            fprintf(stderr, "Cannot move file: already exists.\n");
            return -1;
        }

        move_entry(&entryFrom, offFrom, &entryTo, offTo);
    }
    printf("%s moved to %s\n", from, to);
    return 1;
}

/* "open" command */
int open_file(const char *fname, const char *mode)
{
    if (strcmp(mode, "r") && strcmp(mode, "w") && strcmp(mode, "rw") && strcmp(mode, "wr"))
    {
        fprintf(stderr, "Cannot open file: invalid mode.\n");
        return -1;
    }

    if (find_opened_file(fname) != -1)
    {
        fprintf(stderr, "Cannot open file: already opened.\n");
        return -1;
    }

    uint32_t offset;
    DIR_Entry entry;
    if (!find_dir_entry(fname, &entry, &offset))
    {
        fprintf(stderr, "Cannot open file: does not exists.\n");
        return -1;
    }

    add_to_files(fname, str_to_mode(mode), 0, make_dword(entry.DIR_FirstClusterLow, entry.DIR_FirstClusterHigh));
    printf("%s opened\n", fname);
    return 1;
}

/* "close" command */
int close_file(const char *fname)
{
    int i = find_opened_file(fname);
    if (i == -1)
    {
        fprintf(stderr, "Cannot close file: not opened.\n");
        return -1;
    }
    free(openedFiles[i].filename);
    openedFiles[i].filename = NULL;
    printf("%s closed\n", fname);
    return 1;
}

/* "ls" command */
int list_dir(const char *dname)
{
    uint32_t clust = find_dir(dname);
    if (clust == EOC)
    {
        fprintf(stderr, "%s does not exist or is not a directory.\n", dname);
        return -1;
    }
    list_dir2(clust);
    return 1;
}

/* "cd" command */
int change_dir(const char *dname)
{
    uint32_t clust = find_dir(dname);
    if (clust == EOC)
    {
        fprintf(stderr, "%s does not exist or is not a directory.\n", dname);
        return -1;
    }
    //fprintf(stderr, "chdir %u\n", clust);
    CWD = clust;
    return 1;
}

/* "info" command */
int info()
{
    printf("BPB_BytsPerSec: %u\n", bpb.BPB_BytsPerSec);
    printf("BPB_SecPerClus: %u\n", bpb.BPB_SecPerClus);
    printf("BPB_RsvdSecCnt: %u\n", bpb.BPB_RsvdSecCnt);
    printf("BPB_NumFATs: %u\n", bpb.BPB_NumFATs);
    printf("BPB_TotSec32: %u\n", bpb.BPB_TotSec32);
    printf("BPB_FATSz32: %u\n", bpb.BPB_FATSz32);
    printf("BPB_RootClus: %u\n", bpb.BPB_RootClus);

    uint32_t usedClust, freeClust;
    find_free_space(&usedClust, &freeClust);
    printf("%u bytes used, %u bytes remaining\n",
           usedClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus,
           freeClust * bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus);

    return 1;
}

/* "size" command */
int size_file(const char *fname)
{
	uint32_t currClust = CWD;
	DIR_Entry entry;
	if(!find_dir_entry(fname, &entry, &offset))
	{
		fprintf(stderr, "File does not exist,\n");
		return -1;
	}
	
	currClust = openedFiles[find_opened_file]->firstCluster;
	do
	{
		currClust = next_cluster(currClust);
		size += (bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus);
	} while (currClust < BAD_CLUSTER);
	
	printf("Size of %s in bytes: %d\n",fname,size);
	return 1;
}

/* find directory entry in CWD for the given name.
 * return 1 if found and 0 if not found.
 */
int find_dir_entry(const char *name, DIR_Entry *entry, uint32_t *off)
{
    uint32_t currClust = CWD;
    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and read dir entries
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (dir.DIR_Name[0] == '\0')
                return 0; // no more entries

            if (compare_names(name, dir.DIR_Name))
            {
                // found
                *entry = dir;
                *off = offset + i * (sizeof(dir));
                return 1;
            }
        }
        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);

    return 0; // not found
}

/*
 * Find number of the first cluster for the given directory name
 */
uint32_t find_dir(const char *dname)
{
    if (strcmp(dname, ".") == 0)
    {
        return CWD;
    }
    if (strcmp(dname, "..") == 0 && CWD == bpb.BPB_RootClus)
    {
        return CWD;
    }

    DIR_Entry dir;
    uint32_t offset;
    if (find_dir_entry(dname, &dir, &offset))
    {
        if (dir.DIR_Attr & ATTR_DIRECTORY)
        {
            uint32_t clust = make_dword(dir.DIR_FirstClusterLow, dir.DIR_FirstClusterHigh);
            if (clust == 0)
                clust = bpb.BPB_RootClus; // root directory
            return clust;
        }
    }

    return EOC;
}

/* list files in the directory given by cluster number */
void list_dir2(uint32_t clust)
{
    uint32_t currClust = clust;

    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and read dir entries
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (dir.DIR_Name[0] == '\0')
                return;

            // skip hidden files
            if (dir.DIR_Attr & ATTR_HIDDEN ||
                dir.DIR_Attr & ATTR_SYSTEM ||
                dir.DIR_Attr & ATTR_VOLUME_ID ||
                dir.DIR_Name[0] == (char)0xE5)
                continue;

            char name[SHORT_LEN + 1];
            short_to_long(dir.DIR_Name, name);

            if (dir.DIR_Attr & ATTR_DIRECTORY)
                printf("[%s]\n", name);
            else
                printf("%s\n", name);

            fflush(stdout);
        }

        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);
}

/* Create empty file in the CWD */
int create_file_in_cwd(const char *fname)
{
    uint32_t currClust = CWD;
    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and search empty entry
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (dir.DIR_Name[0] == 0x00 || dir.DIR_Name[0] == 0xE5)
            {
                // empty entry found
                uint32_t firstCluster = allocate_free_cluster();
                if (firstCluster == EOC)
                {
                    fprintf(stderr, "No free space.\n");
                    return -1;
                }
                dir.DIR_FirstClusterLow = firstCluster & 0xFFFF;
                dir.DIR_FirstClusterHigh = firstCluster & 0xFFFF0000;
                long_to_short(fname, dir.DIR_Name);
                dir.DIR_Attr = 0x00;
                dir.DIR_FileSize = 0;
                // update image on the disk
                fseek(fImage, offset + i * (sizeof(dir)), SEEK_SET);
                fwrite(&dir, sizeof(dir), 1, fImage);
                return 1;
            }

            if (compare_names(fname, dir.DIR_Name))
            {
                fprintf(stderr, "File with that name already exists.\n");
                return -1;
            }
        }
        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);

    // no free space in cwd: need to allocate
    fprintf(stderr, "No free space.\n");
    return -1;
}

/* Create empty directory in the CWD */
int create_dir_in_cwd(const char *dname)
{
    uint32_t currClust = CWD;
    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and read dir entries
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (dir.DIR_Name[0] == 0x00 || dir.DIR_Name[0] == 0xE5)
            {
                // empty entry found
                uint32_t firstCluster = allocate_free_cluster();
                if (firstCluster == EOC)
                {
                    fprintf(stderr, "No free space.\n");
                    return -1;
                }
                dir.DIR_FirstClusterLow = firstCluster & 0xFFFF;
                dir.DIR_FirstClusterHigh = firstCluster & 0xFFFF0000;
                long_to_short(dname, dir.DIR_Name);
                dir.DIR_Attr = ATTR_DIRECTORY;
                dir.DIR_FileSize = 0;
                // update image on the disk
                fseek(fImage, offset + i * (sizeof(dir)), SEEK_SET);
                fwrite(&dir, sizeof(dir), 1, fImage);
                // create directory content
                create_empty_dir(firstCluster, currClust);
                return 1;
            }

            if (compare_names(dname, dir.DIR_Name))
            {
                fprintf(stderr, "Directory with that name already exists.\n");
                return -1;
            }
        }
        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);

    // no free space in cwd: need to allocate
    fprintf(stderr, "No free space.\n");
    return -1;
}

/* rename an entry in the CWD */
void rename_file_or_dir(const char *from, const char *to)
{
    uint32_t currClust = CWD;
    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and read dir entries
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (compare_names(from, dir.DIR_Name))
            {
                long_to_short(to, dir.DIR_Name);
                // update image on the disk
                fseek(fImage, offset + i * (sizeof(dir)), SEEK_SET);
                fwrite(&dir, sizeof(dir), 1, fImage);
                return;
            }
        }
        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);
}

/* move file or directory into directory given as entryTo */
int move_entry(DIR_Entry *entryFrom, uint32_t offFrom, DIR_Entry *entryTo, uint32_t offTo)
{
    uint32_t currClust = make_dword(entryTo->DIR_FirstClusterLow, entryTo->DIR_FirstClusterHigh);
    DIR_Entry dir;
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);

    // go through the current directory and read dir entries
    do
    {
        uint32_t offset = cluster_to_data_offset(currClust);
        fseek(fImage, offset, SEEK_SET);

        for (int i = 0; i < numDirs; ++i)
        {
            fread(&dir, sizeof(dir), 1, fImage);

            if (dir.DIR_Name[0] == 0x00 || dir.DIR_Name[0] == 0xE5)
            {
                // empty entry found
                // place entryFrom to this empty entry in entryTo
                fseek(fImage, offset + i * (sizeof(dir)), SEEK_SET);
                fwrite(entryFrom, sizeof(dir), 1, fImage);
                // update old entry
                entryFrom->DIR_Name[0] = 0xE5; // mark entry as free
                fseek(fImage, offFrom, SEEK_SET);
                fwrite(entryFrom, sizeof(dir), 1, fImage);
                // replace parent dir in the moved directory
                if (entryFrom->DIR_Attr & ATTR_DIRECTORY)
                {
                    uint32_t fromClust = make_dword(entryFrom->DIR_FirstClusterLow, entryFrom->DIR_FirstClusterHigh);
                    offset = cluster_to_data_offset(fromClust) + sizeof(DIR_Entry);
                    fseek(fImage, offset, SEEK_SET);
                    fread(&dir, sizeof(dir), 1, fImage);
                    dir.DIR_FirstClusterLow = entryTo->DIR_FirstClusterLow;
                    dir.DIR_FirstClusterHigh = entryTo->DIR_FirstClusterHigh;
                    fseek(fImage, offset, SEEK_SET);
                    fwrite(&dir, sizeof(dir), 1, fImage);
                }
                return 1;
            }
        }
        // find next cluster
        currClust = next_cluster(currClust);
        // go to next cluster

    } while (currClust < BAD_CLUSTER);

    // no free space: need to allocate more space for the directory
    fprintf(stderr, "No free space.\n");
    return -1;
}

/* create empty directory (put "." and ".." entries) */
void create_empty_dir(uint32_t dir, uint32_t parent)
{
    int numDirs = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec / sizeof(DIR_Entry);
    DIR_Entry *dirs = calloc(numDirs, sizeof(DIR_Entry));
    assert(dirs);

    // create link to itself (".")
    dirs[0].DIR_FirstClusterLow = dir & 0xFFFF;
    dirs[0].DIR_FirstClusterHigh = dir & 0xFFFF0000;
    long_to_short(".", dirs[0].DIR_Name);
    dirs[0].DIR_Attr = ATTR_DIRECTORY;
    dirs[0].DIR_FileSize = 0;

    // create link to parent dir ("..")
    dirs[1].DIR_FirstClusterLow = parent & 0xFFFF;
    dirs[1].DIR_FirstClusterHigh = parent & 0xFFFF0000;
    long_to_short("..", dirs[1].DIR_Name);
    dirs[1].DIR_Attr = ATTR_DIRECTORY;
    dirs[1].DIR_FileSize = 0;

    uint32_t offset = cluster_to_data_offset(dir);
    fseek(fImage, offset, SEEK_SET);
    fwrite(dirs, sizeof(*dirs), numDirs, fImage);

    free(dirs);
}

/* count a free and an used clusters */
void find_free_space(uint32_t *usedClust, uint32_t *freeClust)
{
    *usedClust = *freeClust = 0;
    uint32_t offset = fat_offset(0);
    uint32_t end = offset + bpb.BPB_FATSz32 * bpb.BPB_BytsPerSec;
    fseek(fImage, offset, SEEK_SET);
    for (uint32_t curr = offset; curr < end; curr += sizeof(uint32_t))
    {
        uint32_t clust;
        fread(&clust, sizeof(uint32_t), 1, fImage);
        clust &= FAT32_MASK;
        if (clust == EMPTY)
        {
            *freeClust += 1;
        }
        else
        {
            *usedClust += 1;
        }
    }
}

/* find free cluster in FAT */
uint32_t allocate_free_cluster(void)
{
    uint32_t offset = fat_offset(0);
    uint32_t end = offset + bpb.BPB_FATSz32 * bpb.BPB_BytsPerSec;
    fseek(fImage, offset, SEEK_SET);
    for (uint32_t curr = offset; curr < end; curr += sizeof(uint32_t))
    {
        uint32_t clust;
        fread(&clust, sizeof(uint32_t), 1, fImage);
        clust &= FAT32_MASK;
        if (clust == EMPTY)
        {
            clust = EOC; // End-Of-Cluster
            fseek(fImage, curr, SEEK_SET);
            fwrite(&clust, sizeof(uint32_t), 1, fImage);
            return (curr - offset) / sizeof(uint32_t);
        }
    }
    return EOC;
}

/* convert a filename to short name */
void long_to_short(const char *lname, char *sname)
{
    int len = strlen(lname);
    if (len > 8)
        len = 8;
    for (int i = 0; i < len; ++i)
    {
        sname[i] = toupper(lname[i]);
    }
    // fill with spaces
    for (int i = len; i < SHORT_LEN; ++i)
    {
        sname[i] = ' ';
    }
}

/* convert a short to normal name */
void short_to_long(const char *sname, char *lname)
{
    strncpy(lname, sname, SHORT_LEN);
    lname[SHORT_LEN] = '\0';
}

/* compare long and short name */
int compare_names(const char *lname, char *sname)
{
    int len = strlen(lname);
    if (len > 8)
        return 0;
    for (int i = 0; i < len; ++i)
    {
        if (toupper(lname[i]) != sname[i])
            return 0;
    }
    return 1;
}

/* initialize opened files structure */
void initialize_files()
{
    memset(openedFiles, 0, sizeof(openedFiles));
}

/* find an index of opened file */
int find_opened_file(const char *fname)
{
    for (int i = 0; i < MAX_OPENED_FILES; ++i)
    {
        if (openedFiles[i].filename && strcmp(openedFiles[i].filename, fname) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* add filename to list of opened files */
int add_to_files(const char *filename, FileMode mode, uint32_t filePointer, uint32_t firstCluster)
{
    for (int i = 0; i < MAX_OPENED_FILES; ++i)
    {
        if (openedFiles[i].filename == NULL)
        {
            openedFiles[i].filename = malloc(strlen(filename) + 1);
            assert(openedFiles[i].filename);
            strcpy(openedFiles[i].filename, filename);
            openedFiles[i].mode = mode;
            openedFiles[i].filePointer = filePointer;
            openedFiles[i].firstCluster = firstCluster;
            return 1;
        }
    }
    return 0;
}

/* free memory allocated for opened filenames */
void free_files()
{
    for (int i = 0; i < MAX_OPENED_FILES; ++i)
    {
        if (openedFiles[i].filename)
        {
            free(openedFiles[i].filename);
        }
    }
}

/* Return FAT offset */
uint32_t fat_offset(uint32_t clust)
{
    return (bpb.BPB_BytsPerSec * bpb.BPB_RsvdSecCnt) + (clust * sizeof(uint32_t));
}

/* Return offset of the cluster in the data region */
uint32_t cluster_to_data_offset(uint32_t clust)
{
    return ((clust - 2) * bpb.BPB_BytsPerSec) +
           (bpb.BPB_NumFATs * bpb.BPB_FATSz32 * bpb.BPB_BytsPerSec) +
           (bpb.BPB_RsvdSecCnt * bpb.BPB_BytsPerSec);
}

/* find next cluster for the given cluster */
uint32_t next_cluster(uint32_t clust)
{
    uint32_t next;
    fseek(fImage, fat_offset(clust), SEEK_SET);
    fread(&next, sizeof(next), 1, fImage);
    return next & FAT32_MASK;
}

uint32_t make_dword(uint16_t low, uint16_t high)
{
    return low | ((uint32_t)high << 16);
}

FileMode str_to_mode(const char *mode)
{
    if (strcmp(mode, "r"))
        return READ_ONLY;
    if (strcmp(mode, "w"))
        return WRITE_ONLY;
    if (strcmp(mode, "rw"))
        return READ_AND_WRITE;
    if (strcmp(mode, "wr"))
        return WRITE_AND_READ;
    return -1;
}
