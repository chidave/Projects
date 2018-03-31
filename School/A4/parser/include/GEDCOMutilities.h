#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"


typedef struct {
	
	Individual *individual;
	
	char xref[10];
	
} Xref;

typedef struct {
	
	Family *family;
	
	char xref[10];
	
} FXref;

char* insertIndividual(char *fileName, char* firstName, char* surname, char* sex);
char* getIndAsc(char *fileName, char *name);
char* getIndDesc(char *fileName, char *name);
char* indToJSON2(const Individual* ind);
char* retrieveIndividuals(char *fileName);
char *parseFile(char *fileName);
char *getIndividuals(char *fileName);
void writeFile(char *fileName, char *subName, char *subAddress);
char* getDesc(void);
char **contValue(char *value);
char **splitValue(char *value);
void removeEndZeros(char *str);
void fakeDelete(void *toBeDeleted);
char **split(char *input);
int checkFile(char *string);
void freeArray(char **array);
int arraySize(char **array);
int is_Integer(char *s);
char *joinStrings(char *string);
void clearSubmitter(Submitter *sub);
Submitter *createSubmitter(char **Submitter_array);
Field *createField(char *fieldTag, char *fieldValue);
Header *createHeader(Submitter *submitter_data, char **Header_array);
Individual *createIndividual(char **Individual_array);
Family *createFamily(char **Family_array, Xref **refs);
int isInList(List *list, Individual *individual);
int isInList2(List *list, Family *family);
void updateIndividuals(List *individual, List *family);
char* printHeader(Header *header);
List merge(List *One, List *Two);
List getChildren(Individual* person);
void removeEndSpaces(char *str);
void getKids(Individual *person, List *descendants);
int isInArray(char array[24][6], char search[6]);
void getParents(Individual *person, List *ancestors);
int compareIndsInGen(const void* first,const void* second);
int isTag(char *string);
