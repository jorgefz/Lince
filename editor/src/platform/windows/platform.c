#include "platform/platform.h"

#include <lince.h>
#include <windows.h>
#include <ShObjIdl_core.h>

/*
 * 
 */
string_t LinceEditorOpenLoadFileDialog(void){
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
    if(SUCCEEDED(hr)){
        MessageBoxW(NULL, psz_fpath, L"File Path", MB_OK);
        CoTaskMemFree(psz_fpath);
    }
    pitem->lpVtbl->Release(pitem);

    pfd->lpVtbl->Release(pfd);
    CoUninitialize();
    return (string_t){0};
}

string_t LinceEditorOpenSaveFileDialog(void){
    
}