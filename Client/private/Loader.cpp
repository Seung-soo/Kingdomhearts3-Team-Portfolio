#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

// Buffer
#include "VIBuffer_Trail.h"
#include "VIBuffer_Terrain.h"

// GameObject
#include "Trigger.h"
#include "Sky.h"
#include "Axis.h"
#include "Camera_Dynamic.h"
#include "Camera_Perspective.h"
#include "PickingPoint.h"
#include "CameraPoint.h"
#include "Map.h"
#include "Terrain.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "ColliderTool_Model.h"
#include "Portal.h"
#include "Donald.h"
#include "Donald_Weapon.h"
#include "Goofy.h"
#include "Goofy_Weapon.h"
#include "FrogBand.h"
#include "Flower.h"
#include "Donald_Icicle.h"
#include "Donald_IceFlower.h"
#include "Donald_Ice.h"
#include "TT_Guide.h"
#include "TT_NPC0.h"
#include "TreasureBox.h"
#include "Water.h"
#include "Icicle.h"
#include "FrozenWolf.h"
#include "LittleWolf.h"
#include "SummonMovingParticle.h"
#include "PlayerHit.h"

// Monster
#include "MonsterSkill.h"
#include "PillarOfFire.h"
#include "TestMonster.h"
#include "Crow.h"
#include "SkullSoldier.h"
#include "CrowSoldier.h"
#include "ThinSoldier.h"
#include "BowMan.h"
#include "Cannon.h"
#include "ShiningEgg.h"
#include "Monkey.h"
#include "BigTree.h"
#include "Mower.h"
#include "Nightmare.h"
#include "Nobody.h"
#include "IceMan.h"
#include "FrozenWolf.h"
#include "Goat.h"
#include "GoatWeapon.h"
#include "Mini_Crow.h"
#include "Mini_CrowYellow.h"

// Effect
#include "Effect_Texture.h"
#include "Effect_Mesh.h"
#include "Effect_Particle.h"
#include "Effect_VISwordTrail.h"
#include "Nightmare_Appear.h"
#include "Nightmare_Blade.h"
#include "DropFruitZone.h"
#include "BigTree_Howling.h"
#include "MotionTrail.h"
#include "CFireFly.h"

//UI
#include "UI_bt_Command.h"
#include "UI_Player_Circle.h"
#include "UI_MonsterHP.h"
#include "UI_Target.h"
#include "UI_ItemGet.h"
#include "UI_Menu.h"
#include "UI_Tip.h"
#include "Menu_Burble.h"
#include "UI_FriendIcon.h"
#include "UI_TextCommand.h"
#include "UI_TextQuickSlot.h"
#include "UI_StageLogo.h"
#include "UI_Tip_Down.h"
#include "UI_Chat.h"
#include "UI_GetCenter.h"
#include "UI_TBoxOpen.h"
#include "Menu_Window.h"
#include "Menu_ItemText.h"
#include "UI_MiniMap.h"
#include "UI_MiniMap_Monster.h"
#include "UI_BossTarget.h"
#include "UI_QusetNavi.h"
#include "UI_Minigame.h"
#include "UI_BossHP.h"
#include "UI_BossHPBar.h"
#include "UI_BossHPCnt.h"
#include "UI_BossHPCntBack.h"
#include "UI_RageGauge.h"
#include "UI_BossName.h"
#include "UI_Command_Weapon.h"
#include "UI_FormChange.h"
#include "MiniGame_Fade.h"
#include "Good.h"

#include "Map_Object_KGWall.h"

//Item
#include "Item_Object.h"
#include "Money_Object.h"
#include "UI_ItemIcon.h"


//Minigame
#include "Mini_Player.h"
#include "MINI_Bullet.h"
#include "Mini_Boss.h"
#include "Mini_Damage.h"
#include "Mini_BackGround.h"
#include "Mini_Reward.h"
#include "Mini_BossBullet.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

