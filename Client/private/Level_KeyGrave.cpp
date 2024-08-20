#include "stdafx.h"
#include "Level_KeyGrave.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"
#include "Progress_Manager.h"

// GameObject
#include "Camera_Dynamic.h"
#include "Camera_Perspective.h"
#include "Player.h"
#include "UI_Parents.h"

CLevel_KeyGrave::CLevel_KeyGrave(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_KeyGrave::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_GameObjects()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pRenderer = static_cast<CRenderer*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Renderer")));

	m_pRenderer->Set_Fog(false);
	m_pRenderer->Set_HDR(false);
	m_pRenderer->Set_LUTDesc(true, 94);
	m_pRenderer->Set_LUTTexIndexSecond(94);
	m_pRenderer->Set_DOF(false);

	//m_pGameInstance->PlayBGM(TEXT("KeyGraveBGM.mp3"), 0.5f);

	RECT	rc;
	POINT	pt1, pt2;
	GetClientRect(g_hWnd, &rc);

	pt1.x = rc.left;
	pt1.y = rc.top;
	pt2.x = rc.right;
	pt2.y = rc.bottom;

	ClientToScreen(g_hWnd, &pt1);
	ClientToScreen(g_hWnd, &pt2);

	rc.left = pt1.x;
	rc.top = pt1.y;
	rc.right = pt2.x;
	rc.bottom = pt2.y;

	ClipCursor(&rc);

	return S_OK;
}

_int CLevel_KeyGrave::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == GM->Get_MoveOnToNextLevel())
	{
		GM->Set_CurrentLevel(LEVEL_LOADING);
		m_pGameInstance->StopAll();
		if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, GM->Get_NextLevel()))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
		GM->Set_MoveOnToNextLevel(false);
		return RESULT_NOPROBLEM;
	}

	return RESULT_NOPROBLEM;
}

_int CLevel_KeyGrave::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	return RESULT_NOPROBLEM;
}

