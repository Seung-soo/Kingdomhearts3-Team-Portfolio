#pragma once
#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

class CMINI_Bullet final : public CUI_Parents
{
private:
	CMINI_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMINI_Bullet(const CMINI_Bullet& rhs);
	virtual ~CMINI_Bullet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	_double	dTime = 0;
	_double dMoveSpeed = 0;
	_double	dBulletSpeed = 0;

	_int	Random_Damage = 0;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	void	Bullet_Collider();
	void	Create_Damage();

public:
	static CMINI_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

