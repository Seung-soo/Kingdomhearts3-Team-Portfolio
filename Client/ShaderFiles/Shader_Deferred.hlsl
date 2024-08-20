cbuffer LightDesc
{
	vector			g_vLightDir;
	vector			g_vLightPos;
	float			g_fLightRange;
	vector			g_vLightDiffuse;
	vector			g_vLightAmbient;
	vector			g_vLightSpecular;
	vector			g_vCamPosition;
	vector			g_vViewDir;
};

cbuffer MtrlDesc
{
	vector			g_vMtrlDiffuse = (vector)1.f;
	vector			g_vMtrlAmbient = (vector)1.f;
	vector			g_vMtrlSpecular = (vector)1.f;
};

cbuffer ScreenSizeBuffer
{
	float			ScreenWidth;
	float			ScreenHeight;
	float2			padding; //버퍼의 크기가 16의 배수가 되도록 해주는것
};

cbuffer Distortion
{
	float			g_DistortionUV;
};

cbuffer DofBlur
{
	float4			g_DofBlurOffsets[8];
	float			g_FocusDistance;
	float			g_MinBlur;
	float			g_MaxBlur;
	float			g_fMinDistance;
	float			g_fMaxDistance;
};

cbuffer HDR
{
	float			g_fGrayScale = 0.5f;
	float			g_fWhite = 1.f;
};

cbuffer LUT
{
   // float2 LUTTexSize = float2(0.00390625, 0.0625);
    float2 LUTTexSize = float2(0.0009765625, 0.03125);
};

cbuffer Fog
{
	float			g_fFogStart;
	float			g_fFogEnd;
	float4			g_vFogColor;
	float			g_fMaxFogFactor;
}

cbuffer Shadow
{
    bool g_bRenderShadow = true;
};


cbuffer RadialBlur
{
	static const float radialBlurSamples[11] = {
		-0.08f, -0.05f, -0.03f, -0.02f, -0.01f, 0.f, 0.01f, 0.02f, 0.03f, 0.05f, 0.08f
	};
	float			g_fBlurDist = 1.f;
};

cbuffer CatoonRendering
{
	float	mask[9] = {
		-1.f, -1.f, -1.f,
		-1.f, 8.f, -1.f,
		-1.f, -1.f, -1.f,
	};

	float coord[3] = { -1.f, 0.f, 1.f };
};

cbuffer Matrices
{
	matrix			g_ProjMatrixInv;
	matrix			g_ViewMatrixInv;
	//matrix			g_RealViewMatrix;
	//matrix			g_RealProjMatrix;
};

texture2D			g_TargetTexture;
texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_DepthTexture;
texture2D			g_ShadeTexture;
texture2D			g_SpecularTexture;
texture2D			g_BlurTexture;
texture2D			g_ThirdTexture;
texture2D			g_RimTexture;
texture2D			g_ShadowTexture;
texture2D			g_StaticShadowTexture;
texture2D			g_LUTTexture;


sampler DefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC

    filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;

};

sampler ClampSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

sampler2D g_LUT = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

