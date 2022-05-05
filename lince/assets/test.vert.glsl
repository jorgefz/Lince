#version 330

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 u_view_proj = mat4(1.0);
uniform mat4 u_transform = mat4(1.0); // identity matrix

void main(){
   gl_Position = u_view_proj * u_transform * vec4(aPos, 1.0, 1.0);
   vColor = aColor;
   vTexCoord = aTexCoord;
}

