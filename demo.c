#include "evq.h"
#include <stdio.h>
#include <stdbool.h>

bool printA(event_queue* q, branch_event* e){
	printf("A");
	return true;
}

bool printB(event_queue* q, branch_event* e){
	printf("B");
	return true;
}

bool printAndSometimesErrorC(event_queue* q, branch_event* e){
	printf("C");
	return (rand()&0x1);
}

void errorPrintD(event_queue* q, branch_event* e){
	printf("D");
}

bool printAndErrorE(event_queue* q, branch_event* e){
	printf("E");
	return false;
}

void removeEPrintF(event_queue* q, branch_event* e){
	printf("F");
	remove_event(q, e);
}

bool printData(event_queue* q, branch_event* e){
	printf("%s", (char*)e->data);
	return true;
}

int main(int argc, char const *argv[]){
	//First, initialize an empty queue
	event_queue* q = init_evq();

	//Most basic way to add a function
	add_event(q, printA, NULL, NULL);

	//Save the event_id to delete later
	event_id id = add_event(q, printB, NULL, NULL);

	//If function returns true or false, you can also add an false handler
	add_event(q, printAndSometimesErrorC, errorPrintD, NULL);

	int i = 0;
	while(i<25){
		//Repetedly call next() on your queue to keep advancing exection
		next(q);
		i++;
	}
	printf("\n");

	//Remove an event with its id
	remove_id(q, id);

	//Look! No B
	i=0;
	while(i<25){
		next(q);
		i++;
	}
	printf("\n");

	//Events can remove themselves during execution
	add_event(q, printAndErrorE, removeEPrintF, NULL);

	//EF should be printed once then never again
	i=0;
	while(i<25){
		next(q);
		i++;
	}
	printf("\n");

	//Events can also be created with specific data relevent to that instance of an event
	char* s = " hello world! ";
	add_event(q, printData, NULL, (void*)s);
	i=0;
	while(i<5){
		next(q);
		i++;
	}
	printf("\n");

	return 0;
}