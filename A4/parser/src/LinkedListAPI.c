#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "LinkedListAPI.h"
#include "assert.h"

List initializeList(char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)) {
	
	List tmpList;
	
    //Asserts create a partial function...
    assert(printFunction != NULL);
    assert(deleteFunction != NULL);
    assert(compareFunction != NULL);
	
	tmpList.head = NULL;
	tmpList.tail = NULL;
	tmpList.deleteData = deleteFunction;
	tmpList.compare = compareFunction;
	tmpList.printData = printFunction;
	tmpList.length = 0;
	
	return tmpList;
	
}


Node* initializeNode(void* data) {
	
	Node* tmpNode;
	
	tmpNode = (Node*)malloc(sizeof(Node));
	
	if (tmpNode == NULL){
		return NULL;
	}
	
	tmpNode->data = data;
	tmpNode->previous = NULL;
	tmpNode->next = NULL;
	
	return tmpNode;
	
}



void insertFront(List* list, void* toBeAdded) {
	
	if (list == NULL || toBeAdded == NULL){
		return;
	}
	
	Node* newNode = initializeNode(toBeAdded);
	
    if (list->head == NULL && list->tail == NULL){
        list->head = newNode;
        list->tail = list->head;
    }else{
		newNode->next = list->head;
        list->head->previous = newNode;
    	list->head = newNode;
    }
    list->length++;
	
}



void insertBack(List* list, void* toBeAdded) {
	
	if (list == NULL || toBeAdded == NULL){
		return;
	}
	
	Node* newNode = initializeNode(toBeAdded);
	
    if (list->head == NULL && list->tail == NULL){
        list->head = newNode;
        list->tail = list->head;
    }else{
		newNode->previous = list->tail;
        list->tail->next = newNode;
    	list->tail = newNode;
    }
	list->length++;
}



void clearList(List* list) {
	
	if (list == NULL){
		return;
	}
	
	if (list->head == NULL && list->tail == NULL){
		return;
	}
	
	Node* tmp;
	
	while (list->head != NULL){
		list->deleteData(list->head->data);
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}
	
	list->head = NULL;
	list->tail = NULL;
	
}



void insertSorted(List* list, void* toBeAdded) {
	
	if (list == NULL || toBeAdded == NULL){
		return;
	}
	
	if (list->head == NULL){
		insertBack(list, toBeAdded);
		return;
	}
	
	if (list->compare(toBeAdded, list->head->data) <= 0){
		insertFront(list, toBeAdded);
		return;
	}
	
	if (list->compare(toBeAdded, list->tail->data) > 0){
		insertBack(list, toBeAdded);
		return;
	}
	
	Node* currNode = list->head;
	
	while (currNode != NULL){
		if (list->compare(toBeAdded, currNode->data) <= 0){
		
			Node* newNode = initializeNode(toBeAdded);
			newNode->next = currNode;
			newNode->previous = currNode->previous;
			currNode->previous->next = newNode;
			currNode->previous = newNode;
			list->length++;
		
			return;
		}
	
		currNode = currNode->next;
	}
	
	return;
	
}



void* deleteDataFromList(List* list, void* toBeDeleted) {
	
	 if(list == NULL ||list->head == NULL || toBeDeleted == NULL) {
		return NULL;
	}
	
	Node* tmp = list->head;
	
	while(tmp != NULL){
		if (list->compare(toBeDeleted, tmp->data) == 0){
			//Unlink the node
			Node* delNode = tmp;
			
			if (tmp->previous != NULL){
				tmp->previous->next = delNode->next;
			}else{
				list->head = delNode->next;
			}
			
			if (tmp->next != NULL){
				tmp->next->previous = delNode->previous;
			}else{
				list->tail = delNode->previous;
			}
			
			void* data = delNode->data;
			free(delNode);
			list->length--;
			
			return data;
			
		}else{
			tmp = tmp->next;
		}
	}
	
	return NULL;
}



void* getFromFront(List list) {
	
	if (list.head == NULL){
		return NULL;
	}

	return list.head->data;
	
}



void* getFromBack(List list) {
	
	if (list.tail == NULL){
		return NULL;
	}
	
	return list.tail->data;
	
}



char* toString(List list) {
	
	ListIterator iter = createIterator(list);
	char* str;
		
	str = (char*)malloc(sizeof(char));
	strcpy(str, "");
	
	void* elem;
	while( (elem = nextElement(&iter)) != NULL){
		char* currDescr = list.printData(elem);
		int newLen = strlen(str)+50+strlen(currDescr);
		str = (char*)realloc(str, newLen);
		strcat(str, "\n");
		strcat(str, currDescr);
		
		free(currDescr);
	}
	
	return str;
	
}



ListIterator createIterator(List list) {
	
	ListIterator iter;

    iter.current = list.head;
    
    return iter;
	
}


void* nextElement(ListIterator* iter) {
	
	Node* tmp = iter->current;
    
    if (tmp != NULL){
        iter->current = iter->current->next;
        return tmp->data;
    }else{
        return NULL;
    }
	
}


int getLength(List list) {
	
	if(list.head == NULL) {
		return 0;
	}
	
	int count = 0;
	Node* tmp = list.head;
	
	while(tmp != NULL) {
		count++;
		tmp = tmp->next;
	}
	
	return count;
	
}


void* findElement(List list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord) {
	
	if(list.head == NULL) {
		return NULL;
	}
	
	Node* tmp = list.head;
	while(tmp != NULL) {
		if(customCompare(searchRecord, tmp->data) == true) {
			return tmp->data;
		}
		else{
			tmp = tmp->next;
		}
	}
	
	return NULL;
	
}
