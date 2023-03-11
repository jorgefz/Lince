#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aTextureID;

uniform mat4 u_view_proj = mat4(1.0);
uniform float uRedFactor = 1.0;

out vec4 vColor;
out vec2 vTexCoord;
out float vTextureID;

void main(){
   gl_Position = u_view_proj * vec4(aPos, 1.0);
   vColor = vec4(uRedFactor, 1.0, 1.0, 1.0);
   vTexCoord = aTexCoord;
   vTextureID = aTextureID;
};