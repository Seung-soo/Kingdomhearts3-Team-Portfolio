#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

	enum TYPE {
		TYPE_TERRAIN,	// �ͷ��� ���̷� �¿�� Ÿ��
		TYPE_ROAD,		// �Ϲ� Ÿ��
		TYPE_NOTROAD,	// �̵��� �� ���� Ÿ��
		TYPE_RUNNING,	// �������� �ߵ� ������ ���� �׺� Ÿ��
		TYPE_FLOW,		// �����÷� �ߵ� ������ �׺� Ÿ�� => ����, ���� ����
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
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);	// �� ���� ������ �Ǵ� ������ �ﰢ�� ã�� ����
	_bool isIn(_fvector vPosition, _int* pNeighborIndex, _float3** pOut, _bool bClimb); // �Է¹��� ��ġ�� ���� �ȿ� �ִ��� Ȯ��, ���ٸ� ���� ���� �ε����� ���� ������ �������¿� ����
	_bool isIn_Return_Direction_And_Point(_fvector vPosition, _int* pNeighborIndex, _float3* pDir, _float3* pPos, _bool bClimb);

	_float Compute_Y(_fvector vPoint); // ����� ������ ���, �Է¹��� ��ǥ�� ���� ��� ���� ���� ��ȯ
	_bool isAbove(_fvector vPoint, _float* pOut = nullptr); // ����� �������� ���, �Է¹��� ��ġ�� ����� ���� �ִ��� �Ǻ�, ���̰� ���� ��ȯ

	_bool isUpperSide(_fvector vPoint, _float* pOut = nullptr); // ����� ������ ���, �Է¹��� ��ǥ�� ����� ���� ������ �Ʒ��� ������ �Ǻ�
	
	_float Get_CloseLine(_fvector vPoint, LINES* pOut = nullptr, _bool bXZ = false); // �Է¹��� ��ġ�κ��� ���� ����� ���α����� �Ÿ� ��ȯ, out���� Ÿ�� ����
	_float Compute_Distance(_fvector vPoint, LINES eLine, _bool bXZ = false); // �Է¹��� ��ġ�� �Է¹��� ���� ���� �ּҰŸ� ��ȯ

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

	// �÷԰��� �ƴ϶� ���� ����Ʈ�� �޾ƿ��Բ��Ѵ�.
	// �װ͵� _float3�� �ƴ϶� ��� ����ƮŸ������ �����ϰ� �ҷ��� �� �ش� Ÿ���� �ҷ����Բ�
	// ���� ���� ��ġ, ���� ��ġ, ������ �����ص���
	POINTS						m_eHighestPoint = POINTS::POINT_END;
	POINTS						m_eLowestPoint = POINTS::POINT_END;

	LINES						m_eHighestLine = LINES::LINE_END;

	_float3						m_vCenter;

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END