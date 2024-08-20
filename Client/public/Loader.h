#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 스레드를 생성한다. */
/* 스레드의 진입점함수를 정의한다. (각 레벨마다 로딩해야할 자원들을 생성한다.) */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;

public:
	LEVEL				Get_LevelID()	{ return m_eLevel; }
	void				SetFinish()		{ m_isFinished = true; }
	_bool				isFinished()	{ return m_isFinished; }
	CRITICAL_SECTION	Get_CS()		{ return m_CS; }

public:
	HRESULT NativeConstruct(LEVEL eNextLevel);

	// 트와일라잇타운 =========================================================
	HRESULT Loading_ForTwilightTownComponents();
	HRESULT Loading_ForTwilightTownModels();
	HRESULT Loading_ForTwilightTownObjects();
	HRESULT Loading_ForTwilightTownUIs();
	HRESULT Loading_ForTwilightTownPool();

	// 라푼젤 =================================================================
	HRESULT Loading_ForRapunzelComponents();
	HRESULT Loading_ForRapunzelModels();
	HRESULT Loading_ForRapunzelObjects();
	HRESULT Loading_ForRapunzelUIs();
	HRESULT Loading_ForRapunzelPool();

	// 라푼젤 보스 ============================================================
	HRESULT Loading_ForRapunzelBossComponents();
	HRESULT Loading_ForRapunzelBossModels();
	HRESULT Loading_ForRapunzelBossObjects();
	HRESULT Loading_ForRapunzelBossUIs();
	HRESULT Loading_ForRapunzelBossPool();

	// 겨울왕국 ===============================================================
	HRESULT Loading_ForFrozenComponents();
	HRESULT Loading_ForFrozenModels();
	HRESULT Loading_ForFrozenObjects();
	HRESULT Loading_ForFrozenUIs();
	HRESULT Loading_ForFrozenPool();

	// 검의묘지 ===============================================================
	HRESULT Loading_ForKeyGraveComponents();
	HRESULT Loading_ForKeyGraveModels();
	HRESULT Loading_ForKeyGraveObjects();
	HRESULT Loading_ForKeyGraveUIs();
	HRESULT Loading_ForKeyGravePool();

	// FinalBoss ===============================================================
	HRESULT Loading_ForFinalBossComponents();
	HRESULT Loading_ForFinalBossModels();
	HRESULT Loading_ForFinalBossObjects();
	HRESULT Loading_ForFinalBossUIs();
	HRESULT Loading_ForFinalBossPool();

	// 맵툴 ===================================================================
	HRESULT Loading_ForMapToolComponents();
	HRESULT Loading_ForMapToolModels();
	HRESULT Loading_ForMapToolObjects();

	// 이펙트툴 ===============================================================
	HRESULT Loading_ForEffectToolComponents();
	HRESULT Loading_ForEffectToolModels();
	HRESULT Loading_ForEffectToolObjects();

	// 콜라이더툴 =============================================================
	HRESULT Loading_ForColliderToolComponents();
	HRESULT Loading_ForColliderToolModels();
	HRESULT Loading_ForColliderToolObjects();
	
	// 테스트레벨 =============================================================
	HRESULT Loading_ForTestComponents();
	HRESULT Loading_ForTestModels();
	HRESULT Loading_ForTestObjects();

	// 엔딩레벨
	HRESULT Loading_ForEnding();

public:
	HRESULT ReadyMapFromSaveFiles(LEVEL eLevel, const _tchar* pFilePath);	// 맵 세이브데이터 로드
	HRESULT Loading_Triggers(LEVEL eLevel, const _tchar* pFilePath);			// 트리거로드

private:
	HRESULT Loading_TT_Map();		// 트와일라잇 맵 로드
	HRESULT Loading_TS_Andy_Map();	// 토이스토리 앤디의 방 맵 로드
	HRESULT Loading_TS_Doll_Map();	// 토이스토리 인형 맵 로드
	HRESULT Loading_RA_Map();		// 라푼젤 맵 로드
	HRESULT Loading_RA_Boss_Map();	// 라푼젤보스 맵 로드
	HRESULT	Loading_FZ_Map();
	HRESULT Loading_KG_Map();
	HRESULT Loading_FINAL_Map();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	
	LEVEL	m_eLevel     = LEVEL_END;
	_bool	m_isFinished = false;
	HANDLE	m_hThread;

	CRITICAL_SECTION m_CS;

private:
	_matrix mat_rotY180	 = XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix mat_rotX180	 = XMMatrixRotationX(XMConvertToRadians(180.0f));
	_matrix mat_rotZ180	 = XMMatrixRotationZ(XMConvertToRadians(180.0f));
	_matrix mat_scale001 = XMMatrixScaling(0.01f, 0.01f, 0.01f);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END