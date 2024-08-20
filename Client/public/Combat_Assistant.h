#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CCollider;
class CTransform;
END

BEGIN(Client)

/*
	:: 구상한 범위 안에서 필요한 기능
	1. 방향 보조
	2. 위치 보조
	3. 방어 보조(오토가드)

	---------------------------------------------------------

	4. 동작 보조(애니메이션 선택) => CAction_Assistant로 이전
*/

typedef class CCombat_Assistant final : public CBase
{
public:
	enum ASSIST_TYPE
	{
		ASSIST_LOCKON,	// 락온된 대상
		ASSIST_FASTEN,	// 락온 후 고정된 대상
		ASSIST_ATTACK,	// 공격이 적중한 대상
		ASSIST_END
	};

	typedef struct tagCombatAssistantInitStruct
	{
		_float range;
		_uint max_search;
		_uint skip_limit;

	} ASSIST_INIT_DESC;


	typedef struct tagCombatAssistantStruct
	{
		CCollider*	pCollider;		// 충돌체

		_float		fDistance;		// 거리
		_float		fDistanceY;		// Y축 상의 거리
		_float		fDistanceXZ;	// Y축을 제외한 거리

		_float3		vDirection;		// 방향
		_float3		vDirectionXZ;	// Y값 제외 방향

		_float		fAngleXZ;		// Y값 제외 각도

		_bool		bUpDown;		// 플레이어 기준 위아래(true : up, false : down)
		_bool		bLeftRight;		// 플레이어 기준 좌우(true : left, false : right)

	} ASSIST_DESC; // combat assist information for each target

public:
	explicit CCombat_Assistant() = default;
	virtual ~CCombat_Assistant() = default;

public:
	HRESULT Initialize(CGameObject* _chief, ASSIST_INIT_DESC _initDesc);
	void Run(_double Timedelta);
	void Clear();

public:
	// 멤버 변수 관련 함수 ====================================================
	_uint NumberOfColliders() { return (_uint)m_Colliders.size(); }
	_uint NumberOfObjects() { return (_uint)m_Objects.size(); }

	_bool Power() { return m_bPower; }
	void Power(_bool _power) { m_bPower = _power; }

	_bool AutoDir() { return m_bAutoDir; }
	void AutoDir(_bool _dir) { m_bAutoDir = _dir; }

	_bool AutoPos() { return m_bAutoPos; }
	void AutoPos(_bool _pos) { m_bAutoPos = _pos; }

	_float Range() { return m_fRange; }
	void Range(_float _range) { m_fRange = _range; }

	_uint MaxNumber() { return m_iMaxNumber; }
	void MaxNumber(_uint _maxNumber) { m_iMaxNumber = _maxNumber; }

	_uint SkipNumber() { return m_iSkipNumber; }
	void SkipNumber(_uint _skipNumber) { m_iSkipNumber = _skipNumber; }

	_uint SkipLimit() { return m_iSkipLimit; }
	void SkipLimit(_uint _skipLimit) { m_iSkipLimit = _skipLimit; }

	_float ClippingMin() { return m_fClippingMin; }
	void ClippingMin(_float _clippingMin) { m_fClippingMin = _clippingMin; }

	_float ClippingMax() { return m_fClippingMax; }
	void ClippingMax(_float _clippingMax) { m_fClippingMax = _clippingMax; }

	_float ClippingMin_Y() { return m_fClippingMin_Y; }
	void ClippingMin_Y(_float _clippingMinY) { m_fClippingMin_Y = _clippingMinY; }

	_float ClippingMax_Y() { return m_fClippingMax_Y; }
	void ClippingMax_Y(_float _clippingMaxY) { m_fClippingMax_Y = _clippingMaxY; }


	// 구조체 및 객체 관련 함수 ===============================================
	CCollider* Get_TargetCollider(ASSIST_TYPE* out = nullptr);
	ASSIST_TYPE Get_TargetType();

	CCollider* Get_Collider(ASSIST_TYPE _assistType);
	void Set_Collider(ASSIST_TYPE _assistType, CCollider* _collider);

	CGameObject* Get_Object(ASSIST_TYPE _assistType);
	void Set_Object(ASSIST_TYPE _assistType, CGameObject* _object);

	void Setup_Target(ASSIST_TYPE _assistType, CCollider* _collider);
	void Clear_Target(ASSIST_TYPE _assistType);

	ASSIST_DESC Get_Desc(ASSIST_TYPE _assistType);

	list<ASSIST_DESC>* Get_AllSearched() { return &m_Colliders; }
	list<CGameObject*> Get_AllSearched_Objects() { return m_Objects; }

public:
	void Change_FastenTarget();

	void Auto_Rotation();
	void Auto_Position(_float _movementXZ, _float _movementY);

	_uint EnemyNumber_In_Range(_float _range);

	_bool Eliminate_DeadTarget(CGameObject* _deadTarget); // 정상적으로 지워지면 true

private:
	void Calculate_Values(ASSIST_DESC& _desc);

	_bool Check_Duplicated_Col(CCollider* _collider);
	_bool Check_Duplicated_Col(ASSIST_DESC _desc);

	_bool Check_Duplicated_Obj(CGameObject* _object);
	_bool Check_Duplicated_Obj(ASSIST_DESC _desc);

	_bool Add_ColliderList(CCollider* _collider);
	_bool Rmv_ColliderList(ASSIST_DESC _desc);

	_bool Add_ObjectList(CGameObject* _object);
	_bool Rmv_ObjectList(ASSIST_DESC _desc);

private:
	CGameInstance*	m_pGI						= nullptr;

	_bool			m_bPower					= true;			// 작동 여부
	_bool			m_bAutoDir					= true;			// 방향 보정 작동 여부
	_bool			m_bAutoPos					= true;			// 위치 보정 작동 여부

	_float			m_fRange					= 0.f;
	_uint			m_iMaxNumber				= 0;
	_uint			m_iSkipNumber				= 0;
	_uint			m_iSkipLimit				= 0;

	// 클리핑 값을 xz, y로 나눌 필요가 있는지에 대해서 추후에 생각해보기.
	_float			m_fClippingMin				= 0.f;
	_float			m_fClippingMax				= 2.f;
	_float			m_fClippingMin_Y			= 0.f;
	_float			m_fClippingMax_Y			= 1.f;

	CGameObject*	m_pPlayer					= nullptr;		// 플레이어
	CTransform*		m_pPlayerTransform			= nullptr;		// 플레이어 트랜스폼

	CCollider*		m_pLockOn					= nullptr;		// 락온한 콜라이더
	CGameObject*	m_pLockOnObject				= nullptr;		// 락온한 오브젝트

	CCollider*		m_pFasten					= nullptr;		// 고정한 콜라이더
	CGameObject*	m_pFastenObject				= nullptr;		// 고정한 오브젝트

	CCollider*		m_pAttack					= nullptr;		// 공격한 콜라이더
	CGameObject*	m_pAttackObject				= nullptr;		// 공격한 오브젝트

	list<ASSIST_DESC> m_Colliders;
	list<CGameObject*> m_Objects;

public:
	static CCombat_Assistant* Create(CGameObject* _chief, ASSIST_INIT_DESC _initDesc);
	virtual void Free() override;
} C_ASSIST;

END
