#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

char* insertIndividual(char *fileName, char* firstName, char* surname, char* sex) {
	
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror e = createGEDCOM(filepath, &obj);
	
	if(e.type != OK) {
		char *returnStr = malloc(10);
		strcpy(returnStr, "FAILED");
		return returnStr;
	}
	
	Individual *ind = malloc(sizeof(Individual));
	ind->givenName = malloc(sizeof(char) * (strlen(firstName) + 1));
	strcpy(ind->givenName, firstName);
	ind->surname = malloc(sizeof(char) * (strlen(surname) + 1));
	strcpy(ind->surname, surname);
	
	ind->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	ind->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
	ind->otherFields = initializeList(&printField, &deleteField, &compareFields);
	
	if(strcmp(sex, "") != 0) {
		Field* field = malloc(sizeof(Field));
		field->tag = malloc(10);
		field->value = malloc(5);
		strcpy(field->tag, "SEX");
		strcpy(field->value, sex);
		insertSorted(&(ind->otherFields), (void*)field);
	}
	
	addIndividual(obj, ind);
	remove(filepath);
	writeGEDCOM(filepath, obj);
	
	char *returnStr = malloc(10);
	strcpy(returnStr, "SUCCESS");
	return returnStr;
	
	
}

char* getIndAsc(char *fileName, char *name) {
	
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror e = createGEDCOM(filepath, &obj);
	
	if(e.type != OK) {
		char *returnStr = malloc(10);
		strcpy(returnStr, "FAILED");
		return returnStr;
	}
	
	Node *node = obj->individuals.head;
	Individual *ind = NULL;
	char *fullname;
	int size = 0;;
	while(node != NULL) {
		ind = (Individual*)node->data;
		size = strlen(ind->givenName) + strlen(ind->surname) + 10;
		fullname = malloc(sizeof(char) * size);
		strcpy(fullname, ind->givenName);
		strcat(fullname, " ");
		strcat(fullname, ind->surname);
		if(strcmp(name, fullname) == 0) {
			break;
		}
		ind = NULL;
		free(fullname);
		
		node = node->next;
	}
	
	if(ind == NULL) {
		char *returnStr = malloc(10);
		strcpy(returnStr, "FAILED");
		return returnStr;
	}
	
	List ancestors = getAncestorListN(obj, ind, 0);
	char *returnVal = gListToJSON(ancestors);
	
	int i = 0;
	for(i = 0; i < strlen(returnVal); i++) {
		if(returnVal[i] == '\\') {
			returnVal[i] = ',';
		}
	}
	
	return returnVal;
	
}

char* getIndDesc(char *fileName, char *name) {
	
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror e = createGEDCOM(filepath, &obj);
	
	if(e.type != OK) {
		char *returnStr = malloc(10);
		strcpy(returnStr, "FAILED");
		return returnStr;
	}
	
	Node *node = obj->individuals.head;
	Individual *ind = NULL;
	char *fullname;
	int size = 0;;
	while(node != NULL) {
		ind = (Individual*)node->data;
		size = strlen(ind->givenName) + strlen(ind->surname) + 10;
		fullname = malloc(sizeof(char) * size);
		strcpy(fullname, ind->givenName);
		strcat(fullname, " ");
		strcat(fullname, ind->surname);
		if(strcmp(name, fullname) == 0) {
			break;
		}
		ind = NULL;
		free(fullname);
		
		node = node->next;
	}
	
	if(ind == NULL) {
		char *returnStr = malloc(10);
		strcpy(returnStr, "FAILED");
		return returnStr;
	}
	
	List descendants = getDescendantListN(obj, ind, 0);
	char *returnVal = gListToJSON(descendants);
	
	int i = 0;
	for(i = 0; i < strlen(returnVal); i++) {
		if(returnVal[i] == '\\') {
			returnVal[i] = ',';
		}
	}
	
	return returnVal;
	
}

