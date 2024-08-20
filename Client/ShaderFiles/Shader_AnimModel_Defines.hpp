
texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;
texture2D			g_NormalTexture;
texture2D			g_ThirdTexture;
texture2D			g_DissolveTexture;

cbuffer NoiseAndMoveUV
{
	float				g_fMoveUV;
float				g_fMoveNoise;
float				g_fMoveNoise_Y;
float				g_fNoisePower;
};

cbuffer Dissolve
{
	float		g_fDissolveAlpha = 0.f;
float4		g_vDissolveColor1 = float4(1.f, 0.f, 0.f, 1.f);
float4		g_vDissolveColor2 = float4(1.f, 1.f, 0.f, 1.f);
float4		g_vDissolveColor3 = float4(1.f, 1.f, 1.f, 1.f);
};

cbuffer Distortion
{
	float			g_fModelDistortion = 0.f;
}

cbuffer RimLight
{
	float				g_fRimPower;
float				g_fRimRange;
float4				g_vRimColor;
float4				g_vWorldCamPos;

bool				g_isRim = false;
bool				g_isBlur = false;
float				g_fLUTMask = 0.f;
float				g_fAlpha = 0.f;
};

cbuffer LightMatrix
{
	float4x4			g_LightViewMatrix;
float4x4			g_LightProjMatrix;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_IN_SHADOW
{
	float3 vPosition : POSITION;
	uint4  vBlendIndex : BLENDINDEX;
	float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vClipPos : POSITION;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float3		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct VS_OUT_MOTION
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float3		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4		vMotionDir : TEXCOORD4;
};

struct VS_OUT_NORMAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float3		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_IN_MOTION
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vViewDir : TEXCOORD3;
	float3		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4		vMotionDir : TEXCOORD4;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vEffect : SV_TARGET3;
	vector		vEmissive : SV_TARGET4;
	vector		vRimLight : SV_TARGET5;
	vector		vVelocity : SV_TARGET6;
};

struct PS_OUT_NOLIGHT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vDepth : SV_TARGET1;
	vector		vBlur : SV_TARGET2;
};

struct PS_IN_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vClipPos : POSITION;
};

struct PS_OUT_SHADOW
{
	vector vShadowDepth : SV_TARGET0;
};