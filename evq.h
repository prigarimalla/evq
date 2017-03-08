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

event_queue* init_evq(void);
event_id add_event(event_queue* evq, branch_func f, error_func err_f, void* data);
void remove_id(event_queue* evq, event_id id);
void remove_event(event_queue* evq, branch_event* evt);
branch_event* get_event_by_id(event_queue* evq, event_id id);
void next(event_queue* evq);