char* indToJSON2(const Individual* ind) {
	
	char *return_string;
	if(ind == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	int i = 0;
	
	Individual *print = (Individual*)ind;
	char *first;
	char *second;
	char *sex;
	
	if(print->givenName[0] == '\0' || print->givenName == NULL) {
		first = malloc(sizeof(char));
		first[0] = '\0';
	}else{
		for(i = 0; i < strlen(print->givenName); i++) {
			if(print->givenName[i] == '\"') {
				print->givenName[i] = '!';
			}
		}
		first = malloc(sizeof(char) * (strlen(print->givenName) + 1));
		strcpy(first, print->givenName);
	}
	
	if(print->surname[0] == '\0' || print->surname == NULL) {
		second = malloc(sizeof(char));
		second[0] = '\0';
	}else{
		for(i = 0; i < strlen(print->surname); i++) {
			if(print->surname[i] == '\"') {
				print->surname[i] = '!';
			}
		}
		second = malloc(sizeof(char) * (strlen(print->surname) + 1));
		strcpy(second, print->surname);
	}
	
	int sexP = 0;
	Node *node = ind->otherFields.head;
	while(node != NULL) {
		Field *field = (Field*)node->data;
		if(strcmp(field->tag, "SEX") == 0) {
			sexP++;
			sex = malloc(sizeof(char) + (strlen(field->value) + 1));
			strcpy(sex, field->value);
		}
		node = node->next;
	}
	if(sexP == 0) {
		sex = malloc(3);
		strcpy(sex, "");
	}
	
	int printSize = strlen("givenname:surname:sex:") + strlen(first) + strlen(second) + strlen(sex) + 100;
	
	return_string = malloc(sizeof(char) * printSize);
	
	snprintf(return_string, printSize, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\"}", first, second, sex);
	free(first);
	free(second);
	free(sex);
	
	return return_string;
	
}

char* retrieveIndividuals(char *fileName) {
	
	//printf("HERE!!!\n");
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror e = createGEDCOM(filepath, &obj);
	
	if(e.type != OK) {
		char *returnValue = malloc(20);
		strcpy(returnValue, "FAILED");
		
		return returnValue;
	}
	
	if(obj->individuals.length == 0) {
		char *return_string = malloc(sizeof(char) * 4);
		strcpy(return_string, "[]");
		return return_string;
	}
	
	
	Node *node = obj->individuals.head;
	int size = 0;
	char **JSON = malloc(sizeof(char*) * size);
	int i = 0;
	
	while(node != NULL) {
		Individual *ind = (Individual*)node->data;
		
		JSON[i] = indToJSON2(ind);
		//printf("%d\n", i);
		size = size + strlen(JSON[i]) + 3;
		node = node->next;
		i++;
	}
	
	char *returnStr = malloc(sizeof(char) * size);
	strcpy(returnStr, "[");
	strcat(returnStr, JSON[0]);
	int j = 1;
	for(j = 1; j < i; j++) {
		strcat(returnStr, ",");
		strcat(returnStr, JSON[j]);
	}
	strcat(returnStr, "]");
		
	return returnStr;
	
	
}

char *parseFile(char *fileName) {
	
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror e = createGEDCOM(filepath, &obj);
	
	if(e.type != OK) {
		
		char *returnString = malloc(150);
		snprintf(returnString, 150, "{\"filename\":\"%s\",\"source\":\"\",\"version\":\"\",\"encoding\":\"\",\"subName\":\"\",\"subAddress\":\"\",\"numOfInd\":\"\",\"numOfFam\":\"\"}", fileName);
		return returnString;
		
	}
	
	char *encoding = malloc(10);
	if(obj->header->encoding == ANSEL) {
		strcpy(encoding, "ANSEL");
	}else if(obj->header->encoding == UTF8) {
		strcpy(encoding, "UTF-8");
	}else if(obj->header->encoding == UNICODE) {
		strcpy(encoding, "UNICODE");
	}else if(obj->header->encoding == ASCII) {
		strcpy(encoding, "ASCII");
	}
	else{
		//
	}
	
	char *numOfInd = malloc(10);
	snprintf(numOfInd, 10, "%d", obj->individuals.length);
	
	char *numOfFam = malloc(10);
	snprintf(numOfFam, 10, "%d", obj->families.length);
	
	char *version = malloc(5);
	snprintf(version, 5, "%.1f", obj->header->gedcVersion);
	
	int size = strlen(fileName) + strlen(obj->header->source) + strlen(version) + strlen(encoding) + strlen(obj->submitter->submitterName) + strlen(obj->submitter->address) + strlen(numOfInd) + strlen(numOfFam) + 200;
	char *returnVal = malloc(sizeof(char) * size);
	snprintf(returnVal, size, "{\"filename\":\"%s\",\"source\":\"%s\",\"version\":\"%s\",\"encoding\":\"%s\",\"subName\":\"%s\",\"subAddress\":\"%s\",\"numOfInd\":\"%s\",\"numOfFam\":\"%s\"}",fileName,obj->header->source,version,encoding,obj->submitter->submitterName,obj->submitter->address,numOfInd,numOfFam);
	
	free(numOfFam);
	free(numOfInd);
	free(version);
	free(encoding);
	
	return returnVal;
	
}

char *getIndividuals(char *fileName) {
	
	GEDCOMobject *obj;
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	GEDCOMerror error = createGEDCOM(filepath, &obj);
	
	if(error.type != OK) {
		char *returnValue = malloc(20);
		strcpy(returnValue, "FAILED");
		
		return returnValue;
		
	}
	
	
	char *returnVal = iListToJSON(obj->individuals);
	//printf("%s\n", returnVal);
	
	int i = 0;
	for(i = 0; i < strlen(returnVal); i++) {
		if(returnVal[i] == '\\') {
			returnVal[i] = ',';
		}
	}
	
	return returnVal;
	
}

char* getDesc(void){
    char str[] = "[[{\"givenName\":\"Al\",\"surname\":\"Foo\"},{\"givenName\":\"Bill\",\"surname\":\"Foo\"}],[{\"givenName\":\"Jill\",\"surname\":\"Foo\"},{\"givenName\":\"June\",\"surname\":\"Foo\"}]]";
    char* retStr = malloc(strlen(str)+1);
    strcpy(retStr, str);
    
    return retStr;
}

void writeFile(char *fileName, char *subName, char *subAddress) {
	
	GEDCOMobject *obj = malloc(sizeof(GEDCOMobject));
	Header *header = malloc(sizeof(Header));
	
	strcpy(header->source, "Genealogy App");
	header->gedcVersion = 5.5;
	header->encoding = UTF8;
	header->otherFields = initializeList(&printField, &deleteField, &compareFields);
	
	Submitter *sub = malloc(sizeof(Submitter) + (sizeof(char) * (strlen(subAddress) + 1)));
	strcpy(sub->submitterName, subName);
	strcpy(sub->address, subAddress);
	sub->otherFields = initializeList(&printField, &deleteField, &compareFields);
	
	header->submitter = sub;
	obj->header = header;
	obj->submitter = sub;
	obj->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
	obj->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
	
	char dir[10];
	strcpy(dir, "uploads/");
	char *filepath = malloc(sizeof(char) * (strlen(dir) + strlen(fileName) + 1));
	strcpy(filepath, dir);
	strcat(filepath, fileName);
	
	GEDCOMerror error = writeGEDCOM(filepath, obj);
	if(error.type == OK) {
		//
	}
	
	
}

char **contValue(char *value) {
	
	int i = 0;
	char **array = malloc(sizeof(char*));
	int line_count = 0;
	char line[1000];
	int l = 0;
	char *temp;
	int size = 0;
	int reset = 0;
	
	i = 0;
	while(value[i] != '\0') {
		if(value[i] == '\n' || value[i] == '\r') {
			value[i] = ' ';
		}
		i++;
	}
	
	i = 0;
	while(value[i] != '\0') {
		size++;
		if(size > 230 && reset == 1) {
			size = 0;
			reset = 0;
		}
		
		if(value[i] == ' ' && size > 230) {
			reset = 1;
			value[i] = '\n';
		}
		i++;
	}
	
	i = 0;
	while(value[i] != '\0') {
		l = 0;
		memset(line,0,sizeof(line));
		while(value[i] != '\n') {
			line[l] = value[i];
			
			l++;
			i++;
			if(value[i] == '\0') {
				break;
			}
		}
		line[l] = '\0';
		temp = malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(temp, line);
		
		array[line_count] = temp;
		line_count++;
		array = realloc(array, sizeof(char *) * (line_count + 1));
		if(value[i] == '\0') {
			break;
		}
		
		i++;
	}
	array[line_count] = NULL;
	
	return array;
	
}

char **splitValue(char *value) {
	
	int i = 0;
	char line[1000];
	char **array = malloc(sizeof(char*));
	int line_count = 0;
	int l = 0;
	char *temp;
	
	while(value[i] != '\0') {
		l = 0;
		memset(line,0,sizeof(line));
		while(value[i] != '\n') {
			line[l] = value[i];
			
			l++;
			i++;
			if(value[i] == '\0') {
				break;
			}
		}
		line[l] = '\0';
		temp = malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(temp, line);
		
		array[line_count] = temp;
		line_count++;
		array = realloc(array, sizeof(char *) * (line_count + 1));
		if(value[i] == '\0') {
			break;
		}
		
		i++;
	}
	array[line_count] = NULL;
	
	return array;
}


void removeEndZeros(char *str) {
	
    int index;
    int i;

    i = 0;
    while(str[i] != '\0'){
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r' && str[i] != '0'){
			index = i;
		}
		i++;
	}

    str[index + 1] = '\0';
}