unsigned int APIENTRY ThreadFunction(void* pArg)
{	
	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_LevelID())
	{
		// =============================================================================================================================
	case LEVEL_TWILIGHT:
		pLoader->Loading_ForTwilightTownComponents();	// 컴포넌트'
		pLoader->Loading_ForTwilightTownModels();		// 모델
		pLoader->Loading_ForTwilightTownObjects();		// 게임오브젝트
		pLoader->Loading_ForTwilightTownUIs();			// UI
		pLoader->Loading_ForTwilightTownPool();			// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_TWILIGHT, TEXT("../SaveFiles/Map/SaveMap_TT.dat"));		// 맵 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_RAPUNZEL:
		pLoader->Loading_ForRapunzelComponents();		// 컴포넌트
		pLoader->Loading_ForRapunzelModels();			// 모델
		pLoader->Loading_ForRapunzelObjects();			// 게임오브젝트
		pLoader->Loading_ForRapunzelUIs();				// UI
		pLoader->Loading_ForRapunzelPool();				// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_RAPUNZEL, TEXT("../SaveFiles/Map/SaveMap_RA.dat"));		// 맵 로드
		pLoader->Loading_Triggers(LEVEL_RAPUNZEL, TEXT("../SaveFiles/Objects/RA/RA_Triggers.dat"));		// 트리거 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_RAPUNZEL_BOSS:
		pLoader->Loading_ForRapunzelBossComponents();	// 컴포넌트
		pLoader->Loading_ForRapunzelBossModels();	 	// 모델
		pLoader->Loading_ForRapunzelBossObjects();	 	// 게임오브젝트
		pLoader->Loading_ForRapunzelBossUIs();			// UI
		pLoader->Loading_ForRapunzelBossPool();		 	// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_RAPUNZEL_BOSS, TEXT("../SaveFiles/Map/SaveMap_RA_Boss.dat"));	// 맵 로드
		pLoader->Loading_Triggers(LEVEL_RAPUNZEL_BOSS, TEXT("../SaveFiles/Objects/RA_Boss/RA_Boss_Triggers.dat"));// 트리거 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_FROZEN:
		pLoader->Loading_ForFrozenComponents();			// 컴포넌트
		pLoader->Loading_ForFrozenModels();	 			// 모델
		pLoader->Loading_ForFrozenObjects();	 		// 게임오브젝트
		pLoader->Loading_ForFrozenUIs();				// UI
		pLoader->Loading_ForFrozenPool();		 		// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_FROZEN, TEXT("../SaveFiles/Map/SaveMap_FZ.dat"));	// 맵 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_KEYGRAVE:
		pLoader->Loading_ForKeyGraveComponents();		// 컴포넌트
		pLoader->Loading_ForKeyGraveModels();	 		// 모델
		pLoader->Loading_ForKeyGraveObjects();	 		// 게임오브젝트
		pLoader->Loading_ForKeyGraveUIs();				// UI
		pLoader->Loading_ForKeyGravePool();		 		// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_KEYGRAVE, TEXT("../SaveFiles/Map/SaveMap_KG.dat"));	// 맵 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_FINALBOSS:
		pLoader->Loading_ForFinalBossComponents();		// 컴포넌트
		pLoader->Loading_ForFinalBossModels();	 		// 모델
		pLoader->Loading_ForFinalBossObjects();	 		// 게임오브젝트
		pLoader->Loading_ForFinalBossUIs();				// UI
		pLoader->Loading_ForFinalBossPool();		 	// 오브젝트풀
		pLoader->ReadyMapFromSaveFiles(LEVEL_FINALBOSS, TEXT("../SaveFiles/Map/SaveMap_BT.dat"));	// 맵 로드
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_MAPTOOL:
		pLoader->Loading_ForTwilightTownComponents();
		pLoader->Loading_ForTwilightTownModels();
		pLoader->Loading_ForTwilightTownObjects();
		pLoader->Loading_ForTwilightTownPool();

		pLoader->Loading_ForRapunzelComponents();
		pLoader->Loading_ForRapunzelModels();
		pLoader->Loading_ForRapunzelObjects();
		pLoader->Loading_ForRapunzelPool();

		pLoader->Loading_ForRapunzelBossComponents();
		pLoader->Loading_ForRapunzelBossModels();
		pLoader->Loading_ForRapunzelBossObjects();
		pLoader->Loading_ForRapunzelBossPool();

		pLoader->Loading_ForFrozenComponents();
		pLoader->Loading_ForFrozenModels();
		pLoader->Loading_ForFrozenObjects();
		pLoader->Loading_ForFrozenPool();

		pLoader->Loading_ForKeyGraveComponents();
		pLoader->Loading_ForKeyGraveModels();
		pLoader->Loading_ForKeyGraveObjects();
		pLoader->Loading_ForKeyGravePool();

		pLoader->Loading_ForFinalBossComponents();
		pLoader->Loading_ForFinalBossModels();
		pLoader->Loading_ForFinalBossObjects();
		pLoader->Loading_ForFinalBossPool();

		pLoader->Loading_ForMapToolComponents();
		pLoader->Loading_ForMapToolModels();
		pLoader->Loading_ForMapToolObjects();
		pLoader->SetFinish();

		break;
		// =============================================================================================================================
	case LEVEL_EFFECTTOOL:
		pLoader->Loading_ForTwilightTownComponents();
		pLoader->Loading_ForTwilightTownModels();
		pLoader->Loading_ForTwilightTownObjects();

		pLoader->Loading_ForRapunzelComponents();
		pLoader->Loading_ForRapunzelModels();
		pLoader->Loading_ForRapunzelObjects();

		pLoader->Loading_ForRapunzelBossComponents();
		pLoader->Loading_ForRapunzelBossModels();
		pLoader->Loading_ForRapunzelBossObjects();

		pLoader->Loading_ForFrozenComponents();
		pLoader->Loading_ForFrozenModels();
		pLoader->Loading_ForFrozenObjects();

		pLoader->Loading_ForKeyGraveComponents();
		pLoader->Loading_ForKeyGraveModels();	 
		pLoader->Loading_ForKeyGraveObjects();	 

		pLoader->Loading_ForFinalBossComponents();
		pLoader->Loading_ForFinalBossModels();
		pLoader->Loading_ForFinalBossObjects();

		pLoader->Loading_ForEffectToolComponents();
		pLoader->Loading_ForEffectToolModels();
		pLoader->Loading_ForEffectToolObjects();
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_COLLIDERTOOL:
		pLoader->Loading_ForColliderToolComponents();
		pLoader->Loading_ForColliderToolModels();
		pLoader->Loading_ForColliderToolObjects();
		pLoader->SetFinish();
		break;
		// =============================================================================================================================
	case LEVEL_TEST:
		pLoader->Loading_ForTwilightTownComponents();
		pLoader->Loading_ForTwilightTownModels();
		pLoader->Loading_ForTwilightTownObjects();

		pLoader->Loading_ForRapunzelComponents();
		pLoader->Loading_ForRapunzelModels();
		pLoader->Loading_ForRapunzelObjects();

		pLoader->Loading_ForRapunzelBossComponents();
		pLoader->Loading_ForRapunzelBossModels();
		pLoader->Loading_ForRapunzelBossObjects();

		pLoader->Loading_ForFrozenComponents();
		pLoader->Loading_ForFrozenModels();
		pLoader->Loading_ForFrozenObjects();

		pLoader->Loading_ForKeyGraveComponents();
		pLoader->Loading_ForKeyGraveModels();
		pLoader->Loading_ForKeyGraveObjects();

		pLoader->Loading_ForFinalBossComponents();
		pLoader->Loading_ForFinalBossModels();
		pLoader->Loading_ForFinalBossObjects();

		pLoader->Loading_ForTestComponents();
		pLoader->Loading_ForTestModels();
		pLoader->Loading_ForTestObjects();
		pLoader->SetFinish();
		break;
	case LEVEL_ENDING:
		pLoader->Loading_ForEnding();
		pLoader->SetFinish();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());	

	return 0;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	m_eLevel = eNextLevel;

	return S_OK;
}

