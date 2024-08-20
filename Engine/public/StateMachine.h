#pragma once

#include "Component.h"
#include "State.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
public:
	// StateMachine 이 순서대로 실행시킬 State 의 타입을 나열
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
		STATETYPE_MONSTERSTATE, // 몬스터들은 IDLE 이랑 MONSTERSTATE 열거형만 사용
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
	// 기존 상태가 없다면 m_States 에 상태를 바인딩함, bDeletePrevState 가 true 라면 기존 상태 지우고 바인딩
	_bool	BindState(STATETYPE eType, string StateTag, CState* pState, _bool bDeletePrevState = false);
	_bool	FindStateTag(STATETYPE eType, string StateTag); // 이미 이름이 같은 상태가 있는지 찾고 있다면 true 반환
	_bool	FindStateTag(string StateTag);	// 모든 상태를 돌면서 이름이 같은 상태가 있는지 찾고 있다면 true 반환
	_bool	FindState(STATETYPE eType);		// eType 에 상태가 있는지 확인하고 있다면 true 반환
	_bool	FindMonsterState(); // STATETYPE_MONSTERSTATE 에 상태가 있는지 확인하고 있다면 true 반환
	HRESULT ClearState(STATETYPE eType);	// eType 에 있는 상태를 제거

	CState* Get_State(STATETYPE eType);
	string  Get_StateName(STATETYPE eType);
	_uint	Get_NumberOfStates();

	_bool	Get_ExistNextState(STATETYPE eType); // 해당 상태 이후의 다른 상태가 있는지 탐색

public: // 상태머신이 갖고있는 상태를 열거형(STATETYPE) 순서대로 실행
	HRESULT Action(_double TimeDelta);

	// 관련 애니메이션에 대한 컨테이너에 접근하는 메서드
	_bool Add_AnimIndex(_uint iType, _uint iAnimationIndex);
	_bool Rmv_AnimIndex(_uint iType, _uint iAnimationIndex);
	_bool Find_AnimIndex(_uint iType, _uint iAnimationIndex);

	// 공존할 수 없는 스테이트 목록을 관리하는 컨테이너에 접근하는 메서드
	_bool Add_SingleType(STATETYPE eStateType);
	_bool Rmv_SingleType(STATETYPE eStateType);
	_bool Find_SingleType(STATETYPE eStateType);

	// 중복될 수 없는 종류의 상태 중 입력받은 상태를 제외한 모든 상태를 머신에서 제거
	HRESULT Clear_CrashTypes(STATETYPE eStateType, _uint* pOut_NumberOfDeleted = nullptr);

private:
	HRESULT Set_Components(CState* pState);

private: // 열거형의 수만큼 상태들을 갖고있음 (이름 -> 상태)
	pair<string, CState*> m_States[STATETYPE_END] = {};

	list<_uint> m_StateAnimations[STATETYPE_END];		// 해당 상태와 연관된(사용되는) 애니메이션의 인덱스를 모아둘 컨테이너
	list<STATETYPE> m_SingleStateTypes;					// 중복될 수 없는 상태타입을 모아둘 컨테이너

public:
	static	CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END