cbuffer	RenderingPipeLine
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer LightMatrix
{
    matrix g_LightViewMatrix;
    matrix g_LightProjMatrix;
	
    matrix g_LightViewMatrix_Static;
    matrix g_LightProjMatrix_Static;
	
    float g_fBias = 0.01f;
	
    float g_fAlpha = 1.f;
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

struct VS_OUT_DOFBLUR
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV	  : TEXCOORD0;
	float4		texCoord1 : TEXCOORD1;
	float4		texCoord2 : TEXCOORD2;
	float4		texCoord3 : TEXCOORD3;
	float4		texCoord4 : TEXCOORD4;
	float4		texCoord5 : TEXCOORD5;
	float4		texCoord6 : TEXCOORD6;
	float4		texCoord7 : TEXCOORD7;
	float4		texCoord8 : TEXCOORD8;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_BLUR VS_MAIN_BLUR_X(VS_IN In)
{
	VS_OUT_BLUR			Out = (VS_OUT_BLUR)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float			texelSize;
	texelSize = 1.0f / ScreenWidth;

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

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float			texelSize;
	texelSize = 1.0f / ScreenHeight;

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

struct PS_IN_DOFBLUR
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV	  : TEXCOORD0;
	float4		texCoord1 : TEXCOORD1;
	float4		texCoord2 : TEXCOORD2;
	float4		texCoord3 : TEXCOORD3;
	float4		texCoord4 : TEXCOORD4;
	float4		texCoord5 : TEXCOORD5;
	float4		texCoord6 : TEXCOORD6;
	float4		texCoord7 : TEXCOORD7;
	float4		texCoord8 : TEXCOORD8;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UPSAMPLING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);


	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
	vector		vShadow : SV_TARGET2;
	vector		vOutline : SV_TARGET3;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc  = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc   = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vAmbientDesc = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	
	
	float		fViewZ = vDepthDesc.x * 300.f;
	
    float		fShadow = 1.f;
    Out.vShadow = 1.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    
    if (true == g_bRenderShadow)
    {
		// Shadow 연산 =========================================================
        float4 LightSpacePos_Static = vWorldPos;
	
        LightSpacePos_Static = mul(LightSpacePos_Static, g_LightViewMatrix_Static);
        LightSpacePos_Static = mul(LightSpacePos_Static, g_LightProjMatrix_Static);
	
        float2 vShadowUV_Satic = LightSpacePos_Static.xy / LightSpacePos_Static.w;
	
        vShadowUV_Satic.y = -vShadowUV_Satic.y;
        vShadowUV_Satic = vShadowUV_Satic * 0.5f + 0.5f;

	// Static
        if (saturate(vShadowUV_Satic.x) == vShadowUV_Satic.x && saturate(vShadowUV_Satic.y) == vShadowUV_Satic.y)
        {
            float fShadowDepth = g_StaticShadowTexture.Sample(DefaultSampler, vShadowUV_Satic).x;
		

            float fDepth = LightSpacePos_Static.z/* / LightSpacePos_Static.w*/;
		
            if (fDepth > fShadowDepth + g_fBias)
            {
                Out.vShadow = g_vLightAmbient/* * (1 - pow(saturate(dot(normalize(g_vLightDir), vNormal)), 20.f))*/;
				
                fShadow = 0.f;
            }
        }
	
        float4 LightSpacePos = vWorldPos;
	
        LightSpacePos = mul(LightSpacePos, g_LightViewMatrix);
        LightSpacePos = mul(LightSpacePos, g_LightProjMatrix);
	
        float2 vShadowUV = LightSpacePos.xy / LightSpacePos.w;
	
        vShadowUV.y = -vShadowUV.y;
        vShadowUV = vShadowUV * 0.5f + 0.5f;
	
	// 투영스페이스에 있으면 조명을 받는것 아니면 안받는것
        if (fShadow == 1.f)
        {
            if (saturate(vShadowUV.x) == vShadowUV.x && saturate(vShadowUV.y) == vShadowUV.y)
            {
                float fShadowDepth = g_ShadowTexture.Sample(DefaultSampler, vShadowUV).x;
		
                float fDepth = LightSpacePos.z / LightSpacePos.w;
		
                if (fDepth > fShadowDepth + 0.001f)
                {
                    float fValue = pow(saturate(dot(normalize(g_vLightDir), vNormal)), 20.f);
					
                    if (fValue >= 1.f)
                        fValue = 0.8f;
					
                    Out.vShadow = g_vLightAmbient/* * (1 - fValue)*/;
                }
            }
        }
 
	
        Out.vShadow = saturate(Out.vShadow);
    }
	
    else
        Out.vShadow = 1.f;
	
	
    Out.vShadow.a = 1.f;
	
    Out.vShade = g_vLightDiffuse * pow((dot(normalize(g_vLightDir) * -1.f, vNormal)) * 0.5f + 0.5f, 4) + (vAmbientDesc * g_vMtrlAmbient * g_vLightAmbient);

    Out.vShade.a = 1.f;

    vector vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);
    Out.vSpecular.a = 0.f;
	

	//// 외곽선
	//vector		vOutlineColor = (vector)0.f;
	//float3		vGrayScale = float3(0.3f, 0.59f, 0.11f);

	//for (uint i = 0; i < 9; i++)
	//{
	//	vector	vNormalCatoon = g_NormalTexture.Sample(DefaultSampler, In.vTexUV + float2(coord[i % 3] / 1280.f, coord[i / 3] / 720.f));
	//	vector	vNormalC = vector(vNormalCatoon.xyz * 2.f - 1.f, 0.f);
	//	vOutlineColor += mask[i] * vNormalC;
	//}
	//float fGray = 1.f - dot(vOutlineColor.rgb, vGrayScale);
	//Out.vOutline = vector(fGray, fGray, fGray, 1.f);


	return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL_NOTSSAO(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vAmbientDesc = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);


	float		fViewZ = vDepthDesc.x * 300.f;

	float		fShadow = 1.f;
	Out.vShadow = 1.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


	if (true == g_bRenderShadow)
	{
		// Shadow 연산 =========================================================
		float4 LightSpacePos_Static = vWorldPos;

		LightSpacePos_Static = mul(LightSpacePos_Static, g_LightViewMatrix_Static);
		LightSpacePos_Static = mul(LightSpacePos_Static, g_LightProjMatrix_Static);

		float2 vShadowUV_Satic = LightSpacePos_Static.xy / LightSpacePos_Static.w;

		vShadowUV_Satic.y = -vShadowUV_Satic.y;
		vShadowUV_Satic = vShadowUV_Satic * 0.5f + 0.5f;

		// Static
		if (saturate(vShadowUV_Satic.x) == vShadowUV_Satic.x && saturate(vShadowUV_Satic.y) == vShadowUV_Satic.y)
		{
			float fShadowDepth = g_StaticShadowTexture.Sample(DefaultSampler, vShadowUV_Satic).x;


			float fDepth = LightSpacePos_Static.z;

			if (fDepth > fShadowDepth + g_fBias)
			{
				Out.vShadow = g_vLightAmbient;

				fShadow = 0.f;
			}
		}

		float4 LightSpacePos = vWorldPos;

		LightSpacePos = mul(LightSpacePos, g_LightViewMatrix);
		LightSpacePos = mul(LightSpacePos, g_LightProjMatrix);

		float2 vShadowUV = LightSpacePos.xy / LightSpacePos.w;

		vShadowUV.y = -vShadowUV.y;
		vShadowUV = vShadowUV * 0.5f + 0.5f;

		// 투영스페이스에 있으면 조명을 받는것 아니면 안받는것
		if (fShadow == 1.f)
		{
			if (saturate(vShadowUV.x) == vShadowUV.x && saturate(vShadowUV.y) == vShadowUV.y)
			{
				float fShadowDepth = g_ShadowTexture.Sample(DefaultSampler, vShadowUV).x;

				float fDepth = LightSpacePos.z / LightSpacePos.w;

				if (fDepth > fShadowDepth + 0.001f)
				{
					float fValue = pow(saturate(dot(normalize(g_vLightDir), vNormal)), 20.f);

					if (fValue >= 1.f)
						fValue = 0.8f;

					Out.vShadow = g_vLightAmbient;
				}
			}
		}


		Out.vShadow = saturate(Out.vShadow);
	}

	else
		Out.vShadow = 1.f;


	Out.vShadow.a = 1.f;

	Out.vShade = g_vLightDiffuse * pow((dot(normalize(g_vLightDir) * -1.f, vNormal)) * 0.5f + 0.5f, 4) + (g_vMtrlAmbient * g_vLightAmbient);

	Out.vShade.a = 1.f;

	vector vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	//vector		vAmbientDesc = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);
	
    if (fDistance > g_fLightRange)
        discard;

    float fAtt = max(g_fLightRange - fDistance, 0.f) / g_fLightRange;

	Out.vShade = (g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (/*vAmbientDesc * */g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    
    Out.vShadow = 0.f;
	Out.vShadow.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), vNormal);


	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;


	return Out;
}