HRESULT CLoader::Loading_ForTwilightTownComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Shader_AnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_AnimHeavyModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	// 스카이박스
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky%d.dds"), 11))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 포탈
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PortalImage"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Objects/Portal_Image%d.dds"), 4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 히트텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerHit"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Objects/Hit.png")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// TT 네비게이션
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Navigation_Twilight"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/TT_END.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TT 보스 네비게이션
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Navigation_Twilight_Boss"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/TT_BossNavi.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForTwilightTownModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// KingdomKey
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kingdom_Key"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Player_Weapon/KindomKey/KingdomKey.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// FavorityDeputy
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Favorite_Deputy"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Player_Weapon/FavoriteDeputy/FavoriteDeputy/FavoriteDeputy.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// HyperHammer
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hyper_Hammer"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Player_Weapon/FavoriteDeputy/FavoriteDeputy_HyperHammer/FavoriteDeputy_HyperHammer.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// HyperHammer_Changing
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hyper_Hammer_Changing"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Player_Weapon/FavoriteDeputy/FavoriteDeputy_Changing/FavoriteDeputy_Changing.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 테스트몬스터
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TestMonster"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TestMonster/TestMonster.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SkullSldier
	TransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_SkullSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/SkullSoldier/SkullSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// CrowSoldier
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_CrowSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/CrowSoldier/CrowSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ThinSoldier
	TransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_ThinSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/ThinSoldier/ThinSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nightmare
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_Nightmare"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Nightmare/Nightmare.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Bowman
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_BowMan"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/BowMan/BowMan.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Cannon
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_Cannon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Cannon/Cannon.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Donald
	TransformMatrix = mat_rotY180;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Donald"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Donald/Donald.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldWand
	TransformMatrix = mat_rotX180;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldWeapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/DonaldWand/DonaldWand.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldIcicle
	TransformMatrix = XMMatrixScaling(0.6f, 0.6f, 0.6f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldIcicle"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Donald_Icicle/Donald_Icicle.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldFlower
	TransformMatrix = XMMatrixScaling(0.6f, 0.6f, 0.6f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldFlower"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Donald_IceFlower/Donald_IceFlower.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldFlower
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldIce"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Ice/Ice.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Goofy
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Goofy"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Goofy/Goofy.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// GoofyWeapon
	TransformMatrix = mat_rotX180;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GoofyWeapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/GoofyWeapon/GoofyWeapon.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Apple
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Apple"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Fruits/Apple.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Pear
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pear"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Fruits/Pear.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Banana
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Banana"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Fruits/Banana.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Item_Object
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Item_Object"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Item/Model_Item_Object.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	// Money_Object
	TransformMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Money_Object"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Item/Model_Money_Object.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TT_NPC0
	TransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_NPC0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Npc0/TT_Npc0.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TreasureBox_TT
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TreasureBox_TT"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TreasureBox_TT/TreasureBox_TT.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TT_Map
	if (FAILED(Loading_TT_Map()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForTwilightTownObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// CameraPoint
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraPoint"),
		CCameraPoint::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Trigger
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger"),
		CTrigger::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Sky
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Camera_Dynamic
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Camera_Perspective
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Perspective"),
		CCamera_Perspective::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Axis
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axis"),
		CAxis::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Player
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// PlayerHit
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHit"),
		CPlayerHit::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Map
	if (FAILED((pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pDeviceContext)))))
	{
		BREAKPOINT;
			return E_FAIL;
	}

	// TestMonster
	if (FAILED((pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestMonster"),
		CTestMonster::Create(m_pDevice, m_pDeviceContext)))))
	{
		BREAKPOINT;
			return E_FAIL;
	}

	// SkullSoldier
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkullSoldier"),
		CSkullSoldier::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// CrowSoldier
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrowSoldier"),
		CCrowSoldier::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ThinSoldier
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThinSoldier"),
		CThinSoldier::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// BowMan
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BowMan"),
		CBowMan::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Cannon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cannon"),
		CCannon::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nightmare
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Nightmare"),
		CNightmare::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// MonsterSkill
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterSkill"),
		CMonsterSkill::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Donald
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Donald"),
		CDonald::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DonaldWeapon"),
		CDonald_Weapon::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldIcicle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DonaldIcicle"),
		CDonald_Icicle::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldFlower
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DonaldFlower"),
		CDonald_IceFlower::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DonaldIce
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DonaldIce"),
		CDonald_Ice::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Goofy
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goofy"),
		CGoofy::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// GoofyWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GoofyWeapon"),
		CGoofy_Weapon::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Effect ====================================================================================================
	// NightmareAppear
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NightmareAppear"),
		CNightmare_Appear::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// NightmareBlade
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NightmareBlade"),
		CNightmare_Blade::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DropFruitZone
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DropFruitZone"),
		CDropFruitZone::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// BigTreeHowling
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BigTreeHowling"),
		CBigTree_Howling::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// EffectTexture
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectTexture"),
		CEffect_Texture::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// EffectParticle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"),
		CEffect_Particle::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// EffectMesh
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectMesh"),
		CEffect_Mesh::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// EffectTrail_Buffer
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectTrail_Buffer"),
		CEffect_VISwordTrail::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SummonMovingParticle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SummonMovingParticle"),
		CSummonMovingParticle::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Portal
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Portal"),
		CPortal::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TT_NPC0
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TT_NPC0"),
		CTT_NPC0::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TreasureBox
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureBox"),
		CTreasureBox::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// MotionTrail
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MotionTrail"),
		CMotionTrail::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Firefly
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireFly"),
		CFireFly::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForTwilightTownUIs()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// UI_PlayerCircle_Texture =================================================================================================================================================

	/* For.Prototype_Component_Texture_UI_PlayerMainCircle */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Player_MainCircle.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerMainCircle_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Player_MainCircle_Back.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerMainCircle_Back_Black */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle_Back2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Player_MainCircle_Back_Black.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerHpBar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/HPBar.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerMpBar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/MPBar.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerRageGauge */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerRageGauge"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/RageGauge%d.png"), 2)));

	/* For.Prototype_Component_Texture_UI_PlayerRageGauge_Fog */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerRageGauge_Fog"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/fog_%d.png"), 19)));

	/* For.Prototype_Component_Texture_UI_PlayerHpBar_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHpBar_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/HPBar_Back.png"))));

	/* For.Prototype_Component_Texture_UI_PlayerMpBar_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMpBar_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/MPBar_Back.png"))));

	/* For.Prototype_Component_Texture_UI_Circle_MainSora */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Circle_MainSora"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Circle_MainSora_%d.png"), 4)));

	// UI_Monster_Texture =============================================================================================================================================

	/* For.Prototype_Component_Texture_UI_Monster_HP_TEXT */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_TEXT"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/Monster_HP_TEXT.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Large */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Large.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Large_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Large_Back.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Large_Bar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large_Bar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHPBar_Large.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Middle */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Middle.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Middle_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Middle_Back.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Middle_Bar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle_Bar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHPBar_Middle.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Small */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Small.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Small_Back */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHP_Small_Back.png"))));

	/* For.Prototype_Component_Texture_UI_Monster_HP_Small_Bar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small_Bar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/MonsterHPBar_Small.png"))));

	/* For.Prototype_Component_Texture_UI_Boss_HP */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HP"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/BossHP.png"))));

	/* For.Prototype_Component_Texture_UI_Boss_HPBack */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HPBack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/BossHPBack.png"))));

	/* For.Prototype_Component_Texture_UI_Boss_HP_Bar */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HP_Bar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/BossHPBar%d.png"), 7)));

	/* For.Prototype_Component_Texture_UI_Boss_HP_Cnt */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HP_Cnt"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/BossHPBarCnt%d.png"), 2)));


	// UI_LockOn_Texture =============================================================================================================================================

	/* For.Prototype_Component_Texture_UI_LockOn1 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/LockOn1.png"))));

	/* For.Prototype_Component_Texture_UI_LockOn2_1 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn2_1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/LockOn2_1.png"))));

	/* For.Prototype_Component_Texture_UI_LockOn2_2 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn2_2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/LockOn2_2.png"))));

	/* For.Prototype_Component_Texture_UI_BossLockOn */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossLockOn"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/BossLockon.png"))));


	// UI_Command_Texture =================================================================================================================================================

	/* Prototype_Component_Texture_Donald_Icon.*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Donald_Icon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/DonaldIcon.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Goofy_Icon.*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Goofy_Icon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/GoofyIcon.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CommandText */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CommandText"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Text_Command%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ItemText */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemText"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/Text_Item%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ItemIcon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Item/item%d.png"), 7))))
		return E_FAIL;

	// LEVEL_BT

	/* For.Prototype_Component_Texture_UI_bt_Frame */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_bt/BT_Frame%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_bt_Frame7 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Frame7"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_bt/BT_Frame7_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_bt_Slot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Slot"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_bt/BT_Slot%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_bt_SlotN */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_SlotN"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_bt/BT_SlotN%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_bt_Point_Select */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Point_Select"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_bt/BT_Point_Select.png")))))
		return E_FAIL;


	/* Level_Logo ======================================== */
	
	/* For.Prototype_Component_Texture_UI_tt_Logo */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_tt_Logo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/LOGO_TT_%d.png"), 12))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_ra_Logo */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ra_Logo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/LOGO_RA_%d.png"), 12))))
		return E_FAIL;

	// UI_ITEM =================================================================================================================================================

	/* For.Prototype_Component_Texture_Item_Get*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Get"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_UI/ITEM_Get.png")))))
		return E_FAIL;

	// UI_Menu =================================================================================================================================================

	/* For.Prototype_Component_Texture_Menu_Background*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Menu_Background.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Frame*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Menu_Frame.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Frame_Money*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Frame_Money"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Menu_Frame_Money.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Icon_Item*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Icon_Item"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Menu_Icon_Item.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Icon_Shop*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Icon_Shop"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Menu_Icon_Shop.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Player*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Player"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Player_Picture.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Player_Back*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Player_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Player_Picture_Back.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Menu_Simbol*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Menu_Simbol"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Logo_Back%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Inven_Frame*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Inven_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/menu_ItemWindow%d.png"),2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Shop_Frame*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Shop_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/menu_ShopWindow_%d.png"), 9))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_ItemText */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_ItemText"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/InvenItem_Text_%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Select*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Select"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/menu_Select.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Tip*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Tip_UI.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Tip_Text*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Text"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Tip_UI_%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Tip_Down*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Down"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Tip_Down.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Tip_Down_Text*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Down_Text"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Tip_Down_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Chat*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Chat"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/Tip_Down_Chat%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GetCenter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_GetCenter"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Menu/UI_GetCenter%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Open_Button*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Open_Button%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Open_Button_Light*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button_Light"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Open_Button_Light.png")))))
		return E_FAIL;
	
	/* MINI_MAP */
	/* For.Prototype_Component_Texture_UI_MiniMap_Player_Icon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_Player_Icon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_Icon.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_MiniMap_Monster_Icon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_Monster_Icon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_Icon.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_MiniMap_tt */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_tt"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/minimap_tt.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_MiniMap_ra */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_ra"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/minimap_ra.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_QuestNave */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestNave"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/questnavi.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_MiniGame*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniGame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/MiniGame_Frame%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Minigame_Fade */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Minigame_Fade"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Minigame_Fade%d.png"), 21))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_BossName*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossName"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_UI/BOSS_NAME%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Good*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Good"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Good.png")))))
		return E_FAIL;

	


	// Objects ======================================================================================================================

	/* UI :: Player_Circle */
	/*For.Prototype_GameObject_UI_Player_Circle */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Circle"),
		CUI_Player_Circle::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: Player_RageGauge */
	/*For.Prototype_GameObject_UI_Player_RageGauge */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_RageGauge"),
		CUI_RageGauge::Create(m_pDevice, m_pDeviceContext)));


	/* UI :: Monster_Hp */
	/*For.Prototype_GameObject_UI_Monster_Hp */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster_Hp"),
		CUI_MonsterHP::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: Boss_Hp */
	/*For.Prototype_GameObject_UI_BossHp */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHp"),
		CUI_BossHP::Create(m_pDevice, m_pDeviceContext)));

	/*For.Prototype_GameObject_UI_BossHpBar */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHpBar"),
		CUI_BossHPBar::Create(m_pDevice, m_pDeviceContext)));

	/*For.Prototype_GameObject_UI_BossHpCnt */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHpCnt"),
		CUI_BossHPCnt::Create(m_pDevice, m_pDeviceContext)));

	/*For.Prototype_GameObject_UI_BossHpCntBack */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHpCntBack"),
		CUI_BossHPCntBack::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: Target */
	/*For.Prototype_GameObject_UI_Target */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Target"),
		CUI_Target::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: BossTarget */
	/*For.Prototype_GameObject_UI_Target */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossTarget"),
		CUI_BossTarget::Create(m_pDevice, m_pDeviceContext)));


	/* UI :: ItemGet */
	/*For.Prototype_GameObject_UI_ItemGet */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemGet"),
		CUI_ItemGet::Create(m_pDevice, m_pDeviceContext)));
	/*For.Prototype_GameObject_UI_ItemIcon */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemIcon"),
		CUI_ItemIcon::Create(m_pDevice, m_pDeviceContext)));

	/* UI ::  */
	/* For.Prototype_GameObject_UI_Menu */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Menu"),
		CUI_Menu::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_Burble*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Burble"),
		CMenu_Burble::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_TT_Guide*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TT_Guide"),
		CTT_Guide::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Menu_Window */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Menu_Window"),
		CMenu_Window::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Menu_ItemText */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Menu_ItemText"),
		CMenu_ItemText::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_Tip*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Tip"),
		CUI_Tip::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_Tip_Down*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Tip_Down"),
		CUI_Tip_Down::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_Chat*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chat"),
		CUI_Chat::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_GetCenter*/
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_GetCenter"),
		CUI_GetCenter::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_TBoxOpen */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TBoxOpen"),
		CUI_TBoxOpen::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: All_Level */
	/* For.Prototype_GameObject_TextCommand */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextCommand"),
		CUI_TextCommand::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Command_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Command_Weapon"),
		CUI_Command_Weapon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TextQuickSlot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextQuickSlot"),
		CUI_TextQuickSlot::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FriendIcon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FriendIcon"),
		CUI_FriendIcon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* UI :: bt */
	/* For.Prototype_GameObject_UI_tt_Command */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_bt_Command"),
		CUI_bt_Command::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_UI_StageLogo */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StageLogo"),
		CUI_StageLogo::Create(m_pDevice, m_pDeviceContext)));

	/* Obejcet : Item*/
	/* For.Prototype_GameObject_Item_Object */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Object"),
		CItem_Object::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Money_Object */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Money_Object"),
		CMoney_Object::Create(m_pDevice, m_pDeviceContext)));

	/* UI :: MiniMap */
	/* For.Prototype_GameObject_MiniMap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMap"),
		CUI_MiniMap::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MiniMap_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMap_Monster"),
		CUI_MiniMap_Monster::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_QuestNavi */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuestNavi"),
		CUI_QusetNavi::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FormChange */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FormChange"),
		CUI_FormChange::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* UI :: MiniGame */
	/* For.Prototype_GameObject_Minigame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame"),
		CUI_Minigame::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Minigame_Fade */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Fade"),
		CMiniGame_Fade::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* UI :: Boss */
	/* For.Prototype_GameObject_BossName */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossName"),
		CUI_BossName::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Good */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Good"),
		CGood::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForTwilightTownPool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 8; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_SkullSoldier"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	for (_uint i = 0; i < 5; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_CrowSoldier"), TEXT("Prototype_GameObject_CrowSoldier"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	for (_uint i = 0; i < 3; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_ThinSoldier"), TEXT("Prototype_GameObject_ThinSoldier"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_TWILIGHT, TEXT("Layer_Nightmare"), TEXT("Prototype_GameObject_Nightmare"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Texture_TestTerrain
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TestTerrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/texForest_%d.dds"), 4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// VIBUffer_Terrain
	CVIBuffer_Terrain::TERRAIN_BUFFER_DESC terrain_desc;
	terrain_desc.numX  = (_uint)513;
	terrain_desc.numZ  = (_uint)513;
	terrain_desc.tileX = (_uint)36;
	terrain_desc.tileZ = (_uint)36;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, terrain_desc))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Texture_TerrainBrush
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TerrainBrush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/brush_image.png")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Texture_Water
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Objects/T_he_RiverWaterD1.dds")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Texture_Water_Normal
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Normal"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Objects/T_he_RiverWaterN2.dds")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Navigation_Rapunzel
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Navigation_Rapunzel"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/END_RA_Navigation.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// ShiningEgg
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_ShiningEgg"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/ShiningEgg/ShiningEgg.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Monkey
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Monkey"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Monkey/Monkey.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Mower
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Mower"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Mower/Mower.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Flower
	TransformMatrix = XMMatrixScaling(0.0275f, 0.0275f, 0.0275f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Flower"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Flower/Flower.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nobody
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Nobody"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Nobody/Nobody.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TreasureBox_RA
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_TreasureBox_RA"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TreasureBox_RA/TreasureBox_RA.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// RA_Map
	if (FAILED(Loading_RA_Map()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Terrain
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ShiningEgg
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShiningEgg"),
		CShiningEgg::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Monkey
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monkey"),
		CMonkey::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Mower
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mower"),
		CMower::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Flower
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flower"),
		CFlower::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nobody
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Nobody"),
		CNobody::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Water
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Water"),
		CWater::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelUIs()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelPool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (_uint i = 0; i < 6; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL, TEXT("Layer_Monkey"), TEXT("Prototype_GameObject_Monkey"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 5; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL, TEXT("Layer_Mower"), TEXT("Prototype_GameObject_Mower"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 31; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL, TEXT("Layer_ShiningEgg"), TEXT("Prototype_GameObject_ShiningEgg"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL, TEXT("Layer_Flower"), TEXT("Prototype_GameObject_Flower"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL, TEXT("Layer_Nobody"), TEXT("Prototype_GameObject_Nobody"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelBossComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// VIBUffer_Terrain
	CVIBuffer_Terrain::TERRAIN_BUFFER_DESC terrain_desc;
	terrain_desc.numX  = (_uint)257;
	terrain_desc.numZ  = (_uint)257;
	terrain_desc.tileX = (_uint)1;
	terrain_desc.tileZ = (_uint)1;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, terrain_desc))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Navigation_Rapunzel_Boss
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/END_RA_Boss_Navigation.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelBossModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// BigTree
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_BigTree"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/BigTree/BigTree.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Crow
	TransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_Crow"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Crow/Crow.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// RA_Boss_Map
	if (FAILED(Loading_RA_Boss_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelBossObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Crow
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crow"),
		CCrow::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// BigTree
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BigTree"),
		CBigTree::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Minigame */
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrowMini"),
		CMini_Crow::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrowMini_Yellow"),
		CMini_CrowYellow::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//// Firefly
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireFly"),
	//	CFireFly::Create(m_pDevice, m_pDeviceContext))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelBossUIs()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForRapunzelBossPool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_BigTree"), TEXT("Prototype_GameObject_BigTree"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForFrozenComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Navigation_Frozen
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Navigation_Frozen"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/END_FZ_Navigation.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForFrozenModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// IceMan
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_IceMan"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/IceMan/IceMan.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// Icicle_A
	TransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_Icicle_A"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Icicle/Icicle_A/Icicle_A.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Icicle_B
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_Icicle_B"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Icicle/Icicle_B/Icicle_B.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Icicle_C
	TransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_Icicle_C"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Icicle/Icicle_C/Icicle_C.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Icicle_D
	TransformMatrix = XMMatrixScaling(0.022f, 0.022f, 0.022f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_Icicle_D"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Icicle/Icicle_D/Icicle_D.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// TreasureBox_FZ
	TransformMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_TreasureBox_FZ"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TreasureBox_FZ/TreasureBox_FZ.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// FZ_Map
	if (FAILED(Loading_FZ_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForFrozenObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// IceMan
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceMan"),
		CIceMan::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Icicle
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Icicle"),
		CIcicle::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForFrozenUIs()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForFrozenPool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_FROZEN, TEXT("Layer_IceMan"), TEXT("Prototype_GameObject_IceMan"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (_uint i = 0; i < 12; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_FROZEN, TEXT("Layer_Icicle"), TEXT("Prototype_GameObject_Icicle"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForKeyGraveComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Navigation_KeyGrave
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/END_KG_Navigation.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForKeyGraveModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// FrozenWolf
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_FrozenWolf"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/FrozenWolf/FrozenWolf.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// LittleWolf
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_LittleWolf"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/SummonWolf/SummonWolf.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Skydome
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_Skydome"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Skydome.dat"), XMMatrixIdentity()))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// KG_Map
	if (FAILED(Loading_KG_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForKeyGraveObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// FrozenWolf
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FrozenWolf"),
		CFrozenWolf::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// LittleWolf
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LittleWolf"),
		CLittleWolf::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// KGWall
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KGWall"),
		CMap_Object_KGWall::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForKeyGraveUIs()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForKeyGravePool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_KEYGRAVE, TEXT("Layer_FrozenWolf"), TEXT("Prototype_GameObject_FrozenWolf"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForFinalBossComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Navigation_FinalBoss
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Navigation_FinalBoss"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../NaviData/END_BT_Navigation.dat")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLoader::Loading_ForFinalBossModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// Goat
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Goat"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Goat/Goat.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// GoatWeapon
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_GoatWeapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/GoatWeapon/GoatWeapon.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Final_Map
	if (FAILED(Loading_FINAL_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForFinalBossObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Goat
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goat"),
		CGoat::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// GoatWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GoatWeapon"),
		CGoatWeapon::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// PillarOfFire
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PillarOfFire"),
		CPillarOfFire::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForFinalBossUIs()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForFinalBossPool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_FINALBOSS, TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForMapToolComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMapToolModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMapToolObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// PickingPoint
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PickingPoint"),
		CPickingPoint::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForEffectToolComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	///* For.Prototype_Component_Texture_Star*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Texture_Star"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/star_00.dds"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Particle*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Texture_Particle"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Particle0.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_BearTrail*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Texture_EffectMesh"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/BearTrail.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Star_Color*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Texture_Star_Color"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/ktx_str_col0.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Trail_Mesh*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Texture_Trail_Mesh"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/BearTrail.png"), 1))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForEffectToolModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	///* For.Prototype_Component_Model_Storm */
	//TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Model_Storm"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Effect/Storm.dat"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Effect_Star_Default */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Model_Star_Default"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Effect/Star/Star00.dat"), TransformMatrix))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	///* For.Prototype_Component_Model_Effect_Trail_Mesh */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Model_Effect_Trail_Mesh"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Effect/Trail_Mesh/Ring01.dat"), TransformMatrix))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	///* For.Prototype_Component_Model_Effect_Cone*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Model_Effect_Cone"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/EffectMesh/Cone.dat"), TransformMatrix))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	///* For.Prototype_Component_Model_Effect_Slash0 */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("Prototype_Component_Model_Effect_Slash0"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/EffectMesh/Slash0.dat"), TransformMatrix))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLoader::Loading_ForEffectToolObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_EffectTexture */
	//CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectTexture"),
	//	CEffect_Texture::Create(m_pDevice, m_pDeviceContext)));

	///* For.Prototype_GameObject_EffectParticle */
	//CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"),
	//	CEffect_Particle::Create(m_pDevice, m_pDeviceContext)));

	///* For.Prototype_GameObject_EffectMesh */
	//CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectMesh"),
	//	CEffect_Mesh::Create(m_pDevice, m_pDeviceContext)));

	return S_OK;
}

HRESULT CLoader::Loading_ForTestComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForTestModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix = mat_scale001;

	// FrogBand ============================================================================================================================================================
	// Vocal
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Vocal"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Vocal/FrogBand_Vocal.dat"), TransformMatrix))))
		return E_FAIL;

	// Violin
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Violin"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Violin/FrogBand_Violin.dat"), TransformMatrix))))
		return E_FAIL;

	// Tuba
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Tuba"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Tuba/FrogBand_Tuba.dat"), TransformMatrix))))
		return E_FAIL;

	// Trumpet
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Trumpet"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Trumpet/FrogBand_Trumpet.dat"), TransformMatrix))))
		return E_FAIL;

	// Timpani
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Timpani"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Timpani/FrogBand_Timpani.dat"), TransformMatrix))))
		return E_FAIL;

	// Master
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Master"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Master/FrogBand_Master.dat"), TransformMatrix))))
		return E_FAIL;

	// Horn
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Horn"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Horn/FrogBand_Horn.dat"), TransformMatrix))))
		return E_FAIL;

	// Flute
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Flute"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Flute/FrogBand_Flute.dat"), TransformMatrix))))
		return E_FAIL;

	// Cello
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_FrogBand_Cello"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FrogBand/FrogBand_Cello/FrogBand_Cello.dat"), TransformMatrix))))
		return E_FAIL;
	// =====================================================================================================================================================================
	
	return S_OK;
}

