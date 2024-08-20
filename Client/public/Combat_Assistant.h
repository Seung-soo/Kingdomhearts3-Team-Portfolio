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
	:: ������ ���� �ȿ��� �ʿ��� ���
	1. ���� ����
	2. ��ġ ����
	3. ��� ����(���䰡��)

	---------------------------------------------------------

	4. ���� ����(�ִϸ��̼� ����) => CAction_Assistant�� ����
*/

typedef class CCombat_Assistant final : public CBase
{
public:
	enum ASSIST_TYPE
	{
		ASSIST_LOCKON,	// ���µ� ���
		ASSIST_FASTEN,	// ���� �� ������ ���
		ASSIST_ATTACK,	// ������ ������ ���
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
		CCollider*	pCollider;		// �浹ü

		_float		fDistance;		// �Ÿ�
		_float		fDistanceY;		// Y�� ���� �Ÿ�
		_float		fDistanceXZ;	// Y���� ������ �Ÿ�

		_float3		vDirection;		// ����
		_float3		vDirectionXZ;	// Y�� ���� ����

		_float		fAngleXZ;		// Y�� ���� ����

		_bool		bUpDown;		// �÷��̾� ���� ���Ʒ�(true : up, false : down)
		_bool		bLeftRight;		// �÷��̾� ���� �¿�(true : left, false : right)

	} ASSIST_DESC; // combat assist information for each target

public:
	explicit CCombat_Assistant() = default;
	virtual ~CCombat_Assistant() = default;

public:
	HRESULT Initialize(CGameObject* _chief, ASSIST_INIT_DESC _initDesc);
	void Run(_double Timedelta);
	void Clear();

public:
	// ��� ���� ���� �Լ� ====================================================
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


	// ����ü �� ��ü ���� �Լ� ===============================================
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

	_bool Eliminate_DeadTarget(CGameObject* _deadTarget); // ���������� �������� true

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

	_bool			m_bPower					= true;			// �۵� ����
	_bool			m_bAutoDir					= true;			// ���� ���� �۵� ����
	_bool			m_bAutoPos					= true;			// ��ġ ���� �۵� ����

	_float			m_fRange					= 0.f;
	_uint			m_iMaxNumber				= 0;
	_uint			m_iSkipNumber				= 0;
	_uint			m_iSkipLimit				= 0;

	// Ŭ���� ���� xz, y�� ���� �ʿ䰡 �ִ����� ���ؼ� ���Ŀ� �����غ���.
	_float			m_fClippingMin				= 0.f;
	_float			m_fClippingMax				= 2.f;
	_float			m_fClippingMin_Y			= 0.f;
	_float			m_fClippingMax_Y			= 1.f;

	CGameObject*	m_pPlayer					= nullptr;		// �÷��̾�
	CTransform*		m_pPlayerTransform			= nullptr;		// �÷��̾� Ʈ������

	CCollider*		m_pLockOn					= nullptr;		// ������ �ݶ��̴�
	CGameObject*	m_pLockOnObject				= nullptr;		// ������ ������Ʈ

	CCollider*		m_pFasten					= nullptr;		// ������ �ݶ��̴�
	CGameObject*	m_pFastenObject				= nullptr;		// ������ ������Ʈ

	CCollider*		m_pAttack					= nullptr;		// ������ �ݶ��̴�
	CGameObject*	m_pAttackObject				= nullptr;		// ������ ������Ʈ

	list<ASSIST_DESC> m_Colliders;
	list<CGameObject*> m_Objects;

public:
	static CCombat_Assistant* Create(CGameObject* _chief, ASSIST_INIT_DESC _initDesc);
	virtual void Free() override;
} C_ASSIST;

END
