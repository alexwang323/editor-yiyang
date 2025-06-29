#version 400

out vec4 FragColor;

//in vec4 i_position;

void main() {
	gl_FragDepth = gl_FragCoord.z;
	FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

	//gl_FragData[0] = i_position;
}