HRESULT CLevel_KeyGrave::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CImGui_Manager::GetInstance()->show_levelcontrol_window)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("Level Control");

		if (ImGui::BeginTabBar("Level Control", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Separated"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse = pLight->vDiffuse;
				XMFLOAT4 vAmbient = pLight->vAmbient;
				XMFLOAT4 vSpecualr = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse[4] = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
				_float Ambient[4] = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
				_float Specualr[4] = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat4("Diffuse", diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
				if (ImGui::DragFloat4("Ambient", Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient[0], Ambient[1], Ambient[2], Ambient[3]));
				if (ImGui::DragFloat4("Specular", Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr[0], Specualr[1], Specualr[2], Specualr[3]));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("United"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse = pLight->vDiffuse;
				XMFLOAT4 vAmbient = pLight->vAmbient;
				XMFLOAT4 vSpecualr = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse = vDiffuse.x;
				_float Ambient = vAmbient.x;
				_float Specualr = vSpecualr.x;
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat("Diffuse", &diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse, diffuse, diffuse, diffuse));
				if (ImGui::DragFloat("Ambient", &Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient, Ambient, Ambient, Ambient));
				if (ImGui::DragFloat("Specular", &Specualr, 0.01f, 0.0f, 1.f, "%.2f"))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr, Specualr, Specualr, Specualr));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("LUT"))
			{
				_bool bUsingLUT = m_pRenderer->Get_UsingLUT();

				if (ImGui::Checkbox("Using LUT", &bUsingLUT))
					m_pRenderer->Set_UsingLUT(bUsingLUT);

				if (true == bUsingLUT)
				{
					CImGui_Manager::GetInstance()->Set_LUTTexIndex(m_pRenderer);
					if (ImGui::DragInt("LUTIndexSecond", &m_iLUTIndexSecond, 1, 0, 96))
						m_pRenderer->Set_LUTTexIndexSecond(m_iLUTIndexSecond);
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shader"))
			{
				_float	 fSSAO_Radius = m_pRenderer->Get_SSAORadius();
				_float	 fSSAO_Bias = m_pRenderer->Get_SSAOBias();

				if (ImGui::DragFloat("SSAO_Radius", &fSSAO_Radius, 0.1f, -10.f, 10.f))
					m_pRenderer->Set_SSAORadius(fSSAO_Radius);
				if (ImGui::DragFloat("SSAO_Bias", &fSSAO_Bias, 0.001f, 0.f, 1.f))
					m_pRenderer->Set_SSAOBias(fSSAO_Bias);

				_float	 fHDR_GrayScale = m_pRenderer->Get_HDR_GrayScale();
				_float	 fHDR_White = m_pRenderer->Get_HDR_White();

				if (ImGui::DragFloat("HDR_GrayScale", &fHDR_GrayScale, 0.01f, 0.f, 10.f))
					m_pRenderer->Set_HDR_GrayScale(fHDR_GrayScale);
				if (ImGui::DragFloat("HDR_White", &fHDR_White, 0.01f, 0.f, 10.f))
					m_pRenderer->Set_HDR_White(fHDR_White);

				_bool	bHDR = m_pRenderer->Get_HDR();
				_bool	bBloom = m_pRenderer->Get_Bloom();

				if (ImGui::Checkbox("OnHDR", &bHDR))
					m_pRenderer->Set_HDR(bHDR);

				if (ImGui::Checkbox("OnBloom", &bBloom))
					m_pRenderer->Set_Bloom(bBloom);

				_float	fFogStart = m_pRenderer->Get_FogStart();
				_float	fFogEnd = m_pRenderer->Get_FogEnd();
				_float	fFogColor[4];
				memcpy(fFogColor, &m_pRenderer->Get_FogColor(), sizeof(_float4));
				_float	fMaxFogFactor = m_pRenderer->Get_MaxFogFactor();
				_bool	bFog = m_pRenderer->Get_Fog();


				if (ImGui::DragFloat("FogStart", &fFogStart, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_FogStart(fFogStart);
				if (ImGui::DragFloat("FogEnd", &fFogEnd, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_FogEnd(fFogEnd);
				if (ImGui::DragFloat4("FogColor", fFogColor, 0.01f, 0.f, 1.f, "%.2f"))
				{
					_float4	vFogColor;
					memcpy(&vFogColor, fFogColor, sizeof(_float4));
					m_pRenderer->Set_FogColor(vFogColor);
				}
				if (ImGui::DragFloat("MaxFogFactor", &fMaxFogFactor, 0.01f, 0.f, 1.f, "%.2f"))
					m_pRenderer->Set_MaxFogFactor(fMaxFogFactor);
				if (ImGui::Checkbox("OnFog", &bFog))
					m_pRenderer->Set_Fog(bFog);


				_bool	bDOF = m_pRenderer->Get_DOF();
				_float	fDOF_MinDistance = m_pRenderer->Get_DOF_MinDistance();
				_float	fDOF_MaxDistance = m_pRenderer->Get_DOF_MaxDistance();

				if (ImGui::DragFloat("DOF_MinDistance", &fDOF_MinDistance, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_DOF_MinDistance(fDOF_MinDistance);
				if (ImGui::DragFloat("DOF_MaxDistance", &fDOF_MaxDistance, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_DOF_MaxDistance(fDOF_MaxDistance);
				if (ImGui::Checkbox("OnDOF", &bDOF))
					m_pRenderer->Set_DOF(bDOF);

				_bool	bOutline = m_pRenderer->Get_Outline();
				if (ImGui::Checkbox("OnOutline", &bOutline))
					m_pRenderer->Set_Outline(bOutline);

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	return S_OK;
}

HRESULT CLevel_KeyGrave::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_KeyGrave::Ready_Layer_GameObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Camera
	CCamera::CAMERADESC	CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Perspective"), &CameraDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Camera(static_cast<CCamera_Perspective*>(pGameInstance->Get_Back(LEVEL_KEYGRAVE, TEXT("Layer_Camera"))));

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Player
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Player(static_cast<CPlayer*>(m_pGameInstance->Get_Back(LEVEL_KEYGRAVE, TEXT("Layer_Player"))));
	GM->Get_Player()->Get_Transform()->Set_Position(__vector(0.f, 0.f, 39.f, 1.f));
	GM->Get_Player()->Get_Transform()->MakeSameDirection(__vector(0.f, 0.f, -1.f, 0.f));

	// KGWall
	for (int i = 0; i < 44; i++)
	{
		iWall_Cnt = i;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_KGWall"),&iWall_Cnt)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}



	// FrozenWolf
	if (FAILED(pGameInstance->ThrowToLayer(LEVEL_KEYGRAVE, TEXT("Layer_FrozenWolf"), nullptr)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	PM->Set_ReadyWhiteFadeOut(true);

	return S_OK;
}

HRESULT CLevel_KeyGrave::Ready_Layer_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// UI :: Player_Circle* ==================================================================================================================================
	_int iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_MAIN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FACE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	/* UI :: LockOn */
	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	// UI :: bt ===============================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;


	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME6;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME7;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SELECT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	// Command Extra ==========================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ATTACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_SKILL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ITEM;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	// Command_QucikSlot ==================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_3;;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_KEYGRAVE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	return S_OK;
}

CLevel_KeyGrave* CLevel_KeyGrave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_KeyGrave* pInstance = new CLevel_KeyGrave(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_KeyGrave::Free()
{
	__super::Free();
}