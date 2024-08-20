#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CMonster;
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CIcicle final : public CGameObject
{
private:
	CIcicle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CIcicle(const CIcicle& rhs);
	virtual ~CIcicle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
	virtual void	PostRender(_double TimeDelta);

public:
	_bool IsIcicleInRange(_vector vPos, _float fRange);

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT	Load_ObjectData(HANDLE hFile) override;

private:
	HRESULT SetUp_Animations();
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta); // 충돌 처리 함수

private:
	virtual CCollider* Get_HitCollider() { return m_pColliderCom[0]; }

private:
	CModel*			m_pModelCom       = nullptr;
	CShader*		m_pShaderCom      = nullptr;
	CRenderer*		m_pRendererCom    = nullptr;
	CCollider*		m_pColliderCom[2] = { nullptr, };

	const _tchar* m_pPrototypeTag[4] = { TEXT("Prototype_Component_Model_Icicle_A"), TEXT("Prototype_Component_Model_Icicle_B"), TEXT("Prototype_Component_Model_Icicle_C"), TEXT("Prototype_Component_Model_Icicle_D") };

	_double m_TimeAcc   = 0.0;
	_bool	m_bPlayAnim = false;

	_bool m_bSoundEnd = false;

public:
	static	CIcicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END