void removeEndSpaces(char *str) {
	
    int index;
    int i;

    i = 0;
    while(str[i] != '\0'){
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r'){
			index = i;
		}
		i++;
	}

    str[index + 1] = '\0';
}

void fakeDelete(void *toBeDeleted) {
	
	return;
}

int isTag(char *string) {
	
	if(string[0] == '@' && string[strlen(string) - 1] == '@') {
		return 1;
	}
	
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

int checkFile(char *string) {
	
	char *token;
	char *temp = malloc(sizeof(char) * (strlen(string) + 1));
	strcpy(temp, string);
	char s[2] = ".";
	int i = 0;
	char *check = malloc(100);
	
	token = strtok(temp, s);
	while(token != NULL) {
		i++;
		strcpy(check, token);
		token = strtok(NULL, s);
	}
	
	if((strcmp(check,"ged") == 0) && (i == 2)) {
		free(temp);
		free(check);
		return 1;
	}
	free(temp);
	free(check);
	return 0;
	
}

void freeArray(char **array) {
	
	if(array == NULL)return;
	
	int i = 0;
	while(array[i] != NULL) {
		free(array[i]);
		i++;
	}
	free(array);
	
}

int arraySize(char **array) {
	
	int i = 0;
	while(array[i] != NULL) {
		i++;
	}
	
	return i;
	
}

int is_Integer(char *s) {
	
	if(s == NULL) {
		return 0;
	}
	
	int length;
	length = strlen(s);
	int i;
	
	for(i = 0; i < length; i++) {
		if(!isdigit(s[i])) {
			return 0;
		}
	}
	if(strlen(s) == 0) {
		return 0;
	}
	return 1;
	
}

char *joinStrings(char *string) {
	
	char **split_line;
	char *returnString = malloc(1);
	returnString[0] = '\0';
	int string_size = strlen(string) + 1;
	int i;
	int string_len = 0;
	
	char temp[string_size];
	strcpy(temp, string);
	
	split_line = split(temp);
	
	i = 2;
	if(split_line[2] != NULL) {
		string_len = strlen(split_line[2]) + 1;
		returnString = realloc(returnString, (sizeof(char) * string_len));
		strcpy(returnString, split_line[2]);
		i++;
		while(split_line[i] != NULL) {
			string_len = string_len + strlen(split_line[i]) + 2;
			returnString = realloc(returnString, (sizeof(char) * string_len));
			strcat(returnString, " ");
			strcat(returnString, split_line[i]);
			i++;
		}
	}

	free(split_line);
	return returnString;
	
}


int isInArray(char array[24][6], char search[6]) {
	
	int i = 0;
	
	for(i = 0; i < 23; i++) {
		if(strcmp(search, array[i]) == 0) {
			return 1;
		}
	}
	return 0;
}


Field *createField(char *fieldTag, char *fieldValue) {
	
	Field *new_field = malloc(sizeof(Field));


	if(fieldTag != NULL) {
		new_field->tag = malloc(sizeof(char) * (strlen(fieldTag) + 1));
		strcpy(new_field->tag, fieldTag);
		
	}else{
		//return NULL;
	}
	
	if(fieldValue != NULL) {
		new_field->value = malloc(sizeof(char) * (strlen(fieldValue) + 1));
		strcpy(new_field->value, fieldValue);
	}else{
		//return NULL;
	}
	
	return new_field;
	
}

Submitter *createSubmitter(char **Submitter_array) {

	Submitter *new;
	int i;
	char *tempArg;
	char *temp;
	char **split_line;
	char *submitter_name = malloc(500); 
	submitter_name[0] = '\0';
	char *submitter_address = malloc(5000);
	submitter_address[0] = '\0';
	
	//get name
	i = 0;
	while(Submitter_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
		strcpy(temp, Submitter_array[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "NAME") == 0) {
			tempArg = joinStrings(Submitter_array[i]);
			strcpy(submitter_name, tempArg);
			free(tempArg);
			free(split_line);
			free(temp);
			break;
		}
		
		free(split_line);
		free(temp);
		i++;
	}
	
	//get submitter address
	int breaker = 0;
	i = 0;
	while(Submitter_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
		strcpy(temp, Submitter_array[i]);
		split_line = split(temp);
		
		if( strcmp(split_line[1], "ADDR") == 0 ) {
			free(split_line);
			free(temp);
			tempArg = joinStrings(Submitter_array[i]);
			strcpy(submitter_address, tempArg);
			free(tempArg);
			
			if(Submitter_array[i+1] == NULL) {
				break;
			}
			temp = malloc(sizeof(char) * (strlen(Submitter_array[i+1]) + 1));
			strcpy(temp, Submitter_array[i+1]);
			split_line = split(temp);
			
			if(strcmp(split_line[1], "CONT") == 0) {
				free(split_line);
				free(temp);
				i++;
				
				temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
				strcpy(temp, Submitter_array[i]);
				split_line = split(temp);
				while(strcmp(split_line[1], "CONT") == 0) {
					free(split_line);
					free(temp);
				
					tempArg = joinStrings(Submitter_array[i]);
					strcat(submitter_address, "\n");
					strcat(submitter_address, tempArg);
					free(tempArg);
					
					i++;
					if(Submitter_array[i] == NULL) {
						breaker = 1;
						break;
					}
					temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
					strcpy(temp, Submitter_array[i]);
					split_line = split(temp);
				}
				if(breaker != 1) {
					free(split_line);
					free(temp);
				}
			}else{
				free(split_line);
				free(temp);
			}
			
			break;
		}
		
		free(split_line);
		free(temp);
		i++;
	}
	
	new = malloc(sizeof(Submitter) + (sizeof(char) * (strlen(submitter_address) + 1)));
	strcpy(new->submitterName, submitter_name);
	strcpy(new->address, submitter_address);
	free(submitter_name);
	free(submitter_address);
	
	Field *newField;
	i = 0;
	char tempTag[100];
	char *tempValue;
	breaker = 0;
	int stringSize = 0;
	new->otherFields = initializeList(&printField, &deleteField, &compareFields);
	
	while(Submitter_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
		strcpy(temp, Submitter_array[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "NAME") != 0 && strcmp(split_line[1], "ADDR") != 0 && strcmp(split_line[1], "CONT") != 0 && !isTag(split_line[1])) {
			strcpy(tempTag, split_line[1]);
			free(split_line);
			free(temp);
			tempArg = joinStrings(Submitter_array[i]);
			tempValue = malloc(sizeof(char) * (strlen(tempArg) + 1));
	
			stringSize = strlen(tempArg) + 1;
			strcpy(tempValue, tempArg);
			free(tempArg);
			
			if(Submitter_array[i+1] == NULL) {
				newField = createField(tempTag, tempValue);
				free(tempValue);
				insertSorted(&(new->otherFields) , (void*)newField);
				break;
			}
			temp = malloc(sizeof(char) * (strlen(Submitter_array[i+1]) + 1));
			strcpy(temp, Submitter_array[i+1]);
			split_line = split(temp);
			
			if(strcmp(split_line[1], "CONT") == 0) {
				free(split_line);
				free(temp);
				i++;
				
				temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
				strcpy(temp, Submitter_array[i]);
				split_line = split(temp);
				while(strcmp(split_line[1], "CONT") == 0) {
					free(split_line);
					free(temp);
				
					tempArg = joinStrings(Submitter_array[i]);
					stringSize = stringSize + strlen(tempArg) + 2;
					tempValue = realloc(tempValue, stringSize);
					strcat(tempValue, " ");
					strcat(tempValue, tempArg);
					free(tempArg);
					
					i++;
					if(Submitter_array[i] == NULL) {
						breaker = 1;
						break;
					}
					temp = malloc(sizeof(char) * (strlen(Submitter_array[i]) + 1));
					strcpy(temp, Submitter_array[i]);
					split_line = split(temp);
					if(strcmp(split_line[1], "CONT") != 0) {
						i--;
					}
				}
				if(breaker != 1) {
					free(split_line);
					free(temp);
				}
			}else{
				free(split_line);
				free(temp);
			}
			newField = createField(tempTag, tempValue);
			free(tempValue);
			insertSorted(&(new->otherFields) , (void*)newField);
		}else{
			free(split_line);
			free(temp);
		}
		if(breaker == 1) {
			break;
		}
		i++;
	}
	
	
	return new;
	
}


Header *createHeader(Submitter *submitter_data, char **Header_array) {
	
	Header *new = malloc(sizeof(Header));
	int i = 0;
	int size = 0;
	char **split_line;
	char *tempArg;	
	char *temp;
	char *tempValue;
	char tempTag[6];
	int count = 0;
	int count2 = 0;
	
	while(Header_array[i] != NULL) {
		size++;
		i++;
	}
	strcpy(new->source, "\0");
	
	for(i = 0; i < size; i++) {
		temp = malloc(sizeof(char) * (strlen(Header_array[i]) + 1));
		strcpy(temp, Header_array[i]);
		split_line = split(temp);
		//get source
		if(strcmp(split_line[1], "SOUR") == 0) {
			tempArg = joinStrings(Header_array[i]);
			strcpy(new->source, tempArg);
			free(tempArg);
		}
		//get version
		if(strcmp(split_line[1], "VERS") == 0) {
			count = 1;
			new->gedcVersion = atof(split_line[2]);
		}
		//get encoding
		if(strcmp(split_line[1], "CHAR") == 0) {
			count2 = 1;
			if(strcmp(split_line[2], "ANSEL") == 0) {
				new->encoding = ANSEL;
			}
			else if(strcmp(split_line[2], "ASCII") == 0) {
				new->encoding = ASCII;
			}
			else if(strcmp(split_line[2], "UNICODE") == 0) {
				new->encoding = UNICODE;
			}
			else if(strcmp(split_line[2], "UTF-8") == 0 || strcmp(split_line[2], "UTF8") == 0) {
				new->encoding = UTF8;
			}
			else{
				free(new);
				free(split_line);
				free(temp);
				return NULL;
			}
		}
		free(split_line);
		free(temp);
	}
	if(count != 1) {
		free(new);
		return NULL;
	}
	if(count2 != 1) {
		free(new);
		return NULL;
	}
	new->submitter = submitter_data;
	
	new->otherFields = initializeList(&printField, &deleteField, &compareFields);
	Field *field;
	int stringSize = 0;
	int breaker = 0;
	
	for(i = 0; i < size; i++) {
		temp = malloc(sizeof(char) * (strlen(Header_array[i]) + 1));
		strcpy(temp, Header_array[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "VERS") == 0 || strcmp(split_line[1], "SOUR") == 0 || strcmp(split_line[1], "CHAR") == 0 || strcmp(split_line[1], "HEAD") == 0 || isTag(split_line[1])) {
			free(split_line);
			free(temp);
			continue;
		}
		strcpy(tempTag, split_line[1]);
		free(split_line);
		free(temp);
		tempArg = joinStrings(Header_array[i]);
		tempValue = malloc(sizeof(char) * (strlen(tempArg) + 1));
		
		stringSize = strlen(tempArg) + 1;
		strcpy(tempValue, tempArg);
		free(tempArg);
		
		if(Header_array[i+1] == NULL) {
			field = createField(tempTag, tempValue);
			free(tempValue);
			insertSorted(&(new->otherFields) , (void*)field);
			break;
		}
		
		temp = malloc(sizeof(char) * (strlen(Header_array[i+1]) + 1));
		strcpy(temp, Header_array[i+1]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "CONT") == 0) {
			free(split_line);
			free(temp);
			i++;
			
			temp = malloc(sizeof(char) * (strlen(Header_array[i]) + 1));
			strcpy(temp, Header_array[i]);
			split_line = split(temp);
			while(strcmp(split_line[1], "CONT") == 0) {
				free(split_line);
				free(temp);
			
				tempArg = joinStrings(Header_array[i]);
				stringSize = stringSize + strlen(tempArg) + 2;
				tempValue = realloc(tempValue, stringSize);
				strcat(tempValue, " ");
				strcat(tempValue, tempArg);
				free(tempArg);
				
				i++;
				if(Header_array[i] == NULL) {
					breaker = 1;
					break;
				}
				temp = malloc(sizeof(char) * (strlen(Header_array[i]) + 1));
				strcpy(temp, Header_array[i]);
				split_line = split(temp);
				if(strcmp(split_line[1], "CONT") != 0) {
					i--;
				}
			}
			if(breaker != 1) {
				free(split_line);
				free(temp);
			}
		}else{
			free(split_line);
			free(temp);
		}
		field = createField(tempTag, tempValue);
		free(tempValue);
		insertSorted(&(new->otherFields) , (void*)field);	
	}
	
	
	return new;
	
}


