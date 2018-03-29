#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

char* indToJSON(const Individual* ind) {
	
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
	
	int printSize = strlen("givenname:surname:") + strlen(first) + strlen(second) + 15;
	
	return_string = malloc(sizeof(char) * printSize);
	
	snprintf(return_string, printSize, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", first, second);
	free(first);
	free(second);
	
	return return_string;
	
}

Individual* JSONtoInd(const char* str) {
	
	if(str == NULL) {
		return NULL;
	}
	if(str[0] == '\0') {
		return NULL;
	}
	
	char *input = (char*)str;
	int j = 0;
	while(input[j] != '\0') {	
		j++;
	}
	if(input[0] != '{' || input[j-1] != '}') {
		return NULL;
	}
	
	const char s[2] = "\"";
	const char c[2] = ",";
	char *token;
	char **array = malloc(sizeof(char*));
	char **array2 = malloc(sizeof(char*));
	char **array3 = malloc(sizeof(char*));
	int count = 0;
	
	token = strtok(input,c);
	while(token != NULL) {
		array[count] = token;
		count++;
		
		token = strtok(NULL, c);
		array = realloc(array, (count + 1) * sizeof(char*));
	}
	array[count] = NULL;
	
	int i = 0;
	while(array[i] != NULL) {
		i++;
	}
	
	if(i != 2) {
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	Individual* indi = malloc(sizeof(Individual));
	count = 0;
	token = strtok(array[0],s);
	while(token != NULL) {
		array2[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array2 = realloc(array2, (count + 1) * sizeof(char*));
	}
	array2[count] = NULL;
	
	if(count < 3) {
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	if(strcmp(array2[1], "givenName") != 0 || strcmp(array2[2], ":") != 0) {
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	if(count == 3) {
		indi->givenName = malloc(sizeof(char));
		indi->givenName[0] = '\0';
	}
	else if(count == 4) {
		indi->givenName = malloc(sizeof(char) * (strlen(array2[3]) + 1));
		strcpy(indi->givenName, array2[3]);
	}
	else{
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	count = 0;
	token = strtok(array[1],s);
	while(token != NULL) {
		array3[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array3 = realloc(array3, (count + 1) * sizeof(char*));
	}
	array3[count] = NULL;
	
	if(count < 3) {
		free(indi->givenName);
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	if(strcmp(array3[0], "surname") != 0 || strcmp(array3[1], ":") != 0) {
		free(indi->givenName);
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	
	if(count == 3) {
		indi->surname = malloc(sizeof(char));
		indi->surname[0] = '\0';
	}
	else if(count == 4) {
		indi->surname = malloc(sizeof(char) * (strlen(array3[2]) + 1));
		strcpy(indi->surname, array3[2]);
	}
	else{
		free(indi->givenName);
		free(indi);
		free(array);
		free(array2);
		free(array3);
		return NULL;
	}
	indi->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
	indi->otherFields = initializeList(&printField, &deleteField, &compareFields);
	indi->families = initializeList(&printFamily, &fakeDelete, &compareFamilies);
	
	free(array);
	free(array2);
	free(array3);
	
	return indi;
	
}


GEDCOMobject* JSONtoGEDCOM(const char* str) {
	
	if(str == NULL) {
		return NULL;
	}
	if(str[0] == '\0') {
		return NULL;
	}
	
	char *input = (char*)str;
	int j = 0;
	while(input[j] != '\0') {	
		j++;
	}
	if(input[0] != '{' || input[j-1] != '}') {
		return NULL;
	}
	
	const char s[2] = "\"";
	const char c[2] = ",";
	char *token;
	char **array = malloc(sizeof(char*));
	char **array1 = malloc(sizeof(char*));
	char **array2 = malloc(sizeof(char*));
	char **array3 = malloc(sizeof(char*));
	char **array4 = malloc(sizeof(char*));
	char **array5 = malloc(sizeof(char*));
	
	int count = 0;
	token = strtok(input,c);
	while(token != NULL) {
		array[count] = token;
		count++;
		
		token = strtok(NULL, c);
		array = realloc(array, (count + 1) * sizeof(char*));
	}
	array[count] = NULL;
	
	if(count != 5) {
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	//get source
	GEDCOMobject *obj = malloc(sizeof(GEDCOMobject));
	Header *header = malloc(sizeof(Header));
	header->otherFields = initializeList(&printField, &deleteField, &compareFields);
	count = 0;
	token = strtok(array[0],s);
	while(token != NULL) {
		array1[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array1 = realloc(array1, (count + 1) * sizeof(char*));
	}
	array1[count] = NULL;
	
	if(count != 4) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array1[1], "source") != 0 || strcmp(array1[2], ":") != 0) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strlen(array[3]) > 248) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	strcpy(header->source, array1[3]);
	
	//get version
	count = 0;
	token = strtok(array[1],s);
	while(token != NULL) {
		array2[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array2 = realloc(array2, (count + 1) * sizeof(char*));
	}
	array2[count] = NULL;
	
	if(count != 3) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array2[0], "gedcVersion") != 0 || strcmp(array2[1], ":") != 0) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	char *endptr;
	errno = 0;
	double result = strtod(array2[2], &endptr);
	if (errno != 0 || *endptr != '\0') {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	header->gedcVersion = result;
	
	//get encoding
	count = 0;
	token = strtok(array[2],s);
	while(token != NULL) {
		array3[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array3 = realloc(array3, (count + 1) * sizeof(char*));
	}
	array3[count] = NULL;
	
	if(count != 3) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array3[0], "encoding") != 0 || strcmp(array3[1], ":") != 0) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array3[2], "ANSEL") == 0) {
		header->encoding = ANSEL;
	}
	else if(strcmp(array3[2], "UTF8") == 0 || strcmp(array3[2], "UTF-8") == 0) {
		header->encoding = UTF8;
	}
	else if(strcmp(array3[2], "UNICODE") == 0) {
		header->encoding = UNICODE;
	}
	else if(strcmp(array3[2], "ASCII") == 0) {
		header->encoding = ASCII;
	}
	else{
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	//get Submittername
	Submitter *sub;
	count = 0;
	token = strtok(array[3],s);
	while(token != NULL) {
		array4[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array4 = realloc(array4, (count + 1) * sizeof(char*));
	}
	array4[count] = NULL;
	
	if(count != 3) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array4[0], "subName") != 0 || strcmp(array4[1], ":") != 0) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	//get submitteraddress
	count = 0;
	token = strtok(array[4],s);
	while(token != NULL) {
		array5[count] = token;
		count++;
		
		token = strtok(NULL, s);
		array5 = realloc(array5, (count + 1) * sizeof(char*));
	}
	array5[count] = NULL;
	
	if(count != 4) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	if(strcmp(array5[0], "subAddress") != 0 || strcmp(array5[1], ":") != 0) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	
	sub = malloc(sizeof(Submitter) + (sizeof(char) * (strlen(array5[2]) + 1)));
	if(strlen(array4[2]) > 60) {
		clearList(&(header->otherFields));
		free(header);
		free(obj);
		free(sub);
		free(array);
		free(array1);
		free(array2);
		free(array3);
		free(array4);
		free(array5);
		return NULL;
	}
	strcpy(sub->submitterName, array4[2]);
	strcpy(sub->address, array5[2]);
	sub->otherFields = initializeList(&printField, &deleteField, &compareFields);
	
	header->submitter = sub;
	obj->header = header;
	obj->submitter = sub;
	obj->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
	obj->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
	
	free(array);
	free(array1);
	free(array2);
	free(array3);
	free(array4);
	free(array5);
	
	return obj;
	
}


void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded) {
	
	if(obj == NULL || toBeAdded == NULL) {
		return;
	}
	//error check here
	
	Individual* ind = (Individual*)toBeAdded;
	insertSorted(&(obj->individuals), (void*)ind);
	
}


char* iListToJSON(List iList) {
	
	char *return_string;
	
	if(iList.length == 0) {
		return_string = malloc(sizeof(char) * 4);
		strcpy(return_string, "[]");
		return return_string;
	}
	
	int size = 0;
	Individual *ind;
	Node *node = iList.head;
	while(node != NULL) {
		ind = (Individual*)node->data;
		char *new = indToJSON(ind);
		size = size + strlen(new) + 3;
		free(new);
		node = node->next;
	}
	
	//size = size + 20;
	return_string = malloc(sizeof(char) * size);
	strcpy(return_string, "[");
	node = iList.head;
	ind = (Individual*)node->data;
	char *new2 = indToJSON(ind);
	strcat(return_string, new2);
	free(new2);
	node = node->next;
	while(node != NULL) {
		ind = (Individual*)node->data;
		char *data = indToJSON(ind);
		strcat(return_string, ",");
		strcat(return_string, data);
		free(data);
		node = node->next;
	}
	strcat(return_string, "]");
	
	return return_string;
	
}


char* gListToJSON(List gList) {
	
	char *return_string;
	
	if(gList.length == 0) {
		return_string = malloc(sizeof(char) * 4);
		strcpy(return_string, "[]");
		return return_string;
	}
	
	int size = 0;
	List *list;
	Node *node = gList.head;
	while(node != NULL) {
		list = (List*)node->data;
		char *new = iListToJSON(*list);
		size = size + strlen(new);
		free(new);
		node = node->next;
	}
	
	size = size + 20;
	return_string = malloc(sizeof(char) * size);
	strcpy(return_string, "[");
	node = gList.head;
	list = (List*)node->data;
	char *new2 = iListToJSON(*list);
	strcat(return_string, new2);
	free(new2);
	node = node->next;
	while(node != NULL) {
		list = (List*)node->data;
		char *data = iListToJSON(*list);
		strcat(return_string, ",");
		strcat(return_string, data);
		free(data);
		node = node->next;
	}
	strcat(return_string, "]");
	
	return return_string;
	
}


ErrorCode validateGEDCOM(const GEDCOMobject* obj) {
	
	if(obj == NULL) {
		return INV_GEDCOM;
	}
	
	if(obj->header == NULL || obj->submitter == NULL) {
		return INV_GEDCOM;
	}
	
	if(strlen(obj->header->source) == 0) {
		return INV_HEADER;
	}
	
	if(obj->header->encoding != ANSEL && obj->header->encoding != UTF8 && obj->header->encoding != UNICODE && obj->header->encoding != ASCII) {
		return INV_HEADER;
	}
	
	if(obj->header->submitter == NULL) {
		return INV_HEADER;
	}
	
	if(obj->header->submitter != obj->submitter) {
		return INV_HEADER;
	}
	
	if(strlen(obj->submitter->submitterName) == 0) {
		return INV_RECORD;
	}
	
	if(strlen(obj->submitter->submitterName) > 60 || strlen(obj->submitter->address) > 200) {
		return INV_RECORD;
	}
	
	if(strlen(obj->header->source) > 200) {
		return INV_RECORD;
	}
	
	//verify individuals
	Node *node = obj->individuals.head;
	Individual *indi;
	Node *n2;
	Event *e;
	Field *field;
	Family *fam;
	char event_tags[24][6] = {"BIRT","CHR","DEAT","BURI","CREM","ADOP","BAPM","BARM","BASM","BLES","CHRA","CONF","FCOM","ORDN","NATU","EMIG","IMMI","CENS","PROB","WILL","GRAD","RETI","EVEN"};
	char famEvent_tags[13][6] = {"ANUL", "CENS", "DIV", "DIVF", "ENGA", "MARB", "MARC", "MARR", "MARL", "MARS", "RESI", "EVEN"};
	while(node != NULL) {
		indi = (Individual*)node->data;
		if(indi == NULL) {
			return INV_RECORD;
		}
		if(strlen(indi->givenName) > 200 || strlen(indi->surname) > 200) {
			return INV_RECORD;
		}
		
		n2 = indi->events.head;
		while(n2 != NULL) {
			e = (Event*)n2->data;
			if(e == NULL) {
				return INV_RECORD;
			}
			if(!isInArray(event_tags, e->type)) {
				return INV_RECORD;
			}
			if(strlen(e->place) > 200 || strlen(e->date) > 200) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		n2 = indi->otherFields.head;
		while(n2 != NULL) {
			field = (Field*)n2->data;
			if(field == NULL) {
				return INV_RECORD;
			}
			if(strlen(field->value) > 200) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		n2 = indi->families.head;
		while(n2 != NULL) {
			fam = (Family*)n2->data;
			if(fam == NULL) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		node = node->next;
	}
	
	//verify families
	node = obj->families.head;
	while(node != NULL) {
		fam = (Family*)node->data;
		if(fam == NULL) {
			return INV_RECORD;
		}
		
		n2 = fam->events.head;
		while(n2 != NULL) {
			e = (Event*)n2->data;
			if(e == NULL) {
				return INV_RECORD;
			}
			if(!isInArray(famEvent_tags, e->type)) {
				return INV_RECORD;
			}
			if(strlen(e->place) > 200 || strlen(e->date) > 200) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		n2 = fam->children.head;
		while(n2 != NULL) {
			indi = (Individual*)n2->data;
			if(indi == NULL) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		node = node->next;
	}
	
	GEDCOMobject *object = (GEDCOMobject*)obj;
	//verify families in individuals
	node = obj->individuals.head;
	List *famList = &(object->families);
	while(node != NULL) {
		indi = (Individual*)node->data;
		n2 = indi->families.head;
		while(n2 != NULL) {
			fam = (Family*)n2->data;
			if(!isInList2(famList, fam)) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		node = node->next;
	}
	
	//verify individuals in families
	node = obj->families.head;
	List *indList = &(object->individuals);
	while(node != NULL) {
		fam = (Family*)node->data;
		if(fam->husband != NULL) {
			if(!isInList(indList, fam->husband)) {
				return INV_RECORD;
			}
		}
		if(fam->wife != NULL) {
			if(!isInList(indList, fam->wife)) {
				return INV_RECORD;
			}
		}
		n2 = fam->children.head;
		while(n2 != NULL) {
			indi = (Individual*)n2->data;
			if(!isInList(indList, indi)) {
				return INV_RECORD;
			}
			n2 = n2->next;
		}
		
		node = node->next;
	}
	
	
	return OK;
	
}

GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj) {
	
	GEDCOMerror error;
	
	if(fileName == NULL ) {
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	if(!checkFile(fileName) || strlen(fileName) == 0) {
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	if(validateGEDCOM(obj) != OK) {
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;
	}
	
	error.type = OK;
	error.line = -1;
	char temp[1000];
	char **address;
	int i = 0;
	
	
	FILE *fptr;
	fptr = fopen(fileName, "w+");
	if(fptr == NULL) {
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	//write header
	fputs("0 HEAD\n", fptr);
	fputs("1 SOUR ", fptr);
	fputs(obj->header->source, fptr);
	fputs("\n", fptr);
	fputs("1 GEDC\n", fptr);
	fputs("2 VERS ", fptr);
	snprintf(temp, 20, "%f", obj->header->gedcVersion);
	removeEndZeros(temp);
	fputs(temp, fptr);
	memset(temp,0,sizeof(temp)); 
	fputs("\n", fptr);
	fputs("2 FORM LINEAGE-LINKED\n", fptr);
	fputs("1 CHAR ", fptr);
	if(obj->header->encoding == ANSEL) {
		fputs("ANSEL\n", fptr);
	}
	else if(obj->header->encoding == UTF8) {
		fputs("UTF8\n", fptr);
	}
	else if(obj->header->encoding == UNICODE) {
		fputs("UNICODE\n", fptr);
	}
	else{
		fputs("ASCII\n", fptr);
	}
	fputs("1 SUBM @SUB1@\n", fptr);
	
	//write submitter
	fputs("0 @SUB1@ SUBM\n", fptr);
	fputs("1 NAME ", fptr);
	fputs(obj->submitter->submitterName, fptr);
	fputs("\n", fptr);
	if(strlen(obj->submitter->address) != 0) {
		address = splitValue(obj->submitter->address);
		fputs("1 ADDR ", fptr);
		fputs(address[i], fptr);
		fputs("\n", fptr);
		i = 1;
		while(address[i] != NULL) {
			fputs("2 CONT ", fptr);
			fputs(address[i], fptr);
			fputs("\n", fptr);
			i++;
		}
		freeArray(address);
	}

	//creating xrefs
	Xref *ref;
	Xref **ref_array = malloc(sizeof(Xref*));
	FXref *fref;
	FXref **fref_array = malloc(sizeof(FXref*));
	char refNum[8];
	Node *node = obj->individuals.head;
	Individual *indi;
	Family *fam;
	int ref_count = 0;
	i = 1;
	while(node != NULL) {
		memset(refNum,0,sizeof(refNum));
		indi = (Individual*)node->data;
		ref = malloc(sizeof(Xref));
		ref->individual = indi;
		snprintf(refNum, 8, "%d", i);
		strcpy(ref->xref, "@I");
		strcat(ref->xref, refNum);
		strcat(ref->xref, "@");
		ref_array[ref_count] = ref;
		ref_count++;
		ref_array = realloc(ref_array, sizeof(Xref *) * (ref_count + 1));
		
		node = node->next;
		i++;
	}
	ref_array[ref_count] = NULL;
	
	node = obj->families.head;
	int fref_count = 0;
	i = 1;
	while(node != NULL) {
		memset(refNum,0,sizeof(refNum));
		fam = (Family*)node->data;
		fref = malloc(sizeof(FXref));
		fref->family = fam;
		snprintf(refNum, 8, "%d", i);
		strcpy(fref->xref, "@F");
		strcat(fref->xref, refNum);
		strcat(fref->xref, "@");
		fref_array[fref_count] = fref;
		fref_count++;
		fref_array = realloc(fref_array, sizeof(FXref *) * (fref_count + 1));
		
		node = node->next;
		i++;
	}
	fref_array[fref_count] = NULL;
	
	//writing individuals
	i = 0;
	Field *field;
	Event *event;
	int k = 0;
	
	while(ref_array[i] != NULL) {
		fputs("0 ", fptr);
		fputs(ref_array[i]->xref, fptr);
		fputs(" INDI\n", fptr);
		
		fputs("1 NAME ", fptr);
		fputs(ref_array[i]->individual->givenName, fptr);
		fputs(" /", fptr);
		fputs(ref_array[i]->individual->surname, fptr);
		fputs("/\n", fptr);
		
		node = ref_array[i]->individual->otherFields.head;
		while(node != NULL) {
			field = (Field*)node->data;
			if(strcmp(field->tag, "SURN") == 0 || strcmp(field->tag, "GIVN") == 0) {
				fputs("2 ", fptr);
				fputs(field->tag, fptr);
				fputs(" ", fptr);
				fputs(field->value, fptr);
				fputs("\n", fptr);
			}
			node = node->next;
		}
		
		node = ref_array[i]->individual->otherFields.head;
		while(node != NULL) {
			field = (Field*)node->data;
			if(strcmp(field->tag, "SURN") != 0 && strcmp(field->tag, "GIVN") != 0) {
				fputs("1 ", fptr);
				fputs(field->tag, fptr);
				fputs(" ", fptr);
				fputs(field->value, fptr);
				fputs("\n", fptr);
			}
			node = node->next;
		}
		
		node = ref_array[i]->individual->events.head;
		while(node != NULL) {
			event = (Event*)node->data;
			fputs("1 ", fptr);
			fputs(event->type, fptr);
			fputs("\n", fptr);
			if(event->date[0] != '\0') {
				fputs("2 DATE ", fptr);
				fputs(event->date, fptr);
				fputs("\n", fptr);
			}
			if(event->place[0] != '\0') {
				fputs("2 PLAC ", fptr);
				fputs(event->place, fptr);
				fputs("\n", fptr);
			}
			
			node = node->next;
		}
		
		node = ref_array[i]->individual->families.head;
		while(node != NULL) {
			k = 0;
			fam = (Family*)node->data;
			if(fam->wife == ref_array[i]->individual || fam->husband == ref_array[i]->individual) {
				k = 0;
				fputs("1 FAMS ", fptr);
				while(fref_array[k] != NULL) {
					if(fref_array[k]->family == fam) {
						fputs(fref_array[k]->xref, fptr);
						break;
					}
					k++;
				}
				fputs("\n", fptr);
			}
			if(isInList(&(fam->children), ref_array[i]->individual) == 1) {
				k = 0;
				fputs("1 FAMC ", fptr);
				while(fref_array[k] != NULL) {
					if(fref_array[k]->family == fam) {
						fputs(fref_array[k]->xref, fptr);
						break;
					}
					k++;
				}
				fputs("\n", fptr);
			}
			
			node = node->next;
		}
		
		i++;
	}
	
	//writing families
	i = 0;
	Individual *ind;
	
	while(fref_array[i] != NULL) {
		fputs("0 ", fptr);
		fputs(fref_array[i]->xref, fptr);
		fputs(" FAM\n", fptr);
		
		if(fref_array[i]->family->wife != NULL) {
			fputs("1 WIFE ", fptr);
			k = 0;
			while(ref_array[k] != NULL) {
				if(ref_array[k]->individual == fref_array[i]->family->wife) {
					fputs(ref_array[k]->xref, fptr);
					break;
				}
				k++;
			}
			fputs("\n", fptr);
		}
		
		if(fref_array[i]->family->husband != NULL) {
			fputs("1 HUSB ", fptr);
			k = 0;
			while(ref_array[k] != NULL) {
				if(ref_array[k]->individual == fref_array[i]->family->husband) {
					fputs(ref_array[k]->xref, fptr);
					break;
				}
				k++;
			}
			fputs("\n", fptr);
		}
		
		if(fref_array[i]->family->children.length != 0) {
			node = fref_array[i]->family->children.head;
			while(node != NULL) {
				fputs("1 CHIL ", fptr);
				ind = (Individual*)node->data;
				k = 0;
				while(ref_array[k] != NULL) {
					if(ref_array[k]->individual == ind) {
						fputs(ref_array[k]->xref, fptr);
						break;
					}
					k++;
				}
				fputs("\n", fptr);
				node = node->next;
			}
		}
		
		node = fref_array[i]->family->events.head;
		while(node != NULL) {
			event = (Event*)node->data;
			fputs("1 ", fptr);
			fputs(event->type, fptr);
			fputs("\n", fptr);
			if(event->date[0] != '\0') {
				fputs("2 DATE ", fptr);
				fputs(event->date, fptr);
				fputs("\n", fptr);
			}
			if(event->place[0] != '\0') {
				fputs("2 PLAC ", fptr);
				fputs(event->place, fptr);
				fputs("\n", fptr);
			}
			
			node = node->next;
		}
		
		i++;
	}
	
	//writing trailer
	fputs("0 TRLR\n", fptr);
	
	fclose(fptr);
	i = 0;
	while(ref_array[i] != NULL) {
		free(ref_array[i]);
		i++;
	}
	free(ref_array);
	
	i = 0;
	while(fref_array[i] != NULL) {
		free(fref_array[i]);
		i++;
	}
	free(fref_array);
	
	return error;
	
}


GEDCOMerror createGEDCOM(char *fileName, GEDCOMobject **obj) { //should i free object if error occurs
	
	GEDCOMerror error;
	
	if(fileName == NULL ) {
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	if(!checkFile(fileName) || strlen(fileName) == 0) {
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	error.type = OK;
	error.line = -1;
	char line[1000];
	int i = 0;
	int hNum = 0;
	int tNum = 0;
	int sNum = 0;
	int level;
	int nxtLevel;
	int size;
	int line_count = 0;
	int breaker = 0;
	char *temp;
	char **fileRecords = malloc(sizeof(char*));
	char **split_line;
	char c;
	int l = 0;
	
	
	FILE *fptr;
	fptr = fopen(fileName, "r");
	if(fptr == NULL) {
		free(fileRecords);
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
	
	while(!feof(fptr)) {
		c = fgetc(fptr);
		l = 0;
		memset(line,0,sizeof(line));
		
		if(!isdigit(c)) {
			continue;
		}
		
		while(!feof(fptr) && (c != '\r' && c != '\n')) {
			line[l] = c;
			
			c = fgetc(fptr);
			l++;
		}
		line[l] = '\0'; 
		line[strcspn(line, "\r\n")] = 0;
		if(strlen(line) > 255) {
			fileRecords[line_count] = NULL;
			error.type = INV_RECORD;
			error.line = line_count + 1;
			freeArray(fileRecords);
			fclose(fptr);
			return error;
		}
		temp = malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(temp, line);
		
		fileRecords[line_count] = temp;
		line_count++;
		fileRecords = realloc(fileRecords, sizeof(char *) * (line_count + 1));
	}
	fileRecords[line_count] = NULL;
	fclose(fptr);
	
	//check for header and trailer
	if(fileRecords[0] == NULL) {
		freeArray(fileRecords);
		error.type = INV_GEDCOM;
		error.line = -1;
		return error;
	}
	
	temp = malloc(sizeof(char) * (strlen(fileRecords[0]) + 1));
	strcpy(temp, fileRecords[0]);
	split_line = split(temp);
	if(!(strcmp(split_line[0], "0") == 0)) {
		free(split_line);
		free(temp);
		freeArray(fileRecords);
		error.type = INV_HEADER;
		error.line = -1;
		return error;
	}
	free(split_line);
	free(temp);
	
	temp = malloc(sizeof(char) * (strlen(fileRecords[0]) + 1));
	strcpy(temp, fileRecords[0]);
	split_line = split(temp);
	if(!(strcmp(split_line[0], "0") == 0 && strcmp(split_line[1], "HEAD") == 0)) {
		free(split_line);
		free(temp);
		freeArray(fileRecords);
		error.type = INV_GEDCOM;
		error.line = -1;
		return error;
	}
	free(split_line);
	free(temp);
	
	temp = malloc(sizeof(char) * (strlen(fileRecords[line_count - 1]) + 1));
	strcpy(temp, fileRecords[line_count - 1]);
	split_line = split(temp);
	if(!(strcmp(split_line[0], "0") == 0 && strcmp(split_line[1], "TRLR") == 0)) {
		free(split_line);
		free(temp);
		freeArray(fileRecords);
		error.type = INV_GEDCOM;
		error.line = -1;
		return error;
	}
	free(split_line);
	free(temp);
	
	i = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		if(arraySize(split_line) < 2) {
			free(split_line);
			free(temp);
			freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+1;
			return error;
		}
		if(!is_Integer(split_line[0])) {
			free(split_line);
			free(temp);
			freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+1;
			return error;
		}
		level = atoi(split_line[0]);
		if(level < 0 || level > 99) {
			free(split_line);
			free(temp);
			freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+1;
			return error;
		}
		if(strlen(split_line[1]) > 31) {
			free(split_line);
			free(temp);
			freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+1;
			return error;
		}
		if(strcmp(split_line[1], "HEAD") == 0) {
			hNum++;
		}
		if(strcmp(split_line[1], "TRLR") == 0) {
			tNum++;
		}
		if(split_line[2] != NULL) {
			if(strcmp(split_line[2], "SUBM") == 0) {
				sNum++;
			}
		}
		//try to error check for values later on
		//check if every line has a value


		free(split_line);
		free(temp);
		i++;
	}
	if(sNum < 1) {
		freeArray(fileRecords);
		error.type = INV_GEDCOM;
		error.line = -1;
		return error;
	}
	
	i = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		level = atoi(split_line[0]);
		size = arraySize(split_line);
		
		if(strcmp(split_line[1], "HEAD") == 0 || strcmp(split_line[1], "TRLR") == 0){
			i++;
			free(split_line);
			free(temp);
			continue;
		}
		
		free(split_line);
		free(temp);
		
		if(fileRecords[i+1] == NULL)break;
		temp = malloc(sizeof(char) * (strlen(fileRecords[i+1]) + 1));
		strcpy(temp, fileRecords[i+1]);
		split_line = split(temp);
		nxtLevel = atoi(split_line[0]);
		free(split_line);
		free(temp);
		
		if(size < 3 && nxtLevel <= level) {
			/*freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+1;
			return error;*/
		}
		if((nxtLevel - level) > 1) {
			freeArray(fileRecords);
			error.type = INV_RECORD;
			error.line = i+2;
			return error;
		}
		
		i++;
	}
	
	if(hNum > 1 || tNum > 1) {
		freeArray(fileRecords);
		error.type = INV_GEDCOM;
		error.line = -1;
		return error;
	}
	
	
	//get header details
	char **header_array = malloc(sizeof(char*));
	int header_count = 0;
	i = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[0], "0") == 0 && breaker != 0) {
			free(split_line);
			free(temp);
			break;
		}
		
		if(strcmp(split_line[0], "0") == 0 && strcmp(split_line[1], "HEAD") == 0) {
			breaker++;
		}
		
		if(breaker > 0) {
			header_array[header_count] = fileRecords[i];
			header_count++;
			header_array = realloc(header_array, sizeof(char *) * (header_count + 1));
		}
		
		free(split_line);
		free(temp);
		i++;
	}
	header_array[header_count] = NULL;
	
	//check for submitter pointer in header
	int present = 0;
	i = 0;
	while(header_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(header_array[i]) + 1));
		strcpy(temp, header_array[i]);
		split_line = split(temp);
		
		if(split_line[2] != NULL) {
			if(strcmp(split_line[1], "SUBM") == 0) {
				present++;
			}
		}
		i++;
		free(split_line);
		free(temp);
	}
	
	if(present != 1) {
		error.type = INV_HEADER;
		error.line = -1;
		freeArray(fileRecords);
		free(header_array);
		return error;
	}
	
	//get submitter details
	char **submitter_array = malloc(sizeof(char*));
	int submitter_count = 0;
	i = 0;
	breaker = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		
		if(strcmp(split_line[0], "0") == 0 && breaker != 0) {
			free(split_line);
			free(temp);
			break;
		}

		if(split_line[2] != NULL && strcmp(split_line[2], "SUBM") == 0 && strcmp(split_line[0], "0") == 0) {
			breaker++;
		}
		
		if(breaker > 0) {
			submitter_array[submitter_count] = fileRecords[i];
			submitter_count++;
			submitter_array = realloc(submitter_array, sizeof(char *) * (submitter_count + 1));
		}
		free(split_line);
		free(temp);
		i++;
		
	}
	submitter_array[submitter_count] = NULL;
	
	
	
	GEDCOMobject *new = malloc(sizeof(GEDCOMobject));
	Submitter *newSub = createSubmitter(submitter_array);
	if(newSub == NULL) {
		error.type = INV_RECORD;
		error.line = -1;
		freeArray(fileRecords);
		free(header_array);
		free(submitter_array);
		return error;
	}
	new->submitter = newSub;
	
	Header *newHead = createHeader(newSub, header_array);
	if(newHead == NULL) {
		clearSubmitter(newSub);
		free(new);
		error.type = INV_HEADER;
		error.line = -1;
		freeArray(fileRecords);
		free(header_array);
		free(submitter_array);
		return error;
	}
	int sourceLen = strlen(newHead->source);
	if(sourceLen == 0) {
		clearSubmitter(newSub);
		free(new);
		error.type = INV_HEADER;
		error.line = -1;
		freeArray(fileRecords);
		free(header_array);
		free(submitter_array);
		return error;
	}
	new->header = newHead;
	
	
	//get individuals array;
	i = 0;
	char **individual_array = malloc(sizeof(char*));
	int individual_count = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		
		if(split_line[2] == NULL) {
			free(split_line);
			free(temp);
			i++;
			continue;
		}
		
		if(strcmp(split_line[0], "0") == 0 && strcmp(split_line[2], "INDI") == 0) {
			free(split_line);
			free(temp);
			individual_array[individual_count] = fileRecords[i];
			individual_count++;
			individual_array = realloc(individual_array, sizeof(char *) * (individual_count + 1));
			
			if(fileRecords[i+1] == NULL) {
				break;
			}
			
			temp = malloc(sizeof(char) * (strlen(fileRecords[i+1]) + 1));
			strcpy(temp, fileRecords[i+1]);
			split_line = split(temp);
			
			while(strcmp(split_line[0], "0") != 0) {
				free(split_line);
				free(temp);
				
				i++;
				individual_array[individual_count] = fileRecords[i];
				individual_count++;
				individual_array = realloc(individual_array, sizeof(char *) * (individual_count + 1));
				if(fileRecords[i+1] == NULL) {
					break;
				}
				temp = malloc(sizeof(char) * (strlen(fileRecords[i+1]) + 1));
				strcpy(temp, fileRecords[i+1]);
				split_line = split(temp);
			}
		}
		free(split_line);
		free(temp);
		i++;
	}
	individual_array[individual_count] = NULL;
	
	i = 0;
	//int j = 0;
	//int total = 0;
	int single_count = 0;
	new->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
	Individual *newInd;
	char **single_individual;
	Xref *ref;
	Xref **ref_array = malloc(sizeof(Xref*));
	int ref_count = 0;
	
	//here, create new->individuals and SET SINGLE_COUNT TO ZERO EACH TIME
	while(individual_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(individual_array[i]) + 1));
		strcpy(temp, individual_array[i]);
		split_line = split(temp);
		
		if(split_line[2] == NULL) {
			free(split_line);
			free(temp);
			i++;
			continue;
		}
		
		single_count = 0;
		single_individual = malloc(sizeof(char*));
		
		if(strcmp(split_line[0], "0") == 0 && strcmp(split_line[2], "INDI") == 0) {
			ref = malloc(sizeof(Xref));
			strcpy(ref->xref, split_line[1]);
			free(split_line);
			free(temp);
			single_individual[single_count] = individual_array[i];
			single_count++;
			single_individual = realloc(single_individual, sizeof(char *) * (single_count + 1));
			
			if(individual_array[i+1] == NULL) {
				goto INDIVIDUAL;
			}
			
			temp = malloc(sizeof(char) * (strlen(individual_array[i+1]) + 1));
			strcpy(temp, individual_array[i+1]);
			split_line = split(temp);
			
			while(strcmp(split_line[0], "0") != 0) {
				free(split_line);
				free(temp);
				
				i++;
				single_individual[single_count] = individual_array[i];
				single_count++;
				single_individual = realloc(single_individual, sizeof(char *) * (single_count + 1));
				if(individual_array[i+1] == NULL) {
					goto INDIVIDUAL;
				}
				temp = malloc(sizeof(char) * (strlen(individual_array[i+1]) + 1));
				strcpy(temp, individual_array[i+1]);
				split_line = split(temp);
			}
			if(strcmp(split_line[0], "0") == 0) {
				free(split_line);
				free(temp);
			}
			INDIVIDUAL: single_individual[single_count] = NULL;
			newInd = createIndividual(single_individual);
			ref->individual = newInd;
			insertSorted(&(new->individuals), (void*)newInd);
			ref_array[ref_count] = ref;
			ref_count++;
			ref_array = realloc(ref_array, sizeof(Xref *) * (ref_count + 1));
		}
		else{
			free(split_line);
			free(temp);
		}
		free(single_individual);
		i++;
	}
	ref_array[ref_count] = NULL;
	
	//get family array
	i = 0;
	char **family_array = malloc(sizeof(char*));
	int family_count = 0;
	while(fileRecords[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(fileRecords[i]) + 1));
		strcpy(temp, fileRecords[i]);
		split_line = split(temp);
		
		if(split_line[2] == NULL) {
			free(split_line);
			free(temp);
			i++;
			continue;
		}
		
		if(strcmp(split_line[0], "0") == 0 && strcmp(split_line[2], "FAM") == 0) {
			free(split_line);
			free(temp);
			family_array[family_count] = fileRecords[i];
			family_count++;
			family_array = realloc(family_array, sizeof(char *) * (family_count + 1));
			
			if(fileRecords[i+1] == NULL) {
				break;
			}
			
			temp = malloc(sizeof(char) * (strlen(fileRecords[i+1]) + 1));
			strcpy(temp, fileRecords[i+1]);
			split_line = split(temp);
			
			while(strcmp(split_line[0], "0") != 0) {
				free(split_line);
				free(temp);
				
				i++;
				family_array[family_count] = fileRecords[i];
				family_count++;
				family_array = realloc(family_array, sizeof(char *) * (family_count + 1));
				if(fileRecords[i+1] == NULL) {
					break;
				}
				temp = malloc(sizeof(char) * (strlen(fileRecords[i+1]) + 1));
				strcpy(temp, fileRecords[i+1]);
				split_line = split(temp);
			}
		}
		free(split_line);
		free(temp);
		i++;
	}
	family_array[family_count] = NULL;
	
	
	
	i = 0;
	single_count = 0;
	new->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
	Family *newFam;
	char **single_family;
	
	//here, create new->families and SET SINGLE_COUNT TO ZERO EACH TIME
	while(family_array[i] != NULL) {
		temp = malloc(sizeof(char) * (strlen(family_array[i]) + 1));
		strcpy(temp, family_array[i]);
		split_line = split(temp);
		
		if(split_line[2] == NULL) {
			free(split_line);
			free(temp);
			i++;
			continue;
		}
		
		single_count = 0;
		single_family = malloc(sizeof(char*));
		
		if(strcmp(split_line[0], "0") == 0 && strcmp(split_line[2], "FAM") == 0) {
			free(split_line);
			free(temp);
			single_family[single_count] = family_array[i];
			single_count++;
			single_family = realloc(single_family, sizeof(char *) * (single_count + 1));
			
			if(family_array[i+1] == NULL) {
				goto FAMILY;
			}
			
			temp = malloc(sizeof(char) * (strlen(family_array[i+1]) + 1));
			strcpy(temp, family_array[i+1]);
			split_line = split(temp);
			
			while(strcmp(split_line[0], "0") != 0) {
				free(split_line);
				free(temp);
				
				i++;
				single_family[single_count] = family_array[i];
				single_count++;
				single_family = realloc(single_family, sizeof(char *) * (single_count + 1));
				if(family_array[i+1] == NULL) {
					goto FAMILY;
				}
				temp = malloc(sizeof(char) * (strlen(family_array[i+1]) + 1));
				strcpy(temp, family_array[i+1]);
				split_line = split(temp);
			}
			if(strcmp(split_line[0], "0") == 0) {
				free(split_line);
				free(temp);
			}
			FAMILY: single_family[single_count] = NULL;
			newFam = createFamily(single_family, ref_array);
			insertSorted(&(new->families), (void*)newFam);
		}
		else{
			free(split_line);
			free(temp);
		}
		free(single_family);
		i++;
	}
	
	updateIndividuals(&(new->individuals), &(new->families));
	*obj = new;
	
	
	freeArray(fileRecords);
	free(header_array);
	free(submitter_array);
	free(individual_array);
	free(family_array);
	i = 0;
	while(ref_array[i] != NULL) {
		free(ref_array[i]);
		i++;
	}
	free(ref_array);
	
	return error;
	
}

char* printGEDCOM(const GEDCOMobject* obj) {
	
	char *return_string;
	if(obj == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	return_string = malloc(10);
	strcpy(return_string, "RETURN!");
	
	return return_string;
	
}

void deleteGEDCOM(GEDCOMobject* obj) {
	
	clearList(&(obj->families));
	clearList(&(obj->individuals));
	clearSubmitter(obj->submitter);
	clearList(&(obj->header->otherFields));
	free(obj->header);
	free(obj);
	
}

char *printError(GEDCOMerror err) {

	int len = 50;
	
	char *return_string = malloc(sizeof(char) * len);
	
	snprintf(return_string, len, "Type: %d\nLine: %d\n", err.type, err.line);
	
	return return_string;
	
}

Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person) {
	
	if(familyRecord == NULL || person == NULL || compare == NULL) {
		return NULL;
	}
	
	Node* node = familyRecord->individuals.head;
	Individual *Person = (Individual*)person;
	Individual *search;
	
	while(node != NULL) {
		search = node->data;
		if(compare(search, Person) == 1) {
			return search;
		}
		node = node->next;
	}
	
	return NULL;
	
}

List getDescendants(const GEDCOMobject* familyRecord, const Individual* person) {
	
	List descendants;
	descendants = initializeList(&printIndividual, &fakeDelete, &compareIndividuals);
	
	if(familyRecord == NULL || person == NULL) {
		return descendants;
	}
	
	Individual *Person;
	Individual *temp;
	Node *s = familyRecord->individuals.head;
	
	while(s != NULL) {
		temp = (Individual*)s->data;
		if(temp == person) {
			Person = temp;
			break;
		}
		s = s->next;
	}
	
	descendants = getChildren(Person);
	
	return descendants;
}


List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen) {
	
	List descendants;
	descendants = initializeList(&printGeneration, &deleteGeneration, &compareGenerations);
	
	if(familyRecord == NULL || person == NULL || maxGen < 0) {
		return descendants;
	}
	
	int present = 0;
	Individual *Person;
	Individual *temp;
	Node *s = familyRecord->individuals.head;
	
	while(s != NULL) {
		temp = (Individual*)s->data;
		if(temp == person) {
			present = 1;
			Person = temp;
			break;
		}
		s = s->next;
	}
	List allDescendants;
	GEDCOMobject *obj = (GEDCOMobject*)familyRecord;
	int i = 0;
	
	if(present == 1) {
		allDescendants = getDescendants(obj, Person);
	}
	else{
		return descendants;
	}
	
	if(allDescendants.length < 1) {
		return descendants;
	}
	clearList(&allDescendants);
	
	if(maxGen != 0) {
		//start
		List array[maxGen + 1];
		array[0] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
		getKids(Person, &array[0]);
		insertBack(&descendants, (void*)&array[0]);
		
		Node *node;
		for(i= 1; i < maxGen; i++) {
			node = array[i-1].head;
			array[i] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
			while(node != NULL) {
				Individual *ind = (Individual*)node->data;
				getKids(ind, &array[i]);
				node = node->next;
			}
			if(array[i].length == 0) {
				break;
			}
			insertBack(&descendants, (void*)&array[i]);
			
		}
	}
	if(maxGen == 0) {
		//start
		List array[100];
		array[0] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
		getKids(Person, &array[0]);
		insertBack(&descendants, (void*)&array[0]);
		
		Node *node;
		for(i= 1; i < 100; i++) {
			node = array[i-1].head;
			array[i] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
			while(node != NULL) {
				Individual *ind = (Individual*)node->data;
				getKids(ind, &array[i]);
				node = node->next;
			}
			if(array[i].length == 0) {
				break;
			}
			insertBack(&descendants, (void*)&array[i]);
			
		}
	}
	
	
	return descendants;
	
}


List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen) {
	
	List ancestors;
	ancestors = initializeList(&printGeneration, &deleteGeneration, &compareGenerations);
	
	if(familyRecord == NULL || person == NULL || maxGen < 0) {
		return ancestors;
	}
	
	int present = 0;
	Individual *Person;
	Individual *temp;
	Node *s = familyRecord->individuals.head;
	
	while(s != NULL) {
		temp = (Individual*)s->data;
		if(temp == person) {
			present = 1;
			Person = temp;
			break;
		}
		s = s->next;
	}
	
	int i = 0;
	
	if(present != 1) {
		return ancestors;
	}
	
	if(maxGen != 0) {
		//start
		List array[maxGen + 1];
		array[0] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
		getParents(Person, &array[0]);
		if(array[0].length != 0) {
			insertBack(&ancestors, (void*)&array[0]);
			
			Node *node;
			for(i= 1; i < maxGen; i++) {
				node = array[i-1].head;
				array[i] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
				while(node != NULL) {
					Individual *ind = (Individual*)node->data;
					getParents(ind, &array[i]);
					node = node->next;
				}
				if(array[i].length == 0) {
					break;
				}
				insertBack(&ancestors, (void*)&array[i]);
				
			}
		}
	}
	if(maxGen == 0) {
		//start
		List array[100];
		array[0] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
		getParents(Person, &array[0]);
		insertBack(&ancestors, (void*)&array[0]);
		
		Node *node;
		for(i= 1; i < 100; i++) {
			node = array[i-1].head;
			array[i] = initializeList(&printIndividual, &fakeDelete, &compareIndsInGen);
			while(node != NULL) {
				Individual *ind = (Individual*)node->data;
				getParents(ind, &array[i]);
				node = node->next;
			}
			if(array[i].length == 0) {
				break;
			}
			insertBack(&ancestors, (void*)&array[i]);
			
		}
	}
	
	return ancestors;
	
}


void deleteField(void* toBeDeleted) {
	
	if(toBeDeleted == NULL) {
		return;
	}
	
	Field *del = (Field*)toBeDeleted;
	
	free(del->tag);
	free(del->value);
	free(del);
	
}

int compareFields(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	Field *one = (Field*)first;
	Field *two = (Field*)second;
	
	int size1 = strlen(one->tag) + strlen(one->value) + 3;
	int size2 = strlen(two->tag) + strlen(two->value) + 3;
	
	char string1[size1];
	char string2[size2];
	
	strcpy(string1, one->tag);
	strcat(string1, " ");
	strcat(string1, one->value);
	
	strcpy(string2, two->tag);
	strcat(string2, " ");
	strcat(string2, two->value);
	
	return strcmp(string1, string2);
	
}

char* printField(void* toBePrinted) {
	
	char *return_string;
	if(toBePrinted == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	Field *print = (Field*)toBePrinted;
	int printSize = 40 + strlen(print->tag) + strlen(print->value);
	return_string = malloc(sizeof(char) * printSize);
	
	snprintf(return_string, printSize, "Tag: %s\nValue: %s\n", print->tag, print->value);
	
	return return_string;
	
}


void deleteIndividual(void* toBeDeleted) {
	
	if(toBeDeleted == NULL) {
		return;
	}
	
	Individual *del = (Individual*)toBeDeleted;
	
	free(del->givenName);
	free(del->surname);
	
	clearList(&(del->events));
	clearList(&(del->families));
	
	clearList(&(del->otherFields));
	free(del);
	
	
}


int compareIndividuals(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	Individual *one = (Individual*)first;
	Individual *two = (Individual*)second;
	
	int size1 = strlen(one->givenName) + strlen(one->surname) + 4;
	int size2 = strlen(two->givenName) + strlen(two->surname) + 4;
	
	char string1[size1];
	char string2[size2];
	
	strcpy(string1, one->givenName);
	strcat(string1, ",");
	strcat(string1, one->surname);
	
	strcpy(string2, two->givenName);
	strcat(string2, ",");
	strcat(string2, two->surname);
	
	return strcmp(string1, string2);
	
}


char* printIndividual(void* toBePrinted) {
	
	char *return_string;
	if(toBePrinted == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	Individual *print = (Individual*)toBePrinted;
	
	char *eventPrint = toString(print->events);
	int printSize = 45 + strlen(print->givenName) + strlen(print->surname) + strlen(eventPrint);
	return_string = malloc(sizeof(char) * printSize);
	
	snprintf(return_string, printSize, "\nGiven Name: %s\nSurname: %s\n", print->givenName, print->surname);
	strcat(return_string, "EVENTS: ");
	strcat(return_string, eventPrint);
	free(eventPrint);
	
	return return_string;
	
}

void deleteEvent(void* toBeDeleted) {
	
	if(toBeDeleted == NULL) {
		return;
	}
	
	Event *del = (Event*)toBeDeleted;
	
	free(del->date);
	free(del->place);
	
	clearList(&(del->otherFields));
	free(del);
	
}

int compareEvents(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	Event *one = (Event*)first;
	Event *two = (Event*)second;
	
	if(strlen(one->date) == 0 || strlen(two->date) == 0) {
		return strcmp(one->type, two->type);
	}
	
	int len1 = strlen(one->date) + 1;
	int len2 = strlen(two->date) + 1;
	
	char temp1[len1];
	char temp2[len2];
	char **split1;
	char **split2;
	
	strcpy(temp1, one->date);
	strcpy(temp2, two->date);
	
	split1 = split(temp1);
	split2 = split(temp2);
	
	int size1 = 0;
	int size2 = 0;
	
	int i = 0;
	while(split1[i] != NULL) {
		size1++;
		i++;
	}
	i = 0;
	while(split2[i] != NULL) {
		size2++;
		i++;
	}
	
	//check year first
	int year1;
	int year2;
	
	year1 = atoi(split1[size1 - 1]);
	year2 = atoi(split2[size2 - 1]);
	
	free(split1);
	free(split2);
	if(year1 > year2) {
		return 1;
	}
	else if(year1 < year2) {
		return -1;
	}
	else{
		return 0;
	}
	
	return 0;
	
}


char* printEvent(void* toBePrinted) {
	
	char *return_string;
	if(toBePrinted == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	Event *print = (Event*)toBePrinted;
	
	int printSize = 40 + strlen(print->type) + strlen(print->date) + strlen(print->place);
	return_string = malloc(sizeof(char) * printSize);
	
	snprintf(return_string, printSize, "Type: %s\nDate: %s\nPlace: %s\n", print->type, print->date, print->place);
	
	return return_string;
}

void deleteFamily(void* toBeDeleted) {
	
	if(toBeDeleted == NULL) {
		return;
	}
	
	Family *del = (Family*)toBeDeleted;
	
	clearList(&(del->children));
	clearList(&(del->otherFields));
	clearList(&(del->events));
	
	free(del);
	
}


int compareFamilies(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	Family *one = (Family*)first;
	Family *two = (Family*)second;
	
	int size1 = 0;
	int size2 = 0;
	
	if(one->wife != NULL) {
		size1++;
	}
	if(one->husband != NULL) {
		size1++;
	}
	size1 = size1 + one->children.length;
	
	if(two->wife != NULL) {
		size2++;
	}
	if(two->husband != NULL) {
		size2++;
	}
	size2 = size2 + two->children.length;
	
	if(size1 < size2)return -1;
	if(size1 > size2)return 1;
	if(size1 == size2)return 0;
	
	return 0;
	
}


char* printFamily(void* toBePrinted) {

	char *return_string;
	if(toBePrinted == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	Family *print = (Family*)toBePrinted;
	
	char *wife = printIndividual((void*)print->wife);
	char *husband = printIndividual((void*)print->husband);
	char *children = toString(print->children);
	char *events = toString(print->events);
	
	int size = 50 + strlen(wife) + strlen(husband) + strlen(children) + strlen(events);
	return_string = malloc(sizeof(char) * size);
	
	strcpy(return_string, "\nWIFE: \n");
	strcat(return_string, wife);
	strcat(return_string, "HUSBAND: \n");
	strcat(return_string, husband);
	strcat(return_string, "CHILDREN: \n");
	strcat(return_string, children);
	strcat(return_string, "FAMILY EVENTS: \n");
	strcat(return_string, events);
	
	free(wife);
	free(husband);
	free(children);
	
	return return_string;
	
}

void deleteGeneration(void* toBeDeleted) {
	
	if(toBeDeleted == NULL) {
		return;
	}
	
	List list = *(List*)toBeDeleted;
	
	clearList(&list);
	
}


int compareGenerations(const void* first,const void* second) {
	
	if(first == NULL || second == NULL) {
		return 0;
	}
	
	return 1;
	
}


char* printGeneration(void* toBePrinted) {
	
	char *return_string;
	if(toBePrinted == NULL) {
		return_string = malloc(sizeof(char));
		return_string[0] = '\0';
		return return_string;
	}
	
	return_string = malloc(sizeof(char));
	return_string[0] = '\0';
	return return_string;
	
}
