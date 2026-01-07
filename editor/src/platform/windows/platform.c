#include "platform/platform.h"

#include <stdlib.h>
#include <lince.h>
#include <windows.h>
#include <ShObjIdl_core.h>

/*
 * Opens the OS dialog menu to select a file,
 * and returns the full path to the file.
 * The length of the path in wide-chars is written to the input `fpath_len`.
 * If it returns NULL, no file was chosen.
 */
string_t LinceEditorOpenLoadFileDialog(){
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(!SUCCEEDED(hr)){
        return (string_t){0};
    }

    IFileOpenDialog *pfd;
    hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog, (LPVOID*)(&pfd));
    if(!SUCCEEDED(hr)){
        CoUninitialize();
        return (string_t){0};
    }

    hr = pfd->lpVtbl->Show(pfd, NULL);
    if(!SUCCEEDED(hr)){
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return (string_t){0};
    }

    IShellItem *pitem;
    hr = pfd->lpVtbl->GetResult(pfd, &pitem);
    if(!SUCCEEDED(hr)){
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return (string_t){0};
    }

    PWSTR psz_fpath;
    hr = pitem->lpVtbl->GetDisplayName(pitem, SIGDN_FILESYSPATH, &psz_fpath);
    if(!SUCCEEDED(hr)){
        pitem->lpVtbl->Release(pitem);
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return (string_t){0};
    }
    
    // Copy filepath to output wchar buffer
    // size_t len   = wcslen(psz_fpath);
    // size_t sz    = sizeof(wchar_t) * len;
    // size_t bufsz = sz + sizeof(wchar_t);
    // result = malloc(bufsz);
    // memcpy(result, psz_fpath, sz);
    // result[sz] = L'\0';

    string_t result = string_from_fmt("%S", psz_fpath);
    CoTaskMemFree(psz_fpath);

    pitem->lpVtbl->Release(pitem);
    pfd->lpVtbl->Release(pfd);
    CoUninitialize();
    return result;
}

string_t LinceEditorOpenSaveFileDialog(void){
    return (string_t){0};
}

void LinceEditorShowMessageBox(char* title, char* content){
    MessageBoxA(NULL, content, title, MB_OK);
}

int LinceEditorShowMessageBoxYesNo(char* title, char* content){
    int retval = MessageBoxA(NULL, content, title, MB_YESNO);
    if (retval == IDYES){
        return 1;
    }
    return 0;
}

