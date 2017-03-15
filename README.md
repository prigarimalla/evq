# evq
evq (short for **ev**ent **q**ueue) is an event queue intended for microcontroller applications. Several boolean functions can be added to the queue and repeatedly executed through just one function call, potentially branching to an error handler depending on the result before going to the next function in the queue. It supports both dynamic memory allocation or completely static allocation if desired. See `demo.c` for a fully commented example.

## Configuration
In `evq.h` uncomment the following lines to use static memory allocation. Set `EVQ_QUEUES` and `EVQ_EVENTS` as necessary.
(Keep in mind `EVQ_EVENTS` is the amount of events simultaneously available to all queues)
``` C
    #define EVQ_STATIC
    #define EVQ_QUEUES 3
    #define EVQ_EVENTS 40
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

See the Building Events section for more explanation.

It returns an `event_id` that can be saved and used to later delete the event with: 
``` C
    void evq_remove(event_queue* evq, event_id id);
```
Once we have some events in the queue we can start executing them one-by-one with:
``` C
    void evq_next(event_queue* evq);
```
Just specify an event queue and the next event in the queue will start, automatically branching to the error function if necessary.  

If we want to execute until we get to the front of the queue, we have:
``` C
    void evq_front(event_queue* evq);
```
Keep in mind, the intention of this function is only to get to the front of the queue. If we are already at the front of the queue, nothing will execute. 

If we want to guarantee execution of every event in the queue, potentially executing some of them twice, and then return to the front of the queue, we can instead use:
``` C
    void evq_all(event_queue* evq);
```

If we want to execute every function in the queue and return to the same position, we can use:
``` C
    void evq_round(event_queue* evq);
```

To simply jump to the front of the queue **without** executing any events, use:
``` C
    void evq_reset(event_queue* evq);
```

When done with the queue, remove all the events and free the queue itself with:
``` C
    void evq_done(event_queue* evq);
```

### Building Events
An evq event has three parts, one required and two optional. The format for the required "branch" function is defined as:
``` C
    typedef bool (*branch_func)(event_queue*, branch_event*);
```
In simpler terms, it must just be a function that takes an `event_queue*` and a `branch_event*` and returns a `bool`. The reason for taking these parameters is for the function to be aware aware of the context it is being called in to enable functionality such as removing itself from the queue or adding another function to the queue should some condition be met, or looking up some event specific information in `branch_event->data`. If no error function is defined, the return value of the function is ignored. Otherwise, when the function returns `false` the error function is invoked and on `true` execution advances to the next event.

The optional error function is formally defined as: 
``` C
    typedef void (*error_func)(event_queue*, branch_event*);
```
This is just a function that takes an `event_queue*` and a `branch_event*` and returns nothing. The parameters are intended to serve the same purpose as before, giving the function some context of the whole queue, but this time no returned data is necessary since execution will always advance to the next event. 

The optional `data` pointer is used to pass in any data that should be accessible to every invocation of either function (it is accessible through the `branch_event*` parameter). It is stored as type `void*` so the functions should cast it accordingly.