#ifndef EVQ_H
#define EVQ_H

#include <stdlib.h>
#include <stdbool.h>

// #define STATIC
// #define NUM_QUEUES 3
// #define NUM_EVENTS 40

typedef struct branch_event branch_event;
typedef struct event_queue event_queue;
typedef bool (*branch_func)(event_queue*, branch_event*);
typedef void (*error_func)(event_queue*, branch_event*);
typedef int event_id;

struct branch_event{
	#ifdef STATIC
		bool in_use;
	#endif
	event_id id;
	branch_func f;
	error_func err_f;
	void* data;
	branch_event* next;
};

struct event_queue{
	#ifdef STATIC
		bool in_use;
	#endif
	branch_event* start;
	branch_event* current;
	event_id next_id;
	bool exec;
};

static event_queue* alloc_evq(void);
static void free_evq(event_queue* evq);
static branch_event* alloc_branch_event(void);
static void free_branch_event(branch_event* evt);
event_queue* evq_init(void);
void evq_done(event_queue* evq);
event_id evq_add(event_queue* evq, branch_func f, error_func err_f, void* data);
void evq_remove(event_queue* evq, event_id id);
void evq_next(event_queue* evq);
void evq_front(event_queue* evq);
void evq_all(event_queue* evq);
void evq_reset(event_queue* evq);

#endif