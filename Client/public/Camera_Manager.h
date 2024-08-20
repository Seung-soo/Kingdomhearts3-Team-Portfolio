#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)


class CCamera_Manager : public CBase
{
	MAKE_SINGLETON(CCamera_Manager)
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;
public:
	_bool	Get_Finish() { return m_bFinish; }
public:
	//	���� ���� �ҷ����� �Լ�
	void	Load_Camera(char* pLoadFileName);
	void	F_Load_Camera(char* pLoadFileName);
	//	�÷��� �Լ�
	void	Play_Start();
	void	F_Play_Start(_float fWaitTimeAcc);

	_bool	Play(_double TimeDelta);
	_bool	F_Play(_double TimeDelta);

	//	���� �Լ�
	void	Reset();
private:
	// �ε��� ���� �̸� ����
	char m_LoadFileName[128] = "NewCameraData.dat";

private: // BezierMode ���� =================================================
	// ���̵�����Ʈ ����
	 CGameObject* m_pGuidePoint = nullptr;
	_float m_GuidePointY = 0.f;

	// ī�޶�����Ʈ ����
	vector<CGameObject*> m_Position_CameraPoint;
	_uint m_iPositionIndex = 0;

	vector<CGameObject*> m_LookAt_CameraPoint;
	_uint m_iLookAtIndex = 0;

	vector<CGameObject*> m_Via_CameraPoint;
	_uint m_iViaIndex = 0;

private: // FlashMode ���� ===================================================
	_float m_fMaxWaitTime;
	_float m_fWaitTimeAcc = 0.0;

private:
	// ������� ����
	_float4x4	m_OriginWorldMatrix;
	_vector		m_vLerpStartPos;
	_double		m_LerpTimeAcc = 0.0;
	_double		m_LerpSpeed = 1.1;

	_bool		m_bPlayOn = false;
	_bool		m_bIsPlayEnd = false;
	_uint		m_iPlayIndex = 0;

	_bool		m_bIsFlash = false;
	_bool		m_bFinish = false;

public:
	virtual void	Free() override;
};

END
