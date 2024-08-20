#include "..\public\GameInstance.h"

READY_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pSound_Manager(CSoundMgr::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pPicking(CPicking::GetInstance())
	, m_pTargetManager(CTarget_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pMouse(CMouse::GetInstance())
	, m_pCollision_Manager(CCollision_Manager::GetInstance())
{
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pMouse);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pTargetManager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pLevel_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppDeviceContextOut)
{
	CHECK_NULL(m_pGraphic_Device);

	CHECK_NULL(m_pObject_Manager);

	CHECK_NULL(m_pComponent_Manager);

	CHECK_FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppDeviceContextOut));

	CHECK_FAILED(m_pInput_Device->Ready_Input_Device(hInstance, GraphicDesc.hWnd));

	CHECK_FAILED(m_pObject_Manager->Reserve_Container(iNumLevels));

	CHECK_FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels));

	CHECK_FAILED(m_pMouse->NativeConstruct(m_pGraphic_Device->Get_Device(), m_pGraphic_Device->Get_Device_Context(), GraphicDesc.hWnd));

	CHECK_FAILED(m_pFrustum->Initialize());

	CHECK_FAILED(m_pTargetManager->Initialize(*ppDeviceOut, *ppDeviceContextOut));

	CHECK_FAILED(m_pLight_Manager->Initialize(*ppDeviceOut, *ppDeviceContextOut));

	CHECK_FAILED(m_pPicking->NativeConstruct(*ppDeviceOut, *ppDeviceContextOut, GraphicDesc.hWnd));

	m_pSound_Manager->Initialize();

	return S_OK;
}

_int CGameInstance::Tick_Engine(_double dTimeDelta)
{
	RETURN_NEGATIVE(if (nullptr == m_pLevel_Manager));

	RETURN_NEGATIVE(if (nullptr == m_pObject_Manager));

	RETURN_NEGATIVE(if (FAILED(m_pInput_Device->SetUp_InputDeviceState())));

	RETURN_NEGATIVE(if (0 > m_pObject_Manager->Tick(dTimeDelta)));

	RETURN_NEGATIVE(if (0 > m_pLevel_Manager->Tick(dTimeDelta)));

	m_pFrustum->Update();
//#ifdef _DEBUG
	m_pTargetManager->Update();
//#endif

	RETURN_NEGATIVE(if (0 > m_pObject_Manager->LateTick(dTimeDelta)));

	RETURN_NEGATIVE(if (0 > m_pLevel_Manager->LateTick(dTimeDelta)));

	m_pPipeLine->Update();
	m_pLight_Manager->Update();

	m_pCollision_Manager->Clear_Collider();

	RETURN_NEGATIVE(if (FAILED(m_pPicking->Transform_ToWorldSpace())));

	return 0;
}

HRESULT CGameInstance::Clear_LevelResource(_uint iLevelIndex)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	RETURN_FAIL(if (nullptr == m_pComponent_Manager))

	CHECK_FAILED(m_pObject_Manager->Clear_LevelObject(iLevelIndex))

	CHECK_FAILED(m_pComponent_Manager->Clear_LevelComponent(iLevelIndex))

	return S_OK;
}

ID3D11Device* CGameInstance::Get_Device()
{
	RETURN_NULL(if (nullptr == m_pGraphic_Device))

	return m_pGraphic_Device->Get_Device();
}

ID3D11DeviceContext* CGameInstance::Get_Device_Context()
{
	RETURN_NULL(if (nullptr == m_pGraphic_Device))

	return m_pGraphic_Device->Get_Device_Context();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	RETURN_FAIL(if (nullptr == m_pGraphic_Device))

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	RETURN_FAIL(if (nullptr == m_pGraphic_Device))

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	RETURN_FAIL(if (nullptr == m_pGraphic_Device))

	return m_pGraphic_Device->Present();
}

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
	{
		BREAKPOINT;
		return 0.0;
	}

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	RETURN_FAIL(if (nullptr == m_pTimer_Manager))

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::OpenLevel(_uint iNextLevelIndex, CLevel * pNextLevel)
{
	RETURN_FAIL(if (nullptr == m_pLevel_Manager))

	return m_pLevel_Manager->OpenLevel(iNextLevelIndex, pNextLevel);
}

