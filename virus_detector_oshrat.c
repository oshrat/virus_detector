#define _XOPEN_SOURCE 500 
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE !FALSE

#define DIR_INDEX 1
#define VIRUS_SIGN_INDEX 2
#define VALID_ARGS_NUM 3

#define MAX_FD 20 //maximum number of file descriptors
#define AMOUNT_OF_BYTES 1

//functions declarations:
int getFileSize(FILE * filename);
void updateVirusSignature(char* virusPath);
int isELF(const char *fileName);
int isInfected(const char* elf_path);
int getCurrPath(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);
int checkValidInput(int argc);
void removeInfected(void);


//globals:
char *virusSign = 0;
char** infectedFiles = 0;
int numInfected = 0;

int main(int argc, char **argv){
	char *dirPath = "";
	int i = 0;
	infectedFiles = (char**)malloc(sizeof(char));
	if(checkValidInput(argc)){
		//arrange the args:
		dirPath = argv[DIR_INDEX];
		updateVirusSignature(argv[VIRUS_SIGN_INDEX]);

		//scan the directory tree:
		printf("Scanning started...\n");
		if (nftw(dirPath, getCurrPath, MAX_FD, FTW_PHYS) == -1){
			perror("nftw error");
			exit(EXIT_FAILURE);
		}
		free(virusSign);
		if(numInfected){
			removeInfected();
			for(; i < numInfected; i++){
				free(infectedFiles[i]);
			}
		}
		else{
			printf("your ELF's are virus free!\n");
		}
	}
	free(infectedFiles);
	return 0;
}

/*
this function checks if the number of given arguments is valid.
input: number of arguments got by the user.
output: true if valid, false otherwise.
*/
int checkValidInput(int argc){
	int ans = FALSE;
	if(argc == VALID_ARGS_NUM){
		ans = TRUE;
	}
	else{
		printf("usage: \narg[1]: a path to the root directory from which the search should be started.\narg[2]: the name of a file which contains the detected signature as a raw data.\n");
	}
	return ans;
}

/*
this function checks if a file is an ELF file and if so, checks if the file is infected with a specific virus. 
the function also saves the infected files.
input:  fpath - the file's path.
	sb - a pointer to the stat buffer containing information on the object.
	tflag - integer that giving additional information on the object.
	ftwbuf- a pointer to an FTW structure.
nftw() calls this function with the four arguments specified above. 
output: true if infected, false otherwise.
*/
int getCurrPath(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	if(isELF(fpath)){
		if(isInfected(fpath)){
			printf("File %s is infected!\n", fpath);
			numInfected++;
			infectedFiles = (char**)realloc(infectedFiles, sizeof(char*) * numInfected);
			infectedFiles[numInfected - 1] = (char*)calloc(strlen(fpath), sizeof(char));
			strncpy(infectedFiles[numInfected - 1], fpath, strlen(fpath));
		}
	}
	return 0;// To tell nftw() to continue
}

/*
the function finds the size of a file.
input: pointer to the file.
output: the size of the file.
*/
int getFileSize(FILE * filename){
	int size = 0;
	fseek(filename, 0, SEEK_END); // seek to end of file
	size = ftell(filename); 
	fseek(filename, 0, SEEK_SET); //go back to the beginning
	return size;
}

/*
the function takes the virus signature from a file and update the global variable.
input: the file's path.
output: none (the global sign is update now).
*/
void updateVirusSignature(char* virusPath){
	FILE* virus = fopen(virusPath, "rb");
	int sizeVi = 0;
	virusSign = (char*)malloc(sizeof(char) * sizeVi);
	if(virus){
		sizeVi = getFileSize(virus);
		virusSign = (char*)realloc(virusSign, sizeof(char) * sizeVi);
		fread(virusSign, sizeof(char), sizeVi, virus);
		fclose(virus);
	}
}

/*
this function checks if a file is an ELF file.
input: the file's path.
output: true if it is an ELF file, false otherwise.
*/
int isELF(const char *fileName){
	int ans = FALSE;
	char buffer[100];
	FILE* file = fopen(fileName, "rb");
	if(file){
		if(fread(buffer, sizeof(char), 4, file)){
			//every ELF starts with this inital
			if(buffer[0] == 0x7f && buffer[1] == 'E' && buffer[2] == 'L' && buffer[3] == 'F'){
				ans = TRUE;
			}
		}
		fclose(file);
	}
	else{
		printf("ERROR: something went wrong with opening or reading the file specified.\n");
	}
	return ans;
}

/*
this function checks if a file is infected with a specific virus.
input: the file's path.
output: true if infected, false otherwise.
*/
int isInfected(const char* elf_path){
	int found = FALSE;
	FILE* fileToScan = fopen(elf_path, "rb");
	int fileByte = 0, signByte = 0;
	int i = 0; //index for the virus signature
	int currPosition = 0;
	int fileSize = 0;

	if(fileToScan){
		fileSize = getFileSize(fileToScan);
		fseek(fileToScan, 0, SEEK_SET);
		for(; currPosition < fileSize && !found; currPosition++){
			// Go to the beginning of the file (where we should start the scan now)
			fseek(fileToScan, currPosition, SEEK_SET);
			fread(&fileByte, sizeof(char), AMOUNT_OF_BYTES, fileToScan); //read a byte at the time
			signByte = *(virusSign + i);
			if (fileByte == signByte){ //found a match, search for the next byte
				i++;
				if (i == strlen(virusSign)){
					found = TRUE;
				}
			}
			else{
				i = 0; //start a new comparison
			}
		}
		fclose(fileToScan);
	}
	return found;
}

/*
this function remove all the infected files according to the user's choice. 
input: none.
output: none.
*/
void removeInfected(void){
	int i = 0;
	char toRemove = 0;
	printf("would you like to remove the infected files from your computer? Y/N\n");
	toRemove = getchar();
	if(toRemove == 'Y'){
		for(; i < numInfected; i++){
			remove(infectedFiles[i]);
			printf("%s removed\n", infectedFiles[i]);
		}
	}
	else{
		printf("files not deleted, be careful!\n");
	}
}
