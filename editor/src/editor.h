#ifndef EDITOR_H
#define EDITOR_H

#include <lince.h>
#include <nuklear.h>

void EditorInit();
void EditorOnUpdate(float dt);
void EditorOnEvent(LinceEvent* e);
void EditorTerminate();

#endif /* EDITOR_H */