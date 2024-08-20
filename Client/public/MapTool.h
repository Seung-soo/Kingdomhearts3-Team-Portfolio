#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Map.h"

BEGIN(Client)

/*
* 컨트롤 + Z		: 가장 최근에 생성한 객체 삭제
* 컨트롤 + 좌클릭	: 피킹포인트 생성
* 컨트롤 + E		: 피킹포인트 올리기
* 컨트롤 + Q		: 피킹포인트 내리기
* 컨트롤 + W		: 피킹포인트 0 으로
* 객체 선택후 스페이스바 누르고있기 : 객체를 마우스 위치로 이동(정확히는 가이드피킹포인트 위치로 이동)
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
	/* Map 관련 */
	HRESULT ShowCurrentMapList();	// 현재 설치된 맵 레이어의 리스트를 보여줌
	HRESULT CreateMapList();		// 설치할 맵 목록
	HRESULT ResetSaveLoadMap();		// 리셋, 세이브, 로드
	HRESULT CreateMapCreateButton(LEVEL eLevel, const char* pButtonTag, const _tchar* pModelTag);
	HRESULT CreateContinuousMap();
	void	DeleteMap();
	void	PickingMap();
	HRESULT	AutoSave(_double TimeDelta);

	/* GameObjects 관련 */
	HRESULT ShowCurrentGameObjectList();		// 설치된 게임오브젝트 목록
	HRESULT CreateGameObjectList();				// 설치할 게임오브젝트 목록
	HRESULT CreateObjectCreateButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag); // 게임오브젝트 생성버튼 만들어줌

	/* 공통 */
	void	SetGameObjectTransform(TAB eTab);	// 게임오브젝트 Transform 설정
	void	GuidePickingPointUpdate();			// 가이드피킹포인트가 마우스 위치를 따라다니게 만들기
	HRESULT CreatePickingPoint();				// (Ctrl + 마우스 좌클릭) 으로 PickingPoint 생성
	void	SetPickingPointY();					// (Ctrl + X) , (Ctrl + C) 단축키로 PickintPointY 올리고 내리기

	HRESULT MoveToGuidePickingPoint(TAB eTab);	// m_pSelectedGameObject 를 가이드피킹포인트 위치로 이동
	HRESULT DeleteLatestGameObject(TAB eTab);	// 가장 마지막에 생성한 게임오브젝트 지우기

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

	/* 피킹포인트 */
	CGameObject*	m_pGuidePoint    = nullptr;	// 현재 PickingPoint 위치를 확인하기 위한 m_pGuidePoint
	CGameObject*	m_pPickingPoint  = nullptr;	// PickingPoint (Ctrl + LButton 으로 최대 1개만 생성하도록 관리)
	_float			m_fPickintPointY = 0.f;		// PickingPoint 의 높이 값 설정

	/* 연속설치 */
	_bool m_bContinuous = false;
	CMap::MAPDESC m_MapDesc;


	/* 객체관리 */
	CGameObject* m_pSelectedGameObject[TAB_END] = { nullptr, };	// Transform 값 세팅을 위해 선택된 객체
	std::vector<CGameObject*> m_pLatestGameObjects[TAB_END];	// 가장 최근에 생성된 객체 (Ctrl + Z 단축키로 제거하기 위해)

	/* 오토세이브 */
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

#ifdef _DEBUG				// 네비매쉬 랜더에 쓰일 컴포넌트들
private:
	class CVIBuffer_Triangle*		m_pNaviVIBuffer = nullptr;
	class CShader*					m_pNaviShader = nullptr;
#endif

private:
	void					NaviMesh_Make();		// 네비매쉬 만드는 함수
	_int					NaviMesh_List();		// 셀 리스트 함수
	_int					NaviMesh_Load_List();	// 네비매쉬 데이터 파일
	HRESULT					NaviMesh_Render();		// 네비매쉬 랜더

private:
	void					Save_NaviMesh();		// 네비매쉬 저장
	void					Load_NaviMesh();		// 네비매쉬 로드
#pragma endregion
////////////////////////////////////////////////////////////////////////////
#endif

public:
	void virtual Free() override;
};

END