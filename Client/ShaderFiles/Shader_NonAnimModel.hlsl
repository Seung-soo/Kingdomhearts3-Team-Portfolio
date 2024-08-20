#include "Shader_Defines.hpp"


texture2D		g_DiffuseTexture;
texture2D		g_SourTexture;

texture2D		g_WindowTexture;
texture2D		g_NormalTexture;

texture2D		g_ThirdTexture;



cbuffer SPRITE
{
	float		g_fSpriteIndex;
	int			g_iSpriteYIndex = 0;
	int			g_iSpriteMaxXIndex;
	int			g_iSpriteMaxYIndex;
};

cbuffer COLOR_DESC
{
	bool		g_iBlur = 0;
	int			g_iRGBNum = 0;

	float		g_fAlpha;
	float4		g_vColor;
	float4		g_vBlendColor;

	float		g_fTransparency = 0.f;

	// ¿Ö°î ¸ÔÀÏ ¾ÖµéÀº 1.f¸¦ º¸³¿
	bool		g_bDistortion;

	// ÅØ½ºÃÄ ¹Í½ºÇÒ¶§ »ö±ò ¾î¶»°Ô ¼¯À»°ÇÁö
	bool		g_bAddColor = false;
	bool		g_bMulColor = false;
	bool		g_bOneValue = false;

	float		g_fDissolveAlpha = 0.f;
}

cbuffer LightMatrix
{
    float4x4 g_LightViewMatrix;
    float4x4 g_LightProjMatrix;
};

cbuffer RimLight
{
    float		g_fRimPower;
    float		g_fRimRange;
    float4		g_vRimColor;
    float4		g_vWorldCamPos;

    bool		g_isRim = false;
    bool		g_isBlur = false;
};

cbuffer Move_UV
{
	float		g_fMoveUV_X;
	float		g_fMoveUV_Y;
	float		g_fMoveNoise_X;
	float		g_fMoveNoise_Y;

	float		g_fNoisePower = 0.25f;

	bool		g_isPlayOnce = false;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vViewDir : TEXCOORD3;
    float3 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct VS_IN_SHADOW
{
    float3 vPosition : POSITION;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vClipPos : POSITION;
};

struct VS_OUT_SKY
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

    Out.vViewDir = float3(normalize(g_vWorldCamPos - Out.vWorldPos).xyz);
	
	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN_SHADOW In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_LightViewMatrix);
    matWVP = mul(matWV, g_LightProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vClipPos = Out.vPosition;
	
    return Out;
}