Individual *createIndividual(char **Individual_array) {
	
	Individual *new = malloc(sizeof(Individual));
	int i;
	int j;
	int present = 0;
	char *token;
	char s[2] = "/";
	char *temp;
	char **split_line;
	char *tempArg;
	char *temp_name = malloc(250);
	char *first_name = malloc(250);
	char *last_name = malloc(250);
	char event_tags[24][6] = {"BIRT","CHR","DEAT","BURI","CREM","ADOP","BAPM","BARM","BASM","BLES","CHRA","CONF","FCOM","ORDN","NATU","EMIG","IMMI","CENS","PROB","WILL","GRAD","RETI","EVEN"};
	char tempTag[6];
	int size = 0;
	
	i = 0;
	while(Individual_array[i] != NULL) {
		size++;
		i++;
	}
	
	
	//get names
	i = 0;
	while(Individual_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Individual_array[i]) + 1));
		strcpy(temp, Individual_array[i]);
		
		split_line = split(temp);
		if(strcmp(split_line[1], "NAME") == 0) {
			free(split_line);
			free(temp);
			tempArg = joinStrings(Individual_array[i]);
			strcpy(temp_name, tempArg);
			free(tempArg);
			break;
		}
		free(split_line);
		free(temp);
		i++;
	}
	
	j = 0;
	token = strtok(temp_name,s);
	
	if(temp_name[0] != '/') {
		present = 1;
	}
	strcpy(first_name, "\0");
	strcpy(last_name, "\0");
	
	while(token != NULL) {
		j++;
		if(j == 1 && present == 1) {
			strcpy(first_name, token);
		}
		if(j == 2 && present == 1) {
			strcpy(last_name, token);
		}
		if(j == 1 && present == 0) {
			strcpy(last_name, token);
		}
		token = strtok(NULL, s);
	}
	
	removeEndSpaces(first_name);
	removeEndSpaces(last_name);
	new->givenName = malloc(sizeof(char) * (strlen(first_name) + 1));
	new->surname = malloc(sizeof(char) * (strlen(last_name) + 1));
	strcpy(new->givenName, first_name);
	strcpy(new->surname, last_name);
	free(temp_name);
	free(first_name);
	free(last_name);
	
	//getting events
	new->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	i = 0;
	Field* others;
	Event* e;
	
	while(Individual_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Individual_array[i]) + 1));
		strcpy(temp, Individual_array[i]);
		split_line = split(temp);
		strcpy(tempTag, split_line[1]);
		
		if(strcmp(split_line[0], "1") == 0 && isInArray(event_tags, tempTag)) {
			free(split_line);
			free(temp);
			
			e = malloc(sizeof(Event));
			strcpy(e->type, tempTag);
			e->date = malloc(sizeof(char));
			e->date[0] = '\0';
			e->place = malloc(sizeof(char));
			e->place[0] = '\0';
			e->otherFields = initializeList(&printField, &deleteField, &compareFields);
				
			if(Individual_array[i+1] == NULL) {
				insertSorted(&(new->events), (void*)e);
				break;
			}	
			
			temp = malloc(sizeof(char) * (strlen(Individual_array[i+1]) + 1));
			strcpy(temp, Individual_array[i+1]);
			split_line = split(temp);
			
			if(strcmp(split_line[0], "1") == 0) {
				i++;
				free(split_line);
				free(temp);
				insertSorted(&(new->events), (void*)e);
				continue;
			}
			
			while(strcmp(split_line[0], "1") != 0) {
				i++;
				
				if(strcmp(split_line[1], "DATE") == 0) {
					tempArg = joinStrings(Individual_array[i]);
					e->date = realloc(e->date, (sizeof(char) * (strlen(tempArg) + 1)));
					strcpy(e->date, tempArg);
					free(tempArg);
	
				}
				else if(strcmp(split_line[1], "PLAC") == 0) {
					tempArg = joinStrings(Individual_array[i]);
					e->place = realloc(e->place, (sizeof(char) * (strlen(tempArg) + 1)));
					strcpy(e->place, tempArg);
					free(tempArg);
				}
				else{
					tempArg = joinStrings(Individual_array[i]);
					others = createField(split_line[1], tempArg);
					insertSorted(&(e->otherFields), (void*)others);
					free(tempArg);
				}
				free(split_line);
				free(temp);
				
				if(Individual_array[i+1] == NULL) {
					goto INDIVIDUAL2;
				}
				
				temp = malloc(sizeof(char) * (strlen(Individual_array[i+1]) + 1));
				strcpy(temp, Individual_array[i+1]);
				split_line = split(temp);
			}
			if(strcmp(split_line[0], "1") == 0) {
				free(split_line);
				free(temp);
			}
			INDIVIDUAL2: insertSorted(&(new->events), (void*)e);

		}
		else{
			free(split_line);
			free(temp);
		}
		
		
		i++;
	}
	
	
	//getting otherfields
	new->otherFields = initializeList(&printField, &deleteField, &compareFields);
	Field *field;
	int stringSize = 0;
	int breaker = 0;
	char *tempValue;
	
	for(i = 0; i < size; i++) {
		temp = malloc(sizeof(char) * (strlen(Individual_array[i]) + 1));
		strcpy(temp, Individual_array[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "CONT") == 0 || strcmp(split_line[1], "SURN") == 0 || strcmp(split_line[1], "GIVN") == 0 || strcmp(split_line[1], "NAME") == 0 || strcmp(split_line[1], "DATE") == 0 || strcmp(split_line[1], "PLAC") == 0 || isTag(split_line[1]) || isInArray(event_tags, split_line[1])) {
			free(split_line);
			free(temp);
			continue;
		}
		
		if(split_line[2] != NULL) {
			if(isTag(split_line[2])) {
				free(split_line);
				free(temp);
				continue;
			}
		}
		strcpy(tempTag, split_line[1]);
		free(split_line);
		free(temp);
		tempArg = joinStrings(Individual_array[i]);
		tempValue = malloc(sizeof(char) * (strlen(tempArg) + 1));
		
		stringSize = strlen(tempArg) + 1;
		strcpy(tempValue, tempArg);
		free(tempArg);
		
		if(Individual_array[i+1] == NULL) {
			field = createField(tempTag, tempValue);
			free(tempValue);
			insertSorted(&(new->otherFields) , (void*)field);
			break;
		}
		
		temp = malloc(sizeof(char) * (strlen(Individual_array[i+1]) + 1));
		strcpy(temp, Individual_array[i+1]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "CONT") == 0) {
			free(split_line);
			free(temp);
			i++;
			
			temp = malloc(sizeof(char) * (strlen(Individual_array[i]) + 1));
			strcpy(temp, Individual_array[i]);
			split_line = split(temp);
			while(strcmp(split_line[1], "CONT") == 0) {
				free(split_line);
				free(temp);
			
				tempArg = joinStrings(Individual_array[i]);
				stringSize = stringSize + strlen(tempArg) + 2;
				tempValue = realloc(tempValue, stringSize);
				strcat(tempValue, " ");
				strcat(tempValue, tempArg);
				free(tempArg);
				
				i++;
				if(Individual_array[i] == NULL) {
					breaker = 1;
					break;
				}
				temp = malloc(sizeof(char) * (strlen(Individual_array[i]) + 1));
				strcpy(temp, Individual_array[i]);
				split_line = split(temp);
				if(strcmp(split_line[1], "CONT") != 0) {
					i--;
				}
			}
			if(breaker != 1) {
				free(split_line);
				free(temp);
			}
		}else{
			free(split_line);
			free(temp);
		}
		field = createField(tempTag, tempValue);
		free(tempValue);
		insertSorted(&(new->otherFields) , (void*)field);	
	}
	
	new->families = initializeList(&printFamily, &fakeDelete, &compareFamilies);
	
	return new;
	
}

