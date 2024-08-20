#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CHierarchyNode;
class CNavigation;
END

BEGIN(Client)

class CCamera_Perspective final : public CCamera
{
public:
	enum CAMERAWORK {
		WORK_OPENBOX, 
		WORK_RAGE_TRANS, WORK_RAGE_ATTACKEND, WORK_RAGE_FINISHATTACK, 
		WORK_HAMMER_TRANS, WORK_HAMMER_ATTACKEND, WORK_HAMMER_AIRATTACKEND, WORK_HAMMER_FINISHATTACK,
		WORK_RAPUNZEL_PORTAL, WORK_RAPUNZEL_NOBODY_APPEAR,
		WORK_BIGTREE_APPEAR, WORK_BIGTREE_CLIMB, WORK_RAPUNZELBOSS_PORTAL,
		WORK_FROZEN_PORTAL, WORK_KEYGRAVE_PORTAL,
		WORK_ICEMAN_APPEAR,
		WORK_FROZENWOLF_APPEAR, WORK_FROZENWOLF_RAIN,
		WORK_GOAT_APPEAR, WORK_GOAT_PILLARFIRE, WORK_GOAT_PILLARROT,
		WORK_RETURNTOPLAYER, WORK_RETURNTOPLAYERBACK,
		WORK_NIGHTMARE_FOCUS,
		WORK_END};

private:
	explicit CCamera_Perspective(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Perspective(const CCamera_Perspective& rhs);
	virtual ~CCamera_Perspective() = default;

public:
	CGameObject* Get_Target() { return m_pFocusingTarget; }
	void Set_Target(CGameObject* pFocusingTarget) { m_pFocusingTarget = pFocusingTarget; }

	_bool Get_Focusing() { return m_bFocusing; }
	void Set_Focusing(_bool bFocusing) { m_bFocusing = bFocusing; }

	_float Get_Distance() { return m_fDistance; }
	void Set_Distance(_float fDistance) { m_fDistance = fDistance; }

	_float Get_DefaultDistance() { return m_fDefaultDistance; }
	void Set_DefaultDistance(_float fDistance) { m_fDefaultDistance = fDistance; }

	_float Get_Sensitivity() { return m_fSensitivity; }
	void Set_Sensitivity(_float fSensitivity) { m_fSensitivity = fSensitivity; }

	_float Get_AdjustorX() { return m_fAdjustorX; }
	void Set_AdjustorX(_float fX) { m_fAdjustorX = fX; }

	_float Get_AdjustorY() { return m_fAdjustorY; }
	void Set_AdjustorY(_float fY) { m_fAdjustorY = fY; }

	void Set_StartScene(_bool bStartScene) { m_bStartScene = bStartScene; }
	void Set_BossScene(_bool bBossScene) { m_bBossScene = bBossScene; }
	void Set_FinishAttack(_bool bFinishAttack) { m_bFinishAttack = bFinishAttack; }

public:	// 카메라 쉐이크
	void Set_HorizonShake(_float fTime, _float fDownTime, _float fPower) { 
		m_bHorizenShake = true; 
		m_fHSTime = fTime; 
		m_fShakePower = fPower; 
		m_fShakeDownTime = m_fHSTime - fDownTime;
		m_fStartShakePower = m_fShakePower;
	}

	void Set_VerticalShake(_float fTime, _float fDownTime, _float fPower) { 
		m_bVerticalShake = true; 
		m_fVSTime = fTime; 
		m_fShakePower = fPower; 
		m_fShakeDownTime = m_fVSTime - fDownTime;
		m_fStartShakePower = m_fShakePower;
	}

	//	NPC 바라 보게
	void	Set_LookNPC(_bool bLookNPC, class CTransform* pNpcTransform);
	_bool	Get_LookNPC() { return m_bLookNPC; }

	void	Set_LookNPC_Time(_float fLookNPC_Time) { m_fLookNPC_Time = fLookNPC_Time; }

	_bool	Get_TalkNPC() { return m_bTalkNPC; }
	void	Set_TalkNPC(_bool bTalkNPC) { m_bTalkNPC = bTalkNPC; }

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	//		NPC를 바라보는 카메라 무빙
	void	Look_NPC(_double TimeDelta);

	//		카메라 워크를 시작할 때 호출하는 함수
	void	Start_CameraWork(CAMERAWORK eCameraWork, CTransform* pTargetTransform = nullptr);

private:
	CGameObject* m_pFocusingTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;

	_bool m_bFocusing = false;

	_float m_fDistance = 0.f;
	_float m_fDefaultDistance = 0.f;
	_float m_fSensitivity = 0.f;

	_float m_fAdjustorX = 0.f;
	_float m_fAdjustorY = 0.f;

	_float m_fCameraTime = 0.f;
	_float m_fLerpInterval = 0.f;
	_vector m_vLerpPosition = __vector(0.f, 0.f, 0.f, 1.f);

	_bool  m_bFirstTick = false;

	// 카메라 쉐이크 관련 변수
	_bool			m_bHorizenShake = false;
	_bool			m_bVerticalShake = false;
	_bool			m_bLeftShake = false;
	_bool			m_bTopShake = false;
	_double			m_TimeDelta = 0.0;
	_float			m_fHSTime = 0.f;
	_float			m_fVSTime = 0.f;
	_uint			m_iHShakeInterval = 0;
	_uint			m_iVShakeInterval = 0;
	_float			m_fShakePower = 1.f;
	_bool			m_bReturnRotationH = false;
	_bool			m_bReturnRotationV = false;
	_float			m_fShakeDownTime = 0.f;
	_float			m_fStartShakePower = 1.f;


	// NPC 바라볼 때 기준 상태
	_vector			m_vBeforeLookAtNPC_Look = __vector(0.f, 0.f, 1.f, 0.f);
	_vector			m_vBeforeLookAtNPC_Position = __vector(0.f, 0.f, 0.f, 1.f);
	CTransform*		m_pNpcTransform = nullptr;
	_bool			m_bLookNPC = false;
	_float			m_fLookNPC_Time = 0.f;
	_bool			m_bTalkNPC = false;

	// StartScene
	_bool			m_bStartScene = false;
	_bool			m_bBossScene = false;

	CAMERAWORK		m_eCameraWork = WORK_END;
	_float			m_fCameraWorkTime = 0.f;
	_bool			m_bStartCameraWork = false;
	_vector			m_vLookAtPoint = __vector(0.f, 0.f, 0.f, 1.f);
	_vector			m_vLookAtSecondPoint = __vector(0.f, 0.f, 0.f, 1.f);

	_vector			m_vSavePos = __vector(0.f, 0.f, 0.f, 1.f);
	_vector			m_vSaveSecondPos = __vector(0.f, 0.f, 0.f, 1.f);
	_float			m_fLookAtPointY = 0.f;
	_bool			m_bCameraWorkPoint = false;
	CHierarchyNode*	m_pTargetBone = nullptr;
	_matrix			m_TargetModelTransformMatrix = XMMatrixIdentity();
	_float4x4*		m_pTargetTransformMatrix = nullptr;

	_bool			m_bStartTimer = false;
	_float			m_fTimer = 0.f;

	_bool			m_bFinishAttack = false;


private:
	CNavigation*	m_pNavigationCom = nullptr;
	_float			m_fNaviUpdate = 0.f;
private:
	void	DynamicMove(_double TimeDelta);
	HRESULT	Camera_Shake();

private:
	void	Camera_Scene(_double TimeDelta);
	void	TwilightTown_StartScene(_double TimeDelta);
	void	TwilightTown_BossScene(_double TimeDelta);

	void	Rapunzel_StartScene(_double TimeDelta);


	 /* UI */
	_int iCom_Texture_Set = 0;

public:
	void	Camera_Work(_double TimeDelta);
	
	// 카메라 워크 업데이트 함수들
	void	OpenBox(_double TimeDelta);
	void	Rage_Trans(_double TimeDelta);
	void	Rage_AttackEnd(_double TimeDelta);
	void	Rage_FinishAttack(_double TimeDelta);
	void	Hammer_Trans(_double TimeDelta);
	void	Hammer_AttackEnd(_double TimeDelta);
	void	Hammer_AirAttackEnd(_double TimeDelta);
	void	Hammer_FinishAttack(_double TimeDelta);
	void	CreatePortal(_double TimeDelta);
	void	Nobody_Appear(_double TimeDelta);
	void	BigTree_Appear(_double TimeDelta);
	void	BigTree_Climb(_double TimeDelta);
	void	Nightmare_Focus(_double TimeDelta);
	void	ReturnToPlayer(_double TimeDelta);
	void	ReturnToPlayerBack(_double TimeDelta);
	void	IceMan_Appear(_double TimeDelta);
	void	FrozenWolf_Appear(_double TimeDelta);
	void	FrozenWolf_Rain(_double TimeDelta);
	void	Goat_Appear(_double TimeDelta);
	void	Goat_PillarFire(_double TimeDelta);
	void	Goat_PillarRot(_double TimeDelta);

public:
	static CCamera_Perspective* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END