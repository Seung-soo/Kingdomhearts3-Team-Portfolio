#include "Shader_Defines.hpp"

textureCUBE g_DiffuseTexture;

vector g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

    Out.vColor.xyz += g_vColor.xyz;

    return Out;
}

technique11		DefaultTechnique
{
	pass Sky // 0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_cw);

		VertexShader   = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader    = compile ps_5_0 PS_MAIN();
	}

	pass Cube // 1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader   = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader    = compile ps_5_0 PS_MAIN();
	}

    pass PickingPointPass // 2
    {
        SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader    = compile ps_5_0 PS_COLOR();
    }

	pass AddColor // 3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_cw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR();
	}
}