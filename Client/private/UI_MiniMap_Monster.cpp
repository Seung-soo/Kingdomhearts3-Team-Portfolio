#include "stdafx.h"
#include "..\public\UI_MiniMap_Monster.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Monster.h"
#include "Player.h"

CUI_MiniMap_Monster::CUI_MiniMap_Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_MiniMap_Monster::CUI_MiniMap_Monster(const CUI_MiniMap_Monster & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_MiniMap_Monster::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_MiniMap_Monster::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	memcpy(&pMonsterPointer, pArg, sizeof(MONSTERPOINTER));

	CHECK_FAILED(SetUp_Components());

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	m_fX = 0.f;
	m_fY = 0.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SizeX = 16.f;
	SizeY = 16.f;

	m_fDepth = 0.f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));

	//SetX = 640.f;
	//SetY = 360.f;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));

	return S_OK;
}

_int CUI_MiniMap_Monster::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
	{
		vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPlayerPos, vPlayerPos);
		fMonsterPos = *pMonsterPointer.fMonsterPos;

		fSrdPos = _float4(fPlayerPos.x - fMonsterPos.x, fPlayerPos.y - fMonsterPos.y, fPlayerPos.z - fMonsterPos.z, 1.f);

		final_position = XMVectorSet((fSrdPos.z * 4.f), -(fSrdPos.x * 4.f), 0.f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position);

	}

	if (LEVEL_RAPUNZEL == GM->Get_CurrentLevel())
	{
		vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPlayerPos, vPlayerPos);
		fMonsterPos = *pMonsterPointer.fMonsterPos;

		fSrdPos = _float4(fPlayerPos.x - fMonsterPos.x, fPlayerPos.y - fMonsterPos.y, fPlayerPos.z - fMonsterPos.z, 1.f);

		final_position = XMVectorSet(-(fSrdPos.x * 4.f),-(fSrdPos.z * 4.f) , 0.f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position);

	}
	
	return RESULT_NOPROBLEM;
}

_int CUI_MiniMap_Monster::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return RESULT_NOPROBLEM;
}

HRESULT CUI_MiniMap_Monster::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (true == GM->Get_Render_MiniMap())
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}

	return S_OK;
}

HRESULT CUI_MiniMap_Monster::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_Monster_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom))

	return S_OK;
}

HRESULT CUI_MiniMap_Monster::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	return S_OK;
}

CUI_MiniMap_Monster * CUI_MiniMap_Monster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_MiniMap_Monster*	pInstance = new CUI_MiniMap_Monster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_MiniMap_Monster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MiniMap_Monster::Clone(void * pArg)
{
	CUI_MiniMap_Monster*	pInstance = new CUI_MiniMap_Monster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_MiniMap_Monster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MiniMap_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
