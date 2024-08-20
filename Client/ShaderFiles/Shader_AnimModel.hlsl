
#include "Shader_Defines.hpp" 
#include "Shader_AnimModel_Defines.hpp"

struct BoneMatrixArray
{
	matrix BoneMatrices[280];
};

cbuffer Matrices
{
	BoneMatrixArray		g_BoneMatrices;
	matrix				g_OldWorldMatrix;
};

//texture2D			g_DiffuseTexture;
//texture2D			g_SourTexture;
//texture2D			g_NormalTexture;
//texture2D			g_ThirdTexture;
//texture2D			g_DissolveTexture;
//
//cbuffer NoiseAndMoveUV
//{
//	float				g_fMoveUV;
//	float				g_fMoveNoise;
//};
//
//cbuffer Dissolve
//{
//	float		g_fDissolveAlpha  = 0.f;
//    float4		g_vDissolveColor1 = float4(1.f, 0.f, 0.f, 1.f);
//    float4		g_vDissolveColor2 = float4(1.f, 1.f, 0.f, 1.f);
//    float4		g_vDissolveColor3 = float4(1.f, 1.f, 1.f, 1.f);
//};
//
//cbuffer RimLight
//{
//	float				g_fRimPower;
//	float				g_fRimRange;
//	float4				g_vRimColor = float4(1.f, 1.f, 1.f, 1.f);
//	float4				g_vWorldCamPos;
//
//	bool				g_isRim = false;
//	bool				g_isBlur = false;
//	float				g_fLUTMask = 0.f;
//};
//
//cbuffer LightMatrix
//{
//    float4x4			g_LightViewMatrix;
//    float4x4			g_LightProjMatrix;
//};
//
//struct VS_IN
//{
//	float3		vPosition : POSITION;
//	float3		vNormal : NORMAL;
//	float2		vTexUV : TEXCOORD0;
//	float3		vTangent : TANGENT;
//	uint4		vBlendIndex : BLENDINDEX;
//	float4		vBlendWeight : BLENDWEIGHT;
//};
//
//struct VS_IN_SHADOW
//{
//    float3 vPosition	: POSITION;
//    uint4  vBlendIndex	: BLENDINDEX;
//    float4 vBlendWeight : BLENDWEIGHT;
//};
//
//struct VS_OUT_SHADOW
//{
//    float4 vPosition	: SV_POSITION;
//    float4 vClipPos		: POSITION;
//};
//
//struct VS_OUT
//{
//	float4		vPosition : SV_POSITION;
//	float3		vNormal : NORMAL;
//	float2		vTexUV : TEXCOORD0;
//	float4		vWorldPos : TEXCOORD1;
//	float4		vProjPos : TEXCOORD2;
//	float3		vViewDir : TEXCOORD3;
//	float3		vTangent : TANGENT;
//	float4		vBinormal : BINORMAL;
//};
//
//struct VS_OUT_MOTION
//{
//	float4		vPosition : SV_POSITION;
//	float3		vNormal : NORMAL;
//	float2		vTexUV : TEXCOORD0;
//	float4		vWorldPos : TEXCOORD1;
//	float4		vProjPos : TEXCOORD2;
//	float3		vViewDir : TEXCOORD3;
//	float3		vTangent : TANGENT;
//	float4		vBinormal : BINORMAL;
//	float4		vMotionDir : TEXCOORD4;
//};
//
//struct VS_OUT_NORMAL
//{
//	float4		vPosition : SV_POSITION;
//	float3		vNormal : NORMAL;
//	float2		vTexUV : TEXCOORD0;
//	float4		vWorldPos : TEXCOORD1;
//	float4		vProjPos : TEXCOORD2;
//	float4		vTangent : TANGENT;
//	float4		vBinormal : BINORMAL;
//};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vViewDir = float3(normalize(g_vWorldCamPos - Out.vWorldPos).xyz);

	return Out;
}

