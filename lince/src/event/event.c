
#include "event/event.h"

unsigned int LinceEvent_Dispatch(Event* e, EventType etype, EventFn func){
    if (e->type == etype){
        e->handled = func(e);
        return 1;
    }
    return 0;
}

void LinceEvent_Destroy(Event* e){
    if(!e || !e->data.GenericEvent) return;
    free(e->data.GenericEvent);
}





