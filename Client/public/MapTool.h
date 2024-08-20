#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Map.h"

BEGIN(Client)

/*
* ��Ʈ�� + Z		: ���� �ֱٿ� ������ ��ü ����
* ��Ʈ�� + ��Ŭ��	: ��ŷ����Ʈ ����
* ��Ʈ�� + E		: ��ŷ����Ʈ �ø���
* ��Ʈ�� + Q		: ��ŷ����Ʈ ������
* ��Ʈ�� + W		: ��ŷ����Ʈ 0 ����
* ��ü ������ �����̽��� �������ֱ� : ��ü�� ���콺 ��ġ�� �̵�(��Ȯ���� ���̵���ŷ����Ʈ ��ġ�� �̵�)
*/

class CMapTool final : public CBase
{
	MAKE_SINGLETON(CMapTool)

public:
	enum TAB { TAB_MAP, TAB_GAMEOBJECT, TAB_NAVIMESH, TAB_END };

private:
	CMapTool();
	virtual ~CMapTool() = default;

#ifdef _DEBUG
public:
	HRESULT MapTool_SetUp();
	_int	MapTool_Tick(_double TimeDelta);
	HRESULT MapTool_Render();

private:
	/* Map ���� */
	HRESULT ShowCurrentMapList();	// ���� ��ġ�� �� ���̾��� ����Ʈ�� ������
	HRESULT CreateMapList();		// ��ġ�� �� ���
	HRESULT ResetSaveLoadMap();		// ����, ���̺�, �ε�
	HRESULT CreateMapCreateButton(LEVEL eLevel, const char* pButtonTag, const _tchar* pModelTag);
	HRESULT CreateContinuousMap();
	void	DeleteMap();
	void	PickingMap();
	HRESULT	AutoSave(_double TimeDelta);

	/* GameObjects ���� */
	HRESULT ShowCurrentGameObjectList();		// ��ġ�� ���ӿ�����Ʈ ���
	HRESULT CreateGameObjectList();				// ��ġ�� ���ӿ�����Ʈ ���
	HRESULT CreateObjectCreateButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag); // ���ӿ�����Ʈ ������ư �������

	/* ���� */
	void	SetGameObjectTransform(TAB eTab);	// ���ӿ�����Ʈ Transform ����
	void	GuidePickingPointUpdate();			// ���̵���ŷ����Ʈ�� ���콺 ��ġ�� ����ٴϰ� �����
	HRESULT CreatePickingPoint();				// (Ctrl + ���콺 ��Ŭ��) ���� PickingPoint ����
	void	SetPickingPointY();					// (Ctrl + X) , (Ctrl + C) ����Ű�� PickintPointY �ø��� ������

	HRESULT MoveToGuidePickingPoint(TAB eTab);	// m_pSelectedGameObject �� ���̵���ŷ����Ʈ ��ġ�� �̵�
	HRESULT DeleteLatestGameObject(TAB eTab);	// ���� �������� ������ ���ӿ�����Ʈ �����

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

	/* ��ŷ����Ʈ */
	CGameObject*	m_pGuidePoint    = nullptr;	// ���� PickingPoint ��ġ�� Ȯ���ϱ� ���� m_pGuidePoint
	CGameObject*	m_pPickingPoint  = nullptr;	// PickingPoint (Ctrl + LButton ���� �ִ� 1���� �����ϵ��� ����)
	_float			m_fPickintPointY = 0.f;		// PickingPoint �� ���� �� ����

	/* ���Ӽ�ġ */
	_bool m_bContinuous = false;
	CMap::MAPDESC m_MapDesc;


	/* ��ü���� */
	CGameObject* m_pSelectedGameObject[TAB_END] = { nullptr, };	// Transform �� ������ ���� ���õ� ��ü
	std::vector<CGameObject*> m_pLatestGameObjects[TAB_END];	// ���� �ֱٿ� ������ ��ü (Ctrl + Z ����Ű�� �����ϱ� ����)

	/* ���似�̺� */
	_double m_TimeAccForAutoSave = 0.0;


////////////////////////////////////////////////////////////////////////////
#pragma region NAVIMESH
private:
	_bool					m_bMakeCell_Mode = false;
	_bool					m_bTypeSetting_Mode = false;
	_bool					m_bTypeChange_Mode = false;
	_float					m_Points[3][3] = {};
	_uint					m_iPointIndex = 0;
	class CCellPoint*		m_CellPoint[3];
	vector<CCell*>			m_Cells;
	_bool					m_bMakeTestPoint = false;
	_bool					m_bFirstSetting = false;
	_int					m_iCurrentCellIndex = -1;

	_bool					m_bPointA = false;
	_bool					m_bPointB = false;
	_bool					m_bPointC = false;
	char					m_szNaviDataFilePath[MAX_PATH] = "";

	char					m_szSaveNaviDataFileName[MAX_PATH] = "";

	_bool					m_bNaviMeshRender = true;

	_int					m_iCellType = 0;

#ifdef _DEBUG				// �׺�Ž� ������ ���� ������Ʈ��
private:
	class CVIBuffer_Triangle*		m_pNaviVIBuffer = nullptr;
	class CShader*					m_pNaviShader = nullptr;
#endif

private:
	void					NaviMesh_Make();		// �׺�Ž� ����� �Լ�
	_int					NaviMesh_List();		// �� ����Ʈ �Լ�
	_int					NaviMesh_Load_List();	// �׺�Ž� ������ ����
	HRESULT					NaviMesh_Render();		// �׺�Ž� ����

private:
	void					Save_NaviMesh();		// �׺�Ž� ����
	void					Load_NaviMesh();		// �׺�Ž� �ε�
#pragma endregion
////////////////////////////////////////////////////////////////////////////
#endif

public:
	void virtual Free() override;
};

END