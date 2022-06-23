#include "renderer/renderer.h"

#include <glad/glad.h>

typedef struct LinceRenderer {

	struct LinceSceneData {
		void* data;
	} scene_data;

} LinceRenderer;

void LinceInitRenderer() {

	// Adds up alpha channels
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void LinceBeginScene() {

}

void LinceEndScene() {

}

void LinceSubmitQuad() {

}


void LinceDrawIndexed(
	LinceShader* shader, LinceVertexArray* va, LinceIndexBuffer ib
) {
	LinceBindShader(shader);
	LinceBindIndexBuffer(ib);
	LinceBindVertexArray(va);
	glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, 0);
}

void LinceClear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT);
}

void LinceSetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}