struct PS_OUT_BLEND
{
	vector		vColor : SV_TARGET0;
	vector		vDepth : SV_TARGET1;
};

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vBlurDesc     = g_BlurTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDiffuseDesc  = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc    = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vEmissiveDesc = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vRimLightDesc = g_RimTexture.Sample(DefaultSampler, In.vTexUV);
    vector		vShadowDesc   = g_ShadowTexture.Sample(DefaultSampler, In.vTexUV);

    vector vColor = pow(pow(vector(vBlurDesc.xyz, 0.f), 2.2f) + (pow(vDiffuseDesc, 2.2f) * vShadeDesc * vShadowDesc + vSpecularDesc) + /*vEmissiveDesc + */vRimLightDesc, 1 / 2.2f);

	Out.vColor = vColor;

	if (Out.vColor.a == 0.0f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLEND_OUTLINE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vBlurDesc = g_BlurTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vEmissiveDesc = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vRimLightDesc = g_RimTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadowDesc = g_ShadowTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vOutlineDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vColor = pow((pow(vector(vBlurDesc.xyz, 0.f), 2.2f) + (pow(vDiffuseDesc, 2.2f) * vShadeDesc * vShadowDesc + vSpecularDesc)) * vOutlineDesc + /*vEmissiveDesc + */vRimLightDesc, 1 / 2.2f);

	Out.vColor = vColor;

	if (Out.vColor.a == 0.0f)
		discard;

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

	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord1) * Weight4;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord2) * Weight3;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord3) * Weight2;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord4) * Weight1;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord5) * Weight0;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord6) * Weight1;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord7) * Weight2;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord8) * Weight3;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord9) * Weight4;

    Out.vColor.a = g_fAlpha;

	return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNoise = g_BlurTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_DistortionUV));
	vector	vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vAlphaColor;

	vAlphaColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + (vNoise.r * 0.05f) * vDepth.z);

	Out.vColor = vAlphaColor;
	
	return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);	
	//if (0.f == Out.vColor.a)
	//	discard;


	return Out;
}

