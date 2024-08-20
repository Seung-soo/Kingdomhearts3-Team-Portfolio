cbuffer ScreenSizeBuffer
{
	float			g_fWinSizeX;
	float			g_fWinSizeY;
};

cbuffer SSAO
{
	float				g_fRadius = 1.f;
	float				g_fBias = 0.025f;
	static const int	KERNEL_SIZE = 16;
	float4				vSamples[64];

	float3				g_vRandom[16] =
	{
		float3(0.2024537f, 0.841204f,0.f),
		float3(-0.2200423f, 0.6282339f,0.f),
		float3(0.3677659f, 0.1086345f, 0.f),
		float3(0.8775856f, 0.4617546f, 0.f),
		float3(0.7867433f, -0.141479f, 0.f),
		float3(0.4839356f, -0.8253108f,0.f),
		float3(0.4401554f, -0.4228428f,0.f),
		float3(0.0019193f, -0.8048455f,0.f),
		float3(-0.7578573f, -0.5583301f,0.f),
		float3(-0.4540417f, -0.252365f,0.f),
		float3(-0.0483353f, -0.2527294f,0.f),
		float3(-0.4192392f, 0.2084218f,0.f),
		float3(-0.8433938f, 0.1451271f,0.f),
		float3(-0.4037157f, -0.8263387f,0.f),
		float3(-0.6657394f, 0.6298575f,0.f),
		float3(-0.0001783f, 0.2834622f,0.f)
	};
};


cbuffer RealMatrix
{
	matrix			g_RealViewMatrix;
	matrix			g_RealProjMatrix;
	matrix			g_SSAOProjMatrixInv;
};

texture2D			g_SSAOTexture;
texture2D			g_SSAONormalTexture;
texture2D			g_SSAODepthTexture;

sampler SSAODefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler SSAOClampSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

