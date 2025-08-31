#include "platform/platform.h"

#include <stdlib.h>
#include <windows.h>
#include <ShObjIdl_core.h>

/*
 * Opens the OS dialog menu to select a file,
 * and returns the full path to the file.
 * The length of the path in wide-chars is written to the input `fpath_len`.
 * If it returns NULL, no file was chosen.
 */
wchar_t* LinceEditorOpenLoadFileDialog(size_t *fpath_len){
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(!SUCCEEDED(hr)){
        return NULL;
    }

    IFileOpenDialog *pfd;
    hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog, (LPVOID*)(&pfd));
    if(!SUCCEEDED(hr)){
        CoUninitialize();
        return NULL;
    }

    hr = pfd->lpVtbl->Show(pfd, NULL);
    if(!SUCCEEDED(hr)){
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return NULL;
    }

    IShellItem *pitem;
    hr = pfd->lpVtbl->GetResult(pfd, &pitem);
    if(!SUCCEEDED(hr)){
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return NULL;
    }

    wchar_t* result = NULL;

    PWSTR psz_fpath;
    hr = pitem->lpVtbl->GetDisplayName(pitem, SIGDN_FILESYSPATH, &psz_fpath);
    if(!SUCCEEDED(hr)){
        pitem->lpVtbl->Release(pitem);
        pfd->lpVtbl->Release(pfd);
        CoUninitialize();
        return NULL;
    }
    
    // Copy filepath to output wchar buffer
    size_t len   = wcslen(psz_fpath);
    size_t sz    = sizeof(wchar_t) * len;
    size_t bufsz = sz + sizeof(wchar_t);
    result = malloc(bufsz);
    memcpy(result, psz_fpath, sz);
    result[sz] = L'\0';

    if (fpath_len) *fpath_len = bufsz;

    // MessageBoxW(NULL, psz_fpath, L"File Path", MB_OK);
    CoTaskMemFree(psz_fpath);

    pitem->lpVtbl->Release(pitem);
    pfd->lpVtbl->Release(pfd);
    CoUninitialize();
    return result;
}

wchar_t* LinceEditorOpenSaveFileDialog(void){
    return NULL;
}

int LinceEditorShowMessageBoxYesNo(char* title, char* content){
    int retval = MessageBox(NULL, content, title, MB_YESNO);
    if (retval == IDYES){
        return 1;
    }
    return 0;
}

int LinceEditorShowMessageBoxWYesNo(wchar_t* title, wchar_t* content){
    int retval = MessageBoxW(NULL, content, title, MB_YESNO);
    if (retval == IDYES){
        return 1;
    }
    return 0;
}