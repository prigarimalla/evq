#include "evq.h"

#ifdef STATIC
	static event_queue available_evq[NUM_QUEUES];
	static branch_event available_branch_event[NUM_EVENTS]; 
#endif

static event_queue* alloc_evq(){
	#ifdef STATIC
		event_queue* t;
		int i;
		for(i = 0; i < NUM_QUEUES; i++){
			if(!available_evq[i].in_use){
				available_evq[i].in_use = true;
				t = &(available_evq[i]);
				break;
			}
		}
		return t;
	#else
		return (event_queue*)malloc(sizeof(event_queue));
	#endif
}

static void free_evq(event_queue* evq){
	#ifdef STATIC
		evq->in_use = false;
	#else
		if(evq != NULL){
			free(evq);
		}
	#endif
}

static branch_event* alloc_branch_event(void){
	#ifdef STATIC
		branch_event* t;
		int i;
		for(i = 0; i < NUM_EVENTS; i++){
			if(!available_branch_event[i].in_use){
				available_branch_event[i].in_use = true;
				t = &(available_branch_event[i]);
				break;
			}
		}
		return t;
	#else
		return (branch_event*)malloc(sizeof(branch_event));
	#endif
}

static void free_branch_event(branch_event* evt){
	#ifdef STATIC
		evt->in_use = false;
	#else
		if(evt != NULL){
			free(evt);
		}
	#endif
}

event_queue* evq_init(){
	event_queue* evq = alloc_evq();
	evq->start = NULL;
	evq->next_id = 0;
	evq->exec = false;
	return evq;
}

void evq_done(event_queue* evq){
	branch_event* t = evq->start;
	while(t != NULL){
		branch_event* n = t->next;
		evq_remove(evq, t->id);
		t = n;
	}
	free_evq(evq);
}

event_id evq_add(event_queue* evq, branch_func f, error_func err_f, void* data){
	if(f == NULL || evq == NULL){
		return -1;
	}

	branch_event* event = alloc_branch_event();
	event->id = ++evq->next_id;
	event->f = f;
	event->err_f = err_f;
	event->next = evq->start;
	event->data = data;
	evq->start = event;
	return event->id;
}

void evq_remove(event_queue* evq, event_id id){
	if(evq->start->id == id){
		branch_event* t = evq->start->next;
		free_branch_event(evq->start);
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
	if(evq->current == cur && !evq->exec){
		evq->current = prev->next;
	}
	free_branch_event(cur);
}

void evq_next(event_queue* evq){
	evq->exec = true;
	if(evq->current == NULL){
		evq->current = evq->start;
	}

	branch_event* next_event;
	if(evq->current->next != NULL){
		next_event = evq->current->next;
	}
	else
		next_event = evq->start;

	if(!(evq->current->f(evq, evq->current)))
		if(evq->current->err_f != NULL){
			evq->current->err_f(evq, evq->current);
		}

	evq->current = next_event;
	evq->exec = false;
}