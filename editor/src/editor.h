#ifndef EDITOR_H
#define EDITOR_H

#include <lince.h>

void EditorInit();
void EditorOnUpdate(float dt);
void EditorOnEvent(LinceEvent* e);
void EditorTerminate();

#endif /* EDITOR_H */