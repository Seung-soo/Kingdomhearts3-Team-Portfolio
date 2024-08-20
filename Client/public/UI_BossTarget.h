#pragma once

#include "Client_Defines.h"
#include "UI_Parents.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)

class CUI_BossTarget final : public CUI_Parents
{
public:
	CUI_BossTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_BossTarget(const CUI_BossTarget& rhs);
	virtual ~CUI_BossTarget() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private: 
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer*					m_pVIBufferCom = nullptr;

	_int iCom_Texture = 0;

	class CPlayer* m_pPlayer = nullptr;
	_bool m_bRender = false;
	_bool btest = false;

	_float4	m_vTargetPosition;
	_bool	bPosition_Z = false;
	_bool	bPositionCheck = false;

	_vector target_position;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	void Position_Fnc();

public:
	static CUI_BossTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END