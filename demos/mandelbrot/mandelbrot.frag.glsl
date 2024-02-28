uniform vec2 screen; // screen size
uniform vec2 xlim; // x axis bounds
uniform vec2 ylim; // y axis bounds
uniform float max_iter = 100; // maximum number of iterations

layout(location = 0) out vec4 color;

float complex_magnitude(vec2 z){
	return sqrt(z.x*z.x + z.y*z.y);
}

vec2 quadratic(vec2 z){
	return vec2(z.x*z.x - z.y*z.y + 1, 2.0*z.x*z.y);
}

int recurse_quadratic(vec2 z, int max_iter){
	for(int i = 0; i != max_iter; ++i){
		z = quadratic(z);
		if(complex_magnitude(z) >= 2.0){
			return i;
		}
	}
	return 0;
}

void main(){
	
	float re = xlim.x + gl_FragCoord.x * (xlim.y - xlim.x) / screen.x;
	float im = ylim.x + gl_FragCoord.y * (ylim.y - ylim.x) / screen.y;

	int n_iter = recurse_quadratic(vec2(re,im), (int)max_iter);
	float frac = (float)n_iter/max_iter;
	color = vec4(frac, frac, frac, 1.0);
}