PS_OUT PS_MAIN_COMBINE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	
	vector		vNonBlendDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBlurDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	
    Out.vColor = vNonBlendDesc + vBlurDesc;

	return Out;
}

PS_OUT PS_MAIN_TONEMAPPING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vAverLum = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float	LScale = dot(vColor.rgb, float3(0.299f, 0.587f, 0.114f));
	LScale *= g_fGrayScale / vAverLum.r;
	LScale = (LScale + LScale * LScale / g_fWhite) / (1.f + LScale);
	vColor = LScale * vColor;
	if (vColor.a == 0.f)
	{

		vColor.a = 0.01f;
	}

	Out.vColor = vColor;



	return Out;
}

PS_OUT PS_MAIN_BLOOMMASK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = (vector)0.f;
	vector	vHDR = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	float	fBrightness = dot(vHDR.rgb, float3(0.299f, 0.587f, 0.114f));
	if (fBrightness > 0.9f)
		vColor = float4(vHDR.rgb, 1.f);

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_BLOOMMAPPING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vHDR = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vBloomBlurColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vBloomColor = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vBloom = pow(pow(abs(vBloomBlurColor), 2.2f) + pow(abs(vBloomColor), 2.2f), 1.f / 2.2f);

	vHDR = pow(abs(vHDR), 2.2f);
	vBloom = pow(abs(vBloom), 2.2f);

	vHDR += vBloom;

	Out.vColor = pow(abs(vHDR), 1.f / 2.2f);

	return Out;
}

PS_OUT PS_MAIN_AVERLUM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	float	fLum = dot(vColor.rgb, float3(0.299f, 0.587f, 0.114f));

	vColor = vector(fLum, fLum, fLum, 1.f);


	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_CALCAVERLUM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vPrevColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float	fAverLum = lerp(vPrevColor.r, vColor.r, 0.5f);

	vColor = vector(fAverLum, fAverLum, fAverLum, 1.f);

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_DOF(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vFocusColor		= g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vUnFocusColor	= g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vDepthDesc			= g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


	float blur = smoothstep(g_fMinDistance, g_fMaxDistance, abs(length(vWorldPos - g_vCamPosition)));

	float	fFocusWeight = 1.f - blur;

	//Out.vColor = pow(lerp(pow(vUnFocusColor, 2.2f), pow(vFocusColor, 2.2f), fFocusWeight), 1 / 2.2f);
	Out.vColor = lerp(vUnFocusColor, vFocusColor, fFocusWeight);
	
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_MAIN_DOFREALBLUR(PS_IN_BLUR In)
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

	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord1) * Weight4;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord2) * Weight3;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord3) * Weight2;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord4) * Weight1;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord5) * Weight0;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord6) * Weight1;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord7) * Weight2;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord8) * Weight3;
	Out.vColor += g_TargetTexture.Sample(ClampSampler, In.texCoord9) * Weight4;

	return Out;
}

