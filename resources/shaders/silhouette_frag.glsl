#version 330 core
/* ----- Uniforms ----- */

/* ----- In/Out ----- */
in vertexInfo {
	vec4 position;
	vec3 normal;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	// Barycentric interpolation can make the normal vector length less than 1
	vec3 nVec = normalize(normal);
	
	// Eye vector (eye location = origin)
	vec3 eVec = normalize(vec3(0, 0, 0) - position.xyz);

	// Let edges be black (< threshold) and all else be white
	float threshold = 0.3;
	//if (max(0, abs(dot(nVec, eVec))) < threshold) {
	if (dot(nVec, eVec) < threshold) {
		fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	} 
	else {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
}