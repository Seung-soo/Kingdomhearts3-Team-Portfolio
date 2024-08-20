#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	enum ANIM_TYPE { ANIM_ONCE, ANIM_LOOP, ANIM_END };

private:
	explicit CAnimation() = default;
	virtual ~CAnimation() = default;

public:
	void Reserve(_uint iNumChannels) {	// 공간 할당 함수
		m_Channels.reserve(iNumChannels);
		m_iNumChannels = iNumChannels;
	}

	const vector<class CChannel*>* Get_Channels() const { return &m_Channels; }
	void Add_Channel(class CChannel* pChannel) { m_Channels.push_back(pChannel); }

#pragma region Get, Set

	char* Get_Name() { return m_szName; }

	_double Get_Duration() { return m_Duration; }

	_double Get_PlaySpeed() { return m_TickPerSecond; }
	void Set_PlaySpeed(_double dPlaySpeed) { m_TickPerSecond = dPlaySpeed; }
	void Set_PlaySpeed_Rate(_double dPlaySpeedRate) { m_TickPerSecond = m_TickPerSecond_Origin * dPlaySpeedRate; }

	_double Get_PlaySpeed_Origin() { return m_TickPerSecond_Origin; }

	_double	Get_TimeAcc() { return m_PlayTimeAcc; }
	void Set_TimeAcc(_double dTimeAcc) { m_PlayTimeAcc = dTimeAcc; }

	ANIM_TYPE Get_AnimType() { return m_eAnimType; }
	void Set_AnimType(ANIM_TYPE eAnimType) { m_eAnimType = eAnimType; }

	_bool Get_Finished() { return m_isFinished; }
	void Set_Finished(_bool bFinished) { m_isFinished = bFinished; }

	_bool Get_Stopped() { return m_isStopped; }
	void Set_Stopped(_bool bStopped) { m_isStopped = bStopped; }

	_bool Get_Played() { return m_isPlayed; }
	void Set_Player(_bool bPlayed) { m_isPlayed = bPlayed; }

	_bool Get_Separated() { return m_isSeparated; }
	void Set_Separated(_bool bSeparated) { m_isSeparated = bSeparated; }

	_uint Get_RootNodeIndex() { return m_iRootNodeIndex; }
	void Set_RootNodeIndex(_uint iRootNodeIndex) { m_iRootNodeIndex = iRootNodeIndex; }

	_uint Get_RootNodeY() { return m_iRootNodeY; }
	void Set_RootNodeY(_uint iRootNodeY) { m_iRootNodeY = iRootNodeY; }

	_float Get_MovementAmountXZ() { return m_vMovementAmountXZ; }
	_float Get_MovementAmountY() { return m_vMovementAmountY; }

	_float Get_MovementRatioXZ() { return m_vMovementRatioXZ; }
	void Set_MovementRatioXZ(_float vRatioXZ) { m_vMovementRatioXZ = vRatioXZ; }

	_float Get_MovementRatioY() { return m_vMovementRatioY; }
	void Set_MovementRatioY(_float vRatioY) { m_vMovementRatioY = vRatioY; }

	_bool Get_UseRootY() { return m_bUseRootY; }
	void Set_UseRootY(_bool bUseY) { m_bUseRootY = bUseY; }

#pragma endregion

public:
	HRESULT	NativeConstruct(const char* pName, _double Duration, _double TickPerSecond);
	HRESULT	Update_TransformMatrices(_double TimeDelta, class CTransform* pTransform, class CNavigation* pNavigation);

	HRESULT Make_BodyPart(_uint iNumberOfBodyPart);
	HRESULT Fill_BodyPart(const char* pPartName, const char* pSearchName[], _uint iNumberOfSearchName);

	void Push_BodyIndex(_uint iBodyIndex);

	void Reset_Animation();
	void Reset_Animation_EachBody(_uint iBodyIndex);

	void PrevPos_Zero();
	void Calculate_MovementAmount();
	_float3 Calculate_Axes(_float3 _position);

	void Set_RootChannelY();

private:
	char				m_szName[MAX_PATH] = "";			// 애니메이션 이름

	_double				m_Duration = 0.0;					// 전체 재생 시간
	_double				m_TickPerSecond = 0.0;				// 시간당 애니메이션 재생 속도
	_double				m_TickPerSecond_Origin = 0.0;
	_double				m_PlayTimeAcc = 0.0;				// 애니메이션 누적 시간

	ANIM_TYPE			m_eAnimType = ANIM_TYPE::ANIM_END;	// 애니메이션 타입(한 번 재생, 반복 재생)

	_bool				m_isFinished = false;				// 키프레임 초기화하기 위한 루프 완료 여부
	_bool				m_isStopped = false;				// Once 애니메이션에서만 세팅되는 정지 여부
	_bool				m_isPlayed = false;					// 최소한 한 번이상 재생된 적이 있는지 여부

	_bool				m_isSeparated = false;				// 분리 여부
	_bool*				m_isEmpty = nullptr;				// 이름 벡터 덮어쓰기 막기위해 만든 변수
	_uint				m_iNumberOfBodyPart = UINT_MAX;		// 분리된 신체부위의 개수

	vector<const char*> m_Vector_PartName;
	typedef vector<CChannel*>	BODYPART;

	vector<CChannel*>*	m_pBodyPart = nullptr;				// 신체부위별로 분류된 채널을 모아두는 컨테이너
	vector<_uint>		m_Vector_PlayingBody;

	_uint				m_iRootNodeIndex = UINT_MAX;		// 실제 최상위 노드가 아니라 이동값이 있는 노드 중 최상위 노드의 인덱스
	_uint				m_iRootNodeY = UINT_MAX;
	_vector				m_vPrevRootPos = XMVectorZero();
	
	_bool				m_bUseRootY = false;
	_bool				m_bRecorded = false;
	_float				m_bRecorded_Gap = 0.f;
	_float3				m_vPosition_RootY;
	CChannel*			m_pRootChannel_Y = nullptr;

	_float				m_vMovementAmountXZ = 0.f;
	_float				m_vMovementAmountY = 0.f;

	_float				m_vMovementRatioXZ = 1.f;
	_float				m_vMovementRatioY = 1.f;

private:
	_uint							m_iNumChannels = 0;		// 애니메이션을 재생하는데 사용하는 뼈의 갯수
	vector<class CChannel*>			m_Channels;				// 애니메이션 재생하는데 필요한 뼈들을 저장하는 컨테이너
	typedef	vector<class CChannel*> CHANNELS;

public:
	static CAnimation*	Create(const char* pName, _double Duration, _double TickPerSecond);
	virtual void		Free() override;
};

END