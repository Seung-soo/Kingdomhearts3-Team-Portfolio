#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CUI_MenuPlayer final : public CGameObject
{
private:
	CUI_MenuPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_MenuPlayer(const CUI_MenuPlayer& rhs);
	virtual ~CUI_MenuPlayer() = default;

	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;

public:
	static CUI_MenuPlayer*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END