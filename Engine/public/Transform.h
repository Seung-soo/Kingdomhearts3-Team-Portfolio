#pragma once

#include "Component.h"

BEGIN(Engine)
class CNavigation;
class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	enum DAMAGEDDIR { DAMAGED_FRONT, DAMAGED_BACK, DAMAGED_RIGHT, DAMAGED_LEFT, DAMAGED_END };

	enum JUMP_TYPE { 
		JUMP_NORMAL,		// 일반적인 점프 상태
		JUMP_FALL,			// 자유낙하 발생에 의한 점프 상태	| 점프 파워 0부터 시작
		JUMP_CONTINUE,		// 기존의 낙하 에너지를 유지		| 기존의 점프 파워를 유지
		JUMP_LAND,			// 착지

		JUMP_FORCE,			// 외부로부터 받는 충격에 의한 점프상태
		JUMP_PERSONAL,		// 객체별 고유의 점프상태

		JUMP_Editional1,	// 추가슬롯 1 : 플레이어의 2단 점프
		JUMP_Editional2,	// 추가슬롯 2 : 플레이어의 호버링 상태에서의 점프
		JUMP_Editional3,	// 추가슬롯 3
		JUMP_END 
	};

public:
	typedef struct tagTransformDesc
	{
		_float fSpeedPerSec    = 0.f;
		_float fRotationPerSec = 0.f;
		_float fScalingPerSec  = 0.f;

	}TRANSFORMDESC;

	typedef struct tagJumpInfoStruct
	{
		JUMP_TYPE eJumpType = JUMP_END;

		_bool bJump = false;				// 체공 중인지를 나타내는 bool
		_bool bGravityEffect = false;		// 중력의 영향을 받을지 결정하는 bool

		_float fJumpHeight = 0.f;			// 점프 시작 높이
		_float fJumpPower = 0.f;			// 점프 세기 실시간 값
		_float fJumpValue = 0.f;			// 점프 세기 고정값

		_float fJumpFactor = 0.f;			// Speed를 사용할 때는 Speed의 절반만큼 Factor를 만들고 그 값을 Value에 곱해서 사용
		_float fJumpSpeed = 0.f;			// 점프 진행 속도

		_double dJumpTime = 0.0;

	} JUMP_DESC;

	typedef struct tagKnockBackStruct
	{
		_float3 vDirection = _float3(0.f);			// 방향

		_float fInputPower = 0.f;					// 고정 파워
		_float fPower = 0.f;						// 실시간 파워

		_double dInputTimer = 0.0;					// 고정 타이머(총 시간)
		_double dTimer = 0.0;						// 실시간 타이머(누적 시간)

	} KNOCKBACK_DESC;

	typedef struct tagMonsterJumpDesc
	{
		_float	fOriginY;
		_float	fJumpSpeed;
		_float	fJumpPower;
		_double JumpTimeAcc;
	}MONSTERJUMPDESC;

	typedef struct tagLocationAssistanceStruct
	{
		_float fInputHeight;

		_double dInputTimer;
		_double dTimer;
		_double dTimerSpeed;

	} LOCATION_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_Scale(STATE eState) const;
	_vector Get_AllScale() const;
	_vector Get_State(STATE eState) const;

	_vector		Get_NRight() const;
	_vector		Get_NUp() const;
	_vector		Get_NLook() const;
	_vector		Get_Right() const;
	_vector		Get_Up() const;
	_vector		Get_Look() const;
	_vector		Get_Position() const;
	_float4*	Get_PositionFloat4_Pointer();
	_float4		Get_Position_F4();

	void		Set_WorldMatrix(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }
	void		Set_WorldMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	_matrix		Get_WorldMatrix() const;
	_matrix		Get_WorldMatrixInverse() const;
	_float4x4	Get_WorldFloat4x4() const;
	_float4x4*	Get_WorldFloat4x4_Pointer();

	void		Set_OldWorldMatrixFloat4x4(_float4x4 WorldMatrix) { m_OldWorldMatrix = WorldMatrix; }
	_matrix		Get_OldWorldMatrix() const;
	_matrix		Get_OldWorldMatrixInverse() const;
	_float4x4	Get_OldWorldFloat4x4() const;

	void	Set_TransformDesc(TRANSFORMDESC tTransformDesc) { m_TransformDesc = tTransformDesc; }
	TRANSFORMDESC	Get_TransformDesc(TRANSFORMDESC tTransformDesc) { return m_TransformDesc; }
	void	Set_Speed(_float fSpeed) { m_TransformDesc.fSpeedPerSec = fSpeed; }
	_float	Get_Speed() { return m_TransformDesc.fSpeedPerSec; }

	// Degree
	void	Set_RotSpeed(_float fDegree) { m_TransformDesc.fRotationPerSec = XMConvertToRadians(fDegree); }
	_float	Get_RotSpeed() { return XMConvertToDegrees(m_TransformDesc.fRotationPerSec); }

	// Radian
	void	Set_RotSpeedR(_float fRadian) { m_TransformDesc.fRotationPerSec = fRadian; }
	_float	Get_RotSpeedR() { return m_TransformDesc.fRotationPerSec; }