PS_OUT PS_MAIN_LUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 TarggetDesc = saturate((g_TargetTexture.Sample(DefaultSampler, In.vTexUV)).xyz) * 31.f;
    //TarggetDesc.g = -TarggetDesc.g;

    vector LUTUV;
    LUTUV.w = floor(TarggetDesc.b); // floor : 지정된 값보다 작거나 같은 가장 큰 정수 값을 반환
    LUTUV.xy = (TarggetDesc.xy + 0.5f) * LUTTexSize;
    LUTUV.x += LUTUV.w * LUTTexSize.y;
    LUTUV.z = LUTUV.x + LUTTexSize.y;
	
    //vector LUTUV;
    //LUTUV.w = floor(TarggetDesc.b); // floor : 지정된 값보다 작거나 같은 가장 큰 정수 값을 반환
    //LUTUV.xy = (TarggetDesc.xy + 0.5f) * LUTTexSize;
    //LUTUV.x += LUTUV.w * LUTTexSize.y;
    //LUTUV.z = LUTUV.x + LUTTexSize.y;

	vector vMask = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector vFirstLUT = (1.f - vMask.w) * vector(lerp(g_LUTTexture.SampleLevel(DefaultSampler, LUTUV.xy, 0).rgb, g_LUTTexture.SampleLevel(DefaultSampler, LUTUV.zy, 0).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
	vector vSecondLUT = vMask.w * vector(lerp(g_DiffuseTexture.SampleLevel(DefaultSampler, LUTUV.xy, 0).rgb, g_DiffuseTexture.SampleLevel(DefaultSampler, LUTUV.zy, 0).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
	Out.vColor = vFirstLUT + vSecondLUT;
	// 2Dlod 
   // Out.vColor = vector(lerp(tex2Dlod(g_LUT, LUTUV.xyzz).rgb, tex2Dlod(g_LUT, LUTUV.zyzz).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
    //Out.vColor = vector(lerp(g_LUTTexture.SampleLevel(g_LUT, LUTUV.xyzz).rgb, g_LUTTexture.SampleLevel(g_LUT, LUTUV.zyzz).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
    //Out.vColor = vector(lerp(g_LUTTexture.SampleLevel(DefaultSampler, LUTUV.xy, 0).rgb, g_LUTTexture.SampleLevel(DefaultSampler, LUTUV.zy, 0).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
   // Out.vColor = vector(lerp(g_LUTTexture.Sample(DefaultSampler, LUTUV.xy).rgb, g_LUTTexture.Sample(DefaultSampler, LUTUV.zy).rgb, TarggetDesc.b - LUTUV.w).xyz, 1.f);
    return Out;
}

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV);
	vector	vVelocity = g_NormalTexture.Sample(ClampSampler, In.vTexUV);
	vector	vDepth = g_DepthTexture.Sample(ClampSampler, In.vTexUV);

	int		iNumBlurSample = 10;

	vVelocity.xy /= (float)iNumBlurSample;

	int		iCnt = 1;

	float4	vBColor;

	for (int i = iCnt; i < iNumBlurSample; i++)
	{
		vBColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV - vVelocity.xy * (float)i);
		if (vVelocity.w < vDepth.y + 0.04f)
		{
			++iCnt;
			vColor += vBColor;
		}
	}

	vColor /= (float)iCnt;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_RADIALBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_TargetTexture.Sample(ClampSampler, In.vTexUV);

	float2	dir = In.vTexUV - float2(0.5f, 0.5f);
	float4	sum = float4(0.f, 0.f, 0.f, 0.f);

	for (int i = 0; i < 11; i++)
	{
		float2 offset = In.vTexUV - dir * radialBlurSamples[i] * g_fBlurDist;

		offset = clamp(offset, float2(0.f, 0.f), float2(1.f, 1.f));

		sum += g_TargetTexture.Sample(ClampSampler, offset);
	}
	sum /= 11.f;

	Out.vColor = sum;

	return Out;
}

PS_OUT PS_MAIN_FOG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = g_TargetTexture.Sample(ClampSampler, In.vTexUV);
	vector	vDepth = g_DepthTexture.Sample(ClampSampler, In.vTexUV);

	float		fViewZ = vDepth.x * 300.f;

	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepth.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	float		fDistance = distance(vWorldPos, g_vCamPosition);

	float		fFogFactor = saturate(saturate((g_fFogEnd - fDistance) / (g_fFogEnd - g_fFogStart)) + g_fMaxFogFactor);

	Out.vColor = fFogFactor * vColor + (1.f - fFogFactor) * g_vFogColor;

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
	pass DebugBuffer			// 0 
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}	

	pass Light_Directional		// 1
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point			// 2
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend					// 3	
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass Blur_Hori				// 4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_X();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass Blur_Verti				// 5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_Y();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass UpSamling				// 6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UPSAMPLING();
	}

	pass Distortion				// 7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	pass Final				// 8
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

	pass Combine				// 9
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COMBINE();
	}

	pass ToneMapping			// 10
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING();
	}

	pass BloomMask			// 11
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOMMASK();
	}

	pass BloomMapping		// 12
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOMMAPPING();
	}

	pass AverLum			// 13
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AVERLUM();
	}

	pass CalcAverLum			// 14
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CALCAVERLUM();
	}

	pass DOF				// 15
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOF();
	}

	pass DOFRealBlurHori				// 16
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_X();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOFREALBLUR();
	}

	pass DOFRealBlurVerti			// 17
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_Y();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOFREALBLUR();
	}

    pass LUT // 18
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(NonZTestAndWriteState, 0);
        SetRasterizerState(CullMode_ccw);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUT();
    }

	pass MotionBlur // 19
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
	}


	pass RadialBlur // 20
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
	}

	pass Fog // 21
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FOG();
	}

	pass Blend_Outline	// 22
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND_OUTLINE();
	}

	pass Light_Directional_NotSSAO		// 23
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL_NOTSSAO();
	}
	
}