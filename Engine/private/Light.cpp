#include "..\public\Light.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & LightDesc)
{
	memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHTDESC));

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (nullptr == pShader)
	{
		MSGBOX("nullptr == pShader in CLight::Render");
		return E_FAIL;
	}

	_uint		iPassIndex = 2;

	if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightPos)");
		return E_FAIL;
	}
	if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_fLightRange)");
		return E_FAIL;
	}
	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightDiffuse)");
		return E_FAIL;
	}
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightAmbient)");
		return E_FAIL;
	}
	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightSpecular)");
		return E_FAIL;
	}

	return pVIBuffer->Render(pShader, iPassIndex);
}

HRESULT CLight::Render_DirectionalLight(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _bool isRenderShadow)
{

	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightDir)");
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_LightViewMatrix", &CGameInstance::GetInstance()->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_LightProjMatrix", &CGameInstance::GetInstance()->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_LightViewMatrix_Static", &CGameInstance::GetInstance()->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_LightProjMatrix_Static", &CGameInstance::GetInstance()->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float		fBias = 0.2f;
	if (FAILED(pShader->Set_RawValue("g_fBias", &fBias, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightDiffuse)");
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightAmbient)");
		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_vLightSpecular)");
		return E_FAIL;
	}

	_bool bRenderShadow = isRenderShadow;

	if (FAILED(pShader->Set_RawValue("g_bRenderShadow", &bRenderShadow, sizeof(_bool))))
	{
		MSGBOX("pShader->Set_RawValue returned E_FAIL in CLight::Render(g_bRenderShadow)");
		return E_FAIL;
	}

	_uint iPassIndex = 1;
	if (false == m_bSSAO)
		iPassIndex = 23;

	return pVIBuffer->Render(pShader, iPassIndex);
}

_float4x4 CLight::Get_LightViewMatrix()
{
	// LightDesc.vPosition은 플레이어 위치가 될것이고 vLightPos는 라이트 위치로 만들어 줄것
	_float4x4			LightViewMatrix;

	auto				vFixedPos = __vector(-m_LightDesc.vDirection.x * m_fLightDistance, -m_LightDesc.vDirection.y * m_fLightDistance, -m_LightDesc.vDirection.z * m_fLightDistance, 0.f);
	
	auto	vLightPos = XMLoadFloat4(&m_LightDesc.vPosition) + vFixedPos;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(vLightPos, XMLoadFloat4(&m_LightDesc.vPosition), __vector(0.f, 1.f, 0.f, 0.f))));
	
	return LightViewMatrix;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSGBOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