Family *createFamily(char **Family_array, Xref **refs) {
	
	Family *new = malloc(sizeof(Family));
	char event_tags[13][6] = {"ANUL", "CENS", "DIV", "DIVF", "ENGA", "MARB", "MARC", "MARR", "MARL", "MARS", "RESI", "EVEN"};
	int i = 0;
	int k;
	char **split_line;
	char *temp;
	char tempKey[10];
	char childKeys[100][10];
	int numKids = 0;
	for(k = 0; k < 100; k++) {
		childKeys[k][0] = '\0';
	}
	int size = 0;
	while(Family_array[i] != NULL) {
		i++;
		size++;
	}
	
	static int test = 0;
	test++;
	//search for wife
	//start
	int wife = 0;
	i = 0;
	while(Family_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
		strcpy(temp, Family_array[i]);
		
		split_line = split(temp);
		if(split_line[2] != NULL && strcmp(split_line[1], "WIFE") == 0) {
			wife = 1;
			strcpy(tempKey, split_line[2]);
			free(split_line);
			free(temp);
			break;
		}
		free(split_line);
		free(temp);
		i++;
	}
	new->husband = NULL;
	new->wife = NULL;
	if(wife != 1) {
		new->wife = NULL;
	}
	else{
		i = 0;
		while(refs[i] != NULL) {
			if(strcmp(refs[i]->xref, tempKey) == 0) {
				new->wife = refs[i]->individual;
				break;
			}
			i++;
		}
	}
	//end
	
	//search for husband
	//start
	i = 0;
	int husband = 0;
	while(Family_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
		strcpy(temp, Family_array[i]);
		
		split_line = split(temp);
		if(split_line[2] != NULL && strcmp(split_line[1], "HUSB") == 0) {
			husband = 1;
			strcpy(tempKey, split_line[2]);
			free(split_line);
			free(temp);
			break;
		}
		free(split_line);
		free(temp);
		i++;
	}
	
	if(husband != 1) {
		new->husband = NULL;
	}
	else{
		i = 0;
		while(refs[i] != NULL) {
			if(strcmp(refs[i]->xref, tempKey) == 0) {
				new->husband = refs[i]->individual;
				break;
			}
			i++;
		}
	}
	
	
	//search for children
	//start
	int j = 0;
	i = 0;
	while(Family_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
		strcpy(temp, Family_array[i]);
		
		split_line = split(temp);
		if(split_line[2] != NULL && strcmp(split_line[1], "CHIL") == 0) {
			strcpy(childKeys[j], split_line[2]);
			free(split_line);
			free(temp);
			numKids++;
			j++;
		}else{
			free(split_line);
			free(temp);
		}
		i++;
	}
	
	new->children = initializeList(&printIndividual, &fakeDelete, &compareIndividuals);
	for(k = 0; k < numKids; k++) {
		i = 0;
		while(refs[i] != NULL) {
			if(strcmp(refs[i]->xref, childKeys[k]) == 0) {
				insertSorted(&(new->children), (void*)refs[i]->individual);
				break;
			}
			i++;
		}
	}
	//end
	
	
	//getting events
	new->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	i = 0;
	Field* others;
	Event* e;
	char tempTag[6];
	char *tempArg;
	
	while(Family_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
		strcpy(temp, Family_array[i]);
		split_line = split(temp);
		strcpy(tempTag, split_line[1]);
		
		if(strcmp(split_line[0], "1") == 0 && isInArray(event_tags, tempTag)) {
			free(split_line);
			free(temp);
			
			e = malloc(sizeof(Event));
			strcpy(e->type, tempTag);
			e->date = malloc(sizeof(char));
			e->date[0] = '\0';
			e->place = malloc(sizeof(char));
			e->place[0] = '\0';
			e->otherFields = initializeList(&printField, &deleteField, &compareFields);
				
			if(Family_array[i+1] == NULL) {
				insertSorted(&(new->events), (void*)e);
				break;
			}	
			
			temp = malloc(sizeof(char) * (strlen(Family_array[i+1]) + 1));
			strcpy(temp, Family_array[i+1]);
			split_line = split(temp);
			
			if(strcmp(split_line[0], "1") == 0) {
				i++;
				free(split_line);
				free(temp);
				insertSorted(&(new->events), (void*)e);
				continue;
			}
			
			while(strcmp(split_line[0], "1") != 0) {
				i++;
				
				if(strcmp(split_line[1], "DATE") == 0) {
					tempArg = joinStrings(Family_array[i]);
					e->date = realloc(e->date, (sizeof(char) * (strlen(tempArg) + 1)));
					strcpy(e->date, tempArg);
					free(tempArg);
	
				}
				else if(strcmp(split_line[1], "PLAC") == 0) {
					tempArg = joinStrings(Family_array[i]);
					e->place = realloc(e->place, (sizeof(char) * (strlen(tempArg) + 1)));
					strcpy(e->place, tempArg);
					free(tempArg);
				}
				else{
					tempArg = joinStrings(Family_array[i]);
					others = createField(split_line[1], tempArg);
					insertSorted(&(e->otherFields), (void*)others);
					free(tempArg);
				}
				
				free(split_line);
				free(temp);
				
				if(Family_array[i+1] == NULL) {
					goto FAMILY2;
				}
				
				temp = malloc(sizeof(char) * (strlen(Family_array[i+1]) + 1));
				strcpy(temp, Family_array[i+1]);
				split_line = split(temp);
			}
			if(strcmp(split_line[0], "1") == 0) {
				free(split_line);
				free(temp);
			}
			FAMILY2: insertSorted(&(new->events), (void*)e);

		}
		else{
			free(split_line);
			free(temp);
		}
		i++;
	}
	
	//getting otherfields
	new->otherFields = initializeList(&printField, &deleteField, &compareFields);
	Field *field;
	int stringSize = 0;
	int breaker = 0;
	char *tempValue;
	
	for(i = 0; i < size; i++) {
		temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
		strcpy(temp, Family_array[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "CONT") == 0 || strcmp(split_line[1], "NAME") == 0 || strcmp(split_line[1], "DATE") == 0 || strcmp(split_line[1], "PLAC") == 0 || isTag(split_line[1]) || isInArray(event_tags, split_line[1])) {
			free(split_line);
			free(temp);
			continue;
		}
		
		if(split_line[2] != NULL) {
			if(isTag(split_line[2])) {
				free(split_line);
				free(temp);
				continue;
			}
		}
		strcpy(tempTag, split_line[1]);
		free(split_line);
		free(temp);
		tempArg = joinStrings(Family_array[i]);
		tempValue = malloc(sizeof(char) * (strlen(tempArg) + 1));
		
		stringSize = strlen(tempArg) + 1;
		strcpy(tempValue, tempArg);
		free(tempArg);
		
		if(Family_array[i+1] == NULL) {
			field = createField(tempTag, tempValue);
			free(tempValue);
			insertSorted(&(new->otherFields) , (void*)field);
			break;
		}
		
		temp = malloc(sizeof(char) * (strlen(Family_array[i+1]) + 1));
		strcpy(temp, Family_array[i+1]);
		split_line = split(temp);
		
		if(strcmp(split_line[1], "CONT") == 0) {
			free(split_line);
			free(temp);
			i++;
			
			temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
			strcpy(temp, Family_array[i]);
			split_line = split(temp);
			while(strcmp(split_line[1], "CONT") == 0) {
				free(split_line);
				free(temp);
			
				tempArg = joinStrings(Family_array[i]);
				stringSize = stringSize + strlen(tempArg) + 2;
				tempValue = realloc(tempValue, stringSize);
				strcat(tempValue, " ");
				strcat(tempValue, tempArg);
				free(tempArg);
				
				i++;
				if(Family_array[i] == NULL) {
					breaker = 1;
					break;
				}
				temp = malloc(sizeof(char) * (strlen(Family_array[i]) + 1));
				strcpy(temp, Family_array[i]);
				split_line = split(temp);
				if(strcmp(split_line[1], "CONT") != 0) {
					i--;
				}
			}
			if(breaker != 1) {
				free(split_line);
				free(temp);
			}
		}else{
			free(split_line);
			free(temp);
		}
		field = createField(tempTag, tempValue);
		free(tempValue);
		insertSorted(&(new->otherFields) , (void*)field);	
	}
	
	return new;
	
}

