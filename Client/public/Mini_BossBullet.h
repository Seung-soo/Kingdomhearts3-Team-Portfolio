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
class CMini_BossBullet final : public CUI_Parents
{
private:
	CMini_BossBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMini_BossBullet(const CMini_BossBullet& rhs);
	virtual ~CMini_BossBullet() = default;

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

	_float4 fPlayer_Position = _float4{ 0.f, 0.f, 0.f, 0.f };
	_float4 fBossBullet_Position = _float4{ 0.f, 0.f, 0.f, 0.f };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	void	Boss1_Bullet(_double TimeDelta);
	_double	SpeedXTime = 0;
	_double	SpeedX = 1;

	_double	SpeedYTime = 0;
	_double	SpeedY = 1;

	void	Boss2_Bullet1(_double TimeDelta);
	_double MinusCnt = 1;
	_double	Create2_Time = 0;
	_int	Create2_Random = 0;

	void	Boss2_Bullet2(_double TimeDelta);
	_float4	Bullet2_1_Position = _float4{ 0.f, 0.f, 0.f, 0.f };
	_int	Speed2_2Random = 0;

	void	Boss3_Bullet1(_double TimeDelta);
	void	Boss3_Bullet2(_double TimeDelta);
	void	Boss3_Bullet3(_double TimeDelta);
	_int	Bullet3_Random = 0;
	_double	Create3_Time = 0;
	_double Create3_Time2 = 0;
	_float	Bullet3_2_Fade = 0;

	void	Boss4_Bullet1(_double TimeDelta);
	void	Boss4_Bullet2(_double TimeDelta);
	_int	Boss4Pattern = 0;
	_int	Create4_Random = 0;

	/* === */


	_int	Random_Position = 0;
	_int	Random_Frame = 0;
	_int	Move_Random = 0;
	_int	iBullet_Texture = 0;

	void	Bullet_Collider();

public:
	static CMini_BossBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END