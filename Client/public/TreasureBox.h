#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;

END

BEGIN(Client)

class CTreasureBox final : public CGameObject
{
public:
	enum STATE { STATE_CLOSE, STATE_OPENNING, STATE_OPEN, STATE_END };

	enum BOX_THEME { THEME_TT, THEME_RA, THEME_FZ, THEME_END };

	enum EVENT { EVENT_QUEST, EVENT_KEYBLADE, EVENT_DONALD, EVENT_GOOFY, EVENT_HAMMER, EVENT_RAZYFORM, EVENT_END };
public:
	typedef struct tagTreasureBoxDesc
	{
		_vector		vPosition	= __vector(0.f, 0.f, 0.f, 1.f);
		_vector		vLook		= __vector(0.f, 0.f, 1.f, 0.f);
		BOX_THEME	eBoxTheme	= THEME_END;
		EVENT		eEvent      = EVENT_END;
	}BOXDESC;
private:
	CTreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTreasureBox(const CTreasureBox& rhs);
	virtual ~CTreasureBox() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	STATE				m_eCurrentState = STATE_END;
	BOXDESC				m_tBoxDesc;

	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bButton = false;
	CGameObject*		m_pButtonUI = nullptr;

	_bool				m_bOpen = false;
	_float				m_fOpenTime = 0.f;

	// For Effect
	_float				m_fEffectTimeAccum = 0.f;
	_bool				m_bPlayedParticle = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	HRESULT	State_Manage(_double TimeDelta);

	void	Close(_double TimeDelta);
	void	Openning(_double TimeDelta);
	void	Open(_double TimeDelta);

private:
	void	Event();

	void	Quest();
	void	KeyBlade();
	void	Donald();
	void	Goofy();
	void	Hammer();
	void	RazyForm();
public:
	static CTreasureBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END