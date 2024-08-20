#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* �����带 �����Ѵ�. */
/* �������� �������Լ��� �����Ѵ�. (�� �������� �ε��ؾ��� �ڿ����� �����Ѵ�.) */

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

	// Ʈ���϶���Ÿ�� =========================================================
	HRESULT Loading_ForTwilightTownComponents();
	HRESULT Loading_ForTwilightTownModels();
	HRESULT Loading_ForTwilightTownObjects();
	HRESULT Loading_ForTwilightTownUIs();
	HRESULT Loading_ForTwilightTownPool();

	// ��Ǭ�� =================================================================
	HRESULT Loading_ForRapunzelComponents();
	HRESULT Loading_ForRapunzelModels();
	HRESULT Loading_ForRapunzelObjects();
	HRESULT Loading_ForRapunzelUIs();
	HRESULT Loading_ForRapunzelPool();

	// ��Ǭ�� ���� ============================================================
	HRESULT Loading_ForRapunzelBossComponents();
	HRESULT Loading_ForRapunzelBossModels();
	HRESULT Loading_ForRapunzelBossObjects();
	HRESULT Loading_ForRapunzelBossUIs();
	HRESULT Loading_ForRapunzelBossPool();

	// �ܿ�ձ� ===============================================================
	HRESULT Loading_ForFrozenComponents();
	HRESULT Loading_ForFrozenModels();
	HRESULT Loading_ForFrozenObjects();
	HRESULT Loading_ForFrozenUIs();
	HRESULT Loading_ForFrozenPool();

	// ���ǹ��� ===============================================================
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

	// ���� ===================================================================
	HRESULT Loading_ForMapToolComponents();
	HRESULT Loading_ForMapToolModels();
	HRESULT Loading_ForMapToolObjects();

	// ����Ʈ�� ===============================================================
	HRESULT Loading_ForEffectToolComponents();
	HRESULT Loading_ForEffectToolModels();
	HRESULT Loading_ForEffectToolObjects();

	// �ݶ��̴��� =============================================================
	HRESULT Loading_ForColliderToolComponents();
	HRESULT Loading_ForColliderToolModels();
	HRESULT Loading_ForColliderToolObjects();
	
	// �׽�Ʈ���� =============================================================
	HRESULT Loading_ForTestComponents();
	HRESULT Loading_ForTestModels();
	HRESULT Loading_ForTestObjects();

	// ��������
	HRESULT Loading_ForEnding();

public:
	HRESULT ReadyMapFromSaveFiles(LEVEL eLevel, const _tchar* pFilePath);	// �� ���̺굥���� �ε�
	HRESULT Loading_Triggers(LEVEL eLevel, const _tchar* pFilePath);			// Ʈ���ŷε�

private:
	HRESULT Loading_TT_Map();		// Ʈ���϶��� �� �ε�
	HRESULT Loading_TS_Andy_Map();	// ���̽��丮 �ص��� �� �� �ε�
	HRESULT Loading_TS_Doll_Map();	// ���̽��丮 ���� �� �ε�
	HRESULT Loading_RA_Map();		// ��Ǭ�� �� �ε�
	HRESULT Loading_RA_Boss_Map();	// ��Ǭ������ �� �ε�
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