HRESULT CGameInstance::Render_Level()
{
	RETURN_FAIL(if (nullptr == m_pLevel_Manager))

	return m_pLevel_Manager->Render();
}

_uint CGameInstance::Get_LevelIndex()
{
	return m_pLevel_Manager->Get_LevelIndex();
}

void CGameInstance::Level_Free()
{
	m_pLevel_Manager->Free();
}

void CGameInstance::GameObject_PostRender(_double TimeDelta)
{
	if (nullptr == m_pObject_Manager)
	{
		BREAKPOINT;
		return;
	}

	m_pObject_Manager->GameObject_PostRender(TimeDelta);
}

CLayer * CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	RETURN_NULL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	RETURN_NULL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Rmv_Prototype(const _tchar * pPrototypeTag)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Rmv_Prototype(pPrototypeTag);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Rmv_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, CGameObject * pObject)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Rmv_GameObject(iLevelIndex, pLayerTag, pObject);
}

CGameObject* CGameInstance::Add_GameObject_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	RETURN_NULL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Add_GameObject_And_Return(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::ThrowToLayer(_uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->ThrowToLayer(iLevelIndex, pLayerTag, pArg);
}

CGameObject* CGameInstance::ThrowToLayer_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	return m_pObject_Manager->ThrowToLayer_And_Return(iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_ObjectPool(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Add_ObjectPool(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::FrontChange(_uint iLevelIndex, const _tchar * pLayerTag)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->FrontChange(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Release_LayerObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	RETURN_FAIL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Release_LayerObject(iLevelIndex, pLayerTag);
}

size_t CGameInstance::Get_Size(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		BREAKPOINT;
		return 0;
	}

	return m_pObject_Manager->Get_Size(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_Front(_uint iLevelIndex, const _tchar * pLayerTag)
{
	RETURN_NULL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Get_Front(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_Back(_uint iLevelIndex, const _tchar * pLayerTag)
{
	RETURN_NULL(if (nullptr == m_pObject_Manager))

	return m_pObject_Manager->Get_Back(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Get_ObjectPoolBack(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pObject_Manager->Get_ObjectPoolBack(iLevelIndex, pLayerTag);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		BREAKPOINT;		return nullptr;
	}

	return m_pObject_Manager->Get_ObjectList(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pObject_Manager->Get_GameObject(iLevelIndex, pLayerTag, iIndex);
}

map<const _tchar*, CLayer*> CGameInstance::Get_ObjectMap(_uint iLevelIndex)
{
	return m_pObject_Manager->Get_ObjectMap(iLevelIndex);
}

HRESULT CGameInstance::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	return m_pObject_Manager->Clear_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Save_Layer(_uint iLevelIndex, const _tchar* pLayerTag, HANDLE hFile)
{
	return m_pObject_Manager->Save_Layer(iLevelIndex, pLayerTag, hFile);
}

void CGameInstance::Set_TimeSpeed(_uint iLevelIndex, const _tchar * pLayerTag, _double TimeSpeed)
{
	m_pObject_Manager->Set_TimeSpeed(iLevelIndex, pLayerTag, TimeSpeed);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Rmv_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pComponent_Manager->Rmv_Prototype(iLevelIndex, pPrototypeTag);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pComponent_Manager->Find_Component(iLevelIndex, pPrototypeTag);
}

void CGameInstance::Clear_All_Components()
{
	m_pComponent_Manager->Clear_All_Components();
}

_byte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return 0;
	}

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return 0;
	}

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return 0;
	}

	return m_pInput_Device->Get_DIMouseButtonState(eMouseButtonState);
}

_bool CGameInstance::Get_Mouse_Enter(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Get_Mouse_Enter(eMouseButtonState);
}

_bool CGameInstance::Get_Mouse_Up(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Get_Mouse_Up(eMouseButtonState);
}

_bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Mouse_Pressing(eMouseButtonState);
}

_bool CGameInstance::Get_KeyEnter(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Get_KeyEnter(eKeyID);
}

_bool CGameInstance::Get_KeyUp(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Get_KeyUp(eKeyID);
}

_bool CGameInstance::Get_KeyPressing(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
	{
		BREAKPOINT;
		return false;
	}

	return m_pInput_Device->Get_KeyPressing(eKeyID);
}

int CGameInstance::VolumeUp(CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->VolumeUp(eID, _vol);
}

int CGameInstance::VolumeDown(CSoundMgr::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->VolumeDown(eID, _vol);
}

int CGameInstance::BGMVolumeUp(_float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->BGMVolumeUp(_vol);
}

int CGameInstance::BGMVolumeDown(_float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->BGMVolumeDown(_vol);
}

int CGameInstance::Pause(CSoundMgr::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->Pause(eID);
}

_bool CGameInstance::IsPlaying(CSoundMgr::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pSound_Manager->IsPlaying(eID);
}

void CGameInstance::LetsPlaySound(TCHAR * pSoundKey, CSoundMgr::CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->PlaySound(pSoundKey, eID, _vol, b3DSound, vPosition);
}

void CGameInstance::PlayRoofSound(TCHAR * pSoundKey, CSoundMgr::CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->PlayRoofSound(pSoundKey, eID, _vol, b3DSound, vPosition);
}

void CGameInstance::PlayBGM(TCHAR* pSoundKey, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->PlayBGM(pSoundKey, _vol);
}

void CGameInstance::StopSound(CSoundMgr::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->StopAll();
}

void CGameInstance::PauseAll(_bool bPause)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->PauseAll(bPause);
}

void CGameInstance::LetsPlaySound_UI(TCHAR* pSoundKey, CSoundMgr::CHANNELUI eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->PlaySound_UI(pSoundKey, eID, _vol);
}

_uint CGameInstance::LetsPlaySound_Monster(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return -1;
	}

	return m_pSound_Manager->PlayMonsterSound(pSoundKey, _vol, b3DSound, vPosition);
}

_uint CGameInstance::PlayMonsterRoofSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return -1;
	}

	return m_pSound_Manager->PlayMonsterRoofSound(pSoundKey, _vol, b3DSound, vPosition);
}

