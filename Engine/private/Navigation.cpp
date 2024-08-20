#include "..\public\Navigation.h"
#include "Transform.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Cell.h"
#include "GameObject.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
//#ifdef _DEBUG
	, m_pVIBuffer(rhs.m_pVIBuffer)
	, m_pShader(rhs.m_pShader)
//#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
//#ifdef _DEBUG
	Safe_AddRef(m_pVIBuffer);
	Safe_AddRef(m_pShader);
//#endif
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNaviDataFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	_uint		iType = 0;
	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*	pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;
		pCell->Set_Type((CCell::TYPE)iType);
		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

//#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		BREAKPOINT;
		return E_FAIL;
	}
//#endif

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));

	// 생성될 때 어느 셀에 올라가있는지 찾는다
	Find_Cell(m_NaviDesc.pParent->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

_bool CNavigation::Move_OnNavigation(_fvector vPosition, _fvector vCurrentPosition, _float3** pSlidingVector)
{
	if (true == m_bIgnore)
		return MOVABLE;

	m_NaviDesc.iPreviousFrame = m_NaviDesc.iCurrentIndex;

	_int iNeighborIndex = -1;
	_bool is_in = m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex, pSlidingVector, m_bClimb);

	// is_in이 false여서 iNeighborIndex가 갱신되었을 때
	if (-1 != iNeighborIndex)
	{
		CCell::TYPE cell_type = m_Cells[iNeighborIndex]->Get_Type();
		_float3 cell_center = m_Cells[iNeighborIndex]->Get_Center();
		_float highest = m_Cells[iNeighborIndex]->HighestY();

		if (CCell::TYPE_RUNNING == cell_type)
		{
			if(vecy(vPosition) < highest && false == m_bClimb)
				return UNMOVABLE;

			// if(vecy(vPosition) > cell_center.y)
		}
		/*if (false == m_bClimb && CCell::TYPE_RUNNING == cell_type)
			return UNMOVABLE;*/
	}
	
	// 셀 안에 있다
	if (true == is_in)
		return MOVABLE;

	// 위에서 return되지 않았다 == 셀 안에 없다
	// Neighbor 셀이 없다(셀이 없는 곳으로 넘어왔다)
	if (0 > iNeighborIndex)
		return UNMOVABLE;

	// 위에서 return되지 않았다 == Neighbor 셀이 있다
	// Neighbor 셀의 타입을 가져와서 분기
	CCell::TYPE cell_type = m_Cells[iNeighborIndex]->Get_Type();
	switch (cell_type)
	{
	// 옮겨갈 수 있는 cell type일 경우
	case Engine::CCell::TYPE_TERRAIN:
	case Engine::CCell::TYPE_ROAD:
	case Engine::CCell::TYPE_FLOW:
		
		while (true)
		{
			// 옮겨갈 수 있는 이웃cell이지만 어떠한 상황, 조건에 의해서 해당 이웃셀 마저 넘어가진 않았는지 검사해줘야한다.
			// 하나 혹은 그 이상의 셀을 뛰어넘고 다른 셀로 넘어갔을 때에 대한 처리임.

			_int iCurrnetNeighborIndex = -1;
			_bool is_in_current = m_Cells[iNeighborIndex]->isIn(vPosition, &iCurrnetNeighborIndex, pSlidingVector, m_bClimb);

			// 옮겨온 셀 안에 있다
			if (true == is_in_current)
			{
				// 인덱스 갱신, 슬라이딩 벡터 폐기
				m_NaviDesc.iPreviousIndex = m_NaviDesc.iCurrentIndex;
				m_NaviDesc.iCurrentIndex = iNeighborIndex;

				if (nullptr != pSlidingVector)
					*pSlidingVector = nullptr;

				return MOVABLE;
			}

			// 위에서 return되지 않았다 == 옮겨온 셀 안에 없다
			// Neighbor 셀이 없다(셀이 없는 곳으로 넘어왔다)
			if (0 > iCurrnetNeighborIndex)
				return UNMOVABLE;

			iNeighborIndex = iCurrnetNeighborIndex;
		}

		break;

	// 옮겨갈 수 없는 cell type일 경우
	case Engine::CCell::TYPE_NOTROAD:

		break;

	case Engine::CCell::TYPE_RUNNING:

		while (true)
		{
			_int iCurrnetNeighborIndex = -1;
			_bool is_in_current = m_Cells[iNeighborIndex]->isIn(vPosition, &iCurrnetNeighborIndex, pSlidingVector, m_bClimb);

			// 옮겨온 셀 안에 있다
			if (true == is_in_current)
			{
				// 인덱스 갱신, 슬라이딩 벡터 폐기
				m_NaviDesc.iPreviousIndex = m_NaviDesc.iCurrentIndex;
				m_NaviDesc.iCurrentIndex = iNeighborIndex;

				if (nullptr != pSlidingVector)
					*pSlidingVector = nullptr;

				return MOVABLE;
			}

			// 위에서 return되지 않았다 == 옮겨온 셀 안에 없다
			// Neighbor 셀이 없다(셀이 없는 곳으로 넘어왔다)
			if (0 > iCurrnetNeighborIndex)
				return UNMOVABLE;

			iNeighborIndex = iCurrnetNeighborIndex;
		}

		break;
	}

	return UNMOVABLE;
}