cbuffer	SSAORenderingPipeLine
{
	matrix			g_SSAOWorldMatrix;
	matrix			g_SSAOViewMatrix;
	matrix			g_SSAOProjMatrix;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_BLUR
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV	  : TEXCOORD0;
	float2		texCoord1 : TEXCOORD1;
	float2		texCoord2 : TEXCOORD2;
	float2		texCoord3 : TEXCOORD3;
	float2		texCoord4 : TEXCOORD4;
	float2		texCoord5 : TEXCOORD5;
	float2		texCoord6 : TEXCOORD6;
	float2		texCoord7 : TEXCOORD7;
	float2		texCoord8 : TEXCOORD8;
	float2		texCoord9 : TEXCOORD9;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_SSAOWorldMatrix, g_SSAOViewMatrix);
	matWVP = mul(matWV, g_SSAOProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_BLUR VS_MAIN_BLUR_X(VS_IN In)
{
	VS_OUT_BLUR			Out = (VS_OUT_BLUR)0;

	matrix			matWV, matWVP;

	matWV = mul(g_SSAOWorldMatrix, g_SSAOViewMatrix);
	matWVP = mul(matWV, g_SSAOProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float			texelSize;
	texelSize = 1.0f / g_fWinSizeX;

	Out.texCoord1 = In.vTexUV + float2(texelSize * -4.0f, 0.f);
	Out.texCoord2 = In.vTexUV + float2(texelSize * -3.0f, 0.0f);
	Out.texCoord3 = In.vTexUV + float2(texelSize * -2.0f, 0.0f);
	Out.texCoord4 = In.vTexUV + float2(texelSize * -1.0f, 0.0f);
	Out.texCoord5 = In.vTexUV + float2(texelSize *  0.0f, 0.0f);
	Out.texCoord6 = In.vTexUV + float2(texelSize *  1.0f, 0.0f);
	Out.texCoord7 = In.vTexUV + float2(texelSize *  2.0f, 0.0f);
	Out.texCoord8 = In.vTexUV + float2(texelSize *  3.0f, 0.0f);
	Out.texCoord9 = In.vTexUV + float2(texelSize *  4.0f, 0.0f);

	return Out;
}

VS_OUT_BLUR VS_MAIN_BLUR_Y(VS_IN In)
{
	VS_OUT_BLUR			Out = (VS_OUT_BLUR)0;

	matrix			matWV, matWVP;

	matWV = mul(g_SSAOWorldMatrix, g_SSAOViewMatrix);
	matWVP = mul(matWV, g_SSAOProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float			texelSize;
	texelSize = 1.0f / g_fWinSizeY;

	Out.texCoord1 = In.vTexUV + float2( 0.f, texelSize * -4.0f);
	Out.texCoord2 = In.vTexUV + float2( 0.f, texelSize * -3.0f);
	Out.texCoord3 = In.vTexUV + float2( 0.f, texelSize * -2.0f);
	Out.texCoord4 = In.vTexUV + float2( 0.f, texelSize * -1.0f);
	Out.texCoord5 = In.vTexUV + float2( 0.f, texelSize *  0.0f);
	Out.texCoord6 = In.vTexUV + float2( 0.f, texelSize *  1.0f);
	Out.texCoord7 = In.vTexUV + float2( 0.f, texelSize *  2.0f);
	Out.texCoord8 = In.vTexUV + float2( 0.f, texelSize *  3.0f);
	Out.texCoord9 = In.vTexUV + float2( 0.f, texelSize *  4.0f);


	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_IN_BLUR
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV	  : TEXCOORD0;
	float2		texCoord1 : TEXCOORD1;
	float2		texCoord2 : TEXCOORD2;
	float2		texCoord3 : TEXCOORD3;
	float2		texCoord4 : TEXCOORD4;
	float2		texCoord5 : TEXCOORD5;
	float2		texCoord6 : TEXCOORD6;
	float2		texCoord7 : TEXCOORD7;
	float2		texCoord8 : TEXCOORD8;
	float2		texCoord9 : TEXCOORD9;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_SSAOTexture.Sample(SSAODefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN_BLUR In)
{
	PS_OUT		Out = (PS_OUT)0;

	float		Weight0, Weight1, Weight2, Weight3, Weight4;
	float		normalization;

	// 가중치
	Weight0 = 1.0f;
	Weight1 = 0.9f;
	Weight2 = 0.55f;
	Weight3 = 0.18f;
	Weight4 = 0.f;

	// 평균을 내어 정규화 값을 만든다
	normalization = Weight0 + (2.0f * (Weight1 + Weight2 + Weight3 + Weight4));

	Weight0 = Weight0 / normalization;
	Weight1 = Weight1 / normalization;
	Weight2 = Weight2 / normalization;
	Weight3 = Weight3 / normalization;
	Weight4 = Weight4 / normalization;

	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord1) * Weight4;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord2) * Weight3;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord3) * Weight2;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord4) * Weight1;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord5) * Weight0;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord6) * Weight1;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord7) * Weight2;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord8) * Weight3;
	Out.vColor += g_SSAOTexture.Sample(SSAOClampSampler, In.texCoord9) * Weight4;

	Out.vColor.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDepthDesc = g_SSAODepthTexture.Sample(SSAOClampSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vWorldPos;

	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	vWorldPos = mul(vWorldPos, g_SSAOProjMatrixInv);

	float3		vNormal			= g_SSAONormalTexture.Sample(SSAOClampSampler, In.vTexUV).xyz;
	vNormal = vNormal * 2.f - 1.f;

	float3		vViewNormal		= mul(float4(vNormal, 0.f), g_RealViewMatrix).xyz;

	uint		iRandomX		= uint(In.vTexUV.x * g_fWinSizeX) % 4;
	uint		iRandomY		= uint(In.vTexUV.y * g_fWinSizeY) % 4;
	float3		vRandomVector	= g_vRandom[4 * iRandomY + iRandomX];


	float3		vTangent		= normalize(vRandomVector - vViewNormal * dot(vRandomVector, vViewNormal));
	float3		vBinormal		= cross(vViewNormal, vTangent);
	float3x3	TBN				= float3x3(vTangent, vBinormal, vViewNormal);

	
	float		fOcclusion		= 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		float3	vSample			= vWorldPos.xyz + mul(vSamples[i].xyz, TBN) * g_fRadius;
		float4	vScreenSample	=	mul(float4(vSample, 1.f), g_RealProjMatrix);
		vScreenSample.xyz		/=	vScreenSample.w;
		vScreenSample.x			=	vScreenSample.x * 0.5f  + 0.5f;
		vScreenSample.y			=	vScreenSample.y * -0.5f + 0.5f;


		vector vSampleDepth = g_SSAODepthTexture.Sample(SSAOClampSampler, vScreenSample.xy);
		float fSampleViewZ = vSampleDepth.x * 300.f;
		vector vWP;
		vWP.x = (vScreenSample.x * 2.f - 1.f) * fSampleViewZ;
		vWP.y = (vScreenSample.y * -2.f + 1.f) * fSampleViewZ;
		vWP.z = vSampleDepth.y * fSampleViewZ;
		vWP.w = 1.f * fSampleViewZ;

		vWP = mul(vWP, g_SSAOProjMatrixInv);

		float	fSampleDepth	=	vWP.z;
		float	fRangeCheck		=	smoothstep(0.f, 1.f, g_fRadius / abs(vWorldPos.z - fSampleDepth));
		fOcclusion				+=	(fSampleDepth <= vSample.z + g_fBias ? 1.f : 0.f) * fRangeCheck;
	}

	float		fAmbient		=	1.f - fOcclusion / KERNEL_SIZE;

	Out.vColor	=	vector(fAmbient, fAmbient, fAmbient, 1.f);

	return Out;
}

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

BlendState	Blending_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

DepthStencilState  NonZTestAndWriteState
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

RasterizerState CullMode_ccw
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

technique11		DefaultTechnique
{
	pass DebugBuffer		// 0 
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}	

	pass Blur_Hori			// 1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_X();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass Blur_Verti			// 2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_Y();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}


	pass SSAO				// 3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SSAO();
	}
}