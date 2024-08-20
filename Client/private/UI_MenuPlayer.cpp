#include "stdafx.h"
#include "..\public\UI_MenuPlayer.h"
#include "GameInstance.h"

CUI_MenuPlayer::CUI_MenuPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUI_MenuPlayer::CUI_MenuPlayer(const CUI_MenuPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_MenuPlayer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MenuPlayer::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	CHECK_FAILED(SetUp_Components());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	m_fDepth = 0.f;

	m_pTransformCom->Scaled(XMVectorSet(64.f, 64.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	/* ========== */

	// Target Collider Position
	//_vector target_position = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_TWILIGHT, TEXT("Layer_Camera")))->Get_State(CTransform::STATE_POSITION);
	//_vector target_position_Look = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_TWILIGHT, TEXT("Layer_Camera")))->Get_State(CTransform::STATE_LOOK);

	//_vector target_position = m_pGameInstance->Get_Front(LEVEL_TWILIGHT, TEXT("Layer_Camera"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//_vector target_position_Look = m_pGameInstance->Get_Front(LEVEL_TWILIGHT, TEXT("Layer_Camera"))->Get_Transform()->Get_State(CTransform::STATE_LOOK);

	//_vector	Player_State = target_position + (XMVector4Normalize(target_position_Look) * 2.f);
	//// 뷰 스페이스로 연산
	//_matrix view_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	//_vector position_in_view_space = XMVector3TransformCoord(Player_State, view_mat);

	//// 투영 스페이스로 연산
	//_matrix proj_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	//_vector position_in_proj_space = XMVector3TransformCoord(position_in_view_space, proj_mat);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, position_in_proj_space);

	
	return S_OK;
}

_int CUI_MenuPlayer::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_KeyEnter(DIK_N))
		m_bDead = true;

	return _int();
}

_int CUI_MenuPlayer::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	/* 투영 */

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_MenuPlayer::Render()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 모델이 갖고있는 머테리얼 수 가져와서 쉐이더의 텍스처 전역 상수에 값 전달하고 렌더링
	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CUI_MenuPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Model */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sora"), TEXT("Com_Model"), (CComponent**)&m_pModelCom));

	return S_OK;
}

HRESULT CUI_MenuPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	//CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4)));
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4)));

	return S_OK;
}

CUI_MenuPlayer * CUI_MenuPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_MenuPlayer*	pInstance = new CUI_MenuPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_MenuPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MenuPlayer::Clone(void * pArg)
{
	CUI_MenuPlayer*	pInstance = new CUI_MenuPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_MenuPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;;
}

void CUI_MenuPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
