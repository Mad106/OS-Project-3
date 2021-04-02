#include <stdio.h>
#include <stdlib.h>
#inlude <unistd.h>
#include <math.h>

char* reverseEndian(long size, char* buffer);
int binarytoint(long size, char* buffer);

int main(int argc, char *argv[]){
	
	FILE* pFile;
	long lSize;
	char* buffer;
	size_t result;	/* value could reveal errors */
	char* command;
	int value;	/* converted number from binary */
	
	/* run accept .img then wait for user to input commands and respond accordingly */
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