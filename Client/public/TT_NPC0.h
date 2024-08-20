#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CTT_NPC0 final : public CGameObject
{
private:
	CTT_NPC0(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTT_NPC0(const CTT_NPC0& rhs);
	virtual ~CTT_NPC0() = default;
public:
	void			Set_NpcLook(_bool bNpcLook) { m_bNpcLook = bNpcLook; }
	void			Set_Talk(_bool bTalk) { m_bTalk = bTalk; }
	void			Set_TalkFinish(_bool bTalkFinish) { m_bTalkFinish = bTalkFinish; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	void	Quest(_double TimeDelta);

private:
	_float		Sety = 0.0f;
	_float		Sety_Speed = 0.01f;

	_double		dTimeSet = 0.0;
	_bool		SetyBool = false;

private:
	// 진행정도
	_uint						m_iProgress			= 0;
	CCollider*					m_pPlayerCollider	= nullptr;
	_bool						m_bNpcLook			= true;
	_bool						m_bTalk             = false;
	class CCamera_Perspective*	m_pCamera			= nullptr;
	_bool						m_bTalkStart		= false;
	_bool						m_bTalkFinish       = false;

	_bool						m_bTurn				= false;
	_vector						m_vDir              = __vector(0.f, 0.f, 1.f, 0.f);
public:
	static CTT_NPC0*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END