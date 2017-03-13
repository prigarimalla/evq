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
	evq_remove(q, e->id);
}

bool printData(event_queue* q, branch_event* e){
	printf("%s", (char*)e->data);
	return true;
}

int main(int argc, char const *argv[]){
	//First, initialize an empty queue
	event_queue* q = evq_init();

	//Most basic way to add a function
	evq_add(q, printA, NULL, NULL);

	//Save the event_id to delete later
	event_id id = evq_add(q, printB, NULL, NULL);

	//If function returns true or false, you can also add an false handler
	evq_add(q, printAndSometimesErrorC, errorPrintD, NULL);

	int i = 0;
	while(i<25){
		//Repetedly call evq_next() on your queue to keep advancing exection
		evq_next(q);
		i++;
	}
	printf("\n");

	//Remove an event with its id
	evq_remove(q, id);

	//Look! No B
	i=0;
	while(i<25){
		evq_next(q);
		i++;
	}
	printf("\n");

	//Events can remove themselves during execution
	evq_add(q, printAndErrorE, removeEPrintF, NULL);

	//EF should be printed once then never again
	i=0;
	while(i<25){
		evq_next(q);
		i++;
	}
	printf("\n");

	//Events can also be created with specific data relevent to that instance of an event
	char* s = " hello world! ";
	evq_add(q, printData, NULL, (void*)s);
	i=0;
	while(i<5){
		evq_next(q);
		i++;
	}
	printf("\n");

	//When done with a queue remove all the events and free the queue in one step!
	evq_done(q);

	return 0;
}
