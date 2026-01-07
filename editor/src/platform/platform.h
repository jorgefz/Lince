#ifndef LINCE_EDITOR_PLATFORM_H
#define LINCE_EDITOR_PLATFORM_H

#include <lince.h>

string_t LinceEditorOpenLoadFileDialog(void);

string_t LinceEditorOpenSaveFileDialog(void);

void LinceEditorShowMessageBox(char* title, char* content);

int LinceEditorShowMessageBoxYesNo(char* title, char* content);

#endif /* LINCE_EDITOR_PLATFORM_H */