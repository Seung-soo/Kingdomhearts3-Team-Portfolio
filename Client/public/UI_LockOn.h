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

class CUI_LockOn final : public CUI_Parents
{
private:
	CUI_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_LockOn(const CUI_LockOn& rhs);
	virtual ~CUI_LockOn() = default;

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

private:
	_int			iCom_Texture_Set = 0;
	

public:
	static CUI_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END

/*

_matrix      ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

m_vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

ViewMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
m_vViewPortPosition = XMVector3TransformCoord(m_vMyPosition, ViewMatrix);
m_fZ = XMVectorGetZ(m_vViewPortPosition);

_matrix   ProjMatrix;
ProjMatrix = g_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
m_vViewPortPosition = XMVector3TransformCoord(m_vViewPortPosition, ProjMatrix);

m_vViewPortPosition = XMVectorSet((XMVectorGetX(m_vViewPortPosition) + 1) * (g_iWinCX * 0.5f)
, (XMVectorGetY(m_vViewPortPosition) - 1) * -(g_iWinCY * 0.5f), 0.f, 1.f);

*/