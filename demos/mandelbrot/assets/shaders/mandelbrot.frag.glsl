#version 450 core

#define MAX_ITER 500

uniform vec2 uScreen = vec2(1920, 1080); // screen size in pixels
uniform vec2 uXlim   = vec2(-2,2);   // x axis bounds
uniform vec2 uYlim   = vec2(-2,2);   // y axis bounds

in vec4 vColor;
in vec2 vTexCoord;
in float vTextureID;

layout(location = 0) out vec4 frag_color;

float complex_magnitude(vec2 z){
	return sqrt(z.x*z.x + z.y*z.y);
}

// z^2 + c 
vec2 quadratic(vec2 z, vec2 c){
	return vec2(z.x*z.x - z.y*z.y + c.x, 2.0*z.x*z.y + c.y);
}

float recurse_quadratic(vec2 c, int max_iter){
	vec2 z = vec2(0.0, 0.0);
    for(int i = 0; i <= MAX_ITER; i++){
		z = quadratic(z, c);
		if(complex_magnitude(z) >= 2.0){
			return float(i);
		}
	}
	return 0.0f;
}


vec3 colormap(float f){
	f = sqrt(f);
	if(f > 0.75f) return vec3(f, f, 0.0f);
	if(f > 0.5f)  return vec3(f, 0.0f, 0.0f);
	if(f > 0.25f) return vec3(f, 0.0f, f);
	else          return vec3(0.0f, 0.0f, f);
}

void main(){
	
	float re = uXlim.x + gl_FragCoord.x * (uXlim.y - uXlim.x) / uScreen.x;
	float im = uYlim.x + gl_FragCoord.y * (uYlim.y - uYlim.x) / uScreen.y;

	float n_iter = recurse_quadratic(vec2(re,im), int(MAX_ITER));
	float frac = n_iter/float(MAX_ITER);
	frag_color = vec4(colormap(frac), 1.0);
}
