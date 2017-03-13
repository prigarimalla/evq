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
		//Repeatedly call evq_next() on your queue to keep advancing execution
		evq_next(q);
		i++;
	}
	printf("\n");

	//Remove an event with its id
	evq_remove(q, id);

	//Look! No B
	//Use this to execute every event once and wait stop at the front of the queue. 
	evq_all(q);
	printf("\n");

	//Events can remove themselves during execution
	evq_add(q, printAndErrorE, removeEPrintF, NULL);

	//EF should be printed once then never again
	evq_all(q);
	printf("\n");

	//Events can also be created with specific data relevant to that instance of an event
	char* s = " Hello World! ";
	id = evq_add(q, printData, NULL, (void*)s);
	evq_all(q);
	printf("\n");
	evq_remove(q, id);

	//After some arbitrary execution such as...
	evq_next(q);
	evq_next(q);
	//Use this to keep executing to the front of the queue
	//(Note this will not execute all events and if already at the front nothing will execute)
	evq_front(q);
	printf("\n");

	//To simply jump to the front WITHOUT executing, use evq_reset()
	evq_next(q);
	evq_next(q);
	evq_reset(q); //Nothing is printed, but the now at start of queue
	evq_next(q);
	printf("\n");

	//When done with a queue remove all the events and free the queue in one step!
	evq_done(q);

	return 0;
}
