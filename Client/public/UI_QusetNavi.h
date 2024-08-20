#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_QusetNavi final : public CGameObject
{
private:
	CUI_QusetNavi(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_QusetNavi(const CUI_QusetNavi& rhs);
	virtual ~CUI_QusetNavi() = default;

public:
	void		Set_TargetPosition(_fvector vPos) { m_vTargetPosition = vPos; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

private:
	_float	fScaleX = 0.f;
	_float	fScaleY = 0.f;
	_float	fShader_Zero = 0.f;
	_bool	fShader_false = true;

	class CPlayer* m_pPlayer = nullptr;
	_vector	Player_Origin_Pos;
	_vector	m_vTargetPosition = __vector(0.f, 0.f, 0.f, 1.f);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_QusetNavi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END