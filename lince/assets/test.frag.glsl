#version 450

layout(location = 0) out vec4 color;

in vec4 vColor;
in vec2 vTexCoord;
in float vTextureID;

uniform sampler2D uTextureSlots[32];

void main(){
	color = texture(uTextureSlots[int(vTextureID)], vTexCoord) * vColor;
	if (color.a == 0.0) discard; // temporary solution for full transparency, not translucency.
}