HRESULT CLoader::Loading_ForTestObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// PickingPoint
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PickingPoint"),
		CPickingPoint::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// FrogBand
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FrogBand"),
		CFrogBand::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForEnding()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Texture ===================== */
	/* ============================= */

	/* For.Prototype_Component_Texture_MiniGame_Player */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Player"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Minigame_Player%d.png"), 2)));

	/* For.Prototype_Component_Texture_MiniGame_Boss */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Boss"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Boss_%d.png"),5)));

	/* For.Prototype_Component_Texture_MiniGame_Reward */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Reward"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Reward_%d.png"), 6)));

	///* For.Prototype_Component_Texture_MiniGame_Reward */
	//CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Reward"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/ENDENDEND.png"))));


	/* For.Prototype_Component_Texture_MiniGame_Damage_1 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Damage1_%d.png"), 6)));

	/* For.Prototype_Component_Texture_MiniGame_Damage_2 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Damage2_%d.png"), 4)));

	/* For.Prototype_Component_Texture_MiniGame_Damage_3 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_3"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Damage3_%d.png"), 3)));

	/* For.Prototype_Component_Texture_MiniGame_Damage_4 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_4"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Damage4_%d.png"), 3)));

	/* For.Prototype_Component_Texture_MiniGame_Damage_5 */
	CHECK_FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_5"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Damage5_%d.png"), 3)));

	/* === */
	/* For.Prototype_Copmonent_Texture_Boss1_Bullet1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss1_Bullet1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Ending/Logo_Back%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss2_Bullet1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss2_Bullet1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_Icon.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss2_Bullet2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss2_Bullet2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Monster_Icon.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss3_Bullet1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/warning%d.png"),3))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss3_Bullet2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Effect87.dds")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss3_Bullet3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet3"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Effect84.dds")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss4_Bullet1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss4_Bullet1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/Player_bullet.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Boss5_Bullet1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss5_Bullet1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/burger.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_MINI_Background */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_MINI_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/EndBackGround.png")))))
		return E_FAIL;

	/* GameObject ================== */
	/* ============================= */

	/* For.Prototype_GameObject_Minigame_Player */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Player"),
		CMini_Player::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_Bullet */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Bullet"),
		CMINI_Bullet::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_Boss */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Boss"),
		CMini_Boss::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_Damage */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Damage"),
		CMini_Damage::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_BackGround */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_BackGround"),
		CMini_BackGround::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_Reward */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_Reward"),
		CMini_Reward::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_GameObject_Minigame_BossBullet */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minigame_BossBullet"),
		CMini_BossBullet::Create(m_pDevice, m_pDeviceContext)));




	return S_OK;
}