VS_OUT VS_MAIN_COLLIDER(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_MOVEUV(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV + float2(0.f, g_fMoveUV);
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN_SHADOW In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_LightViewMatrix);
    matWVP = mul(matWV, g_LightProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
	
    Out.vClipPos = Out.vPosition;

    return Out;
}

VS_OUT_MOTION VS_MAIN_MOTIONBLUR(VS_IN In)
{
	VS_OUT_MOTION			Out = (VS_OUT_MOTION)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vViewDir = float3(normalize(g_vWorldCamPos - Out.vWorldPos).xyz);


	// 모션 블러
	vector		vNewPosition = Out.vPosition;

	vector		vOldPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	vOldPosition = mul(vOldPosition, g_OldWorldMatrix);
	vOldPosition = mul(vOldPosition, g_ViewMatrix);
	vOldPosition = mul(vOldPosition, g_ProjMatrix);

	vector		vDir = vNewPosition - vOldPosition;

	vector		vViewNormal = mul(vector(Out.vNormal, 0.f), g_ViewMatrix);

	float		fValue = dot(normalize(vDir).xyz, normalize(vViewNormal).xyz);
	if (fValue < 0.f)
		Out.vPosition = vOldPosition;
	else
		Out.vPosition = vNewPosition;

	float2		vVelocity = (vNewPosition.xy / vNewPosition.w) - (vOldPosition.xy / vOldPosition.w);
	Out.vMotionDir.xy     = vVelocity * 0.5f;
	Out.vMotionDir.y     *= -1.f;
	Out.vMotionDir.z      = Out.vPosition.z;
	Out.vMotionDir.w      = Out.vPosition.w;

	return Out;
}

//struct PS_IN
//{
//	float4		vPosition : SV_POSITION;
//	float3		vNormal   : NORMAL;
//	float2		vTexUV    : TEXCOORD0;
//	float4		vWorldPos : TEXCOORD1;
//	float4		vProjPos  : TEXCOORD2;
//	float3		vViewDir  : TEXCOORD3;
//	float3		vTangent  : TANGENT;
//	float4		vBinormal : BINORMAL;
//};
//
//struct PS_IN_MOTION
//{
//	float4		vPosition : SV_POSITION;
//	float3		vNormal   : NORMAL;
//	float2		vTexUV    : TEXCOORD0;
//	float4		vWorldPos : TEXCOORD1;
//	float4		vProjPos  : TEXCOORD2;
//	float3		vViewDir  : TEXCOORD3;
//	float3		vTangent  : TANGENT;
//	float4		vBinormal : BINORMAL;
//	float4		vMotionDir : TEXCOORD4;
//};
//
//struct PS_OUT
//{
//	vector		vDiffuse	:   SV_TARGET0;
//	vector		vNormal		:   SV_TARGET1;
//	vector		vDepth		:   SV_TARGET2;
//	vector		vEffect		:   SV_TARGET3;
//	vector		vEmissive	:	SV_TARGET4;
//	vector		vRimLight   :	SV_TARGET5;
//	vector		vVelocity   :	SV_TARGET6;
//};
//
//struct PS_OUT_NOLIGHT
//{
//	vector		vDiffuse : SV_TARGET0;
//	vector		vDepth : SV_TARGET1;
//	vector		vBlur : SV_TARGET2;
//};
//
//struct PS_IN_SHADOW
//{
//    float4 vPosition : SV_POSITION;
//    float4 vClipPos : POSITION;
//};
//
//struct PS_OUT_SHADOW
//{
//    vector vShadowDepth : SV_TARGET0;
//};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse    = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);


    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fModelDistortion, g_fLUTMask);
	Out.vEffect = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
    if (Out.vDiffuse.a < 0.01f)
        discard;
	
	return Out;
}

PS_OUT PS_MAIN_COLLIDER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = vector(0.f, 1.f, 0.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, g_fLUTMask);

	return Out;
}

PS_OUT PS_MAIN_NOTLIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);

	if (Out.vDiffuse.a < 0.05f)
		discard;


	return Out;
}

PS_OUT PS_MAIN_MOVENOISE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}


PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDiffuse  = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector  vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);
	
	// 디졸브처리
    Out.vDiffuse = vDiffuse;
	
    if (vDissolve.r - g_fDissolveAlpha < 0.09f)
		Out.vDiffuse = g_vDissolveColor1;
    if (vDissolve.r - g_fDissolveAlpha < 0.06f)
        Out.vDiffuse = g_vDissolveColor2;
    if (vDissolve.r - g_fDissolveAlpha < 0.03f)
        Out.vDiffuse = g_vDissolveColor3;
    if (vDissolve.r - g_fDissolveAlpha < 0.01f)
        discard;
	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);


	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = vDiffuse;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE_DONTBURNING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector  vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vDissolve.r - g_fDissolveAlpha < 0.09f)
		Out.vDiffuse = g_vDissolveColor1;
	if (vDissolve.r - g_fDissolveAlpha < 0.06f)
		Out.vDiffuse = g_vDissolveColor2;
	if (vDissolve.r - g_fDissolveAlpha < 0.03f)
		Out.vDiffuse = g_vDissolveColor3;
	if (vDissolve.r - g_fDissolveAlpha < 0.01f)
		discard;

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);


	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, g_fModelDistortion, g_fLUTMask);
	Out.vEffect = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MAPTOOLPICKING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	if (vDiffuse.a >= 0.1f)
		Out.vDiffuse = vector(In.vTexUV.x, In.vTexUV.y, 1.f, 1.f);
	
    Out.vNormal  = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth   = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);

    if (Out.vDiffuse.a < 0.1f)
        discard;

    return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = vDiffuse;

	if (Out.vEffect.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vEmissive = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
    vector  vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);


    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = vEmissive * 1.5f;
	Out.vEmissive = vEmissive;

	return Out;
}

