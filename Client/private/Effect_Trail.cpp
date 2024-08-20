#include "stdafx.h"
#include "..\public\Effect_Trail.h"
#include "GameInstance.h"
#include "VIBuffer_Trail.h"
#include "ImGui_Manager.h"

CEffect_Trail::CEffect_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Trail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(nullptr)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_tDesc, pArg, sizeof(CEffect_Trail::TRAILDESC));

		m_vColor = m_tDesc.vColor;

		m_pTargetTransform = m_tDesc.pTargetTransform;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CEffect_Trail::Tick(_double TimeDelta)
{
	_matrix World;

	if (false == m_tDesc.isNotBone)
	{
		World = XMLoadFloat4x4(m_tDesc.pBoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_tDesc.SocketPivotmMatrix) * m_tDesc.pTargetTransform->Get_WorldMatrix();
	}

	else
	{
		World = XMLoadFloat4x4(m_tDesc.pBoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_tDesc.SocketPivotmMatrix) * m_tDesc.pTargetTransform->Get_WorldMatrix() * m_tDesc.LocalMatrix;
	}


	if (false == m_isActive)
	{
		return 0;
	}

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}


	return _int();
}

_int CEffect_Trail::LateTick(_double TimeDelta)
{
	_matrix World;

	if (false == m_tDesc.isNotBone)
	{
		World = XMLoadFloat4x4(m_tDesc.pBoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_tDesc.SocketPivotmMatrix) * m_tDesc.pTargetTransform->Get_WorldMatrix();
	}

	else
	{
		World = XMLoadFloat4x4(m_tDesc.pBoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_tDesc.SocketPivotmMatrix) * m_tDesc.pTargetTransform->Get_WorldMatrix() * m_tDesc.LocalMatrix;
	}

	if (false == m_isActive)
	{
		if (m_isEnding)
		{
			auto vCenter = World.r[3];

			_float3		vPosition[2];

			auto vUp = XMVectorSet(0.f, 1.0f, 0.f, 1.f);
			auto vDown = XMVectorSet(0.f, -1.0f, 0.f, 1.f);

			vUp = XMVector3TransformCoord(vUp, World);
			vDown = XMVector3TransformCoord(vDown, World);

			//upposition
			XMStoreFloat3(&vPosition[0], vCenter - XMVector3Normalize(World.r[0]));
			//XMStoreFloat3(&vPosition[0], vUp);

			//Downposition
			XMStoreFloat3(&vPosition[1], vCenter - XMVector3Normalize(World.r[0] * 0.2f));
			//XMStoreFloat3(&vPosition[0], vDown);

			m_pBufferCom->UpdateTrail(vPosition[0], vPosition[1], (_float)TimeDelta);

			m_pBufferCom->Update_EndMotion((_float)TimeDelta, &World);

			if (m_pBufferCom->Is_TrailFinished())
			{
				m_pBufferCom->Reset_Trail();
				m_isEnding = false;
			}
		}

		return 0;
	}

	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}



	m_pTransformCom->Set_WorldMatrix(World);

	auto vCenter = World.r[3];

	_float3		vPosition[2];

	if (false == m_tDesc.isWind)
	{
	//upposition
	XMStoreFloat3(&vPosition[0], vCenter + m_tDesc.vAdjustValue);
	//Downposition
	XMStoreFloat3(&vPosition[1], vCenter);
	}

	else
	{
		//upposition
		XMStoreFloat3(&vPosition[0], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()));
		//XMStoreFloat3(&vPosition[0], vUp);

		//Downposition
		XMStoreFloat3(&vPosition[1], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()) * 0.2f);
		//XMStoreFloat3(&vPosition[0], vDown);
	}

	m_pBufferCom->AddNewTrail(vPosition[0], vPosition[1], (_float)TimeDelta);

	m_pBufferCom->Update((_float)TimeDelta, &World);

	if (true == m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CEffect_Trail::Render()
{
	if(m_isActive == false)
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 컬러 추가함 확인해봐야함
	m_pBufferCom->Render(m_pShaderCom, m_tDesc.iPassIndex);
	
	return S_OK;
}

void CEffect_Trail::Set_Active(_bool isActive)
{
	if (false == isActive)
	{
		if (true == m_isActive)
			m_isEnding = true;

		else
			m_pBufferCom->Reset_Trail();

		m_isActive = false;
	}

	else
	{
		m_isActive = true;
	}
}

void CEffect_Trail::PostRender(_double TimeDelta)
{	
	ImGui::Begin("Trail"); 

	ImGui::Text("Active : %d", m_pBufferCom->Get_TrailDataSize());

	ImGui::End();
}

HRESULT CEffect_Trail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Trail's Renderer!");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Trail's Shader!");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Trail's Texture!");
		return E_FAIL;
	}

	/* For.Com_Buffer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_Buffer"), (CComponent**)&m_pBufferCom, &m_tDesc.tTrailBuffDesc)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Trail's Buffer!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Trail::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_tDesc.iTextureIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &m_tDesc.bDistortion, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &m_tDesc.bBlur, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vBlendingColor", &m_vColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTranslucency", &m_tDesc.fTransparency, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Set_Texture("g_SourTexture", pGameInstance->Get_RenderTargetSRV(TEXT("Target_Depth")))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Trail::Ending_Motion(_double TimeDelta)
{
	auto World = m_tDesc.pTargetTransform->Get_WorldMatrix();

	auto vCenter = World.r[3];

	_float3		vPosition[2];

	auto vUp = XMVectorSet(0.f, 1.0f, 0.f, 1.f);
	auto vDown = XMVectorSet(0.f, -1.0f, 0.f, 1.f);

	vUp = XMVector3TransformCoord(vUp, World);
	vDown = XMVector3TransformCoord(vDown, World);

	//upposition
	XMStoreFloat3(&vPosition[0], vCenter - XMVector3Normalize(m_tDesc.pTargetTransform->Get_Right()));

	//Downposition
	XMStoreFloat3(&vPosition[1], vCenter - XMVector3Normalize(m_tDesc.pTargetTransform->Get_Right()) * 0.2f);
	return S_OK;
}

CEffect_Trail * CEffect_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Trail*	pInstance = new CEffect_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect_Trail * CEffect_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	return nullptr;
}

CGameObject * CEffect_Trail::Clone(void * pArg)
{
	CEffect_Trail*	pInstance = new CEffect_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
}
