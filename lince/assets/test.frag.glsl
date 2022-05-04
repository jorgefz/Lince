#version 330

out vec4 color;
in vec4 vColor;

uniform vec4 add_color = vec4(0.0);

void main(){
	color = vColor + add_color;
}