void CGameInstance::StopMonsterSound(_uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->StopMonsterSound(iID);
}

void CGameInstance::StopSound_UI(CSoundMgr::CHANNELUI eID)
{
	if (nullptr == m_pSound_Manager)
	{
		BREAKPOINT;
		return;
	}

	return m_pSound_Manager->StopSound_UI(eID);
}

HRESULT CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pPipeLine->Set_Transform(eStateType, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return XMMatrixIdentity();
	}

	return m_pPipeLine->Get_TransformMatrix(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return _float4x4();
	}

	return m_pPipeLine->Get_TransformFloat4x4(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return _float4x4();
	}

	return m_pPipeLine->Get_TransformFloat4x4_TP(eStateType);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return XMVectorZero();
	}

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::PipeLineUpdate()
{
	if (nullptr == m_pPipeLine)
	{
		BREAKPOINT;
		return;
	}
	m_pPipeLine->Update();
}

LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pLight_Manager->Add_Light(pDevice, pDeviceContext, LightDesc);
}

CLight_Manager::LIGHTMATRIX_DESC CGameInstance::Get_LightMatrix(CLight_Manager::SHADOWTYPE eType)
{
	CLight_Manager::LIGHTMATRIX_DESC tLightMatrix = {};

	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return tLightMatrix;
	}

	tLightMatrix = m_pLight_Manager->Get_LightMatrix(eType);

	return tLightMatrix;
}

void CGameInstance::Set_StaticLightViewMatrix(_fvector vLightPos)
{
	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return;
	}

	m_pLight_Manager->Set_StaticLightViewMatrix(vLightPos);
}

LIGHTDESC* CGameInstance::Get_DirectionalLightDesc()
{
	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pLight_Manager->Get_LightDesc((_uint)m_pLight_Manager->Get_LightSize() - 1);
}

