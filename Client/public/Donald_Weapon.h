#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)

class CDonald_Weapon final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
	} WEAPON_DESC;

public:
	CDonald_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDonald_Weapon(const CDonald_Weapon& rhs);
	virtual ~CDonald_Weapon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

private:
	WEAPON_DESC				m_tWeaponDesc = {};

	CHierarchyNode*			m_pNode = nullptr;
	_float4x4*				m_pCombineMatrix;
	_float4x4*				m_pSocketMatrix;
public:
	static CDonald_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END