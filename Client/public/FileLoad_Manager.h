#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

class CFileLoad_Manager final : public CBase
{
	MAKE_SINGLETON(CFileLoad_Manager)

public:
	enum SAVEFILE
	{
		SAVEFILE_TT_FIRST, SAVEFILE_TT_SECOND, SAVEFILE_TT_BOSS,											// Twilight Town
		SAVEFILE_RA_FIRST, SAVEFILE_RA_SECOND, SAVEFILE_RA_THIRD, SAVEFILE_RA_FOURTH, SAVEFILE_RA_FIFTH,	// Rapunzel
		SAVEFILE_RA_BOSS_BIGTREE,																			// Rapunzel Boss
		SAVEFILE_FZ_ICICLES,																				// Frozen
		SAVEFILE_END
	};

private:
	CFileLoad_Manager();
	virtual ~CFileLoad_Manager() = default;

public:
	HRESULT FileLoadManager_SetUp();

public:
	HRESULT Load_SaveFile(SAVEFILE eSaveFile, OUT vector<class CGameObject*>* pObjects = nullptr);

public: // 이펙트툴에서 맵 파일 로드할 때 사용
	HRESULT Load_MapSaveFile(LEVEL eLevel, const _tchar* pFilePath);

private:
	HRESULT CreateObjectFromFile(LEVEL eLevel, const _tchar* pFilePath, const _tchar* pLayerTag, OUT vector<class CGameObject*>* pObjects = nullptr);

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;

	DWORD		m_dwBytes = 0;
	_float4x4	m_WorldMatrix;

public:
	void virtual Free() override;
};

END