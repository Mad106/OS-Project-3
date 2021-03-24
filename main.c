#include <cstdio>

int main(int argc, char *argv[]){
	/* run accept .img then wait for user to input commands and respond accordingly */
	
	//take in/open .img
	FILE input = fopen(argv[1],"r+");
	
	char* command;
	
	while(command != "Exit"){
		
		gets(command);
		
		if(command == "Info"){
			
		}else if(strncmp("size",command,4)){
			
		}else if(strncmp("ls",command,2)){
			
		}else if(strncmp("cd",command,2)){
			
		}else if(strncmp("creat",command,5)){
			
		}else if(strncmp("mkdir",command,5)){
		
		}else if(strncmp("mv",command,2)){
			
		}else if(strncmp("open",command,4)){
			
		}else if(strncmp("close",command,5)){
			
		}else if(strncmp("lseek",command,5)){
			
		}else if(strncmp("read",command,4)){
			
		}else if(strncmp("write",command,5)){
			
		}else if(strncmp("rm",command,2)){
			
		}else if(strncmp("cp",command,2)){
			
		}else{
			printf("%s is an invalid command\n",command);
		}
		
	}
	
	
	
	return 0;
}