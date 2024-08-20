#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "MeshContainer.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CMotionTrail final : public CGameObject
{
public:
	typedef struct tagMotionTrailDesc
	{
		vector<CModel::MESHDESC*>*	pMeshDescVector;
		_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
		_uint	iSize = 0;
		_float	fRimPower = 1.f;
	}MOTIONTRAILDESC;
private:
	CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMotionTrail(const CMotionTrail& rhs);
	virtual ~CMotionTrail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
private:
	_float						m_fLiveTime = 0.f;
	MOTIONTRAILDESC				m_tMotionTrailDesc;
	vector<CModel::MESHDESC*>	m_MeshDescVector;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMotionTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END