HRESULT CLoader::Loading_ForColliderToolComponents()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 스카이박스
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky%d.dds"), 6))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForColliderToolModels()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix TransformMatrix;

	// SkullSldier
	TransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_SkullSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/SkullSoldier/SkullSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// CrowSoldier
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_CrowSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/CrowSoldier/CrowSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ThinSoldier
	TransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_ThinSoldier"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/ThinSoldier/ThinSoldier.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nightmare
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Nightmare"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Nightmare/Nightmare.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Bowman
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_BowMan"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/BowMan/BowMan.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Cannon
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Cannon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Cannon/Cannon.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ShiningEgg
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_ShiningEgg"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/ShiningEgg/ShiningEgg.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Monkey
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Monkey"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Monkey/Monkey.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Mower
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Mower"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Mower/Mower.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Nobody
	TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Nobody"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Nobody/Nobody.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Crow
	TransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Crow"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Crow/Crow.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// BigTree
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_BigTree"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/BigTree/BigTree.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Goat
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_Goat"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/Goat/Goat.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// IceMan
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_IceMan"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/IceMan/IceMan.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// FrozenWolf
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_FrozenWolf"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/FrozenWolf/FrozenWolf.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// LittleWolf
	TransformMatrix = mat_scale001;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_COLLIDERTOOL, TEXT("Prototype_Component_Model_LittleWolf"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Monster/SummonWolf/SummonWolf.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForColliderToolObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColliderTool_Model"), CColliderTool_Model::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Camera_Dynamic
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Axis
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axis"),
		CAxis::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Sky
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::ReadyMapFromSaveFiles(LEVEL eLevel, const _tchar * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	DWORD dwBytes = 0;
	CMap::MAPDESC MapDesc;
	ZeroMemory(&MapDesc, sizeof(CMap::MAPDESC));
	_float fFrustumRange = 0.f;

	while (true)
	{
		ReadFile(hFile, &MapDesc, sizeof(CMap::MAPDESC), &dwBytes, nullptr);
		ReadFile(hFile, &fFrustumRange, sizeof(_float), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		CGameObject* pNewMap = CGameInstance::GetInstance()->Add_GameObject_And_Return(eLevel, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapDesc);

		// 맵은 생성할 때도 pArg 받고, 세이브데이터 로드할 때도 pArg 받기 때문에 Load_ObjectData 를 호출하는 대신에 객체를 생성하고서 월드행렬과 frustumRange 를 따로 세팅해줌
		pNewMap->Get_Transform()->Set_WorldMatrix(MapDesc.WorldMatrix);
		pNewMap->SetFrustumRange(fFrustumRange);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Loading_Triggers(LEVEL eLevel, const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 생성하고 월드행렬이랑 타입 세팅
	DWORD               dwBytes = 0;
	_float4x4           WorldMatrix;
	CTrigger::TRIGGER   eTrigger;

	while (true)
	{
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwBytes, nullptr);
		if (dwBytes == 0)
			break;

		ReadFile(hFile, &eTrigger, sizeof(CTrigger::TRIGGER), &dwBytes, nullptr);
		if (dwBytes == 0)
			break;

		CGameObject* pGameObject = CGameInstance::GetInstance()->Add_GameObject_And_Return(eLevel, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger"));
		pGameObject->Get_Transform()->Set_WorldMatrix(WorldMatrix);
		dynamic_cast<CTrigger*>(pGameObject)->Set_TriggerType(eTrigger);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Loading_TT_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_BackGround_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_BackGround/TT_BackGround_0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_BackGround_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_BackGround/TT_BackGround_1.dat"), TransformMatrix)))) return E_FAIL;

	// ============================================================================================================================================ //

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Floor_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Floor/TT_Floor_0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Floor_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Floor/TT_Floor_1.dat"), TransformMatrix)))) return E_FAIL;

	// ============================================================================================================================================ //

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_32.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_33"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_33.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_34"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_34.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_35"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_35.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_36"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_36.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_37"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_37.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_38"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_38.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_39"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_39.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_40"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_40.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_41"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_41.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_42"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_42.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_43"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_43.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_44"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_44.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_45"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_45.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_46"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_46.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_47"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_47.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_48"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_48.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_49"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_49.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_50"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_50.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Building_51"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Building/TT_Building_51.dat"), TransformMatrix)))) return E_FAIL;


	// ============================================================================================================================================ //

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_32.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_33"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_33.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_34"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_34.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_35"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_35.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_36"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_36.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_37"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_37.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_38"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_38.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_39"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_39.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_40"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_40.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_41"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_41.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_42"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_42.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_43"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_43.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_44"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_44.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_45"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_45.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_46"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_46.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_47"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_47.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_48"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_48.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_49"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_49.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_50"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_50.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_51"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_51.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_52"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_52.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_53"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_53.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_54"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_54.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_55"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_55.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_56"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_56.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_57"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_57.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_58"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_58.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_59"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_59.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_60"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_60.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_61"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_61.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_62"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_62.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_63"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_63.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_64"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_64.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_65"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_65.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_66"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_66.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_67"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_67.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_68"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_68.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_69"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_69.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_70"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_70.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_71"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_71.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_72"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_72.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_Object_73"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TT_Object/TT_Object_73.dat"), TransformMatrix)))) return E_FAIL;

	return S_OK;
}