_bool CNavigation::Move_OnNavigation_NotRunning(_fvector vPosition, _fvector vCurrentPosition, _float3 ** pSlidingVector)
{
	if (true == m_bIgnore)
		return MOVABLE;

	m_NaviDesc.iPreviousFrame = m_NaviDesc.iCurrentIndex;

	_int iNeighborIndex = -1;
	_bool is_in = m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex, pSlidingVector, m_bClimb);

	// is_in이 false여서 iNeighborIndex가 갱신되었을 때
	if (-1 != iNeighborIndex)
	{
		CCell::TYPE cell_type = m_Cells[iNeighborIndex]->Get_Type();
		_float3 cell_center = m_Cells[iNeighborIndex]->Get_Center();
		_float highest = m_Cells[iNeighborIndex]->HighestY();

		if (CCell::TYPE_RUNNING == cell_type)
		{
		}
	}

	// 셀 안에 있다
	if (true == is_in)
		return MOVABLE;

	// 위에서 return되지 않았다 == 셀 안에 없다
	// Neighbor 셀이 없다(셀이 없는 곳으로 넘어왔다)
	if (0 > iNeighborIndex)
		return UNMOVABLE;

	// 위에서 return되지 않았다 == Neighbor 셀이 있다
	// Neighbor 셀의 타입을 가져와서 분기
	CCell::TYPE cell_type = m_Cells[iNeighborIndex]->Get_Type();
	switch (cell_type)
	{
		// 옮겨갈 수 있는 cell type일 경우
	case Engine::CCell::TYPE_TERRAIN:
	case Engine::CCell::TYPE_ROAD:
	case Engine::CCell::TYPE_FLOW:

		while (true)
		{
			// 옮겨갈 수 있는 이웃cell이지만 어떠한 상황, 조건에 의해서 해당 이웃셀 마저 넘어가진 않았는지 검사해줘야한다.
			// 하나 혹은 그 이상의 셀을 뛰어넘고 다른 셀로 넘어갔을 때에 대한 처리임.

			_int iCurrnetNeighborIndex = -1;
			_bool is_in_current = m_Cells[iNeighborIndex]->isIn(vPosition, &iCurrnetNeighborIndex, pSlidingVector, m_bClimb);

			// 옮겨온 셀 안에 있다
			if (true == is_in_current)
			{
				// 인덱스 갱신, 슬라이딩 벡터 폐기
				m_NaviDesc.iPreviousIndex = m_NaviDesc.iCurrentIndex;
				m_NaviDesc.iCurrentIndex = iNeighborIndex;

				if (nullptr != pSlidingVector)
					*pSlidingVector = nullptr;

				return MOVABLE;
			}

			// 위에서 return되지 않았다 == 옮겨온 셀 안에 없다
			// Neighbor 셀이 없다(셀이 없는 곳으로 넘어왔다)
			if (0 > iCurrnetNeighborIndex)
				return UNMOVABLE;

			iNeighborIndex = iCurrnetNeighborIndex;
		}

		break;

		// 옮겨갈 수 없는 cell type일 경우
	case Engine::CCell::TYPE_NOTROAD:

		break;

	case Engine::CCell::TYPE_RUNNING:
		return MOVABLE;
	}

	return UNMOVABLE;
}

_float CNavigation::Compute_Y(_fvector vPoint)
{
	_float4		vPlane;
	
	XMStoreFloat4(&vPlane, XMPlaneFromPoints(
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C)
	));

	return (-vPlane.x * XMVectorGetX(vPoint) - vPlane.z * XMVectorGetZ(vPoint) - vPlane.w) / vPlane.y;
}

_bool CNavigation::isAbove(_fvector vPoint)
{
	return Compute_Y(vPoint) <= vecy(vPoint) ? true : false;
}

_bool CNavigation::isIn(_fvector vPosition, _int * pNeighborIndex, _float3 ** pOut, _bool bClimb)
{
	return m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, pNeighborIndex, pOut, bClimb);
}

_bool CNavigation::Check_PrevAndCurrIndex()
{
	return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Type() == m_Cells[m_NaviDesc.iPreviousIndex]->Get_Type() ? true : false;
}

_bool CNavigation::Check_PrevAndCurrFrame()
{
	return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Type() == m_Cells[m_NaviDesc.iPreviousFrame]->Get_Type() ? true : false;
}

void CNavigation::Find_Cell(_fvector vPosition)
{
	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		_int iCurrnetNeighborIndex = -1;
		_float3* pNotUse = nullptr;

		if (CCell::TYPE_RUNNING == m_Cells[i]->Get_Type())
			continue;

		if (true == m_Cells[i]->isIn(vPosition, &iCurrnetNeighborIndex, &pNotUse, m_bClimb))
		{
			m_NaviDesc.iCurrentIndex = (_uint)i;
			m_NaviDesc.iPreviousIndex = (_uint)i;
			break;
		}
	}
}
//#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pVIBuffer)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CPipeLine*	pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint	iIndex = 0;

	/*if (-1 == m_NaviDesc.iCurrentIndex)
	{*/
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				// 현재 위치한 셀이면 빨강, 아니면 초록
				pCell->Render(m_pVIBuffer, m_pShader, iIndex++ == m_NaviDesc.iCurrentIndex ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f));
			}
		}
	/*}
	else
	{
		m_Cells[m_NaviDesc.iCurrentIndex]->Render(m_pVIBuffer, m_pShader, _float4(1.f, 0.f, 0.f, 1.f));
	}*/

	 

	return S_OK;
}
//#endif
HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell->Get_Index());
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell->Get_Index());
			}
		}
	}
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNaviDataFilePath)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNaviDataFilePath)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();
//#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
//#endif
}
