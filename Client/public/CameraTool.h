#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

class CCameraTool final : public CBase
{
	MAKE_SINGLETON(CCameraTool) 

private:
	CCameraTool();
	virtual ~CCameraTool() = default;

#ifdef _DEBUG
public:
	HRESULT CCameraTool_SetUp();
	_int	CCameraTool_Tick(_double TimeDelta);
	HRESULT CCameraTool_Render();

private: // BezierMode =====================================================================
	void UpdateGuidePoint();
	void RenderDebug(_double TimeDelta);		// ����� �ʿ��� �׸�� ������
	void SetCameraPoint(_double TimeDelta);		// ī�޶�����Ʈ ��ġ
	void ModifyCameraPoint(_double TimeDelta);	// ����
	void DeleteCameraPoint();					// ī�޶�����Ʈ ����
	void Play(_double TimeDelta);				// ����
	void Reset();								// ����
	void SaveLoad();							// ���̺�ε�
	void AutoSave(_double TimeDelta);			// �ڵ�����

private: // FlashrMode =====================================================================
	void F_RenderDebug(_double TimeDelta);
	void F_SetWaitTime(_double TimeDelta);
	void F_SetCameraPoint(_double TimeDelta);
	void F_ModifyCameraPoint(_double TimeDelta);
	void F_DeleteCameraPoint();
	void F_Play(_double TimeDelta);
	void F_SaveLoad();
	void F_AutoSave(_double TimeDelta);

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

private: // BezierMode ���� =================================================
	// ���̵�����Ʈ ����
	class CGameObject* m_pGuidePoint = nullptr;
	_float m_GuidePointY = 0.f;

	// ī�޶�����Ʈ ����
	vector<class CGameObject*> m_Position_CameraPoint;
	_uint m_iPositionIndex = 0;

	vector<class CGameObject*> m_LookAt_CameraPoint;
	_uint m_iLookAtIndex = 0;

	vector<class CGameObject*> m_Via_CameraPoint;
	_uint m_iViaIndex = 0;

	// ��ġ ���� ����
	class CGameObject* m_pSelectedCameraPoint = nullptr;

	// ������� ����
	_float4x4	m_OriginWorldMatrix;
	_vector		m_vLerpStartPos;
	_double		m_LerpTimeAcc = 0.0;
	_double		m_LerpSpeed   = 0.3;

	_bool	m_bPlayOn    = false;
	_bool	m_bIsPlayEnd = false;
	_uint	m_iPlayIndex = 0;

	// ���̺� ����
	char m_LoadFileName[128] = "NewCameraData.dat";

	// ���似�̺� ����
	_double m_TimeAccForAutoSave = 0.0;

private: // FlashMode ���� ===================================================
	_float m_fMaxWaitTime;
	_float m_fWaitTimeAcc = 0.0;

#endif

public:
	void virtual Free() override;
};

END

/*
* ��Ʈ�� + ��Ŭ�� : ������ ī�޶�����Ʈ ����
* ��Ʈ�� + ��Ŭ�� : LookAt ī�޶�����Ʈ ����
* ��Ʈ�� + ��Ŭ�� : Via ī�޶�����Ʈ ����
* (������ -> LookAt -> Via ������� ���� ����)

* ��Ʈ�� + Z : ������ ������ ī�޶�����Ʈ ����
* ��Ʈ�� + X : ������ LookAt ī�޶�����Ʈ ����
* ��Ʈ�� + C : ������ Via ī�޶�����Ʈ ����

* ��Ʈ�� + E : ��ŷ��ǥ ���� �ø���
* ��Ʈ�� + Q : ��ŷ��ǥ �Ʒ��� ������

* ��Ŭ�� ���� ���·� ���콺 �̵� : ī�޶�����Ʈ �̵�
* ��Ŭ�� ���� ���·� ��Ʈ�� + E : ī�޶�����Ʈ ���� �̵�
* ��Ŭ�� ���� ���·� ��Ʈ�� + Q : ī�޶�����Ʈ �Ʒ��� �̵�

*/