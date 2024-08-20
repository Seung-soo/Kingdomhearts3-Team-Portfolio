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
		_float				fSize;					// ������
		_vector				vPos;					// ù ������ġ
		_vector				vDir;					// ���Ⱚ
		_float				fSpeed;					// ���ǵ�
		_float				fDamage;				// ������
		_double				Duration;				// ���ӽð�
		_bool				bNoModel = false;		// �� ����
		LEVEL				ModelPrototypeLevel;	// ���� ������Ÿ���� �ִ� ����
		_tchar				ModelTag[MAX_PATH];		// �� �±�
		_bool				bNoCollider = false;	// �ݶ��̴� ����
		_float4				vColliderPos = _float4(0.f, 0.f, 0.f, 1.f);	// �ݶ��̴� ��ġ

		// MONSTERSKILLTYPE_BEZIER �� �� �߰��� �ʿ��� ������
		_float				fLerpSpeed = 1.0;		// ���� ���ǵ�
		_vector				vDestPos;				// ���� ������ġ
		_vector				vViaPos;				// ���� ������ġ

		// MONSTERSKILLTYPE_SPREAD �� �� �߰��� �ʿ��� ������
		_double				SpreadTime = 0.0;		// ������ �ð�
		_float				fSpreadPower;			// ������ �Ŀ�
		_float				fOriginY;				// ���� ����

		// MONSTERSKILLTYPE_SHURIKEN �� �� �߰��� �ʿ��� ������
		CTransform*			pPlayerTransform;		// �÷��̾� Transform
		_double				PauseTime;				// ���߿����� �Ͻ����� �ð�

		// MONSTERSKILLTYPE_AWAIT_STRAIGHT �� �� �߰��� �ʿ��� ������
		_vector				vFirstDir;				// ���� �� ���� �ð����� ���� ����

		// MONSTERSKILLTYPE_ICEMANTRACEPLAYER �� �� �߰��� �ʿ��� ������
		_bool				bChase = true;

		//MONSTERSKILLTYPE_GOATMAGICSHOT �� �� �߰��� �ʿ��� ������
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
	void OnCollision(_double TimeDelta); // �浹 ó�� �Լ�

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