#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedListAPI.h"

#define MAXSWAPS 3

typedef struct {
    
    char* id;
    
    int size;
    
    int swapTimes;
    
    int location;
    
    int end;
    
} Process;

char **split(char *input);
char *print(void *toBePrinted);
void delete(void *toBeDeleted);
int compare(const void* first,const void* second);
void reset(Process **procArray);
void simFirstFit(Process **procArray);
void simBestFit(Process **procArray);
void simWorstFit(Process **procArray);

int totalMem = 128;
int mem[128];

int main(int argc, char** argv) {
	
	char **array;
	char **fileRecords = malloc(sizeof(char*));
	char line[1000];
	int line_count = 0;
	int count = 0;
	Process **processes;
	char *temp;
	
	if(argc != 2) {
		printf("usage: %s inputFile\n", argv[0]);
		return -1;
	}
	
	FILE *fptr;
	fptr = fopen(argv[1], "r");
	
	if(fptr == NULL) {
		printf("Unable to open file %s\n", argv[1]);
		return -1;
	}
	
	while(fgets(line, 1000, fptr) != NULL) {
		
		count = 0;
		line[strcspn(line, "\r\n")] = 0;
		temp = malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(temp, line);
		array = split(line);
		
		while(array[count] != NULL) {
			count++;
		}
		if(count != 2) {
			printf("Invalid file contents\n");
			return -1;
		}
		
		fileRecords[line_count] = temp;
		line_count++;
		fileRecords = realloc(fileRecords, sizeof(char *) * (line_count + 1));
		
	}
	
	processes = malloc(sizeof(Process*) * line_count);
	
	count = 0;
	while(fileRecords[count] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[count]) + 1));
		strcpy(temp, fileRecords[count]);
		array = split(temp);
		
		processes[count] = malloc(sizeof(Process));
		processes[count]->id = array[0];
		processes[count]->size = atoi(array[1]);
		processes[count]->swapTimes = 0;
		processes[count]->location = -1;
		processes[count]->end = -1;
		
		count++;
	}
	processes[count] = NULL;
	
	/*List new = initializeList(&print, &delete, &compare);
	count = 0;
	while(processes[count] != NULL) {
		
		insertBack(&new, (void*)processes[count]);
		count++;
		
	}*/
	
	simFirstFit(processes);
	reset(processes);
	simBestFit(processes);
	reset(processes);
	simWorstFit(processes);
	
	
	return 0;
	
}

