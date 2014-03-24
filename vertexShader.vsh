#version 130

in vec4 s_vPosition;
in vec4 s_vNormal;	

uniform mat4 mM;	
uniform mat4 mV;	
uniform mat4 mP;	

uniform mat4 mRotations;

uniform vec4 vLight;

out vec3 fN;
out vec3 fL;

out vec3 fE;

void main () {
	
	fN = (mRotations*s_vNormal).xyz;	
	fL = vLight.xyz;					
	
	fE=(mV*mM*s_vPosition).xyz;
	
	gl_Position = mP*mV*mM*s_vPosition;

}
