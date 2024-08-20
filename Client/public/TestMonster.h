#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "MonsterDefines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CStateMachine;
class CCollider;
class CStatus;
END

BEGIN(Client)

class CTestMonster final : public CGameObject
{
private:
	CTestMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTestMonster(const CTestMonster& rhs);
	virtual ~CTestMonster() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT Load_ObjectData(HANDLE hFile) override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void KeyInput(_double TimeDelta);
	void OnCollision(_double TimeDelta); // 충돌 처리 함수
	void ChangeState(_double TimeDelta); // 상태 변경 함수

private: /* 테스트용 함수 */
	HRESULT Set_Model(_tchar* pPrototypeTag);	// 모델 변경해서 테스트하는 함수

private:
	_int m_SelectMonster = -1;
	const char* m_Monsters[17] = {	"Sora",
									"SkullSoldier", "CrowSoldier", "ThinSoldier", "Nightmare", "TreasureBox_TT",
									"Monkey", "Mower", "ShiningEgg", "Nobody",
									"BigTree",
									"BowMan", "Cannon",
									"Goat", "IceMan", "FrozenWolf", "LittleWolf" };

private:
	CStateMachine*	m_pStateMachineCom = nullptr;
	CShader*		m_pShaderCom       = nullptr;
	CRenderer*		m_pRendererCom     = nullptr;
	CModel*			m_pModelCom        = nullptr;
	CCollider*		m_pColliderCom     = nullptr;
	CStatus*		m_pStatusCom       = nullptr;

	_bool			m_bPositionLock = false;

	CGameObject*	m_pPlayer          = nullptr;
	CTransform*		m_pPlayerTransform = nullptr;
public:
	static CTestMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END