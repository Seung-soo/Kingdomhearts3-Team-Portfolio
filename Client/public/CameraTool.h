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
	void RenderDebug(_double TimeDelta);		// 디버깅 필요한 항목들 렌더링
	void SetCameraPoint(_double TimeDelta);		// 카메라포인트 설치
	void ModifyCameraPoint(_double TimeDelta);	// 수정
	void DeleteCameraPoint();					// 카메라포인트 삭제
	void Play(_double TimeDelta);				// 실행
	void Reset();								// 리셋
	void SaveLoad();							// 세이브로드
	void AutoSave(_double TimeDelta);			// 자동저장

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

private: // BezierMode 변수 =================================================
	// 가이드포인트 변수
	class CGameObject* m_pGuidePoint = nullptr;
	_float m_GuidePointY = 0.f;

	// 카메라포인트 변수
	vector<class CGameObject*> m_Position_CameraPoint;
	_uint m_iPositionIndex = 0;

	vector<class CGameObject*> m_LookAt_CameraPoint;
	_uint m_iLookAtIndex = 0;

	vector<class CGameObject*> m_Via_CameraPoint;
	_uint m_iViaIndex = 0;

	// 위치 수정 변수
	class CGameObject* m_pSelectedCameraPoint = nullptr;

	// 실행관련 변수
	_float4x4	m_OriginWorldMatrix;
	_vector		m_vLerpStartPos;
	_double		m_LerpTimeAcc = 0.0;
	_double		m_LerpSpeed   = 0.3;

	_bool	m_bPlayOn    = false;
	_bool	m_bIsPlayEnd = false;
	_uint	m_iPlayIndex = 0;

	// 세이브 변수
	char m_LoadFileName[128] = "NewCameraData.dat";

	// 오토세이브 변수
	_double m_TimeAccForAutoSave = 0.0;

private: // FlashMode 변수 ===================================================
	_float m_fMaxWaitTime;
	_float m_fWaitTimeAcc = 0.0;

#endif

public:
	void virtual Free() override;
};

END

/*
* 컨트롤 + 좌클릭 : 포지션 카메라포인트 생성
* 컨트롤 + 우클릭 : LookAt 카메라포인트 생성
* 컨트롤 + 휠클릭 : Via 카메라포인트 생성
* (포지션 -> LookAt -> Via 순서대로 생성 가능)

* 컨트롤 + Z : 마지막 포지션 카메라포인트 삭제
* 컨트롤 + X : 마지막 LookAt 카메라포인트 삭제
* 컨트롤 + C : 마지막 Via 카메라포인트 삭제

* 컨트롤 + E : 피킹좌표 위로 올리기
* 컨트롤 + Q : 피킹좌표 아래로 내리기

* 좌클릭 누른 상태로 마우스 이동 : 카메라포인트 이동
* 좌클릭 누른 상태로 컨트롤 + E : 카메라포인트 위로 이동
* 좌클릭 누른 상태로 컨트롤 + Q : 카메라포인트 아래로 이동

*/