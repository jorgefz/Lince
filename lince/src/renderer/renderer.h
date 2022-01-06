#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

void LinceRender_Init();
void LinceRender_BeginScene();
void LinceRender_EndScene();
void LinceRender_Submit();

void LinceRender_Clear();
void LinceRender_SetClearColor(float r, float g, float b, float a);
void LinceRender_DrawIndexed();

#endif // LINCE_RENDERER_H