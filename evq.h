#include <stdbool.h>
#include <stdlib.h>	

typedef struct branch_event branch_event;
typedef struct event_queue event_queue;
typedef bool (*branch_func)(event_queue*, branch_event*);
typedef void (*error_func)(event_queue*, branch_event*);
typedef int event_id;

struct branch_event{
	event_id id;
	branch_func f;
	error_func err_f;
	void* data;
	branch_event* next;
};

struct event_queue{
	branch_event* start;
	branch_event* current;
	event_id next_id;
	bool exec;
};

event_queue* init_evq(){
	event_queue* evq = (event_queue*)malloc(sizeof(event_queue));
	evq->start = NULL;
	evq->next_id = 0;
	evq->exec = false;
	return evq;
}

event_id add_event(event_queue* evq, branch_func f, error_func err_f, void* data){
	if(f == NULL || evq == NULL)
		return -1;

	branch_event* event = (branch_event*)malloc(sizeof(branch_event));
	event->id = ++evq->next_id;
	event->f = f;
	event->err_f = err_f;
	event->next = evq->start;
	event->data = data;
	evq->start = event;
	return event->id;
}

void remove_id(event_queue* evq, event_id id){
	if(evq->start->id == id){
		branch_event* t = evq->start->next;
		free(evq->start);
		evq->start = t;
		return;
	}

	branch_event* cur = evq->start->next;
	branch_event* prev = evq->start;
	while(cur->id != id){
		prev = cur;
		cur = cur->next;
	}
	prev->next = cur->next;
	if(evq->current == cur && !evq->exec)
		evq->current = prev->next;
	free(cur);
}

void remove_event(event_queue* evq, branch_event* evt){
	remove_id(evq, evt->id);
}

branch_event* get_event_by_id(event_queue* evq, event_id id){
	branch_event* cur = evq->start;
	while(cur->id != id){
		cur = cur->next;
		if(cur->next == NULL)
			break;
	}
	return cur;
}

void next(event_queue* evq){
	evq->exec = true;
	if(evq->current == NULL)
		evq->current = evq->start;

	branch_event* next_event;
	if(evq->current->next != NULL)
		next_event = evq->current->next;
	else
		next_event = evq->start;

	if(!(evq->current->f(evq, evq->current)))
		if(evq->current->err_f != NULL)
			evq->current->err_f(evq, evq->current);
	
	evq->current = next_event; 
	evq->exec = false;
}

