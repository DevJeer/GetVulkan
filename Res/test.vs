#version 420
#extension GL_KHR_vulkan_glsl : enable
layout(location=0)in vec4 position;
layout(location=1)in vec4 texcoord;
layout(location=0)out vec4 V_Color;
layout(binding = 0)uniform AliceBuiltinVertexVectors{
	vec4 Value[8];
} U_DefaultVertexVectors;
layout(binding = 1)uniform AliceBuiltinVertexMatrix{
	mat4 Value[8];
} U_DefaultVertexMatrices;
void main(){
	V_Color=texcoord*U_DefaultVertexVectors.Value[gl_VertexIndex];
	gl_Position=U_DefaultVertexMatrices.Value[0] * position;
}