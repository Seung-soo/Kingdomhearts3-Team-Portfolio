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
		JUMP_NORMAL,		// �Ϲ����� ���� ����
		JUMP_FALL,			// �������� �߻��� ���� ���� ����	| ���� �Ŀ� 0���� ����
		JUMP_CONTINUE,		// ������ ���� �������� ����		| ������ ���� �Ŀ��� ����
		JUMP_LAND,			// ����

		JUMP_FORCE,			// �ܺηκ��� �޴� ��ݿ� ���� ��������
		JUMP_PERSONAL,		// ��ü�� ������ ��������

		JUMP_Editional1,	// �߰����� 1 : �÷��̾��� 2�� ����
		JUMP_Editional2,	// �߰����� 2 : �÷��̾��� ȣ���� ���¿����� ����
		JUMP_Editional3,	// �߰����� 3
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

		_bool bJump = false;				// ü�� �������� ��Ÿ���� bool
		_bool bGravityEffect = false;		// �߷��� ������ ������ �����ϴ� bool

		_float fJumpHeight = 0.f;			// ���� ���� ����
		_float fJumpPower = 0.f;			// ���� ���� �ǽð� ��
		_float fJumpValue = 0.f;			// ���� ���� ������

		_float fJumpFactor = 0.f;			// Speed�� ����� ���� Speed�� ���ݸ�ŭ Factor�� ����� �� ���� Value�� ���ؼ� ���
		_float fJumpSpeed = 0.f;			// ���� ���� �ӵ�

		_double dJumpTime = 0.0;

	} JUMP_DESC;

	typedef struct tagKnockBackStruct
	{
		_float3 vDirection = _float3(0.f);			// ����

		_float fInputPower = 0.f;					// ���� �Ŀ�
		_float fPower = 0.f;						// �ǽð� �Ŀ�

		_double dInputTimer = 0.0;					// ���� Ÿ�̸�(�� �ð�)
		_double dTimer = 0.0;						// �ǽð� Ÿ�̸�(���� �ð�)

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

	//	���� ��ġ���� Right �������� ���ϱ� fCoefficient��ŭ ������
	void Set_MoveBasedOnCurrentRight(_float fCoefficient);
	//	���� ��ġ���� Up �������� ���ϱ� fCoefficient��ŭ ������
	void Set_MoveBasedOnCurrentUp(_float fCoefficient);
	//	���� ��ġ���� Look �������� ���ϱ� fCoefficient��ŭ ������
	void Set_MoveBasedOnCurrentLook(_float fCoefficient);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Bind_OnShader(class CShader* pShader, const char* pValueName);
	HRESULT Bind_OnOldShader(class CShader* pShader, const char* pValueName);

public:
	HRESULT	Sliding_Vector(_fvector vPosition, _fvector vCurrentPosition, CNavigation* pNavigation, _double TimeDelta);

	/* �̵� �Լ��� (�����̵� ���ͱ���) */
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

	/* ���ϴ� �������� ���� �Լ� */
	HRESULT Go_Vector(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Go_VectorXZ(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation = nullptr);

	/* �׺���̼� ���̱��� �������� �Լ� */
	_bool	SetHeightZero(_double TimeDelta, CNavigation* pNavigation = nullptr);
	_bool	SetHeight(_double TimeDelta, _float fTargetHeight, CNavigation* pNavigation = nullptr);
	void	SetHeightNavi(_double TimeDelta, CNavigation* pNavigation);

	/* ��ü�� ������� �Լ� */
	_bool PullObject(CTransform* pTargetTransform, _double TimeDelta, _double PullPower);

	/* ��ü�� �о�� �Լ� �Լ� */
	_bool PushObject(CTransform* pTargetTransform, _double TimeDelta, _double MaxTime, _double PushPower, CNavigation* pNavigation);

	/* ���� �ȿ� �ִ��� �Ǵ��ϴ� �Լ� */
	_bool IsInRange(CTransform* pTargetTransform, _float fRange);

	/* ȸ�� �Լ��� */
	HRESULT Turn(_fvector vAxis, _double TimeDelta);
	HRESULT Rotation(_fvector vAxis, _float fRadian);
	HRESULT NowRotation(_fvector vAxis, _float fRadian); // ���� ���¿��� ȸ��
	HRESULT AllRotation(_fvector vRotationNum); // X, Y, Z ������ ȸ���� ����

	// ��, ���� ������ �־��ش�
	HRESULT Revolution(_fvector vAxis, _fvector vTarget, _double TimeDelta);	// ����
	_vector Revolution_And_ReturnLook(_fvector vAxis, _fvector vTarget, _double TimeDelta);	// ����

	void	TeleportActivate(CNavigation* pNavigation);
	_bool	TeleportAroundTarget(CTransform* pTargetTransform, _fvector vDir, _float fDistance, CNavigation* pNavigation); // Ÿ�� �ֺ����� �ڷ���Ʈ
	_bool	Teleport(_fvector vDir, _float fDistance, CNavigation* pNavigation);

	_bool	IsUnderTarget(CTransform* pTargetTransform, _float fDistance);

	HRESULT Scaled(_fvector vScale);
	HRESULT OneScaled(STATE eState, _float fScale);			// �ϳ��� ������ ����
	HRESULT Scaling(_double TimeDelta);					
	HRESULT OneScaling(STATE eState, _double TimeDelta);	// �ϳ��� ������ ���� ����
	_bool	ScaledLerp(_fvector fStartScale, _fvector fDestScale, _float fLerpRatio, _double TimeDelta);

	// ���󰡴� �Լ�
	HRESULT Chase(_fvector vTargetPos, _double TimeDelta, CNavigation* pNavigation = nullptr);
	HRESULT Chase(CTransform* pTargetTransform, _double TimeDelta, CNavigation* pNavigation = nullptr);

	HRESULT LookAt(_fvector vTargetPos);
	HRESULT LookAt(CTransform* pTargetTransform);
	HRESULT LookAtXZ(_fvector vTargetPos); // ���� �Ʒ��� ����� �ʰ� �����ϰ� X�� Z�� �ٶ󺸰�
	HRESULT LookAtXZ(CTransform* pTargetTransform); // ���� �Ʒ��� ����� �ʰ� �����ϰ� X�� Z�� �ٶ󺸰�
	HRESULT LookAtY(_fvector vTargetPos);
	HRESULT LookTurn(_fvector vAxis, _fvector vTarget, _double TimeDelta);

	HRESULT Turn_Right(_fvector vAxis, _double TimeDelta);
	HRESULT Turn_Left(_fvector vAxis, _double TimeDelta);
	HRESULT Turn_Up(_double TimeDelta);
	HRESULT Turn_Down(_double TimeDelta);

	HRESULT CameraTurnY(_fvector vAxis, _double TimeDelta);

	_float	Get_Distance(CTransform* pTargetTransform); // �ڽŰ� Ÿ�� Transform �� �Ÿ� ���
	_float	Get_Distance(_fvector vTargetPosition);		// �ڽŰ� Ÿ�� Position �� �Ÿ� ���
	_vector Get_Dir(CTransform* pTargetTransform);		// �ڽŰ� Ÿ�� Transform �� ���� ���
	_vector Get_Dir(_fvector vTargetPosition);			// �ڽŰ� Ÿ�� Position �� ���� ���

	_bool Turn_To_Direction(_fvector vDir, _double TimeDelta, _float fDiff = 0.2f); // vDir �������� ȸ�� (�¿�)
	_bool Turn_To_Direction(CTransform* pTargetTransform, _double TimeDelta, _float fDiff = 0.2f); // pTargetTransform �� �ٶ󺸵��� ȸ�� (�¿�)
	_bool Turn_To_Direction_AxisX(CTransform* pTargetTransform, _double TimeDelta, _float fDiff = 0.3f); // vDir �������� ȸ�� (����)
	_bool Turn_To_Direction_AxisX(_fvector vTargetPos, _double TimeDelta, _float fDiff = 0.3f); // vDir �������� ȸ�� (����)

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

	// ���������� ����� Move, LookAt
	_bool	Move_To_Target_Lerp(_fvector vStartPos, _fvector vDestPos, _float fLerpPercentage, _double TimeDelta);
	_bool	Look_Target_Lerp(_fvector vDestPos, _float fLerpPercentage, _double TimeDelta, _bool bRemoveY = false);

	HRESULT	MakeSameDirection(CTransform* pTransform); // ���� �������� ���� ����
	HRESULT	MakeSameDirection(_fvector vDir);

	// ���� ����, ���� ����
	void	SpiralMoveXZ(_double TimeDelta, class CNavigation* pNavigation = nullptr);

	/* Functions about Jumping */ // ==============================================================================================================================
	void JumpInput(_float fHeight, _float fPower, _float fSpeed = 1.f, _bool bJump = true, _bool bGravityEffect = true); // JumpInput �Է��� ���ϰ� �ϱ����� �Լ�
	void BasicJump(JUMP_TYPE eJumpType); // JumpInput �������� ����Ǿ��ִ� �Լ�

	_float Calc_DirectionOnJump(_float fGravity); // ���, �ϰ� ������ ����

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
		KnockBack Power Input :  0.5 ~ 2.5	������ �� 0.5����
		 - 0.5
		 - 1.0
		 - 1.5
		 - 2.0
		 - 2.5

		KnockBack Timer Input :  0.25 ~ 1	������ �� 0.25����
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
	// �ִϸ��̼� ��ũ�Լ�
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