public:
	void Set_State(STATE eState, _fvector vState);

	void	Set_Right(_fvector vState);
	void	Set_Up(_fvector vState);
	void	Set_Look(_fvector vState);
	void	Set_Position(_fvector vPos);
	_bool	Set_PositionCameraNavi(_fvector vPos, CNavigation* pNavigation);
	void	Set_PositionX(_float vPosX);
	void	Set_PositionY(_float vPosY);
	void	Set_PositionZ(_float vPosZ);
	void	Set_LookY_Zero(_double TimeDelta);
	void	Set_LookY_Zero_Once(_double TimeDelta);

	//	현재 위치에서 Right 방향으로 곱하기 fCoefficient만큼 움직임
	void Set_MoveBasedOnCurrentRight(_float fCoefficient);
	//	현재 위치에서 Up 방향으로 곱하기 fCoefficient만큼 움직임
	void Set_MoveBasedOnCurrentUp(_float fCoefficient);
	//	현재 위치에서 Look 방향으로 곱하기 fCoefficient만큼 움직임
	void Set_MoveBasedOnCurrentLook(_float fCoefficient);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Bind_OnShader(class CShader* pShader, const char* pValueName);
	HRESULT Bind_OnOldShader(class CShader* pShader, const char* pValueName);

public:
	HRESULT	Sliding_Vector(_fvector vPosition, _fvector vCurrentPosition, CNavigation* pNavigation, _double TimeDelta);

	/* 이동 함수들 (슬라이딩 벡터까지) */
	HRESULT Go_Straight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Right(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Left(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Backward(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_FrontLeft(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_FrontRight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_BackLeft(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_BackRight(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Up(_double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_Down(_double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool	Go_Straight_Check_NaviEnd(_double TimeDelta, CNavigation* pNavigation);

	/* 원하는 방향으로 가는 함수 */
	HRESULT Go_Vector(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_VectorXZ(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation = nullptr);

	/* 네비게이션 높이까지 떨어지는 함수 */
	_bool	SetHeightZero(_double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool	SetHeight(_double TimeDelta, _float fTargetHeight, CNavigation* pNavigation = nullptr);
	void	SetHeightNavi(_double TimeDelta, CNavigation* pNavigation);

	/* 객체를 끌어당기는 함수 */
	_bool PullObject(CTransform* pTargetTransform, _double TimeDelta, _double PullPower);

	/* 객체를 밀어내는 함수 함수 */
	_bool PushObject(CTransform* pTargetTransform, _double TimeDelta, _double MaxTime, _double PushPower, CNavigation* pNavigation);

	/* 범위 안에 있는지 판단하는 함수 */
	_bool IsInRange(CTransform* pTargetTransform, _float fRange);

	/* 회전 함수들 */
	HRESULT Turn(_fvector vAxis, _double TimeDelta);
	HRESULT Rotation(_fvector vAxis, _float fRadian);
	HRESULT NowRotation(_fvector vAxis, _float fRadian); // 현재 상태에서 회전
	HRESULT AllRotation(_fvector vRotationNum); // X, Y, Z 축으로 회전된 상태

	// 축, 공전 기준점 넣어준다
	HRESULT Revolution(_fvector vAxis, _fvector vTarget, _double TimeDelta);	// 공전
	_vector Revolution_And_ReturnLook(_fvector vAxis, _fvector vTarget, _double TimeDelta);	// 공전

	void	TeleportActivate(CNavigation* pNavigation);
	_bool	TeleportAroundTarget(CTransform* pTargetTransform, _fvector vDir, _float fDistance, CNavigation* pNavigation); // 타겟 주변으로 텔레포트
	_bool	Teleport(_fvector vDir, _float fDistance, CNavigation* pNavigation);

	_bool	IsUnderTarget(CTransform* pTargetTransform, _float fDistance);

	HRESULT Scaled(_fvector vScale);
	HRESULT OneScaled(STATE eState, _float fScale);			// 하나만 스케일 수정
	HRESULT Scaling(_double TimeDelta);					
	HRESULT OneScaling(STATE eState, _double TimeDelta);	// 하나만 스케일 점차 증가
	_bool	ScaledLerp(_fvector fStartScale, _fvector fDestScale, _float fLerpRatio, _double TimeDelta);

	// 따라가는 함수
	HRESULT Chase(_fvector vTargetPos, _double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Chase(CTransform* pTargetTransform, _double TimeDelta, CNavigation* pNavigation = nullptr);

	HRESULT LookAt(_fvector vTargetPos);
	HRESULT LookAt(CTransform* pTargetTransform);
	HRESULT LookAtXZ(_fvector vTargetPos); // 위나 아래로 기울지 않고 평평하게 X와 Z만 바라보게
	HRESULT LookAtXZ(CTransform* pTargetTransform); // 위나 아래로 기울지 않고 평평하게 X와 Z만 바라보게
	HRESULT LookAtY(_fvector vTargetPos);
	HRESULT LookTurn(_fvector vAxis, _fvector vTarget, _double TimeDelta);

	HRESULT Turn_Right(_fvector vAxis, _double TimeDelta);
	HRESULT Turn_Left(_fvector vAxis, _double TimeDelta);
	HRESULT Turn_Up(_double TimeDelta);
	HRESULT Turn_Down(_double TimeDelta);

	HRESULT CameraTurnY(_fvector vAxis, _double TimeDelta);

	_float	Get_Distance(CTransform* pTargetTransform); // 자신과 타겟 Transform 간 거리 계산
	_float	Get_Distance(_fvector vTargetPosition);		// 자신과 타겟 Position 간 거리 계산
	_vector Get_Dir(CTransform* pTargetTransform);		// 자신과 타겟 Transform 간 방향 계산
	_vector Get_Dir(_fvector vTargetPosition);			// 자신과 타겟 Position 간 방향 계산

	_bool Turn_To_Direction(_fvector vDir, _double TimeDelta, _float fDiff = 0.2f); // vDir 방향으로 회전 (좌우)
	_bool Turn_To_Direction(CTransform* pTargetTransform, _double TimeDelta, _float fDiff = 0.2f); // pTargetTransform 을 바라보도록 회전 (좌우)
	_bool Turn_To_Direction_AxisX(CTransform* pTargetTransform, _double TimeDelta, _float fDiff = 0.3f); // vDir 방향으로 회전 (상하)
	_bool Turn_To_Direction_AxisX(_fvector vTargetPos, _double TimeDelta, _float fDiff = 0.3f); // vDir 방향으로 회전 (상하)

	_bool Go_To_Target(CTransform* pTargetTransform, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool Go_To_Target(_fvector vTargetPos, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool Go_To_TargetXZ(CTransform* pTargetTransform, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool Go_To_TargetXZ(_fvector vTargetPos, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool Go_To_TargetXZ_SetHeight(CTransform* pTargetTransform, _float fDistDiff, _float fTargetHeight, _double SetHeightSpeedRatio, _double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool Go_To_TargetXZ_SetHeight(_fvector vTargetPos, _float fDistDiff, _float fTargetHeight, _double SetHeightSpeedRatio, _double TimeDelta, CNavigation* pNavigation = nullptr);

	DAMAGEDDIR Get_Damaged_4Dir(CTransform* pTargetTransform);
	DAMAGEDDIR Get_Damaged_4Dir(_fvector vTargetPosition);
	DAMAGEDDIR Get_Damaged_2Dir(CTransform* pTargetTransform);
	DAMAGEDDIR Get_Damaged_2Dir(_fvector vTargetPosition);

	_float	Get_Distance_From_Mouse();
	void	Random_Rotation(_double TimeDeltla);

	// 선형보간을 사용한 Move, LookAt
	_bool	Move_To_Target_Lerp(_fvector vStartPos, _fvector vDestPos, _float fLerpPercentage, _double TimeDelta);
	_bool	Look_Target_Lerp(_fvector vDestPos, _float fLerpPercentage, _double TimeDelta, _bool bRemoveY = false);

	HRESULT	MakeSameDirection(CTransform* pTransform); // 같은 방향으로 룩을 설정
	HRESULT	MakeSameDirection(_fvector vDir);

	// 나선 무브, 몬스터 점프
	void	SpiralMoveXZ(_double TimeDelta, class CNavigation* pNavigation = nullptr);

	/* Functions about Jumping */ // ==============================================================================================================================
	void JumpInput(_float fHeight, _float fPower, _float fSpeed = 1.f, _bool bJump = true, _bool bGravityEffect = true); // JumpInput 입력을 편리하게 하기위한 함수
	void BasicJump(JUMP_TYPE eJumpType); // JumpInput 프리셋이 적용되어있는 함수

	_float Calc_DirectionOnJump(_float fGravity); // 상승, 하강 중인지 구분

	JUMP_DESC Get_JumpDesc() { return m_JumpDesc; }
	void Set_JumpDesc(JUMP_DESC tJumpDesc) { m_JumpDesc = tJumpDesc; }

	JUMP_DESC* Get_JumpDescPtr() { return &m_JumpDesc; }
	void Set_JumpDescPtr(JUMP_DESC* pJumpDesc) { m_JumpDesc = *pJumpDesc; }

	_bool Get_JumpBoolean() { return m_JumpDesc.bJump; }
	void Set_JumpBoolean(_bool bJumpBoolean) { m_JumpDesc.bJump = bJumpBoolean; }

	_bool Get_GravityEffect() { return m_JumpDesc.bGravityEffect; }
	void Set_GravityEffect(_bool bGravityEffect) { m_JumpDesc.bGravityEffect = bGravityEffect; }

	_float Get_JumpValue() { return m_JumpDesc.fJumpValue; }
	void Set_JumpValue(_float fJumpValue) { m_JumpDesc.fJumpValue = fJumpValue; }

	_float Get_JumpSpeed() { return m_JumpDesc.fJumpSpeed; }
	void Set_JumpSpeed(_float fJumpSpeed) { m_JumpDesc.fJumpSpeed = fJumpSpeed; }

	void SetMonsterJumpDesc(MONSTERJUMPDESC monsterJumpDesc) { m_MonsterJumpDesc = monsterJumpDesc; }
	void MonsterJump(_double TimeDelta, _float fMinY);
	// ============================================================================================================================================================
	/* Functinos about KnockBack */ // ============================================================================================================================

	/* 
		KnockBack Power Input :  0.5 ~ 2.5	사이의 값 0.5단위
		 - 0.5
		 - 1.0
		 - 1.5
		 - 2.0
		 - 2.5

		KnockBack Timer Input :  0.25 ~ 1	사이의 값 0.25단위
		 - 0.25
		 - 0.5
		 - 0.75
		 - 1.0
	*/

	void KnockBackInput(_float3 vDirection, _float fPower, _double dTimer);
	void KnockBackInput_v(_fvector vDirection, _float fPower, _double dTimer);

	_int KnockBackTick(_double TimeDelta, CNavigation* pNavigation);
	_float Calculate_KnockBack_Attenuation();

	KNOCKBACK_DESC Get_KnockBackDesc() { return m_KnockBackDesc; }
	void Set_KnockBackDesc(KNOCKBACK_DESC tKnockBackDesc) { m_KnockBackDesc = tKnockBackDesc; }

	KNOCKBACK_DESC* Get_KnockBackDesc_Ptr() { return &m_KnockBackDesc; }
	void Set_KnockBackDesc_Ptr(KNOCKBACK_DESC* pKnockBackDesc) { m_KnockBackDesc = *pKnockBackDesc; }

	_float3 Get_KnockBackDir() { return m_KnockBackDesc.vDirection; }
	void Set_KnockBackDir(_float3 vDir) { m_KnockBackDesc.vDirection = vDir; }

	_float Get_KnockBackInputPower() { return m_KnockBackDesc.fInputPower; }
	void Set_KnockBackInputPower(_float fPower) { m_KnockBackDesc.fInputPower = fPower; }

	_float Get_KnockBackPower() { return m_KnockBackDesc.fPower; }
	void Set_KnockBackPower(_float fPower) { m_KnockBackDesc.fPower = fPower; }
	
	_double Get_KnockBackInputTime() { return m_KnockBackDesc.dInputTimer; }
	void Set_KnockBackInputTime(_double dTimer) { m_KnockBackDesc.dInputTimer = dTimer; }

	_double Get_KnockBackTime() { return m_KnockBackDesc.dTimer; }
	void Set_KnockBackTime(_double dTimer) { m_KnockBackDesc.dTimer = dTimer; }

	// ============================================================================================================================================================
	/* Functinos about LocationAssistance */ // ============================================================================================================================
	void LocationAssistanceInput(_float fMoveAmountY, _double dDuration, _double dTimerSpeed);

	_int LocationAssistanceTick(_double Timedelta, CNavigation* pNavigation);

	LOCATION_DESC Get_LocationDesc() { return m_LocationDesc; }
	void Set_LocationDesc(LOCATION_DESC tLocationDesc) { m_LocationDesc = tLocationDesc; }

	LOCATION_DESC* Get_LocationDesc_Ptr() { return &m_LocationDesc; }
	void Set_LocationDesc_Ptr(LOCATION_DESC* tLocationDesc) { m_LocationDesc = *tLocationDesc; }

	_float Get_LocationInputHeight() { return m_LocationDesc.fInputHeight; }
	void Set_LocationInputHeight(_float fHeight) { m_LocationDesc.fInputHeight = fHeight; }

	_double Get_LocationInputTimer() { return m_LocationDesc.dInputTimer; }
	void Set_LocationInputTimer(_double dTimer) { m_LocationDesc.dInputTimer = dTimer; }

	_double Get_LocationTimer() { return m_LocationDesc.dTimer; }
	void Set_LocationTimer(_double dTimer) { m_LocationDesc.dTimer = dTimer; }

	// ============================================================================================================================================================
	// 애니메이션 싱크함수
	void Sync_Animation(_vector vMoveValue, _double TimeDelta, CNavigation* pNavigation = nullptr);

private:
	TRANSFORMDESC			m_TransformDesc;
	JUMP_DESC				m_JumpDesc;
	KNOCKBACK_DESC			m_KnockBackDesc;
	LOCATION_DESC			m_LocationDesc;

	_double					m_TimeAcc = 0.0;

	MONSTERJUMPDESC			m_MonsterJumpDesc;

	_float4x4				m_WorldMatrix;
	_float4x4				m_OldWorldMatrix;

	_float4					m_vPosition;

	_vector					m_vTeleportPos;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END