//HRESULT CLoader::Loading_TS_Andy_Map()
//{
//	_matrix TransformMatrix;
//	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
//
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_0"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_0.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_1"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_1.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_2"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_2.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_3"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_3.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_4"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_4.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_5"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_5.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_6"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_6.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_7"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_7.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_8"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_8.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_9"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_9.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_10"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_10.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_11"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_11.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_12"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_12.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_13"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_13.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_14"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_14.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_15"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_15.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_16"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_16.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_17"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_17.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_18"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_18.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_19"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_19.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_20"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_20.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_21"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_21.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_22"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_22.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_23"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_23.dat"), TransformMatrix)))) return E_FAIL;	
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_24"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_24.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_25"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_25.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_26"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_26.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_27"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_27.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_28"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_28.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_29"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_29.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_30"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_30.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_31"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_31.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_32"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_32.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_33"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_33.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_34"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_34.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_35"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_35.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_36"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_36.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_37"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_37.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_38"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_38.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_39"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_39.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_40"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_40.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_41"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_41.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_42"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_42.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_43"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_43.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_44"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_44.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_45"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_45.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_46"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_46.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_47"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_47.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_48"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_48.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_49"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_49.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_50"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_50.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_51"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_51.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_52"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_52.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_53"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_53.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_54"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_54.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_55"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_55.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_56"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_56.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_57"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_57.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_58"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_58.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_59"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_59.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Andy_60"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Andy/TS_Andy_60.dat"), TransformMatrix)))) return E_FAIL;
//
//	// ============================================================================================================================================ //
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_0"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_0.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_1"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_1.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_2"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_2.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_3"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_3.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_4"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_4.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_5"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_5.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_6"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_6.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_7"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_7.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_8"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_8.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_9"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_9.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_10"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_10.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_11"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_11.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_12"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_12.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_13"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_13.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_14"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_14.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_15"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_15.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_16"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_16.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_17"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_17.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_18"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_18.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_19"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_19.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_20"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_20.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_21"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_21.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_22"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_22.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_23"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_23.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_24"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_24.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_25"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_25.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Duct_26"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Duct/TS_Duct_26.dat"), TransformMatrix)))) return E_FAIL;
//
//	return S_OK;
//}