int isInList(List *list, Individual *individual) {
	
	List temp = *list;
	
	Node *node = temp.head;
	Individual *tempInd;
	while(node != NULL) {
		tempInd = (Individual*)node->data;
		if(tempInd == individual)return 1;
		
		node = node->next;
	}
	
	return 0;
}

int isInList2(List *list, Family *family) {
	
	List temp = *list;
	
	Node *node = temp.head;
	Family *tempFam;
	while(node != NULL) {
		tempFam = (Family*)node->data;
		if(tempFam == family)return 1;
		
		node = node->next;
	}
	
	return 0;
}

void updateIndividuals(List *individual, List *family) {
	
	List indi_List = *individual;
	List fam_List = *family;
	
	
	Node *update = indi_List.head;
	Individual *temp;
	Node *fam;
	Family *tempFam;
	
	while(update!= NULL) {
		temp = (Individual*)update->data;
		
		fam = fam_List.head;
		while(fam != NULL) {
			tempFam = (Family*)fam->data;
			
			if((temp == tempFam->wife) || (temp == tempFam->husband) || isInList(&(tempFam->children), temp)) {
				insertSorted(&(temp->families), (void*)tempFam);
			}
			fam = fam->next;
		}
		update = update->next;
	}
	
	
}


void clearSubmitter(Submitter *sub) {
	
	clearList(&(sub->otherFields));
	free(sub);
	
}

