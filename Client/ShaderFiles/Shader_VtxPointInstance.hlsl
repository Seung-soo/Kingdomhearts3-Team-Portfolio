
#include "Shader_Defines.hpp"

cbuffer CameraDesc
{
	vector			g_vCamPosition;
};

cbuffer ColorDesc
{
	vector			g_vAddColor;
	vector			g_vMulColor;
	bool			g_bBlur;
};

texture2D			g_DiffuseTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float		fPSize : PSIZE;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
	
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix = float4x4(normalize(In.vRight), normalize(In.vUp), normalize(In.vLook), In.vTranslation);

	vector			vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.fPSize = In.fPSize;
    
    Out.vRight = normalize(In.vRight);
    Out.vUp = normalize(In.vUp);
    Out.vLook = normalize(In.vLook);

	return Out;
}

VS_OUT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix = float4x4(normalize(In.vRight), normalize(In.vUp), normalize(In.vLook), In.vTranslation);

	vector			vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = vPosition;
	Out.fPSize = length(In.vRight);
    
    Out.vRight = normalize(In.vRight);
    Out.vUp = normalize(In.vUp);
    Out.vLook = normalize(In.vLook);


	return Out;
}

struct GS_IN
{	
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;	
	
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook: TEXCOORD3;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(in point GS_IN In[1], inout TriangleStream<GS_OUT> Trianglestream)
{
	GS_OUT			Out[4];

    matrix Local = float4x4(normalize(In[0].vRight), normalize(In[0].vUp), normalize(In[0].vLook), vector(0.f, 0.f, 0.f, 1.f));
	
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vAxisY = float3(0.f, 1.f, 0.f);
    float3 vRight = cross(vAxisY, vLook);
    float3 vUp = cross(vLook, vRight);
	
	vLook = normalize(vLook);
	vRight = normalize(vRight);
	vUp = normalize(vUp);


	float	fHalfSize = In[0].fPSize * 0.5f;

	matrix		matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

    vPosition = In[0].vPosition.xyz + vRight * fHalfSize + vUp * fHalfSize;
    Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[0].vTexUV = float2(0.f, 0.f);
    Out[0].vProjPos = Out[0].vPosition;

    vPosition = In[0].vPosition.xyz - vRight * fHalfSize + vUp * fHalfSize;
    Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[1].vTexUV = float2(1.f, 0.f);
    Out[1].vProjPos = Out[1].vPosition;

    vPosition = In[0].vPosition.xyz - vRight * fHalfSize - vUp * fHalfSize;
    Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[2].vTexUV = float2(1.f, 1.f);
    Out[2].vProjPos = Out[2].vPosition;

    vPosition = In[0].vPosition.xyz + vRight * fHalfSize - vUp * fHalfSize;
    Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[3].vTexUV = float2(0.f, 1.f);
    Out[3].vProjPos = Out[3].vPosition;


	Trianglestream.Append(Out[0]);
	Trianglestream.Append(Out[1]);
	Trianglestream.Append(Out[2]);
	Trianglestream.RestartStrip();

	Trianglestream.Append(Out[0]);
	Trianglestream.Append(Out[2]);
	Trianglestream.Append(Out[3]);
	Trianglestream.RestartStrip();
}

[maxvertexcount(6)]
void GS_EXPLOSION(in point GS_IN In[1], inout TriangleStream<GS_OUT> Trianglestream)
{
    GS_OUT Out[4];

    matrix Local = float4x4(normalize(In[0].vRight), normalize(In[0].vUp), normalize(In[0].vLook), vector(0.f, 0.f, 0.f, 1.f));
	
    vector vLook = (g_vCamPosition - In[0].vPosition).xyzw;
    vector vAxisY = vector(0.f, 1.f, 0.f, 0.f);
    vector vRight = vector(cross(vAxisY.xyz, vLook.xyz).xyz, 0.f);
    vector vUp = vector(cross(vLook.xyz, vRight.xyz).xyz, 0.f);
	
    vLook = normalize(vLook);
    vRight = normalize(vRight);
    vUp = normalize(vUp);
	
    vLook = mul(vLook, Local);
    vRight = mul(vRight, Local);
    vUp = mul(vUp, Local);
	

    float fHalfSize = In[0].fPSize * 0.5f;

    matrix matVP;
    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    float3 vPosition;
	
    vPosition = In[0].vPosition.xyz + vRight.xyz * fHalfSize + vUp.xyz * fHalfSize;
    Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[0].vTexUV = float2(0.f, 0.f);
    Out[0].vProjPos = Out[0].vPosition;

    vPosition = In[0].vPosition.xyz - vRight.xyz * fHalfSize + vUp.xyz * fHalfSize;
    Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[1].vTexUV = float2(1.f, 0.f);
    Out[1].vProjPos = Out[1].vPosition;

    vPosition = In[0].vPosition.xyz - vRight.xyz * fHalfSize - vUp.xyz * fHalfSize;
    Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[2].vTexUV = float2(1.f, 1.f);
    Out[2].vProjPos = Out[2].vPosition;

    vPosition = In[0].vPosition.xyz + vRight.xyz * fHalfSize - vUp.xyz * fHalfSize;
    Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
    Out[3].vTexUV = float2(0.f, 1.f);
    Out[3].vProjPos = Out[3].vPosition;
 
    
    

    Trianglestream.Append(Out[0]);
    Trianglestream.Append(Out[1]);
    Trianglestream.Append(Out[2]);
    Trianglestream.RestartStrip();

    Trianglestream.Append(Out[0]);
    Trianglestream.Append(Out[2]);
    Trianglestream.Append(Out[3]);
    Trianglestream.RestartStrip();
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
	vector		vDepth : SV_TARGET1;
	vector		vBlur : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + g_vAddColor) * g_vMulColor;
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vBlur = Out.vColor * g_bBlur;
	if (Out.vColor.a <= 0.f) 
		discard;

	Out.vColor.a = 1.f;


	return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = (g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + g_vAddColor) * g_vMulColor;
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    Out.vBlur = Out.vColor;
    if (Out.vColor.a <= 0.1f) 
        discard;

    Out.vColor.a = 1.f;


    return Out;
}


technique11		DefaultTechnique
{
	pass Point
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
	pass Effect
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass DiscardBiggerAlphaRange
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }

    pass Explosion
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(ZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = compile gs_5_0 GS_EXPLOSION();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}