//HRESULT CLoader::Loading_TS_Doll_Map()
//{
//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//
//	_matrix TransformMatrix;
//	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_0"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_0.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_1"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_1.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_2"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_2.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_3"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_3.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_4"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_4.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_5"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_5.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_6"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_6.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_7"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_7.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_8"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_8.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_9"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_9.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_10"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_10.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_11"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_11.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_12"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_12.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_13"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_13.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_14"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_14.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_15"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_15.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_16"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_16.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_17"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_17.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_18"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_18.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_19"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_19.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_20"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_20.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_21"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_21.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_22"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_22.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_23"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_23.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_24"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_24.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_25"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_25.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_26"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_26.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_27"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_27.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_28"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_28.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_29"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_29.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_30"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_30.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_31"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_31.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_32"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_32.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_33"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_33.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_34"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_34.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_35"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_35.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_36"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_36.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_37"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_37.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_38"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_38.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_39"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_39.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_40"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_40.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_41"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_41.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_42"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_42.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_43"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_43.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_44"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_44.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_45"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_45.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_46"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_46.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_47"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_47.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_48"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_48.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_49"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_49.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_50"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_50.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_51"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_51.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_52"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_52.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_53"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_53.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_54"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_54.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_55"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_55.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_56"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_56.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_57"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_57.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_58"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_58.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_59"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_59.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_60"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_60.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_61"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_61.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_62"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_62.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_63"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_63.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_64"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_64.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_65"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_65.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_66"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_66.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_67"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_67.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_68"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_68.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_69"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_69.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_70"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_70.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_71"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_71.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_72"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_72.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_73"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_73.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_74"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_74.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_75"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_75.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_76"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_76.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_77"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_77.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_78"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_78.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_79"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_79.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_80"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_80.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_81"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_81.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_82"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_82.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_83"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_83.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_84"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_84.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_85"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_85.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_86"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_86.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_87"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_87.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_88"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_88.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_89"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_89.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_90"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_90.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_91"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_91.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_92"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_92.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_93"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_93.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_94"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_94.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_95"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_95.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_96"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_96.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_97"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_97.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_98"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_98.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_99"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_99.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_100"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_100.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_101"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_101.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_102"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_102.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_103"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_103.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_104"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_104.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_105"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_105.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_106"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_106.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_107"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_107.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_108"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_108.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_109"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_109.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_110"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_110.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_111"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_111.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_112"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_112.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_113"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_113.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_114"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_114.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_115"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_115.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_116"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_116.dat"), TransformMatrix)))) return E_FAIL;
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_117"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_117.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_118"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_118.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_119"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_119.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_120"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_120.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_121"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_121.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_122"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_122.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_123"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_123.dat"), TransformMatrix)))) return E_FAIL;
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_124"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_124.dat"), TransformMatrix)))) return E_FAIL;
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_125"),
//		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_125.dat"), TransformMatrix)))) return E_FAIL;
//	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TS_Doll_126"),
//	//	CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/TS_Doll/TS_Doll_126.dat"), TransformMatrix)))) return E_FAIL;
//
//	return S_OK;
//}

