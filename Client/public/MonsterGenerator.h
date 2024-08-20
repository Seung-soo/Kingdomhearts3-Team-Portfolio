#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

class CMonsterGenerator final : public CBase
{
	MAKE_SINGLETON(CMonsterGenerator)

private:
	CMonsterGenerator();
	virtual ~CMonsterGenerator() = default;

#ifdef _DEBUG
public:
	HRESULT CMonsterGenerator_SetUp();
	_int	CMonsterGenerator_Tick(_double TimeDelta);
	HRESULT CMonsterGenerator_Render();

private:
	void CreateGuidePoint();
	void CreateMonster();
	void DeleteGameObject();
	void HiddenGenerator();

	void CreateButton(const char* ButtonTag, _tchar* pLayerTag, _tchar* pPrototypeTag);
	void CreateFrogBandButton(const char* ButtonTag, _tchar* pModelTag);

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

	// 가이드포인트 변수
	class CGameObject* m_pGuidePoint = nullptr;

	// 선택된 게임오브젝트
	class CGameObject* m_pSelectedGameObject = nullptr;

	// 생성한 몬스터 배열
	vector<class CGameObject*> m_CreatedMonsters;
#endif

public:
	void virtual Free() override;
};

END