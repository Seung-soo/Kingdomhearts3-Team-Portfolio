#include "stdafx.h"
#include "..\Public\UI_BossHealthBar_Inner.h"

#include "BeetleQueen.h"

CUI_BossHealthBar_Inner::CUI_BossHealthBar_Inner(ID3D11Device * pDevice, ID3D11DeviceContext * pDC)
	:CUI(pDevice, pDC)
{
}

CUI_BossHealthBar_Inner::CUI_BossHealthBar_Inner(const CUI_BossHealthBar_Inner & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_BossHealthBar_Inner::Init_Proto()
{
	if (FAILED(__super::Init_Proto()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossHealthBar_Inner::Init(void * pArg)
{
	if (FAILED(__super::Init(nullptr)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// Set_DefaultName();

	if (nullptr != pArg)
	{
		memcpy(&bossBar_Inner_Desc, pArg, sizeof(BOSS_INNER_DESC));
		m_pQueen = static_cast<CBeetleQueen*>(bossBar_Inner_Desc.unit);

		switch (bossBar_Inner_Desc.boss_bar_type)
		{
		case Client::CUI_BossHealthBar_Inner::BOSS_BAR_FRONT:
			m_EditionalColor = _float4(1.f, 0.f, 0.f, 1.f);
			m_pObjectName = TEXT("Boss Health Bar Inner - Front");
			break;
		case Client::CUI_BossHealthBar_Inner::BOSS_BAR_MID:
			m_EditionalColor = _float4(0.8476f, 0.3632f, 0.3632f, 1.f);
			m_pObjectName = TEXT("Boss Health Bar Inner - Middle");
			break;
		case Client::CUI_BossHealthBar_Inner::BOSS_BAR_BACK:
			m_EditionalColor = _float4(0.f, 0.f, 0.f, 1.f);
			m_pObjectName = TEXT("Boss Health Bar Inner - Back");
			break;
		}
	}

	m_fOriginCenterX = g_iWinCX * 0.5f;
	m_fOriginCenterY = 200.f;

	m_fOriginSizeX = 640.f;
	m_fOriginSizeY = 9.f;

	return S_OK;
}

_int CUI_BossHealthBar_Inner::Update(_double deltaTime)
{
	if (0 > __super::Update(deltaTime))
		return -1;

	if (true == m_bFirstUpdate)
	{
		if (nullptr == m_pQueen)
			m_pQueen = static_cast<CBeetleQueen*>(INST->Object(TEXT("[BOSS]Beetle Queen"), TEXT("Layer_Monster"), INST->SceneIndex()));

		if (nullptr == m_pQueen)
		{
			assert(SUCCEEDED(E_FAIL));
			return RETURN_INT_ERROR;
		}

		if (bossBar_Inner_Desc.boss_bar_type == CUI_BossHealthBar_Inner::BOSS_BAR_MID)
			m_pFront = static_cast<CUI_BossHealthBar_Inner*>(INST->Object(TEXT("Boss Health Bar Inner - Front"), TEXT("Layer_UI"), INST->SceneIndex()));

		m_HoldingPointLB = this->Get_Corner(CORNER_LB);
		m_HoldingPointCT = this->Center();
	}

	if (true == m_pQueen->Dead())
		m_bDead = true;

	switch (bossBar_Inner_Desc.boss_bar_type)
	{
	case Client::CUI_BossHealthBar_Inner::BOSS_BAR_FRONT:
	{
		COMBAT_DESC* boss_combat_data = m_pQueen->CombatData();

		m_fScaleX = boss_combat_data->fCurrent_HP / boss_combat_data->fMax_HP;
		Set_Corner(CORNER_LB, m_HoldingPointLB);

		m_CurrentHP_Text = to_wstring((_int)boss_combat_data->fCurrent_HP);
		m_MaxHP_Text = L" / " + to_wstring((_int)boss_combat_data->fMax_HP);
	}
		break;
	case Client::CUI_BossHealthBar_Inner::BOSS_BAR_MID:

		if (m_pFront->ScaleX() != m_fScaleX)
		{
			m_bChange = true;
		}
		else
		{
			m_bChange = false;
			m_dLerpTime = 0.f;
		}

		if (true == m_bChange)
		{
			m_fScaleX = LinearInterpolation(m_fScaleX, m_pFront->ScaleX(), (_float)(m_dLerpTime / 0.5));
			Set_Corner(CORNER_LB, m_HoldingPointLB);

			m_dLerpTime += deltaTime;
		}

		break;
	case Client::CUI_BossHealthBar_Inner::BOSS_BAR_BACK:
		// Nothing to do
		break;
	}

	//if (ImGui::CollapsingHeader("CUI_BossHealthBar_Inner"))
	//{
	//	_float center[2] = { m_fOriginCenterX , m_fOriginCenterY };
	//	_float size[2] = { m_fOriginSizeX , m_fOriginSizeY };
	//	_float color[4] = { m_EditionalColor.x, m_EditionalColor.y, m_EditionalColor.z, m_EditionalColor.w };
	//
	//	ImGui::DragFloat2("Center", center, 0.25f);
	//	ImGui::DragFloat2("Size", size, 0.25f);
	//	ImGui::DragFloat4("Color", color, 0.25f);
	//
	//	this->Center(_float2(center[0], center[1]));
	//	this->Size(_float2(size[0], size[1]));
	//
	//	m_EditionalColor = _float4(color[0], color[1], color[2], color[3]);
	//}

	return _int();
}

_int CUI_BossHealthBar_Inner::Late_Update(_double deltaTime)
{
	if (0 > __super::Late_Update(deltaTime))
		return -1;

	return _int();
}

HRESULT CUI_BossHealthBar_Inner::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (false == m_bRender)
		return S_OK;

	m_pTransform->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_EditionalColor", &m_EditionalColor, sizeof(_float4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 0)))
		return E_FAIL;

	if (CUI_BossHealthBar_Inner::BOSS_BAR_FRONT == bossBar_Inner_Desc.boss_bar_type)
	{
		_uint curr_length = (_uint)m_CurrentHP_Text.length();
		_uint max_length = (_uint)m_MaxHP_Text.length();

		//ImGui::Text("Current Legnth : %d", (_int)curr_length);
		//ImGui::Text("Max Legnth : %d", (_int)max_length);

		_float2 center = m_HoldingPointCT;

		//_float adj_x = ((curr_length + 1) * 4.f) + 6.f;
		_float adj_x = (max_length + 1) * 4.f;
		_float adj_y = 5.f;

		//ImGui::DragFloat("X factor", &adj_x, 0.25f, 0.f);
		//ImGui::DragFloat("Y factor", &adj_y, 0.25f, 0.f);

		center.x -= adj_x;
		center.y -= adj_y;

		INST->Render_Font(m_pDC, TEXT("NSB_8"), m_CurrentHP_Text.c_str(), center, XMVectorSet(1.f, 1.f, 1.f, 1.f));

		static _float adj_extra = 0.f;
		//ImGui::DragFloat("Extra factor", &adj_extra, 0.25f, 0.f);

		center.x = m_HoldingPointCT.x - adj_extra;

		INST->Render_Font(m_pDC, TEXT("NSB_8"), m_MaxHP_Text.c_str(), center, XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}
	
	return S_OK;
}

void CUI_BossHealthBar_Inner::Set_DefaultName()
{
	m_pObjectName = TEXT("Boss Health Bar Inner");
}

HRESULT CUI_BossHealthBar_Inner::SetUp_Components()
{
	SCENE next_scene = SYS->NextScene();

	if (FAILED(__super::Add_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(SCENE_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(next_scene, TEXT("Prototype_Component_Texture_UI_BossHealthBar_Inner"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CUI_BossHealthBar_Inner * CUI_BossHealthBar_Inner::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDC)
{
	CUI_BossHealthBar_Inner*	pInstance = new CUI_BossHealthBar_Inner(pDevice, pDC);

	if (FAILED(pInstance->Init_Proto()))
	{
		MSGBOX("Failed to Created CUI_BossHealthBar_Inner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_BossHealthBar_Inner::Clone(void * pArg)
{
	CUI_BossHealthBar_Inner*	pInstance = new CUI_BossHealthBar_Inner(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSGBOX("Failed to Clone CUI_BossHealthBar_Inner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossHealthBar_Inner::Free()
{
	__super::Free();
}
