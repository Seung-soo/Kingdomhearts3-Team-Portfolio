
#include "Shader_Defines.hpp"

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;

float				g_Translucency;
float				g_TimeAccum;

// ø÷∞Ó ∏‘¿œ æ÷µÈ¿∫ 1.f∏¶ ∫∏≥ø
float				g_fDistortion = 0.f;

// ≥Î¿Ã¡Ó »∏¿¸
float				g_fNoiseRot;

cbuffer ColorDesc
{
	float4		g_vBlendingColor;
	float4		g_vExtraColor;
	float		g_fTranslucency;

	bool		g_iBlur;
};

cbuffer SpriteDesc
{
	int			g_iSpriteMaxXIndex;
	int			g_iSpriteMaxYIndex;

	int			g_iSpriteIndex;
	int			g_iSpriteYIndex;
};

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN_SPRITE(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / (float)g_iSpriteMaxXIndex + (float)g_iSpriteIndex / (float)g_iSpriteMaxXIndex;
	In.vTexUV.y = In.vTexUV.y / (float)g_iSpriteMaxYIndex + (float)g_iSpriteYIndex / (float)g_iSpriteMaxYIndex;
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vDepth : SV_TARGET1;
    vector vBlur : SV_TARGET2;
};

struct PS_OUT_UI
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	Out.vColor.a = Out.vColor.a - g_fTranslucency;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_FADE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDepth = (vector)0.f;

	Out.vBlur = (vector)0.f;

	Out.vColor.a += g_Translucency;

	return Out;
}

PS_OUT PS_MAIN_NAME(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDepth = (vector)0.f;

	Out.vBlur = (vector)0.f;

	Out.vColor.a *= g_Translucency;

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor + (1 - vColor.r) * g_vBlendingColor;

	Out.vColor.a = vColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECTTEX(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor + (1 - vColor.r) * g_vBlendingColor;

	Out.vColor.a = vColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DYEING(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = vColor.a - g_fTranslucency;

	if (Out.vColor.a <= 0.01f)
		discard;
	Out.vColor.xyz = vColor.xyz  * g_vBlendingColor.xyz;


    float2 vUV = In.vProjPos.xy / In.vProjPos.w;

    vUV.x = vUV.x * 0.5f + 0.5f;
    vUV.y = vUV.y * -0.5f + 0.5f;

    vector vDepthDesc = g_SourTexture.Sample(DefaultSampler, vUV);

    float fViewZ = vDepthDesc.x * 300.f;

    Out.vColor.a = Out.vColor.a * saturate((fViewZ - In.vProjPos.w) * 2.f);

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;
	 

	return Out;
}

PS_OUT PS_MAIN_WAVE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r;
	Out.vDepth = (vector)0.f;

	Out.vBlur = (vector)0.f;

	Out.vColor.a = Out.vColor.a - g_fTranslucency;


	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DYEING2(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor  * g_vBlendingColor + (1 - vColor.r) * g_vExtraColor;

    Out.vColor.a = vColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

    if (Out.vColor.a < 0.1f)
        discard;


	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a <= 0.f)
		discard;

	vector vDissolve = g_SourTexture.Sample(DefaultSampler, In.vTexUV);
	float fDissolveRed = vDissolve.r;

	fDissolveRed += g_fTranslucency;

	if (vColor.r < fDissolveRed)
		discard;

	Out.vColor = vColor;

	if (vColor.r < fDissolveRed + 0.05f)
		Out.vColor = vector(0.f, 0.f, 1.f, 1.f);
	if (vColor.r < fDissolveRed + 0.03f)
		Out.vColor = vector(0.f, 1.f, 1.f, 1.f);
	if (vColor.r < fDissolveRed + 0.02f)
		Out.vColor = vector(1.f, 1.f, 1.f, 1.f);


	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vBlur = vColor * g_iBlur;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_REVERSEDIFFUSE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor + (1 - vColor.r)  * g_vBlendingColor;

	Out.vColor.a = (g_vBlendingColor.a) - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a < 0.1f || Out.vColor.r > 0.9f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLENDEDEFFECT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vSource = g_SourTexture.Sample(DefaultSampler, In.vTexUV);

	//Out.vColor = vColor * g_vBlendingColor + (1 - vColor.r) * g_vExtraColor;
	
	vector vBlendedColor = vColor * vSource;

	Out.vColor = vBlendedColor *  g_vBlendingColor + (1 - vBlendedColor.r) * g_vExtraColor;

	Out.vColor.a = vBlendedColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECTBUBBLE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//vector vBlendedColor = vColor * v;

	/*Out.vColor = vBlendedColor *  g_vBlendingColor + (1 - vBlendedColor.r) * g_vExtraColor;*/

	Out.vColor = vColor * g_vBlendingColor + (1 - vColor.r) * g_vExtraColor;
	Out.vColor.a = vColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);
	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECTFLAME(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float2 vNew = In.vTexUV;
	vNew.y += g_fTranslucency;
	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, vNew);
	vector vOriginal = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector vSource = g_SourTexture.Sample(DefaultSampler, vNew);
	//vNew.y += g_fTranslucency * 0.5f;
	vNew = In.vTexUV;
	vNew.x += g_fTranslucency;
	vector vSource = g_SourTexture.Sample(DefaultSampler, vNew);
	float fDis = sqrt(saturate(length(vNew - float2(0.5f, 0.5f)) / 0.5f));
	
	vector vBlendedColor = vSource ;
	Out.vColor = vBlendedColor *  g_vBlendingColor /*+ (1 - vBlendedColor.r) * g_vExtraColor*/;

	Out.vColor.a = vColor.a * vBlendedColor.r - 0.1f;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);
	//Out.vColor.a = vBlendedColor.r * vColor.a;
	//Out.vColor.a = vColor.a;
	//Out.vColor.a = vBlendedColor.a - g_fTranslucency;
	//Out.vColor.a = vBlendedColor.a + g_fTranslucency;

	Out.vBlur = Out.vColor * g_iBlur;



	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECTFLAMESMALL(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float2 vNew = In.vTexUV;
	vNew.y += g_fTranslucency;
	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//vNew = In.vTexUV;
	//vNew.x += g_fTranslucency;
	vector vSource = g_SourTexture.Sample(DefaultSampler, vNew);

	vector vBlendedColor = vSource * vColor;
	Out.vColor = vBlendedColor *  g_vBlendingColor;

	Out.vColor.a = vColor.a * vBlendedColor.r;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECTFOG(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float2 vNew = In.vTexUV;
	vNew.x += g_fTranslucency;
	float fDis = saturate(length(vNew - float2(0.5f, 0.5f)) / 0.5f);
	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, vNew);

	//Out.vColor = vColor *  g_vBlendingColor;

	////Out.vColor.a = fDis - g_fTranslucency;
	//Out.vColor.a = vColor.a - fDis;

	//vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor  * g_vBlendingColor + (1 - vColor.r) * g_vExtraColor;

	Out.vColor.a = vColor.a - g_fTranslucency;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = 0.f;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_RECT_COLOR(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = vector(1.f,0.f,0.f,0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);
	Out.vBlur = 0.f;

	return Out;
}

PS_OUT PS_MAIN_NOTUIRECT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);
	Out.vBlur = Out.vColor * g_iBlur;

	Out.vColor += g_fTranslucency;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_SPRITE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
    Out.vColor += g_vBlendingColor;
	
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);
	
	Out.vBlur = Out.vColor * g_iBlur;

	Out.vColor.a += g_fTranslucency;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_NOISE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;
	float2x2	RotMatrix = float2x2(cos(g_fNoiseRot), -sin(g_fNoiseRot), sin(g_fNoiseRot), cos(g_fNoiseRot));
	float2		vUV = mul(In.vTexUV - float2(0.5f, 0.5f), RotMatrix);
	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, vUV + float2(0.5f, 0.5f));
	vector		vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r);

	Out.vColor = vColor;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a == 0.f)
		discard;

	Out.vColor.a += 0.3f;

	return Out;
}

