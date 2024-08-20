#include "Shader_Defines.hpp"

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;

cbuffer	Setting_Player
{
	float		HP = 0.f;
	float		MP = 0.f;
	float		FOCUS = 0.f;

	float		HPBack = 0.f;
	float		MPBack = 0.f;
	float		FOCUSBack = 0.f;

	float		FadeOut = 0.f;
	float		MoneyBar = 0.f;
	float		StageLogo = 0.f;
	float		TBoxOpen = 0.f;
}

cbuffer Setting_Monster
{
	float		MONSTERHP = 0.f;
	float		MONSTERHP_BACK = 0.f;
	float		BOSSHP = 0.f;
	float		BOSSHP_BACK = 0.f;
};

cbuffer UVTest
{
	float		fUV_X = 0.f;
	float		fUV_Y = 0.f;
	float		fUV_R = 1.f;
	float		fUV_G = 1.f;
	float		fUV_B = 1.f;

	float		fUV_A = 1.f;
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
};

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_TEST(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//if (Out.vColor.a < 0.1f)
	//	discard;

	return Out;
}


PS_OUT PS_MAIN_HP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < HP)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < MP)
		discard;

	return Out;
}

PS_OUT PS_MAIN_FOCUS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < FOCUS)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HP_BACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < HPBack)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MP_BACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < MPBack)
		discard;

	return Out;
}

PS_OUT PS_MAIN_FOCUS_BACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < FOCUSBack)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MONSTER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < MONSTERHP)
		discard;

	return Out;
}

PS_OUT PS_MAIN_MONSTER_BACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.x < MONSTERHP_BACK)
		discard;

	return Out;
}


PS_OUT PS_MAIN_CommandTEXT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.9f)
		discard;

	Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT PS_MAIN_FADEOUT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	Out.vColor.a -= FadeOut;

	return Out;
}

PS_OUT PS_MAIN_MONEYBAR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	//if (In.vTexUV.y < MoneyBar)
	//	discard;

	Out.vColor.a -= MoneyBar;

	return Out;
}

PS_OUT PS_MAIN_STAGELOGO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	//if (In.vTexUV.y < MoneyBar)
	//	discard;

	Out.vColor.a -= StageLogo;

	return Out;
}

PS_OUT PS_MAIN_TBOXOPEN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	//if (In.vTexUV.y < MoneyBar)
	//	discard;

	Out.vColor.a -= TBoxOpen;
	if (Out.vColor.a == 0)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BOSSMONSTER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.y < BOSSHP)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BOSSMONSTER_BACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//UV = XY

	if (In.vTexUV.y < BOSSHP_BACK)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HIT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.r = Out.vColor.a;
	Out.vColor.gb = 0.f;

	Out.vColor.a -= TBoxOpen;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

technique11		DefaultTechnique
{
	pass Rect //0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}
	pass Hp //1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

	pass Mp //2
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MP();
	}

	pass Focus //3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FOCUS();
	}

	pass HpBack //4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP_BACK();
	}

	pass MpBack //5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MP_BACK();
	}

	pass FocusBack //6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FOCUS_BACK();
	}
	pass MonsterHp //7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTER();

	}
	pass MonsterHpBack //8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTER_BACK();
	}
	pass CommandText //9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CommandTEXT();
	}
	pass FadeOut //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FADEOUT();
	}
	pass CullNone //11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);
		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}
	pass Money_Bar //12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONEYBAR();
	}
	pass StageLogo //13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAGELOGO();
	}
	pass TBoxOpen //14
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TBOXOPEN();
	}
	pass Bosshp //15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOSSMONSTER();
	}

	pass Bosshp_back //16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOSSMONSTER_BACK();
	}

	pass PlayerHit //17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}
};