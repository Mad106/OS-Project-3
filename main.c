#include <stdio.h>

int main(int argc, char *argv[]){
	/* run accept .img then wait for user to input commands and respond accordingly */
	
	//take in/open .img
	FILE input = fopen(argv[1],"r+");
	
	char* command;
	
	while(command != "exit"){
		
		print("$");
		scanf("%s",command);
		
		if(command == "info"){
			
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
