#pragma once
#include "GameObject.h"
#include "VIBuffer_Trail.h"
#include "Client_Defines.h"
#include "Model.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CEffect_Trail final: public CGameObject
{
public:
	enum TYPE {TYPE_PRE, TYPE_CUR, TYPE_END};
	enum DIR { RIGHT, UP, LOOK, DIR_END };

	typedef struct tagTrailDesc {
		CTransform*						pTargetTransform = nullptr;
		CModel::BONEMATRIX_PTR			pBoneMatrixPtr;
		_float4x4						SocketPivotmMatrix;

		_float2							vLocalPos = _float2(0.5f, -0.5f);
		CVIBuffer_Trail::TRAILDESC		tTrailBuffDesc;
		_float							fTransparency = 0.f;
		_bool							bBlur = false;
		_float							bDistortion = false;
		_float4							vColor;
		_float							fInterval = 0.5f;

		_uint							iTextureIndex = 0;

		DIR								eDir = RIGHT;

		_uint							iPassIndex = 5;

		_vector							vAdjustValue = XMVectorSet(0.f, 0.2f, 0.f, 0.f);

		_bool							isNotBone = false;			

		_matrix							LocalMatrix = XMMatrixIdentity();

		_bool							isWind = false;
	}TRAILDESC;

public:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void Set_Active(_bool isActive) override; 

	TRAILDESC Get_Desc() {return m_tDesc;}

	void	  Set_Desc(TRAILDESC tDesc) {m_tDesc = tDesc;}

	void	  Set_Render(_bool bRender) { m_bRender = bRender; }

	void	  ChangeColor(_float4 vColor) { m_vColor = vColor;}
	void	  ResetColor() { m_vColor = m_tDesc.vColor; }

	virtual void	PostRender(_double TimeDelta);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	HRESULT Ending_Motion(_double TimeDelta);

public:
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Trail*			m_pBufferCom = nullptr;
	CTransform*					m_pTargetTransform = nullptr;


	_float						m_fTimeAccum = 0.f;
	_float						m_fInterval = 0.f;

	_matrix						m_vWorld[TYPE_END];

	_bool						m_isTest = false;

	_bool						m_isEnding = false;

	TRAILDESC					m_tDesc;

	_float4						m_vColor;

	_bool						m_bRender = true;
};

END