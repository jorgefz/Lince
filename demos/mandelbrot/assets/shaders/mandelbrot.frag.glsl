
/*
// SHADERTOY

#define MAX_ITER 10

in vec4 vColor;
in vec2 vTexCoord;
in float vTextureID;


float complex_magnitude(vec2 z){
	return sqrt(z.x*z.x + z.y*z.y);
}

vec2 quadratic(vec2 z, vec2 c){
    // z^2 + c 
	return vec2(z.x*z.x - z.y*z.y + c.x, 2.0*z.x*z.y + c.y);
}

int recurse_quadratic(vec2 c, int max_iter){
	vec2 z = vec2(0.0, 0.0);
    for(int i = 0; i <= MAX_ITER; i++){
		z = quadratic(z, c);
		if(complex_magnitude(z) >= 2.0){
			return i;
		}
	}
	return 0;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

    float re = 2.0 * gl_FragCoord.x/iResolution.x - 1.0;
	float im = 1.0 - 2.0 * gl_FragCoord.y/iResolution.y;

	int n_iter = recurse_quadratic(vec2(re,im), int(MAX_ITER));
	float frac = float(n_iter)/float(MAX_ITER);

    // Output to screen
    fragColor = vec4(frac, frac,frac, 1.0);
}
*/

#version 450 core

#define MAX_ITER 1000

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

int recurse_quadratic(vec2 c, int max_iter){
	vec2 z = vec2(0.0, 0.0);
    for(int i = 0; i <= MAX_ITER; i++){
		z = quadratic(z, c);
		if(complex_magnitude(z) >= 2.0){
			return i;
		}
	}
	return 0;
}

void main(){
	
	float re = uXlim.x + gl_FragCoord.x * (uXlim.y - uXlim.x) / uScreen.x;
	float im = uYlim.x + gl_FragCoord.y * (uYlim.y - uYlim.x) / uScreen.y;

	int n_iter = recurse_quadratic(vec2(re,im), int(MAX_ITER));
	float frac = float(n_iter)/float(MAX_ITER);
	frag_color = vec4(frac, frac, frac, 1.0);
}
