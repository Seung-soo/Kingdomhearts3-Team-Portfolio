#include "stdafx.h"
#include "Terrain.h"

#include "GameInstance.h"
#include "TerrainTool.h"
#include "GameManager.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC transform_desc;
	transform_desc.fSpeedPerSec    = 5.f;
	transform_desc.fRotationPerSec = XMConvertToRadians(90.0f);
	transform_desc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&transform_desc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_iWidth = m_pVIBufferCom->NumX();
	m_iDepth = m_pVIBufferCom->NumZ();

	return S_OK;
}

_int CTerrain::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

	return RESULT_NOPROBLEM;
}

_int CTerrain::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CTerrain::Render()
{
	if (nullptr == m_pVIBufferCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(__super::Render()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, m_iPass)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CTerrain::PostRender(_double TimeDelta)
{

}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorMulTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_BrushTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TerrainBrush"), TEXT("Com_BrushTexture"), (CComponent**)&m_pBrushTextureCom)))
		return E_FAIL;

	/* For.Com_FieldTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TestTerrain"), TEXT("Com_FieldTexture"), (CComponent**)&m_pFieldTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// 행렬 정보
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	// 텍스쳐 정보
	if (FAILED(m_pFieldTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture1", 0)))
		return E_FAIL;
	if (FAILED(m_pFieldTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture2", 1)))
		return E_FAIL;
	if (FAILED(m_pFieldTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture3", 2)))
		return E_FAIL;
	if (FAILED(m_pFieldTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture4", 3)))
		return E_FAIL;

	// 여기서부터는 맵툴에서만 진행할 브러시 관련 코드
	if (LEVEL_MAPTOOL != m_pGameInstance->Get_LevelIndex())
		return S_OK;

	// 터레인 툴 클래스가 생성되지 않았음
	if (nullptr == CTerrainTool::GetInstance())
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 브러시 텍스쳐
	if (FAILED(m_pBrushTextureCom->SetUp_OnShader(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;
#ifdef _DEBUG
	// 브러시는 어차피 맵툴에서만 띄울 것. 터레인 툴에 피킹 위치 저장하면서 받아와서 적용.
	_float4 picked_point = CTerrainTool::GetInstance()->m_vPickedPoint;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &picked_point, sizeof(_float4))))
		return E_FAIL;

	// 현재 진행 중인 툴 작업이 posbrush인지 texbrush인지 확인 후 조건에 맞게 적용
	_bool pos_brush = CTerrainTool::GetInstance()->m_isPosBrush;
	if (true == pos_brush)
	{
		// 포지션 브러시
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRange", &CTerrainTool::GetInstance()->m_PosBrush.brush_range, sizeof(_float))))
			return E_FAIL;
	}
	else
	{
		// 텍스쳐 브러시
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRange", &CTerrainTool::GetInstance()->m_TexBrush.brush_range, sizeof(_float))))
			return E_FAIL;
	}
#endif
	return S_OK;
}

_vector CTerrain::Compute_PositionOnTerrain(_fvector _position)
{
	// 예외처리 후 진행
#pragma region 터레인을 벗어난 포지션에 대한 예외처리

	_float3 position;
	XMStoreFloat3(&position, _position);

	_float3 this_position;
	XMStoreFloat3(&this_position, m_pTransformCom->Get_Position());

	if (position.x < this_position.x ||
		position.x > this_position.x + m_iWidth - 1 ||
		position.z < this_position.z ||
		position.z > this_position.z + m_iDepth - 1)
		return XMVectorZero();

#pragma endregion

	// 월드 역행렬
	_matrix world_mat_inv = m_pTransformCom->Get_WorldMatrixInverse();

	// 들어온 포지션과 연산해서 들어온 포지션을 터레인의 로컬 스페이스 상의 좌표로 변환
	_vector input_position_in_this_local = XMVector3TransformCoord(_position, world_mat_inv);

	// 변환된 XZ 좌표상의 Y높이를 측정 후 해당 값으로 세팅
	vecy(input_position_in_this_local) = m_pVIBufferCom->Compute_Y(input_position_in_this_local);

	// 결과값을 다시 월드 스페이스로 변환 후 반환
	return XMVector3TransformCoord(input_position_in_this_local, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CTerrain::Load_TerrainData(const _tchar * pFilePath)
{
	if (nullptr == m_pVIBufferCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pVIBufferCom->Load_Terrain(pFilePath);
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBrushTextureCom);
	Safe_Release(m_pFieldTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