HRESULT CGameInstance::Release_Light()
{
	if (nullptr == m_pLight_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pLight_Manager->Release_Light();
}

_bool CGameInstance::isIn_WorldSpace(_fvector vPoint, _float fRange)
{
	RETURN_FALSE(if (nullptr == m_pFrustum))

	return m_pFrustum->isIn_WorldSpace(vPoint, fRange);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	RETURN_FAIL(if (nullptr == m_pFont_Manager))

	return m_pFont_Manager->Add_Font(pDevice, pDeviceContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	RETURN_FAIL(if (nullptr == m_pFont_Manager))

	return m_pFont_Manager->Render_Font(pDeviceContext, pFontTag, pText, vPosition, vColor);
}

pair<_bool, _vector> CGameInstance::Check_Ray_VirtualPlane_Collision(_float fPlaneY)
{
	return m_pMouse->Check_Ray_VirtualPlane_Collision(fPlaneY);
}

_vector CGameInstance::Get_MouseWorldDir()
{
	return m_pMouse->Get_MouseWorldDir();
}

_bool CGameInstance::Ray_Box_Collision(const BoundingBox & box, float & Dist)
{
	return m_pMouse->Ray_Box_Collision(box, Dist);
}

_bool CGameInstance::Ray_Sphere_Collision(const BoundingSphere& sphere, float& Dist)
{
	return m_pMouse->Ray_Sphere_Collision(sphere, Dist);
}

list<CCollider*>* CGameInstance::Get_ColliderList(string ColliderTag)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return nullptr;
	}

	return m_pCollision_Manager->Get_ColliderList(ColliderTag);
}

HRESULT CGameInstance::Add_Collider(string ColliderTag, CCollider * pCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pCollision_Manager->Add_Collider(ColliderTag, pCollider);
}

HRESULT CGameInstance::Pop_DeadObjectCollider(string pColliderTag, CCollider * pCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pCollision_Manager->Pop_DeadObjectCollider(pColliderTag, pCollider);
}

_bool CGameInstance::Collision_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_AABB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_OBB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Sphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float * pDist)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_RayToSphere(vStartPosition, vDir, pDestCollider, pDist);
}

_bool CGameInstance::Collision_RayToAABB(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float * pDist)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_RayToAABB(vStartPosition, vDir, pDestCollider, pDist);
}

_bool CGameInstance::Collision_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_AABBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_OBBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Enter_AABB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Enter_AABB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Enter_OBB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Enter_OBB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Enter_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Enter_Sphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Enter_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Enter_AABBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Enter_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Enter_OBBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Stay_AABB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Stay_AABB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Stay_OBB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Stay_OBB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Stay_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Stay_Sphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Stay_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Stay_AABBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Stay_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Stay_OBBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Exit_AABB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Exit_AABB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Exit_OBB(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Exit_OBB(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Exit_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Exit_Sphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Exit_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Exit_AABBToSphere(pSrcCollider, pDestCollider);
}

_bool CGameInstance::Collision_Exit_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider)
{
	if (nullptr == m_pCollision_Manager)
	{
		BREAKPOINT;
		return false;
	}

	return m_pCollision_Manager->Collision_Exit_OBBToSphere(pSrcCollider, pDestCollider);
}

ID3D11ShaderResourceView * CGameInstance::Get_RenderTargetSRV(const _tchar * pTargetTag)
{
	if (nullptr == m_pTargetManager)
		return nullptr;

	return m_pTargetManager->Get_SRV(pTargetTag);
}

void CGameInstance::Release_Engine()
{
	if (0 != CGameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGameInstance");

	if (0 != CMouse::GetInstance()->DestroyInstance())
		MSGBOX("Failed To Delete CMouse");

	if (0 != CTimer_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CTimer_Manager");

	if (0 != CObject_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CObject_Manager");

	if (0 != CCollision_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CCollision_Manager");

	if (0 != CComponent_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CComponent_Manager");

	if (0 != CPicking::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CPicking");

	if (0 != CSoundMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CSoundMgr");

	if (0 != CFrustum::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CFrustum");

	if (0 != CFont_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CFont_Manager");

	if (0 != CPipeLine::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CPipeLine");

	if (0 != CLevel_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLevel_Manager");

	if (0 != CInput_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CInput_Device");

	if (0 != CLight_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLight_Manager");

	if (0 != CTarget_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CTarget_Manager");

	if (0 != CGraphic_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGraphic_Device");
}

void CGameInstance::Free()
{
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pMouse);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTargetManager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