PS_OUT PS_MAIN_NOISE_Y(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_fNoiseRot));
	vector		vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);

	Out.vColor = vColor;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

	Out.vBlur = Out.vColor * g_iBlur;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DYINGADDMULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
    float2 vUV = In.vProjPos.xy / In.vProjPos.w;
    vector vDepthDesc = g_SourTexture.Sample(DefaultSampler, vUV);

    vUV.x = vUV.x * 0.5f + 0.5f;
    vUV.y = vUV.y * -0.5f + 0.5f;

    float fViewZ = vDepthDesc.x * 300.f;

    Out.vColor = (vColor + g_vBlendingColor) * g_vExtraColor;
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fDistortion, 1.f - In.vProjPos.w / 300.f);

    Out.vBlur = Out.vColor * g_iBlur;
	
    Out.vColor.a += g_fTranslucency;

    if (Out.vColor.a <= 0.f)
        discard;

    return Out;
}

technique11		DefaultTechnique
{
	pass Rect		// 0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Fade		// 1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FADE();
	}

	pass Name		// 2
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NAME();
	}

	pass Effect		// 3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass Effect_Texture		// 4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTTEX();
	}

	pass Effect_DYEING		// 5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DYEING();
	}

	pass Effect_ShockWave	//6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WAVE();
	}

	pass Effect_DYEING2		// 7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DYEING2();
	}

	pass Effect_DISSOLVE	// 8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass Effect_ReverseDiffuse	// 9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_REVERSEDIFFUSE();
	}

	pass Effect_BlendedEffect	// 10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDEDEFFECT();
	}

	pass Effect_Bubble			// 11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTBUBBLE();
	}

	pass Effect_Flame			// 12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTFLAME();
	}

	pass Effect_Flame_Small		// 13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTFLAMESMALL();
	}

	pass Effect_Fog				// 14
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		//SetRasterizerState(CullMode_ccw);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECTFOG();
	}

	pass Transparent					// 15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_COLOR();
	}

	pass NotUI_Rect		// 16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOTUIRECT();
	}

	pass Sprite		// 17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE();
	}
 
	pass Noise		// 18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE();
	}

	pass Effect_DYEING_CULLNONE		// 19
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DYEING();
	}

	pass Noise_Y		// 20
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_Y();
	}

    pass Dying_AddMult // 21
    {
        SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_none);
        VertexShader = compile vs_5_0 VS_MAIN_RECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DYINGADDMULT();
    }
};