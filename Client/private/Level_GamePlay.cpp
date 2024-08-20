#include "stdafx.h"
#include "..\Public\LEVEL_TWILIGHT.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"

// GameObject
#include "Camera_Dynamic.h"
#include "Donald.h"
#include "Goofy.h"

// UI
#include "UI_Parents.h"
#include "UI_Num_Manager.h"
#include "UI_Parents.h"

CLEVEL_TWILIGHT::CLEVEL_TWILIGHT(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLEVEL_TWILIGHT::NativeConstruct()
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

	if (FAILED(Ready_Layer_Effect()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CLEVEL_TWILIGHT::Tick(_double TimeDelta)
{
	if(0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == GM->Get_MoveOnToNextLevel())
	{
		if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, GM->Get_NextLevel()))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
		GM->Set_MoveOnToNextLevel(false);
	}


	//TEST
	if (pGameInstance->Get_KeyEnter(DIK_I))
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_ItemGet"), &iCom_Texture_Set)))
			return E_FAIL;
	}
	
	return RESULT_NOPROBLEM;
}

_int CLEVEL_TWILIGHT::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_1))
	{
		_uint eState = CDonald::STATE_HEAL;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Donald"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_2))
	{
		_uint eState = CDonald::STATE_ATTACK;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Donald"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_3))
	{
		_uint eState = CDonald::STATE_DANCE;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Donald"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_4))
	{
		_uint eState = CGoofy::STATE_PUSH;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Goofy"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_5))
	{
		_uint eState = CGoofy::STATE_RUN;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Goofy"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (true == m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && true == m_pGameInstance->Get_KeyEnter(DIK_6))
	{
		_uint eState = CGoofy::STATE_THROWSTART;
		if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_TWILIGHT, TEXT("Layer_Goofy"), &eState)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	return RESULT_NOPROBLEM;
}

HRESULT CLEVEL_TWILIGHT::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CImGui_Manager::GetInstance()->show_lightcontrol_window)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("Light Control");

		ImGui::Text("Control Light");
		LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

		XMFLOAT4 vDiffuse   = pLight->vDiffuse;
		XMFLOAT4 vAmbient   = pLight->vAmbient;
		XMFLOAT4 vSpecualr  = pLight->vSpecular;
		XMFLOAT4 vDirection = pLight->vDirection;

		_float diffuse[4]   = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
		_float Ambient[4]   = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
		_float Specualr[4]  = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
		_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

		if (ImGui::DragFloat4("Diffuse", diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
			XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
		if (ImGui::DragFloat4("Ambient", Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
			XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient[0], Ambient[1], Ambient[2], Ambient[3]));
		if (ImGui::DragFloat4("Specular", Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
			XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr[0], Specualr[1], Specualr[2], Specualr[3]));
		if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
			XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

		ImGui::End();
	}

	return S_OK;
}

HRESULT CLEVEL_TWILIGHT::Ready_Lights()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse   = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient   = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular  = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLEVEL_TWILIGHT::Ready_Layer_GameObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Camera
	CCamera::CAMERADESC	CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye   = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt    = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy   = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear   = 0.2f;
	CameraDesc.fFar    = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec    = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Perspective"), &CameraDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Axis
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Axis"), TEXT("Prototype_GameObject_Axis"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Player
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SkullSoldier
	_float4 vPosition = _float4(25.f, 0.f, 30.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_SkullSolidier"), TEXT("Prototype_GameObject_SkullSoldier"), &vPosition)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CTransform* testmonster_tf_gameplay = nullptr;

	// TestMonster_Ground
	vPosition = _float4(20.f, 0.f, 40.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_TestMonster"), TEXT("Prototype_GameObject_TestMonster"), &vPosition)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	testmonster_tf_gameplay = pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_TestMonster"))->Get_Transform();
	testmonster_tf_gameplay->Set_Speed(0.f);
	testmonster_tf_gameplay->Set_RotSpeed(0.f);

	// TestMonster_Air
	vPosition = _float4(45.f, 3.5f, 35.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_TestMonster"), TEXT("Prototype_GameObject_TestMonster"), &vPosition)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	testmonster_tf_gameplay = pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_TestMonster"))->Get_Transform();
	testmonster_tf_gameplay->Set_Speed(0.f);
	testmonster_tf_gameplay->Set_RotSpeed(0.f);

	// DistortionText
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_DistortionText"), TEXT("Prototype_GameObject_DistortionText"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Portal
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_Portal"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 도널드 오브젝트풀
	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_Donald"), TEXT("Prototype_GameObject_Donald"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 구피 오브젝트풀
	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_Goofy"), TEXT("Prototype_GameObject_Goofy"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLEVEL_TWILIGHT::Ready_Layer_UI()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* UI :: Player_Circle*/
	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_MAIN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FACE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_TEXTMP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_TEXTFOCUS;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_TEXTMAX;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCSE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	/* UI :: LockOn */
	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_LockOn"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_LockOn"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_LockOn"), &iCom_Texture_Set)))
		return E_FAIL;

	/*UI :: Monster_Hp*/ // 임시로 GamePlay에서 생성

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_TEXT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	//TESTSETSETSETSETSETSETSETSETSETSETSET
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UVTEST"))))
		return E_FAIL;

	////TESTSETSETSETSETSETSETSETSETSETSETSET
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UVTEST"))))
	//	return E_FAIL;

	/* UI :: tt */

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_FRAME1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"),&iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_FRAME2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_FRAME3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_TEX;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_TEXT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	//iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SHORTCUT;
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
	//	return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SELECT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_TT_SLOT2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_tt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	 

	return S_OK;
}

HRESULT CLEVEL_TWILIGHT::Ready_Layer_Effect()
{
	return S_OK;
}

CLEVEL_TWILIGHT * CLEVEL_TWILIGHT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLEVEL_TWILIGHT*	pInstance = new CLEVEL_TWILIGHT(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLEVEL_TWILIGHT::Free()
{
	__super::Free();
}
