#include "Shader_Defines.hpp"

struct VS_IN
{
	float3		vPosition : POSITION;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	return Out;
}

//struct PS_IN
//{
//	float4		vPosition : SV_POSITION;
//};
//
//struct PS_OUT
//{
//	vector		vColor : SV_TARGET0;
//};
//
//PS_OUT PS_MAIN_DEFAULT(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	Out.vColor = (vector)1.f
//
//	return Out;
//}

technique11		DefaultTechnique
{
	pass Frustum
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
}