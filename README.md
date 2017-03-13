# evq
evq (short for **ev**ent **q**ueue) is an event queue intended for microcontroller applications. Several boolean functions can be added to the queue and repetedly executed through just one function call, potentially branching to an error handler depending on the result before going to the next function in the queue. It supports both dynamic memory allocation or completely static allocation if desired. See `demo.c` for a fully commented example.

## Configuration
In `evq.h` uncomment the following lines to use static memory allocation. Set `NUM_QUEUES` and `NUM_EVENTS` as necessary.
(Keep in mind `NUM_EVENTS` is the amount of events simultaneously availible to all queues)
``` C
    #define STATIC
    #define NUM_QUEUES 3
    #define NUM_EVENTS 40
````
## Usage
First, start out by initializing an event queue with the init function with:
``` C
    event_queue* evq_init(void);
```
The return value of type `event_queue*` is the way we later refer to the queue. 

Next, we'll add some events. An evq event can have several parts. Looking at the prototype for the add function we see:
 ``` C   
    event_id evq_add(event_queue* evq, branch_func f, error_func err_f, void* data);
```
An event consists of 

 - the queue that it is part of (`evq`, *required*)
 - the primary function that is executed then the event comes up in the rotation (`f`, *required*)
 - the error function if `f` returns false (`err_f`, *optional*)
 - a pointer to some arbitrary data that the event might need (`data`, *optional*)

It returns an `event_id` that can be used to later delete the event with: 
``` C
    void evq_remove(event_queue* evq, event_id id);
```
Once we have some events in the queue we can start executing them with:
``` C
    void evq_next(event_queue* evq);
```
Just specify an event queue and the next event in the queue will start, automatically branching to the error function if necessary.  