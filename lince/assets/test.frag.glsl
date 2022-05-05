#version 330

layout(location = 0) out vec4 color;

in vec4 vColor;
in vec2 vTexCoord;

uniform vec4 add_color = vec4(0.0);
uniform sampler2D textureID;

void main(){
	// color = vColor + add_color;
	color = texture(textureID, vTexCoord) * (vColor + add_color);
}