char* printHeader(Header *header) {
	
	char *return_string;
	if(header == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	char code[10];
	if(header->encoding == ANSEL) {
		strcpy(code, "ANSEL");
	}
	else if(header->encoding == UTF8) {
		strcpy(code, "UTF8");
	}
	else if(header->encoding == UNICODE) {
		strcpy(code, "UNICODE");
	}
	else{
		strcpy(code, "ASCII");
	}
	
	int size = 30 + strlen(header->source) + strlen(code);
	return_string = malloc(sizeof(char) * size);
	
	snprintf(return_string, size, "------HEADER------\nSource: %s\nEncoding: %s\n", header->source, code);
	
	return return_string;
	
}

List merge(List *One, List *Two) {
	
	List combine = initializeList(&printIndividual, &fakeDelete, &compareIndividuals);
	
	List one = *One;
	List two = *Two;
	Node *node = one.head;
	while(node != NULL) {
		insertSorted(&combine, node->data);
		node = node->next;
	}
	node = two.head;
	while(node != NULL) {
		insertSorted(&combine, node->data);
		node = node->next;
	}
	clearList(One);
	clearList(Two);
	
	return combine;
	
}

List getChildren(Individual* person) {
	
	List descendants = initializeList(&printIndividual, &fakeDelete, &compareIndividuals);
	if(person == NULL) {
		return descendants;
	}
	
	Node *node = person->families.head;
	Family *fam;
	Family *get = NULL;
	while(node != NULL) {
		fam = (Family*)node->data;
		if(fam->husband == person || fam->wife == person) {
			get = fam;
			Node *n2 = get->children.head;
			while(n2 != NULL) {
				Individual* indi = (Individual *)n2->data;
				insertSorted(&descendants, (void*)indi);
				List new = getChildren(indi);
				descendants = merge(&descendants, &new);
				
				n2 = n2->next;
			}
		}
		node = node->next;
	}
	if(get == NULL) {
		return descendants;
	}
	
	return descendants;
}

void getKids(Individual *person, List *descendants) {
	
	if(person == NULL) {
		return;
	}
	
	Node *node = person->families.head;
	Family *fam;
	Family *get = NULL;
	while(node != NULL) {
		fam = (Family*)node->data;
		if(fam->husband == person || fam->wife == person) {
			get = fam;
			Node *n2 = get->children.head;
			while(n2 != NULL) {
				Individual* indi = (Individual *)n2->data;
				if(!isInList(descendants, indi)) {
					insertSorted(descendants, (void*)indi);
				}
				n2 = n2->next;
			}
		}
		node = node->next;
	}
	if(get == NULL) {
		return;
	}
	
	
}

void getParents(Individual *person, List *ancestors) {
	
	if(person == NULL) {
		return;
	}
	
	Node *node = person->families.head;
	Family *fam;
	Family *get = NULL;
	while(node != NULL) {
		fam = (Family*)node->data;
		if(isInList(&fam->children, person)) {
			get = fam;
			if(get->wife != NULL) {
				if(!isInList(ancestors, get->wife)) {
					insertSorted(ancestors, (void*)get->wife);
				}
			}
			if(get->husband != NULL) {
				if(!isInList(ancestors, get->husband)) {
					insertSorted(ancestors, (void*)get->husband);
				}
			}
			/*Node *n2 = get->children.head;
			while(n2 != NULL) {
				Individual* indi = (Individual *)n2->data;
				insertSorted(descendants, (void*)indi);
				
				n2 = n2->next;
			}*/
		}
		node = node->next;
	}
	if(get == NULL) {
		return;
	}
	
	
}

int compareIndsInGen(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	Individual *one = (Individual*)first;
	Individual *two = (Individual*)second;
	
	if(one->surname == NULL) {
		return 1;
	}
	
	if(one->surname[0] == '\0') {
		return 1;
	}
	
	if(strcmp(one->surname, two->surname) != 0) {
		return strcmp(one->surname, two->surname);
	}
	
	return strcmp(one->givenName, two->givenName);
	
}
