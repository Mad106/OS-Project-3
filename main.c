<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#inlude <unistd.h>
#include <math.h>

char* reverseEndian(long size, char* buffer);
int binarytoint(long size, char* buffer);
=======
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
>>>>>>> f31f49329c5cb4bf8b69dceafc572ae7b3f75d51

int main(int argc, char *argv[]){
	
	FILE* pFile;
	long lSize;
	char* buffer;
	size_t result;	/* value could reveal errors */
	char* command;
	int value;	/* converted number from binary */
	
	/* run accept .img then wait for user to input commands and respond accordingly */
<<<<<<< HEAD
	/* take in/open .img */
	FILE* pFile = open(argv[1],"r+");
	if(pFile == NULL){
		fputs("File error\n", stderr);
		exit(1);
	}
	
	/* allocate memory to contain the whole file: */
	buffer = (char*)malloc(sizeof(char)*lSize);
	if(buffer == NULL){
		fputs("Memory error\n",stderr);
		exit(2);
	}
	
	/* copy file into buffer */
	/* this section is probably temporary bc will likely only store bytes
		needed for specific commands */
	result = read(0,buffer,lSize);
	if(result != lSize){
		fputs("Reading error",stderr);
		exit(3);
	}
	
	/* whole file is not loaded in the memory buffer */
	
	while(command != "exit"){
		
		print("$");
		scanf("%s",command);
		if(command == "exit"){
			fclose(pFile);
			free(buffer);
		}else if(command == "info"){	/* deal with little endian */
			/* bytes per sector */
			read(11,(void*)buffer,2);
			lSize = 2;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("Bytes per Sector: %d\n", value);
			
			/* sectors per cluster */
			read(13,(void*)buffer,1);
			lSize = 1;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("Sectors per Cluster: %d\n", value);
			
			/* reserved sector count */
			read(14,(void*)buffer,2);
			lSize = 2;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("Reserved Sector Count: %d\n", value);
			
			/* number of FATs */
			read(16,(void*)buffer,1);
			lSize = 1;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("Number of FATs: %d\n", value);
			
			/* total sectors */
			read(19,(void*)buffer,2);
			lSize = 2;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("Total Sector: %d\n", value);
			
			/* FAT size */
			read(22,(void*)buffer,2);
			lSize = 2;
			value = binarytoint(lSize,reverseEndian(lSize,(char*)buffer));
			printf("FAT Size: %d\n", value;);
			
			/* root cluster */
=======

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
>>>>>>> f31f49329c5cb4bf8b69dceafc572ae7b3f75d51
			
		}else if(command == "size"){
			scanf("%s",command);
			
		}else if(command == "ls"){
			scanf("%s",command);
			
		}else if(command == "cd"){
			scanf("%s",command);
			
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
<<<<<<< HEAD
	
	return 0;
}

char* reverseEndian(long size, char* buffer){
	char result[size];
	for(int i = 0; i < size; i++){
		result[i] = buffer[size-i-1];
	}
	return result;
}

int binarytoint(long size, char* buffer){
	int value = 0;
	for(int i = 0; i < size; i++){
		value += buffer[i]*pow(2,i);
	}
	return value;
}
=======
		
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

>>>>>>> f31f49329c5cb4bf8b69dceafc572ae7b3f75d51
