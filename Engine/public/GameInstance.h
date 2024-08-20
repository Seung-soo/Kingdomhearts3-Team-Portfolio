#pragma once

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Input_Device.h"
#include "SoundMgr.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Picking.h"
#include "Target_Manager.h"
#include "Frustum.h"
#include "Font_Manager.h"
#include "Mouse.h"
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	MAKE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;
public:
	HRESULT			Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	_int			Tick_Engine(_double dTimeDelta);
	HRESULT			Clear_LevelResource(_uint iLevelIndex);
public:		// 그래픽 디바이스 기능
	ID3D11Device*			Get_Device();
	ID3D11DeviceContext*	Get_Device_Context();
	HRESULT					Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT					Clear_DepthStencil_View();
	HRESULT					Present();
public:		// 타이머 매니저 기능
	_double			Get_TimeDelta(const _tchar* pTimerTag); // 델타타임 반환
	HRESULT			Add_Timer(const _tchar* pTimerTag);		// 새로운 타이머 추가
public:		// 레벨 매니저 기능
	HRESULT			OpenLevel(_uint iNextLevelIndex, CLevel* pNextLevel);
	HRESULT			Render_Level();
	_uint			Get_LevelIndex();
	void			Level_Free();
public:		// 오브젝트 매니저 기능
	void						GameObject_PostRender(_double TimeDelta);
	CLayer*						Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	CComponent*					Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT						Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT						Rmv_Prototype(const _tchar* pPrototypeTag);
	HRESULT						Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT						Rmv_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, CGameObject* pObject);
	CGameObject*				Add_GameObject_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT						ThrowToLayer(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg);
	CGameObject*				ThrowToLayer_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg);
	HRESULT						Add_ObjectPool(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT						FrontChange(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT						Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag);
	size_t						Get_Size(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_Front(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_Back(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_ObjectPoolBack(_uint iLevelIndex, const _tchar* pLayerTag);
	list<CGameObject*>*			Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	map<const _tchar*, CLayer*> Get_ObjectMap(_uint iLevelIndex);
	HRESULT						Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT						Save_Layer(_uint iLevelIndex, const _tchar* pLayerTag, HANDLE hFile);
	void						Set_TimeSpeed(_uint iLevelIndex, const _tchar* pLayerTag, _double TimeSpeed);
public:		// 컴포넌트 매니저 기능
	HRESULT			Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	HRESULT			Rmv_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	CComponent*		Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CComponent*		Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);
	void			Clear_All_Components();
public:		// 인풋 디바이스 기능
	_byte			Get_DIKeyState(_ubyte eKeyID);

	_long			Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState);
	_byte			Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_Mouse_Enter(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_Mouse_Up(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);
	_bool			Get_KeyEnter(_ubyte eKeyID);
	_bool			Get_KeyUp(_ubyte eKeyID);
	_bool			Get_KeyPressing(_ubyte eKeyID);
public:		// 사운드 매니저 기능
	int				VolumeUp(CSoundMgr::CHANNELID eID, _float _vol);
	int				VolumeDown(CSoundMgr::CHANNELID eID, _float _vol);
	int				BGMVolumeUp(_float _vol);
	int				BGMVolumeDown(_float _vol);
	int				Pause(CSoundMgr::CHANNELID eID);
	_bool			IsPlaying(CSoundMgr::CHANNELID eID);
	void			LetsPlaySound(TCHAR* pSoundKey, CSoundMgr::CHANNELID eID, _float _vol, _bool b3DSound = false, _fvector vPosition = __vector(0.f, 0.f, 0.f, 1.f));
	void			PlayRoofSound(TCHAR* pSoundKey, CSoundMgr::CHANNELID eID, _float _vol, _bool b3DSound = false, _fvector vPosition = __vector(0.f, 0.f, 0.f, 1.f));
	void			PlayBGM(TCHAR* pSoundKey, _float _vol);
	void			StopSound(CSoundMgr::CHANNELID eID);
	void			StopAll();
	void			PauseAll(_bool bPause);
	void			LetsPlaySound_UI(TCHAR* pSoundKey, CSoundMgr::CHANNELUI eID, _float _vol);
	_uint			LetsPlaySound_Monster(TCHAR* pSoundKey, _float _vol, _bool b3DSound = false, _fvector vPosition = __vector(0.f, 0.f, 0.f, 1.f));
	_uint			PlayMonsterRoofSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition);
	void			StopMonsterSound(_uint iID);
	void			StopSound_UI(CSoundMgr::CHANNELUI eID);

public:		// 파이프라인 기능
	HRESULT			Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);
	_matrix			Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4		Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4		Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_vector			Get_CamPosition();
	void			PipeLineUpdate();
public:		// 라이트 매니저 기능
	LIGHTDESC*			Get_LightDesc(_uint iIndex);
	HRESULT				Add_Light
	(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	CLight_Manager::LIGHTMATRIX_DESC Get_LightMatrix(CLight_Manager::SHADOWTYPE eType);
	void Set_StaticLightViewMatrix(_fvector vLightPos);
	LIGHTDESC*			Get_DirectionalLightDesc();

	HRESULT				Release_Light();
public:		// 절두체 기능
	_bool				isIn_WorldSpace(_fvector vPoint, _float fRange = 0.f);
public:		// 폰트 매니저 기능
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor);
public:		// 마우스 기능
	pair<_bool, _vector>	Check_Ray_VirtualPlane_Collision(_float fPlaneY); // 가상의 평면이랑 충돌했는지 체크하고, pair 로 충돌여부, 충돌지점을 반환
	_vector					Get_MouseWorldDir();
	_bool					Ray_Box_Collision(const BoundingBox& box, _Out_ float& Dist);
	_bool					Ray_Sphere_Collision(const BoundingSphere& sphere, _Out_ float& Dist);

public:		// 콜리전 매니저 기능

			// 원하는 태그의 콜라이더 리스트를 가져온다.
	list<CCollider*>*	Get_ColliderList(string ColliderTag);
	HRESULT				Add_Collider(string ColliderTag, CCollider* pCollider);
	HRESULT				Pop_DeadObjectCollider(string pColliderTag, CCollider* pCollider);
	_bool				Collision_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);
	_bool				Collision_RayToAABB(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);
	_bool				Collision_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);

public:		// 타겟 매니저 기능
	ID3D11ShaderResourceView* Get_RenderTargetSRV(const _tchar* pTargetTag);
private:
	CGraphic_Device*		m_pGraphic_Device    = nullptr;
	CTimer_Manager*			m_pTimer_Manager     = nullptr;
	CLevel_Manager*			m_pLevel_Manager     = nullptr;
	CObject_Manager*		m_pObject_Manager    = nullptr;
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	CInput_Device*			m_pInput_Device      = nullptr;
	CSoundMgr*				m_pSound_Manager     = nullptr;
	CPipeLine*				m_pPipeLine          = nullptr;
	CLight_Manager*			m_pLight_Manager     = nullptr;
	CPicking*				m_pPicking           = nullptr;
	CTarget_Manager*		m_pTargetManager     = nullptr;
	CFrustum*				m_pFrustum           = nullptr;
	CFont_Manager*			m_pFont_Manager      = nullptr;
	CMouse*					m_pMouse             = nullptr;
	CCollision_Manager*		m_pCollision_Manager = nullptr;
public:
	static void		Release_Engine();
	virtual void	Free() override;
};

END