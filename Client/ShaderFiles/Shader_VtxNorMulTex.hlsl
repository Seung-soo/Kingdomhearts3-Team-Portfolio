#include "Shader_Defines.hpp"

cbuffer BrushDesc
{
	float4		g_vBrushPos = float4(10.f, 0.f, 10.f, 1.f);
	float		g_fRange = 3.f;
};

texture2D g_DiffuseTexture1;
texture2D g_DiffuseTexture2;
texture2D g_DiffuseTexture3;
texture2D g_DiffuseTexture4;

texture2D g_BrushTexture;

struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vSplatting	: TEXCOORD1;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	float4 vSplatting	: TEXCOORD3;
};

VS_OUT VS_MAIN_TERRAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vSplatting = In.vSplatting;

	return Out;
}

struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
	float4		vProjPos	: TEXCOORD2;
	float4		vSplatting	: TEXCOORD3;
};

struct PS_OUT
{
	vector		vDiffuse	:   SV_TARGET0;
	vector		vNormal		:   SV_TARGET1;
	vector		vDepth		:   SV_TARGET2;
	vector		vEffect		:   SV_TARGET3;
	vector		vEmissive	:	SV_TARGET4;
	vector		vRimLight   :	SV_TARGET5;
	vector		vVelocity   :	SV_TARGET6;
};

PS_OUT PS_MAIN_TERRAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	/*vector vDiffuse1 = pow(g_DiffuseTexture1.Sample(DefaultSampler, In.vTexUV * 3.f), 2.2f);
	vector vDiffuse2 = pow(g_DiffuseTexture2.Sample(DefaultSampler, In.vTexUV * 3.f), 2.2f);
	vector vDiffuse3 = pow(g_DiffuseTexture3.Sample(DefaultSampler, In.vTexUV * 3.f), 2.2f);
	vector vDiffuse4 = pow(g_DiffuseTexture4.Sample(DefaultSampler, In.vTexUV * 3.f), 2.2f);*/

	vector vDiffuse1 = g_DiffuseTexture1.Sample(DefaultSampler, In.vTexUV);
	vector vDiffuse2 = g_DiffuseTexture2.Sample(DefaultSampler, In.vTexUV);
	vector vDiffuse3 = g_DiffuseTexture3.Sample(DefaultSampler, In.vTexUV);
	vector vDiffuse4 = g_DiffuseTexture4.Sample(DefaultSampler, In.vTexUV);

	vector vBrushColor = (vector) 0.f;

	if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRange &&
		g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRange)
	{
		float2 vBrushUV;

		vBrushUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange);
		vBrushUV.y = ((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange);

		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vBrushUV);
	}

	vector vMtrlDiffuse = 
		vDiffuse1 * In.vSplatting.r +
		vDiffuse2 * In.vSplatting.g +
		vDiffuse3 * In.vSplatting.b +
		vDiffuse4 * In.vSplatting.a + vBrushColor;

	/*vMtrlDiffuse = pow(vMtrlDiffuse, 1.f / 2.2f);*/

	Out.vDiffuse = vMtrlDiffuse;
	//Out.vDiffuse.a = 1.f;

	// -1~1사이의 값을 0~1로 치환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vEffect = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
	Out.vVelocity = 0.f;

	return Out;
}

technique11	DefaultTechnique
{
	pass Terrain_Blend
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN();
	}

	pass Terrain_NonBlend
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN();
	}

	pass Terrain_WireFrame
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN();
	}
}