PS_OUT_NOLIGHT PS_MAIN_NOLIGHTBLUR(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vBlur = vDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vEmissive = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	float	fRim = saturate(dot(In.vNormal, In.vViewDir));
	vector	vRimLight = pow(1.f - fRim, g_fRimPower) * g_vRimColor;

	if (fRim > g_fRimRange)
		fRim = 1.f;

	else
		fRim = -1.f;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = (vDiffuse * g_isBlur) + (vRimLight * g_isRim);
	Out.vEmissive = vEmissive;
	Out.vRimLight = vRimLight;

	return Out;
}

PS_OUT PS_MAIN_RIMLIGHT_NO_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse    = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    vector vEmissive   = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    float fRim       = saturate(dot(In.vNormal, In.vViewDir));
    vector vRimLight = pow(1.f - fRim, g_fRimPower) * g_vRimColor;

    if (fRim > g_fRimRange)
        fRim = 1.f;
    else
        fRim = -1.f;

    Out.vDiffuse  = vDiffuse;
    Out.vNormal   = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth    = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
    Out.vEffect   = (vDiffuse * g_isBlur) + (vRimLight * g_isRim);
    Out.vEmissive = 0;
    Out.vRimLight = vRimLight;

    return Out;
}

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN_MOTION In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	float fRim = saturate(dot(In.vNormal, In.vViewDir));
	vector vRimLight = pow(1.f - fRim, g_fRimPower) * g_vRimColor;

	if (fRim > g_fRimRange)
		fRim = 1.f;

	vector	vVelocity;
	vVelocity.xy = In.vMotionDir.xy;
	vVelocity.z = 1.f;
	vVelocity.w = In.vMotionDir.z / In.vMotionDir.w;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = (vDiffuse * g_isBlur) + (vRimLight * g_isRim);;
	Out.vEmissive = 0.f;
	Out.vRimLight = vRimLight * g_isRim;
	Out.vVelocity = vVelocity;
	if (Out.vDiffuse.a < 0.01f)
		discard;

	return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
	
    Out.vShadowDepth = In.vClipPos.z / In.vClipPos.w;
	
    return Out;
}

PS_OUT_NOLIGHT PS_MAIN_MOTIONTRAIL(PS_IN In)
{
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	float fRim = saturate(dot(In.vNormal, In.vViewDir));
	vector vRimLight = pow(1.f - fRim, g_fRimPower) * g_vRimColor;

	vRimLight.a -= g_fAlpha;

	if (0.f == vRimLight.a)
		discard;

	Out.vDiffuse = vRimLight;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vBlur = vRimLight;

	return Out;
}

technique11		DefaultTechnique
{
	pass Default //0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Collider	// 1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_COLLIDER();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLLIDER();
	}

	pass NotLight	// 2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOTLIGHT();
	}

	pass MoveUV	// 3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Dissolve //4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}
	pass Test //5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass Blur //6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass Dissolve_DontBurning //7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE_DONTBURNING();
	}

    pass MapToolPicking //8
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader   = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader    = compile ps_5_0 PS_MAIN_MAPTOOLPICKING();
    }

	pass Effect //9
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass Emissive //10
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
	}

	pass NoLight_Blur//11
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOLIGHTBLUR();
	}

	pass RimLight//12
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
	}

    pass RimLight_NoEmissive //13
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader   = compile vs_5_0 VS_MAIN_DEFAULT();
        GeometryShader = NULL;
        PixelShader    = compile ps_5_0 PS_MAIN_RIMLIGHT_NO_EMISSIVE();
    }

    pass Shadow //14
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

	pass MotionBlur //15
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOTIONBLUR();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
	}

	pass MotionTrail //16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOTIONTRAIL();
	}
}