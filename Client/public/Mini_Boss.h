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

class CMini_Boss final : public CUI_Parents
{
private:
	CMini_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMini_Boss(const CMini_Boss& rhs);
	virtual ~CMini_Boss() = default;

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
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	void	Boss1_Pattern(_double TimeDelta);
	void	Boss2_Pattern(_double TimeDelta);
	void	Boss3_Pattern(_double TimeDelta);
	void	Boss4_Pattern(_double TimeDelta);
	void	Boss5_Pattern(_double TimeDelta);
	/* === */

	void	Position_Check();
	_vector	vBossPosition;
	_float4	fBossPosition = _float4{ 0.f, 0.f, 0.f, 0.f };

	void	Boss_Dead(_double TimeDelta);
	void	Render_Reward();
	_float4	DeathPosition = _float4{ 0.f, 0.f, 0.f, 0.f };
	_int	Fade_Cnt = 0;
	/* === */
	_int	Boss_Cnt = 0;
	
	_double	dTime = 0;

	_double	dMoveTIme = 0.0;
	_double	dMoveSpeed_X = 1.f;
	_double	dMoveSpeed_Y = 1.f;
	_bool	bMoveUp = false;

	_float	Fade = 0.f;
	_int	iBullet_Texture = 0;
	_double	Boss_DeathTime = 0;

	/* Boss1 */
	_double	Position_Cnt_X = 1;
	_double	Position_Cnt_Y = 1;

	_double	BossAttackTime = 0;

	/* Boss2 */
	_bool	Create_2 = false;

	/* Boss3 */
	_double	dBoss3_Time = 0;
	_bool	bBoss3_Time = false;
	_bool	bBoss3_Pattern = false;

	_double	dCreateTime_3 = 0;

	/* Boss4 */
	_double	dCreateTime_4 = 0;

	_double DeadSoundTime = 0;
	_int	DeadSoundCnt = 0;


public:
	static CMini_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END