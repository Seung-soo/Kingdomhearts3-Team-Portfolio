#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class	CShader;
class	CRenderer;
class	CModel;
class	CTransform;
class   CCollider;
END

BEGIN(Client)

class CLevel_ColliderTool final : public CLevel
{
public:
	explicit CLevel_ColliderTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_ColliderTool() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Ready_Lights();
	HRESULT Ready_GameObjects();

private:
	//		애니메이션 이름
	char						m_szAnimationFileName[MAX_PATH] = "";

	//		모델 이름
	char						m_szModelFileName[MAX_PATH] = "";

	//		모델 프로토타입
	char						m_szModelPrototypeName[MAX_PATH] = "";


	//		콜라이더 반지름 정보
	vector<_tchar*>				m_vModelName;
	vector<char*>				m_vAnimationName;
	vector<char*>				m_vBoneName;

	_bool						m_bAnimationWireFrame = false;
	_bool						m_bAnimationBlend = false;

	_uint						m_iAnimationSelect = 0;
	_uint						m_iBoneSelect = 0;
	_float						m_fColliderLocalPos[3] = {};

	_float						m_fAnimModelScale[3] = { 1.f, 1.f, 1.f };
	_float						m_fAnimModelRotation[3] = {};
	_float						m_fAnimModelPosition[3] = {};

	_float						m_fColliderRadius = 1.f;


	class CColliderTool_Model*		m_pColliderModel;
private:
	void		Collider_Create();
	void		Animation_List();
	void		Model_Pick();
	void		Collider_BoneList();
	void		AnimModelTransform();

public:
	static CLevel_ColliderTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END