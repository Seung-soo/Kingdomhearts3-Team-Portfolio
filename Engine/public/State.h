#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Transform.h"
#include "Model.h"

BEGIN(Engine)

#define DELETE_STATE true
#define KEEP_STATE false

class ENGINE_DLL CState abstract : public CBase
{
protected:
	explicit CState() = default;
	virtual ~CState() = default;

protected: // 자식 상태 객체에서는 private 선언
	virtual HRESULT Initialize(void* pArg) = 0;
	virtual _bool	KeyInput(_double TimeDelta) { return true; }

public: // 객체들의 매 Tick 에서 실행시킬 함수 (상태가 끝나면 true 리턴해서 StateMachine 에서 삭제)
	virtual _bool Action(_double TimeDelta) = 0;

public: // StateMachine 에서 실행시킬 함수
	HRESULT Set_Components(CGameObject* pGameObject, CTransform* pTransform, CModel* pModel, class CStateMachine* pStateMachine); // 상태가 생성될 때 멤버변수 세팅
	void Set_StateIndex(_uint iStateType) { m_iStateType = iStateType; }

public: // 상태 In, Out 때 실행시킬 함수
	virtual HRESULT EnterState() { return S_OK; }
	virtual HRESULT ExitState() { return S_OK; }
	virtual HRESULT ClearState() { return S_OK; }	// 다른 상태의 생성으로 강제로 삭제될 때 호출되는 함수

protected: // State 가 GameObject 를 갖고있다면 GameObject 의 m_Components 에서 컴포넌트 쉽게 접근 가능
	class CStateMachine*	m_pStateMachine = nullptr;
	CGameObject*			m_pGameObject	= nullptr;
	CModel*					m_pModelCom		= nullptr;
	CTransform*				m_pTransformCom = nullptr;

	_uint					m_iStateType = UINT_MAX;

public:
	virtual void Free() override {};
};

END