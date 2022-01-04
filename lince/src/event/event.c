
#include "event/event.h"

LinceBool LinceEvent_Dispatch(LinceEvent* e, LinceEventType etype, LinceEventFn func){
    if (e->type == etype){
        e->handled = func(e);
        return LinceTrue;
    }
    return LinceFalse;
}

void LinceEvent_Destroy(LinceEvent* e){
    if(!e || !e->data.GenericEvent) return;
    free(e->data.GenericEvent);
}





