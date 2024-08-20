#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)

class CColliderTool_Model final : public CGameObject
{
private:
	CColliderTool_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderTool_Model(const CColliderTool_Model& rhs);
	virtual ~CColliderTool_Model() = default;

public:
	CModel*			Get_Model();
	CCollider*		Get_Collider() { return m_pColliderCom; }

	void			Set_BoneName(const char* pName) { memcpy(m_szBoneName, pName, MAX_PATH); }
	void			Set_LocalPos(_fvector vLocalPos) { m_vLocalPos = vLocalPos; }
	HRESULT			Set_Model(_tchar* pPrototypeTag);
	void			Set_ColliderRadius(_float fRadius);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	char					m_szBoneName[MAX_PATH] = {};
	CHierarchyNode*			m_pNode = nullptr;
	_float					m_fColliderRadius = 1.f;
	CCollider::COLLIDERDESC m_tColliderDesc = {};
	_vector					m_vLocalPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CColliderTool_Model*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END