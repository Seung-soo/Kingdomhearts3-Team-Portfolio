#pragma once

#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

/* 
	:: �����̵� ���Ͱ� ����Ǵ� ��� 
	1. Ʈ������ �Լ����� ȣ��
	2. ���� �ε����� ��ġ�ϰ� �ִ��� Ȯ��
	3. �� �� ���� ���� ��
	4. ���� ������ ��������
	5. Ʈ���������� �޾Ƽ� �����̵� ���͸� ����ؼ� �����Ų��.
*/

#define MOVABLE		true
#define UNMOVABLE	false

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		class CTransform* pParent = nullptr;
		_uint	iCurrentIndex;
		_uint	iPreviousIndex;
		_uint	iPreviousFrame;

	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNaviDataFilePath);
	virtual HRESULT	NativeConstruct(void* pArg) override;

public:
	_uint			Get_NumberOfCell() { return (_uint)m_Cells.size(); }
	_uint			Get_CurrentIndex() { return m_NaviDesc.iCurrentIndex; }
	_uint			Get_PreviousIndex() { return m_NaviDesc.iPreviousIndex; }

	CCell*			Get_CurrentCell() { return m_Cells[m_NaviDesc.iCurrentIndex]; }

	// Main Values
	_int			Get_CurrentNeighborIndex(CCell::LINES eLine) { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_NeighborIndex(eLine); }
	_float3			Get_CurrentLine(CCell::LINES eLine) { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Line(eLine); }
	_vector			Get_CurrentPoint(CCell::POINTS ePoint) { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(ePoint); }
	
	// Sub Values
	CCell::TYPE		Get_CurrentType() { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Type(); }
	_float3			Get_CurrentNormal() { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_NormalVector(); }
	_float3			Get_CurrentRight() { return m_Cells[m_NaviDesc.iCurrentIndex]->Get_RightVector(); }
	_float			Get_CurrentHighest() { return m_Cells[m_NaviDesc.iCurrentIndex]->HighestY(); }
	_float			Get_CurrentLowest() { return m_Cells[m_NaviDesc.iCurrentIndex]->LowestY(); }

	CCell*			Get_InputCell(_uint iIndex) { return m_Cells[iIndex]; }

	_bool			Get_Ignore() { return m_bIgnore; }
	void			Set_Ignore(_bool bIgnore) { m_bIgnore = bIgnore; }

	_bool			Get_Climb() { return m_bClimb; }
	void			Set_Climb(_bool bClimb) { m_bClimb = bClimb; }

	_bool			Get_Land() { return m_bLand; }
	void			Set_Land(_bool bLand) { m_bLand = bLand; }

	_bool			Move_OnNavigation(_fvector vPosition, _fvector vCurrentPosition, _float3** pSlidingVector = nullptr);
	_bool			Move_OnNavigation_NotRunning(_fvector vPosition, _fvector vCurrentPosition, _float3** pSlidingVector = nullptr);

	// Current Cell �� �ش��ϴ� �Լ���
	_float			Compute_Y(_fvector vPoint);		// ���� �̷�� ����� ����
	_bool			isAbove(_fvector vPoint);	// ��麸�� ���� �����ϸ� true ��ȯ
	_bool			isIn(_fvector vPosition, _int* pNeighborIndex, _float3** pOut, _bool bClimb);

	_bool			Check_PrevAndCurrIndex();
	_bool			Check_PrevAndCurrFrame();

	// �����ǿ� �ش��ϴ� ���� ã�Ƽ� ���� �ε��� ������ ����.
	void			Find_Cell(_fvector vPosition);

//#ifdef _DEBUG
public:
	HRESULT Render();
//#endif

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

//#ifdef _DEBUG
private:
	class CVIBuffer_Triangle*		m_pVIBuffer = nullptr;
	class CShader*					m_pShader = nullptr;
//#endif
private:
	// �׺�޽��� ����ϴ� ��ü�� �����ϰ� �ִ� ���� �ε���
	NAVIDESC m_NaviDesc;

	_bool m_bIgnore = false;	// �׺���̼� �޽��� ������ ������ ����
	_bool m_bClimb  = false;	// ���� �׺���̼����� �Ѿ �� ������ ����
	_bool m_bLand   = true;

private:
	HRESULT			SetUp_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNaviDataFilePath);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END