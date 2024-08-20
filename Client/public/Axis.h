#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Axis;
END

BEGIN(Client)

// È­¸é Áß¾Ó¿¡ x(»¡°­),y(ÆÄ¶û),z(³ë¶û) ¸¦ ¼±À¸·Î Ç¥½ÃÇÏ´Â °´Ã¼

class CAxis final : public CGameObject
{
private:
	CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CAxis(const CAxis& rhs);
	virtual ~CAxis() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*		m_pShaderCom   = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CVIBuffer_Axis* m_pVIBufferCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CAxis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END