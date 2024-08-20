#include "Shader_Defines.hpp"

struct VS_IN
{
    float4 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV         = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP        = mul(matWV, g_ProjMatrix);
    Out.vPosition = mul(In.vPosition, matWVP);
    Out.vWorldPos = mul(In.vPosition, g_WorldMatrix);

    return Out;
}

// ===================================================================================================================================================================== //

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPos : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vWorldPos.y == 0.f && In.vWorldPos.z == 0.f)
        Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
    else if (In.vWorldPos.x == 0.f && In.vWorldPos.z == 0.f)
        Out.vColor = float4(0.f, 1.f, 1.f, 1.f);
    else if (In.vWorldPos.x == 0.f && In.vWorldPos.y == 0.f)
        Out.vColor = float4(1.f, 1.f, 0.f, 1.f);

    return Out;
}

// ===================================================================================================================================================================== //

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(NonZTestAndWriteState, 0);
        SetRasterizerState(CullMode_none);

        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader    = compile ps_5_0 PS_MAIN();
    }
}