char **split(char *input) {
	
	const char s[2] = " ";
	char *token;
	char **array = malloc(sizeof(char*));
	int count = 0;
	
	token = strtok(input,s);
	
	while(token != NULL) {
		array[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array = realloc(array, (count + 1) * sizeof(char*));
	}
	array[count] = NULL;
	
	return array;
	
}

char *print(void *toBePrinted) {
	
	char *returnVal = malloc(10);
	strcpy(returnVal, "RETURN");
	
	return returnVal;
	
}

void delete(void *toBeDeleted) {

	return;

}

int compare(const void* first,const void* second) {
	
	return 0;
	
}

void reset(Process **procArray) {
	
	int count = 0;
	while(procArray[count] != NULL) {
		procArray[count]->swapTimes = 0;
		procArray[count]->location = -1;
		procArray[count]->end = -1;
		count++;
	}
	
}

void simFirstFit(Process **procArray) {
	
	int count = 0;
	List ready = initializeList(&print, &delete, &compare);
	List storage = initializeList(&print, &delete, &compare);
	while(procArray[count] != NULL) {
		insertBack(&ready, (void*)procArray[count]);
		count++;
	}
	count = 0;
	for(count = 0; count < 128; count++) {
		mem[count] = 0;
	}
	
	int a = 0;
    int largestHole = 0;
    int largestHoleAddress;
    int currentHole = 0;
    int totalHoles = 0;
    int numHoles = 0;
    int loads = 0;
    int numProcesses = 0;
    int totalProcesses = 0;
    int memUsed = 0;
    int i = 0;
    int fit = 0;
    double memory = 0;

	//int counter = 0;
    printf("First Fit Algorithm\n");
    Node *node = ready.head;
    while(node != NULL) {
		Process *tempo = (Process*)node->data;
		i = 0;
		while(i < 128) {
            if(mem[i] == 0){
                a = i;
                if(i < 128){
                    while(mem[i] == 0 && i < 128){
                        currentHole++;
                        i++;
                    }
                }
                numHoles++;
            }
            if(currentHole >= tempo->size && fit == 0){
               largestHole = currentHole;
               largestHoleAddress = a;
               fit = 1;
            }
            currentHole = 0;
            if(i < 128){            
                memUsed++;
            }
            i++;
        }
		
        if(largestHole >= tempo->size){
            for(i = largestHoleAddress; i < (tempo->size + largestHoleAddress); i++){
                mem[i] = 1;
                memUsed++;
            }
            
            memory = memory + (double)((memUsed/1.28));
            
            tempo->location = largestHoleAddress;
			tempo->end = tempo->location + tempo->size;
            insertBack(&storage, (void*)tempo);
            node = node->next;
            numProcesses++;
            loads++;
            totalHoles += numHoles;
            totalProcesses += numProcesses;
            
			
            printf("%s loaded, #processes = %d, #holes = %d, memusage = %.2f, cumulative mem = %.2f\n", tempo->id, numProcesses, numHoles, (double)((memUsed/1.28)), (double)(memory/loads));  
        }
        else{
			
			Node *node2 = storage.head;
			if(node2 == NULL) {
				break;
			}
			Process *run = (Process*)node2->data;
			
			for(i = run->location; i < run->end; i++){
				mem[i]=0;
            }
			run->swapTimes++;
			if(run->swapTimes == 3){
				removeFront(&storage);
				numProcesses--;
			}
			else{
				
				insertBack(&ready, (void*)run);;
				removeFront(&storage);
				numProcesses--;    
			}
		}
        currentHole = 0;
        largestHole = 0;
        largestHoleAddress = 0;
        numHoles = 0;
        memUsed = 0;
        fit = 0;
        
		//node = node->next;
	}
	/*printf("\n******************************************\n");
    printf("First Fit ended:\n");*/
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative mem = %.2f\n\n", loads, (double)((double)totalProcesses/(double)loads), (double)((double)totalHoles/(double)loads), (double)(memory/loads));

}

void simBestFit(Process **procArray) {
	
	int count = 0;
	List ready = initializeList(&print, &delete, &compare);
	List storage = initializeList(&print, &delete, &compare);
	while(procArray[count] != NULL) {
		insertBack(&ready, (void*)procArray[count]);
		count++;
	}
	count = 0;
	for(count = 0; count < 128; count++) {
		mem[count] = 0;
	}
	
    int a = 0;
    int largestHole = 0;
    int largestHoleAddress;
    int currentHole = 0;
    int totalHoles = 0;
    int numHoles = 0;
    int loads = 0;
    int numProcesses = 0;
    int totalProcesses = 0;
    int memUsed = 0;
    int i = 0;
    double memory = 0;
    //queue * new = NULL;
    //queue * mem = NULL;
    printf("Best Fit Algorithm\n");
    
    Node *node = ready.head;
    while(node != NULL) {
		Process *tempo = (Process*)node->data;
		i = 0;
		while(i < 128) {
            if(mem[i] == 0){
                a = i;
                if(i < 128){
                    while(mem[i] == 0 && i < 128){
                        currentHole++;
                        i++;
                    }
                }
                numHoles++;
            }
            if(largestHole == 0){
                largestHole = currentHole;
            }
            if(currentHole <= largestHole && currentHole >= tempo->size && currentHole != 0){
               largestHole = currentHole;
               largestHoleAddress = a;
            }
            currentHole = 0;
            if(i < 128){            
                memUsed++;
            }
            i++;
        }
		
        if(largestHole >= tempo->size){
            for(i = largestHoleAddress; i < (tempo->size + largestHoleAddress); i++){
                mem[i] = 1;
                memUsed++;
            }
            
            memory = memory + (double)((memUsed/1.28));
            
            
            tempo->location = largestHoleAddress;
			tempo->end = tempo->location + tempo->size;
            insertBack(&storage, (void*)tempo);
            node = node->next;
            numProcesses++;
            loads++;
            totalHoles += numHoles;
            totalProcesses += numProcesses;
            
			
            printf("%s loaded, #processes = %d, #holes = %d, memusage = %.2f, cumulative mem = %.2f\n", tempo->id, numProcesses, numHoles, (double)((memUsed/1.28)), (double)(memory/loads));  
        }
        else{
			
			Node *node2 = storage.head;
			if(node2 == NULL) {
				break;
			}
			Process *run = (Process*)node2->data;
			
			for(i = run->location; i < run->end; i++){
				mem[i]=0;
            }
			run->swapTimes++;
			if(run->swapTimes == 3){
				removeFront(&storage);
				numProcesses--;
			}
			else{
				
				insertBack(&ready, (void*)run);;
				removeFront(&storage);
				numProcesses--;    
			}
		}
        currentHole = 0;
        largestHole = 0;
        largestHoleAddress = 0;
        numHoles = 0;
        memUsed = 0;
        //fit = 0;
        
		//node = node->next;
	}
	/*printf("\n******************************************\n");
    printf("Best Fit ended:\n");*/
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative mem = %.2f\n\n", loads, (double)((double)totalProcesses/(double)loads), (double)((double)totalHoles/(double)loads), (double)(memory/loads));
	
}

void simWorstFit(Process **procArray) {
	
	int count = 0;
	List ready = initializeList(&print, &delete, &compare);
	List storage = initializeList(&print, &delete, &compare);
	while(procArray[count] != NULL) {
		insertBack(&ready, (void*)procArray[count]);
		count++;
	}
	count = 0;
	for(count = 0; count < 128; count++) {
		mem[count] = 0;
	}
	
	int a = 0;
    int largestHole = 0;
    int largestHoleAddress;
    int currentHole = 0;
    int totalHoles = 0;
    int numHoles = 0;
    int loads = 0;
    int numProcesses = 0;
    int totalProcesses = 0;
    int memUsed = 0;
    int i = 0;
    double memory = 0;
	
	printf("Worst Fit Algorithm\n");
    Node *node = ready.head;
    while(node != NULL) {
		Process *tempo = (Process*)node->data;
		i = 0;
		while(i < 128) {
            if(mem[i] == 0){
                a = i;
                if(i < 128){
                    while(mem[i] == 0 && i < 128){
                        currentHole++;
                        i++;
                    }
                }
                numHoles++;
            }
            if(currentHole > largestHole){
               largestHole = currentHole;
               largestHoleAddress = a;
            }
            currentHole = 0;
            if(i < 128){            
                memUsed++;
            }
            i++;
        }
        
        if(largestHole >= tempo->size){
            for(i = largestHoleAddress; i < (tempo->size + largestHoleAddress); i++){
                mem[i] = 1;
                memUsed++;
            }
            
            memory = memory + (double)((memUsed/1.28));
            
            tempo->location = largestHoleAddress;
			tempo->end = tempo->location + tempo->size;
            insertBack(&storage, (void*)tempo);
            node = node->next;
            numProcesses++;
            loads++;
            totalHoles += numHoles;
            totalProcesses += numProcesses;
            
			
            printf("%s loaded, #processes = %d, #holes = %d, memusage = %.2f, cumulative mem = %.2f\n", tempo->id, numProcesses, numHoles, (double)((memUsed/1.28)), (double)(memory/loads));  
        }
        else{
			
			Node *node2 = storage.head;
			if(node2 == NULL) {
				break;
			}
			Process *run = (Process*)node2->data;
			
			for(i = run->location; i < run->end; i++){
				mem[i]=0;
            }
			run->swapTimes++;
			if(run->swapTimes == 3){
				removeFront(&storage);
				numProcesses--;
			}
			else{
				
				insertBack(&ready, (void*)run);;
				removeFront(&storage);
				numProcesses--;    
			}
		}
        currentHole = 0;
        largestHole = 0;
        largestHoleAddress = 0;
        numHoles = 0;
        memUsed = 0;
        
		//node = node->next;
	}

    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative mem = %.2f\n\n", loads, (double)((double)totalProcesses/(double)loads), (double)((double)totalHoles/(double)loads), (double)(memory/loads));
		
}

