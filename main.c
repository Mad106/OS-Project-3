#include <cstdio>

int main(int argc, char *argv[]){
	/* run accept .img then wait for user to input commands and respond accordingly */
	
	//take in/open .img
	FILE input = fopen(argv[1],"r+");
	
	char* command;
	
	while(command != "exit"){
		
		print("$");
		read_from_stdin(command);
		
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
