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

class CUI_Target final : public CUI_Parents
{
public:
	CUI_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Target(const CUI_Target& rhs);
	virtual ~CUI_Target() = default;

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

	/*
		얘 해야되는 연출.

		yellow : 타겟이 잡힐 때 
		 - start scale에서 end scale로 보간을 통해 작아짐.

		blue : 타겟이 잡힐 때 
		 - 외곽 ui는 start scale에서 end scale로 보간을 통해 작아짐.
		 - 내부 ui는 반시계 방향으로 천천히 회전함

		 :: 타겟이 잡힐 때 -> Render = true 걸릴 때 타이머 주면 될듯.
	*/

	class CPlayer* m_pPlayer = nullptr;
	_bool m_bRender = false;

	_float4	m_vTargetPosition;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
, (XMVectorGetY(m_vViewPortPosition) - 1) * -(g_iWinCY * 0.5f)
, 0.f, 1.f);

*/