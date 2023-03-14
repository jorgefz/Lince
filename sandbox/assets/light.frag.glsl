#version 450 core

layout(location = 0) out vec4 OutColor;

in vec4 vColor;
in vec2 vTexCoord;
in float vTextureID;

uniform sampler2D uTextureSlots[32];
uniform vec2 uWindowSize = vec2(500,500); // viewport dimensions in pixels

struct PointLight {
    vec2 position;
    float ambient;
    float diffuse;
    float size;
    float plaw;
    vec4 ambient_color;
    vec4 diffuse_color;
};

#define MAX_POINT_LIGHTS 32
// uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform vec2 uPointLightPositions[MAX_POINT_LIGHTS];
uniform float uPointLightCount = 0;
uniform vec4 uAmbientLight = vec4(0.2, 0.2, 0.2, 1.0);

vec4 pointlight_brightness(vec2 light_pos){
    // Settings and parameters
    float diffuse = 0.7; // added light reflected from sources (0,1)
    float cutoff = 0.2; // visual (max) distance
    float plaw = 2.5; // power law index for light spread
    vec4 diffuse_color = vec4(1.0); // color of diffuse light

    // gl_FragCoord goes from lowerleft (0.5,0.5) to upper right (W-0.5, H-0.5)
    float aspect_ratio = uWindowSize.x / uWindowSize.y;
    vec2 fragment_pos = (gl_FragCoord.xy-0.5) / uWindowSize.xy;
    vec2 dr = fragment_pos - light_pos;
    dr.x *= aspect_ratio;
    float fdiff = min(length(dr), cutoff)/cutoff;
    fdiff = pow(fdiff,plaw);
    vec4 bright = (1.0-fdiff)*diffuse*diffuse_color;
    return bright;
}


void main(){

    vec4 bright = uAmbientLight;
    for(int i = 0; i != int(uPointLightCount); ++i){
        bright += pointlight_brightness(uPointLightPositions[i]);
    }
	
    OutColor = texture(uTextureSlots[int(vTextureID)], vTexCoord) * vColor * bright;
	if (OutColor.a == 0.0){
        discard;
    }
	// temporary solution for full transparency, not translucency.
}; 