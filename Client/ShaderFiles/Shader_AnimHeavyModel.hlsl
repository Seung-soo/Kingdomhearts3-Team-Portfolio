
#include "Shader_Defines.hpp" 
#include "Shader_AnimModel_Defines.hpp"

struct BoneMatrixArray
{
    matrix BoneMatrices[450];
};

cbuffer Matrices
{
	BoneMatrixArray		g_BoneMatrices;
	matrix				g_OldWorldMatrix;
};


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
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix)).xyz;
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
	Out.vMotionDir.xy = vVelocity * 0.5f;
	Out.vMotionDir.y *= -1.f;
	Out.vMotionDir.z = Out.vPosition.z;
	Out.vMotionDir.w = Out.vPosition.w;

	return Out;
}



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
	Out.vEffect = 0;

    if (Out.vDiffuse.a < 0.01f)
        discard;
	
	return Out;
}

PS_OUT PS_MAIN_COLLIDER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = vector(0.f, 1.f, 0.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

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
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);

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
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
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

	vector	vVelocity;
	vVelocity.xy = In.vMotionDir.xy;
	vVelocity.z = 1.f;
	vVelocity.w = In.vMotionDir.z / In.vMotionDir.w;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;
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

PS_OUT PS_MAIN_NOISE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise, g_fMoveNoise_Y));
	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * g_fNoisePower);
	vector	vEmissive = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, g_fLUTMask);
	Out.vEffect = 0.f;
	Out.vEmissive = 0.f;
	Out.vRimLight = 0.f;

	return Out;
}

PS_OUT PS_MAIN_NOISE_RIM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise, g_fMoveNoise_Y));
	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * g_fNoisePower);
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

	pass MoveUV_Noise //17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE();
	}

	pass MoveUV_Noise_Rim //18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_RIM();
	}
}