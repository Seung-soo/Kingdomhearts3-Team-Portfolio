#include "stdafx.h"
#include "..\public\Effect_VISwordTrail.h"
#include "GameInstance.h"
#include "VIBuffer_Trail.h"
#include "ImGui_Manager.h"

CEffect_VISwordTrail::CEffect_VISwordTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_VISwordTrail::CEffect_VISwordTrail(const CEffect_VISwordTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_VISwordTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	//line_strip

	return S_OK;
}

HRESULT CEffect_VISwordTrail::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(nullptr)))
		return E_FAIL;

	m_fInterval = 0.5f;

	if (nullptr != pArg)
		m_pTargetTransform = *(CTransform**)pArg;

	Safe_AddRef(m_pTargetTransform);

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_isActive = false;

	m_bImGuiController = true;

	return S_OK;
}

_int CEffect_VISwordTrail::Tick(_double TimeDelta)
{
	if (false == m_isActive)
	{
		if (m_isEnding)
		{
			auto World = m_pTargetTransform->Get_WorldMatrix();

			auto vCenter = World.r[3];

			_float3		vPosition[2];

			auto vUp = XMVectorSet(0.f, 1.0f, 0.f, 1.f);
			auto vDown = XMVectorSet(0.f, -1.0f, 0.f, 1.f);

			vUp = XMVector3TransformCoord(vUp, World);
			vDown = XMVector3TransformCoord(vDown, World);

			//upposition
			XMStoreFloat3(&vPosition[0], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()));
			//XMStoreFloat3(&vPosition[0], vUp);

			//Downposition
			XMStoreFloat3(&vPosition[1], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()) * 0.2f);
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

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	auto World = m_pTargetTransform->Get_WorldMatrix();

	auto vCenter = World.r[3];

	_float3		vPosition[2];

	auto vUp = XMVectorSet(0.f, 1.0f, 0.f, 1.f);
	auto vDown = XMVectorSet(0.f, -1.0f, 0.f, 1.f);

	vUp = XMVector3TransformCoord(vUp, World);
	vDown = XMVector3TransformCoord(vDown, World);

	//upposition
	XMStoreFloat3(&vPosition[0], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()));
	//XMStoreFloat3(&vPosition[0], vUp);

	//Downposition
	XMStoreFloat3(&vPosition[1], vCenter - XMVector3Normalize(m_pTargetTransform->Get_Right()) * 0.2f);
	//XMStoreFloat3(&vPosition[0], vDown);


	m_pBufferCom->AddNewTrail(vPosition[0], vPosition[1], (_float)TimeDelta);

	m_pBufferCom->Update((_float)TimeDelta, &World);

	return _int();
}

_int CEffect_VISwordTrail::LateTick(_double TimeDelta)
{
	if (false == m_isActive)
		return 0;

	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return _int();
}

HRESULT CEffect_VISwordTrail::Render()
{
	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 컬러 추가함 확인해봐야함
	m_pBufferCom->Render(m_pShaderCom, 19);

	return S_OK;
}

void CEffect_VISwordTrail::PostRender(_double TimeDelta)
{
	//ImGui::Begin("Trail"); 

	//ImGui::Text("Active : %d", m_pBufferCom->Get_TrailDataSize());

	//ImGui::End();
}

void CEffect_VISwordTrail::Set_Active(_bool isActive)
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

HRESULT CEffect_VISwordTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_VISwordTrail's Renderer!");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_VISwordTrail's Shader!");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_VISwordTrail's Texture!");
		return E_FAIL;
	}

	CVIBuffer_Trail::TRAILDESC		tDesc;

	tDesc.iNumVerticies = 1000;
	tDesc.fLifeTime = 0.5f;
	tDesc.fLerpCnt = 10.f;
	tDesc.fDuration = 0.005f; 

	/* For.Com_Buffer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_Buffer"), (CComponent**)&m_pBufferCom, &tDesc)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_VISwordTrail's Buffer!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_VISwordTrail::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTargetTransform->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
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
	     
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 341)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	_float		fDistor = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fDistor, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	bool		bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	_float4		vColor = _float4(1.0f, 1.0f, 1.0f, 0.6f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vBlendingColor", &vColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	_float		fTransparency = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTranslucency", &fTransparency, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	_float		fDistortion = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fDistortion, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}


CEffect_VISwordTrail * CEffect_VISwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_VISwordTrail*	pInstance = new CEffect_VISwordTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect_VISwordTrail * CEffect_VISwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	return nullptr;
}

CGameObject * CEffect_VISwordTrail::Clone(void * pArg)
{
	CEffect_VISwordTrail*	pInstance = new CEffect_VISwordTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_VISwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTargetTransform);
}
