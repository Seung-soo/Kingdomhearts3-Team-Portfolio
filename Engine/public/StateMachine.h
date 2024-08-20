#pragma once

#include "Component.h"
#include "State.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
public:
	// StateMachine �� ������� �����ų State �� Ÿ���� ����
	enum STATETYPE
	{
		STATETYPE_FORM,

		STATETYPE_IDLE,
		STATETYPE_NORMALMOVE,
		STATETYPE_DYNAMICMOVE,
		STATETYPE_JUMP,
		STATETYPE_ATK,
		STATETYPE_SUMMON,
		STATETYPE_GUARD,
		STATETYPE_AVOID,
		STATETYPE_DAMAGED,
		STATETYPE_INTERACTION,
		STATETYPE_MONSTERSTATE, // ���͵��� IDLE �̶� MONSTERSTATE �������� ���
		STATETYPE_END
	};

private:
	explicit CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	// ���� ���°� ���ٸ� m_States �� ���¸� ���ε���, bDeletePrevState �� true ��� ���� ���� ����� ���ε�
	_bool	BindState(STATETYPE eType, string StateTag, CState* pState, _bool bDeletePrevState = false);
	_bool	FindStateTag(STATETYPE eType, string StateTag); // �̹� �̸��� ���� ���°� �ִ��� ã�� �ִٸ� true ��ȯ
	_bool	FindStateTag(string StateTag);	// ��� ���¸� ���鼭 �̸��� ���� ���°� �ִ��� ã�� �ִٸ� true ��ȯ
	_bool	FindState(STATETYPE eType);		// eType �� ���°� �ִ��� Ȯ���ϰ� �ִٸ� true ��ȯ
	_bool	FindMonsterState(); // STATETYPE_MONSTERSTATE �� ���°� �ִ��� Ȯ���ϰ� �ִٸ� true ��ȯ
	HRESULT ClearState(STATETYPE eType);	// eType �� �ִ� ���¸� ����

	CState* Get_State(STATETYPE eType);
	string  Get_StateName(STATETYPE eType);
	_uint	Get_NumberOfStates();

	_bool	Get_ExistNextState(STATETYPE eType); // �ش� ���� ������ �ٸ� ���°� �ִ��� Ž��

public: // ���¸ӽ��� �����ִ� ���¸� ������(STATETYPE) ������� ����
	HRESULT Action(_double TimeDelta);

	// ���� �ִϸ��̼ǿ� ���� �����̳ʿ� �����ϴ� �޼���
	_bool Add_AnimIndex(_uint iType, _uint iAnimationIndex);
	_bool Rmv_AnimIndex(_uint iType, _uint iAnimationIndex);
	_bool Find_AnimIndex(_uint iType, _uint iAnimationIndex);

	// ������ �� ���� ������Ʈ ����� �����ϴ� �����̳ʿ� �����ϴ� �޼���
	_bool Add_SingleType(STATETYPE eStateType);
	_bool Rmv_SingleType(STATETYPE eStateType);
	_bool Find_SingleType(STATETYPE eStateType);

	// �ߺ��� �� ���� ������ ���� �� �Է¹��� ���¸� ������ ��� ���¸� �ӽſ��� ����
	HRESULT Clear_CrashTypes(STATETYPE eStateType, _uint* pOut_NumberOfDeleted = nullptr);

private:
	HRESULT Set_Components(CState* pState);

private: // �������� ����ŭ ���µ��� �������� (�̸� -> ����)
	pair<string, CState*> m_States[STATETYPE_END] = {};

	list<_uint> m_StateAnimations[STATETYPE_END];		// �ش� ���¿� ������(���Ǵ�) �ִϸ��̼��� �ε����� ��Ƶ� �����̳�
	list<STATETYPE> m_SingleStateTypes;					// �ߺ��� �� ���� ����Ÿ���� ��Ƶ� �����̳�

public:
	static	CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END