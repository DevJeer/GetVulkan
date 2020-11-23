#version 420
layout(location=0)in vec3 V_WorldPos;
layout(location=1)in vec3 V_Normal;
layout(binding=2)uniform AliceBuiltinFragmentVectors{
	vec4 LightPosition;
	vec4 LightColor;
	vec4 CameraPosition;
}U_DefaultFragmentVectors;
layout(push_constant)uniform AliceBuiltinConstants{
	vec4 Params[8];
}U_Constants;
layout(binding=4)uniform samplerCube U_Texture0;
layout(location=0)out vec4 OutColor0;
void main(){
	vec3 n=normalize(V_Normal);
	vec3 eye_vec=normalize(V_WorldPos.xyz-U_DefaultFragmentVectors.CameraPosition.xyz);
	vec3 r=reflect(eye_vec,n);
	OutColor0=texture(U_Texture0,r);
}