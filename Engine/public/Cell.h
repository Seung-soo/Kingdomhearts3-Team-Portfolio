#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

	enum TYPE {
		TYPE_TERRAIN,	// 터레인 높이로 태우는 타입
		TYPE_ROAD,		// 일반 타입
		TYPE_NOTROAD,	// 이동할 수 없는 타입
		TYPE_RUNNING,	// 프리런닝 발동 가능한 세로 네비 타입
		TYPE_FLOW,		// 프리플로 발동 가능한 네비 타입 => 가로, 세로 결정
		TYPE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	_uint Get_Index() { return m_iIndex; }

	_int Get_NeighborIndex(LINES eLine) { return m_iNeighbor[eLine]; }
	void Set_NeighborIndex(LINES eLine, _int iNeighborIndex) { m_iNeighbor[eLine] = iNeighborIndex;	}

	_float3	Get_Line(LINES eLine) { return m_vLineDir[eLine]; }
	_vector Get_Point(POINTS ePoint) { return XMLoadFloat3(&m_vPoints[ePoint]); }

	// PTR
	_float3*	Get_Points(POINTS ePoint) { return &m_vPoints[ePoint]; }

	// GameObject
	class CGameObject** Get_CellPoints();
	void Set_CellPoint(POINTS ePoint, class CGameObject* pGameObject) { m_pCellPoint[ePoint] = pGameObject; }

	// Values
	TYPE Get_Type() { return m_eType; }
	void Set_Type(TYPE eType) { m_eType = eType; }

	_float3 Get_NormalVector() { return m_vNormal; }
	_float3 Get_RightVector() { return m_vRight; }

	_float3 Get_HighestPoint() { return m_vPoints[m_eHighestPoint]; }
	_float HighestY() { return m_vPoints[m_eHighestPoint].y; }

	_float3 Get_LowestPoint() { return m_vPoints[m_eLowestPoint]; }
	_float LowestY() { return m_vPoints[m_eLowestPoint].y; }

	_float3 Get_HighestLine() { return m_vLineDir[m_eHighestLine]; }

	_float3 Get_Center() { return m_vCenter; }

public:
	HRESULT NativeConstruct(const _float3* pPoints, _uint iIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);	// 두 점이 같은지 판단 인접한 삼각형 찾는 과정
	_bool isIn(_fvector vPosition, _int* pNeighborIndex, _float3** pOut, _bool bClimb); // 입력받은 위치가 셀의 안에 있는지 확인, 없다면 나간 셀의 인덱스와 라인 정보를 참조형태에 대입
	_bool isIn_Return_Direction_And_Point(_fvector vPosition, _int* pNeighborIndex, _float3* pDir, _float3* pPos, _bool bClimb);

	_float Compute_Y(_fvector vPoint); // 평면의 방정식 사용, 입력받은 좌표에 대한 평면 상의 높이 반환
	_bool isAbove(_fvector vPoint, _float* pOut = nullptr); // 평면의 방정식을 사용, 입력받은 위치가 평면의 위에 있는지 판별, 높이값 차이 반환

	_bool isUpperSide(_fvector vPoint, _float* pOut = nullptr); // 평면의 방정식 사용, 입력받은 좌표가 평면의 윗면 쪽인지 아랫면 쪽인지 판별
	
	_float Get_CloseLine(_fvector vPoint, LINES* pOut = nullptr, _bool bXZ = false); // 입력받은 위치로부터 가장 가까운 라인까지의 거리 반환, out으로 타입 참조
	_float Compute_Distance(_fvector vPoint, LINES eLine, _bool bXZ = false); // 입력받은 위치와 입력받은 라인 간의 최소거리 반환

//#ifdef _DEBUG

public:
	void	CellPointsDead();

public:
	HRESULT	Render(class CVIBuffer_Triangle* pVIBuffer, class CShader* pShader, _float4 vColor);

//#endif

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

private:
	_int						m_iNeighbor[LINE_END] = { -1,-1,-1 };
	_float3						m_vPoints[POINT_END];
	_float3						m_vLineDir[LINE_END];
	_uint						m_iIndex = 0;

	class CGameObject*			m_pCellPoint[3];

	TYPE						m_eType = TYPE_END;
	_float3						m_vNormal;
	_float3						m_vRight;

	// 플롯값이 아니라 실제 포인트를 받아오게끔한다.
	// 그것도 _float3이 아니라 어느 포인트타입인지 저장하고 불러올 때 해당 타입을 불러오게끔
	// 셀의 높은 위치, 낮은 위치, 중점을 저장해두자
	POINTS						m_eHighestPoint = POINTS::POINT_END;
	POINTS						m_eLowestPoint = POINTS::POINT_END;

	LINES						m_eHighestLine = LINES::LINE_END;

	_float3						m_vCenter;

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END