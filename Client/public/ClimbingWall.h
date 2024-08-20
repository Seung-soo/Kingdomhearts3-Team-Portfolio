#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CClimbingWall final : public CGameObject
{
private:
	CClimbingWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CClimbingWall(const CClimbingWall& rhs);
	virtual ~CClimbingWall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	CTransform*			m_pPlayerTransform = nullptr;		// 플레이어 트랜스폼
	_vector				m_vPrevPlayerPos;					// 이전 플레이어 포지션

	_float				m_fPlayerHalfSize = 0.3f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CClimbingWall*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END