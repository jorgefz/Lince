#version 330

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec4 vColor;
out vec2 vTexCoord;

uniform vec2 xyoffset = vec2(0.0, 0.0);

void main(){
   gl_Position = vec4(aPos, 1.0, 1.0) + vec4(xyoffset, 0.0, 0.0);
   vColor = aColor;
   vTexCoord = aTexCoord;
}

