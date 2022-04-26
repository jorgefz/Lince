
#include "event/event.h"

LinceBool LinceDispatchEvent(LinceEvent* e, LinceEventType etype, LinceEventFn func){
    if (e->type == etype){
        e->handled = func(e);
        return LinceTrue;
    }
    return LinceFalse;
}

void LinceEndEvent(LinceEvent* e){
    if(!e || !e->data.GenericEvent) return;
    free(e->data.GenericEvent);
}





