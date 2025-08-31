#ifndef LINCE_EDITOR_PLATFORM_H
#define LINCE_EDITOR_PLATFORM_H

#include <stdio.h>

wchar_t* LinceEditorOpenLoadFileDialog(size_t *fpath_len);

wchar_t* LinceEditorOpenSaveFileDialog(void);

int LinceEditorShowMessageBoxYesNo(char* title, char* content);

int LinceEditorShowMessageBoxWYesNo(wchar_t* title, wchar_t* content);

#endif /* LINCE_EDITOR_PLATFORM_H */