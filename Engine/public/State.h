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

protected: // �ڽ� ���� ��ü������ private ����
	virtual HRESULT Initialize(void* pArg) = 0;
	virtual _bool	KeyInput(_double TimeDelta) { return true; }

public: // ��ü���� �� Tick ���� �����ų �Լ� (���°� ������ true �����ؼ� StateMachine ���� ����)
	virtual _bool Action(_double TimeDelta) = 0;

public: // StateMachine ���� �����ų �Լ�
	HRESULT Set_Components(CGameObject* pGameObject, CTransform* pTransform, CModel* pModel, class CStateMachine* pStateMachine); // ���°� ������ �� ������� ����
	void Set_StateIndex(_uint iStateType) { m_iStateType = iStateType; }

public: // ���� In, Out �� �����ų �Լ�
	virtual HRESULT EnterState() { return S_OK; }
	virtual HRESULT ExitState() { return S_OK; }
	virtual HRESULT ClearState() { return S_OK; }	// �ٸ� ������ �������� ������ ������ �� ȣ��Ǵ� �Լ�

protected: // State �� GameObject �� �����ִٸ� GameObject �� m_Components ���� ������Ʈ ���� ���� ����
	class CStateMachine*	m_pStateMachine = nullptr;
	CGameObject*			m_pGameObject	= nullptr;
	CModel*					m_pModelCom		= nullptr;
	CTransform*				m_pTransformCom = nullptr;

	_uint					m_iStateType = UINT_MAX;

public:
	virtual void Free() override {};
};

END