VS_OUT VS_MAIN_SPRITE(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	In.vTexUV.x = In.vTexUV.x / (float)g_iSpriteMaxXIndex + (float)g_fSpriteIndex / (float)g_iSpriteMaxXIndex;
	In.vTexUV.y = In.vTexUV.y / (float)g_iSpriteMaxYIndex + (float)g_iSpriteYIndex / (float)g_iSpriteMaxYIndex;

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_MOVEUV(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x += g_fMoveUV_X;
	In.vTexUV.y += g_fMoveUV_Y;

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_SKY VS_MAIN_SKY(VS_IN In)
{
	VS_OUT_SKY		Out = (VS_OUT_SKY)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x += g_fMoveUV_X * 0.5f;
	In.vTexUV.y += g_fMoveUV_Y * 0.5f;

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vViewDir : TEXCOORD3;
    float3 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vMask : SV_TARGET3;
	vector		vEmissive :	SV_TARGET4;
	vector		vRimLight :	SV_TARGET5;
	vector		vVelocity :	SV_TARGET6;
};

struct PS_OUT_NOLIGHT
{
	vector		vDiffuse :  SV_TARGET0;
	vector		vDepth   :  SV_TARGET1;
	vector		vBlur    :  SV_TARGET2;
};

struct PS_FORWARD_OUT
{
	vector		vColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
	vector		vMask : SV_TARGET2;
};

struct PS_IN_SKY
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT_SKY
{
	vector		vDiffuse :  SV_TARGET0;
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vClipPos : POSITION;
};

struct PS_OUT_SHADOW
{
    vector vShadowDepth_Static : SV_TARGET0;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);


	Out.vDiffuse.a += g_fTransparency;
	

	Out.vMask = 0.f;

	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
    Out.vVelocity = (vector) 0.f;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 1.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_WAVE(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vBlur = 0.f;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_WireFrame(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_AURA(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = vector(0.7f, 0.7f, 1.f, 0.3);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOTLIGHT(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vMtrlDiffuse;
	Out.vSecondColor = vMtrlDiffuse;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_SWORDTRAIL(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;


	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_NOISE(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * g_fNoisePower);

	float		fValue = 0;
	vector		vColor;

	if (true == g_bOneValue)
	{
		if (g_iRGBNum == 0)
			fValue = vNoiseColor.r;

		else if (g_iRGBNum == 1)
			fValue = vNoiseColor.g;

		else if (g_iRGBNum == 2)
			fValue = vNoiseColor.b;

		else if (g_iRGBNum == 3)
			fValue = vNoiseColor.a;

		vColor += fValue * g_vColor + (1 - fValue) * g_vBlendColor;
	}

	else if (true == g_bAddColor)
		vDiffuse += g_vColor;

	else if (true == g_bMulColor)
		vDiffuse *= g_vColor;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a += g_fTransparency;
	Out.vBlur = vDiffuse * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_ONLYNOISE(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_fMoveUV_X));

	Out.vColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vColor += vector(0.1f, 0.1f, 0.3f, 0.f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT PS_MAIN_ALPHADOWN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vDiffuse.a = g_fMoveUV_X;
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveUV_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_TRAIL_Y(PS_IN In)
{
	if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
		discard;

	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;
	if (Out.vDiffuse.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X_REAL(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X_TRAIL(PS_IN In)
{
	if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
		discard;

	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vector	vColor = vector(0.f, 0.f, 0.f, 0.f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.1f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f/1.5f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_ALPHADOWN_DEFFERED(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vBlur = Out.vDiffuse;
	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE_CUT(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
		discard;
	if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
		discard;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a < 0.01f)
		discard;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE_CUT_ALPHADOWN(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	//if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
	//	discard;
	//if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
	//	discard;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a < 0.01f)
		discard;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vColor.a -= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT PS_MAIN_GRAY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vDiffuse.gb = vDiffuse.r;
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_EFFECT_LERPALPHA(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	int iTexV = In.vTexUV.y;
	float fTexUV_Y = In.vTexUV.y - iTexV;
	Out.vColor.a *= fTexUV_Y;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_ZEROALPHA_DISTORTION(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	Out.vDiffuse = g_vColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = (vector)0.f;

	return Out;
}

PS_OUT PS_MAIN_MAPTOOLPICKING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
    if (vDiffuse.a >= 0.1f)
        vDiffuse = vector(In.vTexUV.x, In.vTexUV.y, 1.f, 1.f);
	
    Out.vDiffuse = vDiffuse;
    Out.vNormal  = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth   = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    Out.vMask    = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
	
	if (Out.vDiffuse.a == 0.f)
		discard;

    return Out;
}

PS_OUT PS_MAIN_MAPMASKZERO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vEmissive = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDiffuse = vDiffuse;
    Out.vDepth   = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    Out.vMask    = g_isBlur * vEmissive;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
	
	if (Out.vDiffuse.a == 0.f)
		discard;

    return Out;
}

PS_OUT_NOLIGHT PS_MAIN_NOLIGHTBLUR(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = vDiffuse;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_NOLIGHTBLUR_COLOR(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	Out.vDiffuse = g_vColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = g_vColor * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_MULCOLOR_H(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse * g_vColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_ADDCOLOR(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse + g_vColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_BLENDCOLOR(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}
	
	float fValue = 0;
	
	if(g_iRGBNum == 0)
		fValue = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).r;

	else if(g_iRGBNum == 1)
		fValue = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).g;

	else if(g_iRGBNum == 2)
		fValue = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).b;

	else if (g_iRGBNum == 3)
		fValue = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).a;

	Out.vDiffuse = fValue * g_vColor + (1 - fValue) * g_vBlendColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_ADDTEXTURE(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vSource = g_SourTexture.Sample(DefaultSampler, In.vTexUV);
	
	float fValue = 0;

	if (true == g_bOneValue)
	{
		if (g_iRGBNum == 0)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).r;

		else if (g_iRGBNum == 1)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).g;

		else if (g_iRGBNum == 2)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).b;

		else if (g_iRGBNum == 3)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).a;

		vSource = fValue * g_vColor + (1 - fValue) * g_vBlendColor;
	}

	else if (true == g_bAddColor)
		vSource += g_vColor;

	else if (true == g_bMulColor)
		vSource *= g_vColor;

	Out.vDiffuse = vDiffuse + vSource;

	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;

	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_NONORMAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_MULTEXTURE(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vSource = g_SourTexture.Sample(DefaultSampler, In.vTexUV);

	float fValue = 0;

	if (true == g_bOneValue)
	{
		if (g_iRGBNum == 0)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).r;

		else if (g_iRGBNum == 1)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).g;

		else if (g_iRGBNum == 2)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).b;

		else if (g_iRGBNum == 3)
			fValue = g_SourTexture.Sample(DefaultSampler, In.vTexUV).a;

		vSource = fValue * g_vColor + (1 - fValue) * g_vBlendColor;
	}

	else if (true == g_bAddColor)
		vSource += g_vColor;

	else if (true == g_bMulColor)
		vSource *= g_vColor;

	Out.vDiffuse = vDiffuse * vSource;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);
	Out.vBlur = Out.vDiffuse * g_iBlur;
	
	Out.vDiffuse.a += g_fTransparency;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_MOVINGUV(PS_IN In)
{
	PS_OUT_NOLIGHT		Out = (PS_OUT_NOLIGHT)0;

	if (g_isPlayOnce)
	{
		if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
			discard;
		if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
			discard;
	}

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_bDistortion, 0.f);

	Out.vDiffuse.a += g_fTransparency;

	Out.vBlur = vDiffuse * g_iBlur;

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector vEmissive = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    float fRim = saturate(dot(In.vNormal, In.vViewDir));
    vector vRimLight = pow(1.f - fRim, g_fRimPower) * g_vRimColor;

    if (fRim > g_fRimRange)
        fRim = 1.f;
    else
        fRim = -1.f;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    Out.vMask = (vDiffuse * g_isBlur) + (vRimLight * g_isRim);

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse    = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector vDissolve   = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
    float fDissolveRed = vDissolve.r;

    fDissolveRed += g_fDissolveAlpha;
	
    if (vDiffuse.r < fDissolveRed)
        discard;

    Out.vDiffuse = vDiffuse;

    if (vDiffuse.r < fDissolveRed + 0.05f)
        Out.vDiffuse = vector(0.f, 0.f, 1.f, 1.f);
    if (vDiffuse.r < fDissolveRed + 0.03f)
        Out.vDiffuse = vector(0.f, 1.f, 1.f, 1.f);
    if (vDiffuse.r < fDissolveRed + 0.02f)
        Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);


	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = vDiffuse * g_isBlur;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

    if (Out.vDiffuse.a < 0.1f)
        discard;

    return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
	
    Out.vShadowDepth_Static = In.vClipPos.z;
	
    return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW_DYNAMIC(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth_Static = In.vClipPos.z / In.vClipPos.w;

	return Out;
}

PS_OUT PS_MAIN_DEFAULT_BLUR_VARI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    Out.vMask = vDiffuse * g_iBlur;

    Out.vEmissive = 0.f;
    Out.vRimLight = 0.f;

    if (Out.vDiffuse.a < 0.1f)
        discard;

    return Out;
}

PS_OUT_SKY PS_MAIN_SKY(PS_IN_SKY In)
{
	PS_OUT_SKY		Out = (PS_OUT_SKY)0;

	vector vNoise = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoise.r * 0.25f);

	return Out;
}

technique11		DefaultTechnique
{
	pass Mesh_NonBlend //0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Mesh_Blend //1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Mesh_WireFrame //2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		GeometryShader = NULL;
		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_5_0 PS_MAIN_WireFrame();
	}

	pass Mesh_Aura //3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AURA();
	}

	pass Mesh_Wave //4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WAVE();
	}

	pass NotLight //5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOTLIGHT();
	}
	pass SwordEffect //6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SWORDTRAIL();
	}
	pass MoveUV //7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SWORDTRAIL();
	}
	pass Noise //8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE();
	}
	pass OnlyNoise //9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ONLYNOISE();
	}
	pass AlphaDown //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN();
	}
	pass Noise_X //11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X();
	}
	pass Trail_Y //12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRAIL_Y();
	}
	pass Effect //13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}
	pass OnlyMoveUV //14
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}
	pass MoveUV_AND_NOISE //15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X_REAL();
	}
	pass MoveUV_AND_NOISE_TRAIL //16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X_TRAIL();
	}
	pass Distortion //17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}
	pass MoveDistortion //18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}
	pass MoveDistortionTexture //19
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE();
	}
	pass DownAlpha //20
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN_DEFFERED();
	}
	pass MoveDistortionTextureCut //21
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT();
	}
	pass MoveDistortionTextureCut_ALPHADOWN //22
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT_ALPHADOWN();
	}
	pass MoveDistortionTextureCut_CullNone //23
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT();
	}
	pass Effect_ccw //24
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass Gray //25
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GRAY();
	}

	pass OnlyMoveUV_LERPALPHA //26
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_LERPALPHA();
	}

	pass ZeroAlpha_Distortion //27
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ZEROALPHA_DISTORTION();
	}
	pass Blur //28
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass Test//29
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

    pass MapToolPicking //30
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MAPTOOLPICKING();
    }

    pass MapMaskZero //31
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader   = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader    = compile ps_5_0 PS_MAIN_MAPMASKZERO();
    }

	pass NoLight_Blur//32
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOLIGHTBLUR();
	}

	pass NoLight_Blur_Color //33
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOLIGHTBLUR_COLOR();
	}

	pass MulColor_H //34
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MULCOLOR_H();
	}

	pass AddColor //35
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADDCOLOR();
	}

	pass BlendColor //36
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDCOLOR();
	}

	pass AddTexture//37
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADDTEXTURE();
	}

	pass MulTexture//38
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MULTEXTURE();
	}	
	
	pass MovingUV//39
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOVINGUV();
	}

	pass NoNormalMap//40
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NONORMAL();
	}

    pass RimLight //41
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
    }

	pass Dissolve //42
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

    pass Shadow // 43
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass  Default_BlurVariable// 44
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT_BLUR_VARI();
    }

	pass Shadow_Dynamic // 45
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW_DYNAMIC();
	}

	pass Sky // 46
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_SKY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKY();
	}

	pass MapMaskZeroCullnone //47
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPMASKZERO();
	}
}