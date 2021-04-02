#include <stdlib.h>
#include <stdio.h>

#define READSIZE 32
#define FIRSTDATASECTOR 2050

struct DIRENTRY {
	unsigned char DIR_Name[11];
	unsigned char DIR_Attr[1];
	unsigned char DIR_NTRes[1];
	unsigned char DIR_CrtTimeTenth[1];
	unsigned char DIR_CrtTime[2];
	unsigned char DIR_CrtDate[2];
	unsigned char DIR_LstAccDate[2];
	unsigned char DIR_FstClusHI[2];
	unsigned char DIR_WrtTime[2];
	unsigned char DIR_WrtDate[2];
	unsigned char DIR_FstClusLo[2];
	unsigned char DIR_FileSize[4];
} __attribute__((packed));

/* Function Prototypes */
int FileOffset(int clusterNum);
int FileExists(const unsigned char* fName, int fd); // ret 1 if file exists, else 0
void ReadEntry(struct DIRENTRY* entry, int fd);

int main(int argc, char *argv[]){
	/* run accept .img then wait for user to input commands and respond accordingly */

	/* open fat32 image (argv[1] with read/write permissions */
	/* fd tracks position inside fat32 image, reads data into buff */
	int fd;
	fd = open(argv[1], 2);

	/* if input file didn't open, print error and quit */
	if(fd < 0){
		printf("File %s not found", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	/* user command input */
	char* command = (char*)malloc(sizeof(char*));
	printf("$");
	scanf("%s", command);

	while(strcmp("exit", command) != 0){
		/* prompt and obtain user input */	
		printf("$");
		scanf("%s", command);
		
		if(command == "info"){
			
		}else if(command == "size"){
			
		}else if(command == "ls"){
			
		}else if(command == "cd"){
			
		}else if(command == "creat"){
			
		}else if(command == "mkdir"){
		
		}else if(command == "mv"){
			
		}else if(command == "open"){
			
		}else if(command == "close"){
			
		}else if(command == "lseek"){
			
		}else if(command == "read"){
			
		}else if(command == "write"){
			
		}else if(command == "rm"){
			
		}else if(command == "cp"){
			
		}else{
			printf("%s is an invalid command\n",command);
		}
		
	}
		
	return 0;
}

int FileOffset(int clusterNum)
{
	return (FIRSTDATASECTOR + (clusterNum-2))*512;
}

int FileExists(const unsigned char* fName, int fd)
{
	struct DIRENTRY* entry = (struct DIRENTRY*) malloc(sizeof(struct DIRENTRY*));

	/* go to fileContents in fd */
	lseek(fd, FileOffset(2), SEEK_SET);

	/* read into DIR_entry struct. Find fName entry */
	/* If long file entry, skip. */

	/* LEFT OFF HERE */
}


void ReadEntry(struct DIRENTRY* entry, int fd)
{
	read(fd, entry->DIR_Name, 11);
	read(fd, entry->DIR_Attr, 1);
	read(fd, entry->DIR_NTRes, 1);
	read(fd, entry->DIR_CrtTimeTenth, 1);
	read(fd, entry->DIR_CrtTime, 2);
	read(fd, entry->DIR_CrtDate, 2);
	read(fd, entry->DIR_LstAccDate, 2);
	read(fd, entry->DIR_FstClusHI, 2);
	read(fd, entry->DIR_WrtTime, 2);
	read(fd, entry->DIR_WrtDate, 2);
	read(fd, entry->DIR_FstClusLo, 2);
	read(fd, entry->DIR_FileSize, 4);
}

