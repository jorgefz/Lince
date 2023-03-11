#version 450 core

layout(location = 0) out vec4 color;

in vec4 vColor;
in vec2 vTexCoord;
in float vTextureID;

uniform sampler2D uTextureSlots[32];
uniform vec2 uWindowSize = vec2(500,500); // viewport dimensions in pixels
uniform vec2 uLightSourcePos = vec2(0.5, 0.5); // light source in screen coords?

float light_brightness(){
    float vd = 0.2; // visual distance (max distance)
    // gl_FragCoord goes from lowerleft (0.5,0.5) to upper right (W-0.5, H-0.5)
    vec2 fragment_pos = (gl_FragCoord.xy + vec2(0.5,0.5))/(uWindowSize-vec2(1,1));;
    float diff = min( length(fragment_pos-uLightSourcePos) , vd ) / vd;
    diff = pow(diff,2.0);
    float bright = ((1.0-diff)*0.8 + 0.0);
    return bright;
}

void main(){
    float bright = light_brightness();
	color = texture(uTextureSlots[int(vTextureID)], vTexCoord) * vColor * bright;
	if (color.a == 0.0) discard;
	// temporary solution for full transparency, not translucency.
}; 