HRESULT CLoader::Loading_RA_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Bridge0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Bridge0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Bridge1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Bridge1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Bridge2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Bridge2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Bridge3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Bridge3.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Building9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Building9.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Window26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Window26.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_CloseWindow0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/CloseWindow0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_CloseWindow1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/CloseWindow1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_CloseWindow2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/CloseWindow2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_CloseWindow3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/CloseWindow3.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Door10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Door10.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_LargeDoor5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/LargeDoor5.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Ground0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Ground0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Ground1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Ground1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Ground2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Ground2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Ground3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Ground3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Ground4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Ground4.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Plant7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Plant7.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Stairs0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Stairs0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Stairs1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Stairs1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Stairs2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Stairs2.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object32.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object33"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object33.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object34"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object34.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object35"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object35.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object36"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object36.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Object37"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_Model/Object37.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree32.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_33"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree33.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Tree_34"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree34.dat"), TransformMatrix)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_RA_Rock_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock6.dat"), TransformMatrix)))) return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_RA_Boss_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Grass_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Grass32.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Rock_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Rock17.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tower_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tower19.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree7.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree8.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree9.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree10.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree11.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree12.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree13.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree14.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree15.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree16.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree17.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree18.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree19.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_20"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree20.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_21"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree21.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree22.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree23.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree24.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree25.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree26.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree27.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree28.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree29.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree30.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree31.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree32.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_33"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree33.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_RA_Tree_34"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/RA_MapModel/ra_Tree34.dat"), TransformMatrix)))) return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_FZ_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_FZ_Castle0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FZ_Model/castle0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FROZEN, TEXT("Prototype_Component_Model_FZ_Castle1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/FZ_Model/castle1.dat"), TransformMatrix)))) return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_KG_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapGround"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Ground.dat"), TransformMatrix)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapMoon"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Moon.dat"), TransformMatrix)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_Area0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Area0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_Area1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Area1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_Area2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Area2.dat"), TransformMatrix)))) return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapObject0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Object0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapObject1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Object1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapObject2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Object2.dat"), TransformMatrix)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapRock7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/Rock7.dat"), TransformMatrix)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapStone0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/stone0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapStone1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/stone1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapStone2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/stone2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapStone3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/stone3.dat"), TransformMatrix)))) return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch0"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch0.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch1"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch1.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch2"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch2.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch3"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch3.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch4"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch4.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch5"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch5.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch6"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch6.dat"), TransformMatrix)))) return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch7"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/KG_Model/switch7.dat"), TransformMatrix)))) return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_FINAL_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_BT_MapObject"),
		CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/BT_Model/Object0.dat"), TransformMatrix)))) return E_FAIL;


	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSGBOX("Failed to Creating CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_CS);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
