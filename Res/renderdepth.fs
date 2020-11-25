#version 420
layout(location=0)in vec4 V_Color;
layout(location=1)in vec4 V_Texcoord;
layout(push_constant)uniform AliceBuiltinConstants{
	vec4 Params[8];
}U_Constants;
layout(binding=4)uniform sampler2D U_Texture0;
layout(location=0)out vec4 OutColor0;
void main(){
	float depth=texture(U_Texture0,vec2(V_Texcoord.x,1.0-V_Texcoord.y)).r;
	float display_depth=pow(depth,32.0);
	OutColor0=vec4(display_depth,display_depth,display_depth,1.0);
}