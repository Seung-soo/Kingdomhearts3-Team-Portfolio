#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CStatus;
class CModel;
END

BEGIN(Client)

class CMonsterSkill final : public CMonster
{
public:
	enum MONSTERSKILLTYPE {
		MONSTERSKILLTYPE_STRAIGHT,
		MONSTERSKILLTYPE_AWAIT_STRAIGHT,
		MONSTERSKILLTYPE_BEZIER,
		MONSTERSKILLTYPE_BEZIERROTATION,
		MONSTERSKILLTYPE_SPREAD,
		MONSTERSKILLTYPE_SPIRAL,
		MONSTERSKILLTYPE_BOUNCE,
		MONSTERSKILLTYPE_SHURIKEN,
		MONSTERSKILLTYPE_TRACING,
		MONSTERSKILLTYPE_FALLSTRAIGHT,
		MONSTERSKILLTYPE_ICEMANTRACEPLAYER,
		MONSTERSKILLTYPE_GOATMAGICSHOT,
	};

	typedef struct tagMonsterSkillDesc
	{
		MONSTERSKILLTYPE	eSkillType = MONSTERSKILLTYPE_STRAIGHT;
		_float				fSize;					// 사이즈
		_vector				vPos;					// 첫 생성위치
		_vector				vDir;					// 방향값
		_float				fSpeed;					// 스피드
		_float				fDamage;				// 데미지
		_double				Duration;				// 지속시간
		_bool				bNoModel = false;		// 모델 여부
		LEVEL				ModelPrototypeLevel;	// 모델의 프로토타입이 있는 레벨
		_tchar				ModelTag[MAX_PATH];		// 모델 태그
		_bool				bNoCollider = false;	// 콜라이더 여부
		_float4				vColliderPos = _float4(0.f, 0.f, 0.f, 1.f);	// 콜라이더 위치

		// MONSTERSKILLTYPE_BEZIER 일 때 추가로 필요한 변수들
		_float				fLerpSpeed = 1.0;		// 보간 스피드
		_vector				vDestPos;				// 최종 보간위치
		_vector				vViaPos;				// 보간 경유위치

		// MONSTERSKILLTYPE_SPREAD 일 때 추가로 필요한 변수들
		_double				SpreadTime = 0.0;		// 퍼지는 시간
		_float				fSpreadPower;			// 퍼지는 파워
		_float				fOriginY;				// 시작 높이

		// MONSTERSKILLTYPE_SHURIKEN 일 때 추가로 필요한 변수들
		CTransform*			pPlayerTransform;		// 플레이어 Transform
		_double				PauseTime;				// 공중에서의 일시정지 시간

		// MONSTERSKILLTYPE_AWAIT_STRAIGHT 일 때 추가로 필요한 변수들
		_vector				vFirstDir;				// 시작 시 일정 시간동안 향할 방향

		// MONSTERSKILLTYPE_ICEMANTRACEPLAYER 일 때 추가로 필요한 변수들
		_bool				bChase = true;

		//MONSTERSKILLTYPE_GOATMAGICSHOT 일 때 추가로 필요한 변수들
		_double				RandTime;
	}MONSTERSKILLDESC;

private:
	CMonsterSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonsterSkill(const CMonsterSkill& rhs);
	virtual ~CMonsterSkill() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	void SetChaseState(_bool bOption) { m_MonsterSkillDesc.bChase = bOption; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	void Action(_double TimeDelta);
	void OnCollision(_double TimeDelta); // 충돌 처리 함수

private:
	CShader*		m_pShaderCom   = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CStatus*		m_pStatusCom   = nullptr;
	CModel*			m_pModelCom    = nullptr;

	class CTerrain* m_pTerrain = nullptr;

	MONSTERSKILLDESC	m_MonsterSkillDesc;
	_double				m_TimeAcc        = 0.0;
	_double				m_EffectTimeAcc  = 0.3;
	_double				m_EffectCountAcc = 0;
	_double				m_LerpAcc        = 0.0;

	_bool m_bStartBounce = false;

	_bool m_bEffectEnd = false;

	// For FrozenWolf
	_bool m_bStartChasing = false;
	_bool m_bFinishTurning = false;
	_vector m_vTargetPos;
	_vector m_vDestScale = __vector(6.f, 6.f, 6.f, 0.f);



public:
	static	CMonsterSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END