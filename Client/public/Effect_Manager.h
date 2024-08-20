#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Effect_Mesh.h"
#include "Effect_Particle.h"
#include "Effect_Texture.h"
#include "MonsterDefines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CEffect_Manager final : public CBase
{
	MAKE_SINGLETON(CEffect_Manager)

public:
	enum EFFECT_NAME { HIT_STAR, TRAIL_RING, SKULL_APPEAR };

	enum EFFECT_TYPE { 
		
		TYPE_MOVEONE_SPIN, TYPE_TRAIL_MESH, TYPE_APPEAR, TYPE_TRAIL_SHORT, TYPE_RED_PEARL, TYPE_RAGEFORM_BASEATTACK, TYPE_RAGEFORM_SCRATCH, 

		// Hammer Effects
		TYPE_HAMMER_ATK1, TYPE_HAMMER_ATK2, TYPE_HAMMER_ATK3, TYPE_HAMMER_ATK4, TYPE_HAMMER_ATK5, TYPE_HAMMER_LAST_ATTACK_GROUND, TYPE_HAMMER_LAST_ATTACK_AIR, 
		TYPE_HAMMER_LAST_UPSTREAM, TYPE_HAMMER_LAST_SPIKE, TYPE_HAMMER_LAST_RING,
		TYPE_HAMMER_HEAD_STREAM, TYPE_HAMMER_HEAD_AIRCUT, TYPE_HAMMER_AIRCUT, TYPE_HAMMER_RINGS, TYPE_HAMMER_WAVE, TYPE_HAMMER_CROWN, TYPE_HAMMER_GUST,

		TYPE_WAVERING, 
		
		TYPE_END };
	
	enum TRAIL_TYPE  { TRAIL_DEFAULT, TRAIL_LEFT, TRAIL_RIGHT, TRAIL_END};
	
	enum MONSTER_TYPE { SKULL, CROW, THIN, SHININGEGG, MONKEY, MONSTER_END };
	
	enum EFFECT_PTCL_TYPE {TYPE_SHINING_PTCL, TYPE_RAGEFORM_PTCL, 
		TYPE_HAMMERFORM_ATTACK_PTCL, TYPE_HAMMERFORM_GROUND_GLOW_PTCL, TYPE_HAMMERFORM_GROUND_ROCK_PTCL, 
		TYPE_HAMMERFORM_EMBERS_PTCL, TYPE_ATTACKED_PTCL, TYPE_GETITEM_PTCL,  TYPE_PTCL_END};

	enum TEXTURE_EFFECT_TYPE { 
		// 보스 몬스터 텍스쳐 이펙트
		TEXTURE_GOATSMOKE, TEXTURE_BIGTREESMOKE, 
		
		// 플레이어 텍스쳐 이펙트
		TEXTURE_RAGEFORM_ATTACK_SPARKLE, TEXTURE_RAGEFORM_ATTACK_CIRCLE,
		TEXTURE_END };


private:
	CEffect_Manager() = default;
	virtual ~CEffect_Manager() = default;

public:
	void Initialize();
	virtual void Free() override;

	HRESULT Create_Effect(EFFECT_NAME eName, void* pArg = nullptr);

	// 공통
	HRESULT Create_AppearAction(_fvector vPosition, MONSTER_TYPE eType);
	HRESULT Create_Ribbons(_float4x4*	pTargetWorld);

	HRESULT Create_DeadEffect(CTransform* pTransform, _float fSize, _float fOffsetY);
	HRESULT Create_SkillDeadEffect(CTransform* pTransform, _float fOffsetY); // 스킬 터짐

	HRESULT Create_ShineParticle(CTransform* pTransform, _fvector vOffset);

	// Treasure_Box
	HRESULT Create_OpenBoxEffect(CTransform* pTransform);
	HRESULT Create_InBoxEffect(CTransform* pTransform);

	//Skull
	HRESULT Create_Landing(_fvector vPosition);

	//Crow
	HRESULT Create_DashMotion(_float4x4* pTargetWorld);
	CEffect_Mesh* Create_CrowAttack_LongDi_Moon(CTransform* pTransform, _bool* pDead, _float fOffsetY = 0.f);
	HRESULT Create_CrowAttack_LongDi_Wind(_float4x4* pTargetWorld);
	HRESULT Create_CrowAttack_Roaring(_fvector vPosition);

	//ThinSoldier
	HRESULT Create_Teleport(_fvector vPosition);
	HRESULT Create_AttackSlash(_float4x4*	pTargetWorld, _bool* pDead);
	HRESULT Create_AttackSlash_Short(_float4x4*	pTargetWorld, _bool* pDead);

	//ShiningEgg
	HRESULT Create_Cloud(_fvector vPosition);
	HRESULT Create_Tornado(_float4x4* pTargetWorldm, _bool* pDead);
	HRESULT Create_MagicGrass(CTransform* pTransform);
	HRESULT Create_ShiningEggPatricle(CTransform* pTransform);

	//Monkey
	HRESULT Create_ShockWave(_float4x4* pTargetWorld);
	HRESULT Create_Monkey_AttackSlash(_float4x4* pTargetWorld);

	//Mower
	HRESULT Create_BigTornado(_fvector vPosition);
	HRESULT Create_Poison(_float4x4* pTargetWorld);

	//Crow
	HRESULT Create_CrowParticle(CTransform* pTransform, _bool* pDead);

	// Nightmare
	void Create_NightmareSpawnTornado(CTransform* pTransform);
	void Create_NightmareSpawnXCross(CTransform* pTransform);
	void Create_NightmareRunEffect(CTransform* pTransform, _uint* iEffectCount);
	void Create_NightmareNails(CTransform* pTransform, _bool* pParendDead, _uint iNum);
	void Create_NightmareNails_New(CTransform* pTransform, _bool* pParendDead, _uint iNum);
	void Create_NightmareNailTrail(CTransform* pTransform, _uint iNum = 0);
	void Create_NightmareBeforeNails_Rings(CTransform* pTransform, _uint iSequence);
	void Create_NightmareRotationAtk(CTransform* pTransform);
	void Create_NightmareRush(CTransform* pTransform);
	void Create_NightmareFlyKick(CTransform* pTransform);
	void Create_NightmareFlyKick_Before(CTransform* pTransform);
	void Create_NightmareShuriken(CTransform* pTransform, _bool* pParendDead, _bool bReserve = false);
	void Create_NightmareBlade(CTransform* pTransform);
	HRESULT Create_SkillDeadEffect_Tornado(CTransform* pTransform); // 나이트메어 표창터짐

	// Nobody
	void Create_NobodySpawnMagicCircle(CTransform* pTransform);	// 등장할때 마법진
	void Create_NobodySpawnTurn(CTransform* pTransform);		// 등장하면서 돌 때 나오는 이펙트
	void Create_NobodyChargeRush(CTransform* pTransform);		// 돌진 전 기모으기 이펙트
	void Create_NobodyRush(CTransform* pTransform);				// 돌진
	void Create_NobodyRotationAtk(CTransform* pTransform);		// 회전공격
	void Create_Flowers(CTransform* pTransform);				// 꽃잎

	// BigTree
	void	Create_BigTreeHowling(CTransform* pTransform);
	void	Create_BigTreeSmoke(CTransform* pTransform, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval);
	void	Create_BigTreeSmoke(_float4 vBonePos, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval);
	void	Create_BigTreeFall(CTransform* pTransform, _float4x4* pBoneMatrix, _float fLifeTime, _fmatrix localMat = XMMatrixIdentity());
	HRESULT Create_Apple(_float4x4* pTargetWorld);
	void	Create_FruitZone(_vector vPosition);
	void	Create_BigTreeSpawn(CTransform* pTransform); // 스폰될때 리본모양
	void	Create_BigTree_LandingWind(CTransform* pTransform); // 까마귀 소환할때 몸 주변 바람
	void	Create_BigTree_SummonWings(CTransform* pTransform); // 까마귀 소환할 때 날개
	void	Create_BigTree_Slash(CTransform* pTransform, _fvector vOffset); // Hug 공격할때
	void	Create_SoaringWind(_float4x4* pBoneMatrix); // 하늘로 올라갈 때
	void	Create_BigTreeSummonApple(CTransform* pTransform);
	void	Create_BigTreeSummonAppleParticle(CTransform* pTransform);
	void	Create_BigTreeSummonApple_Ring(CTransform* pTransform);
	void	Create_Explosion(_vector vPosition, _float vScale,  _uint iNum = 0);
	void	Create_Leaves(_vector vPosition, _fvector vOffset);
	void	Create_BigTreeParticle(_float4* pPos, _bool* pDead);

	// IceMan
	void Create_IceManSlash(CTransform* pTransform, _bool bIsLeftHandSlash = true); // 손톱 휘두르기
	void Create_IceManJump(CTransform* pTransform); // 점프
	void Create_IceManJumpLanding(CTransform* pTransform); // 점프 착지
	void Create_IceManClap(CTransform* pTransform); // 박수공격
	void Create_IceManJumpAtkLanding(CTransform* pTransform, _bool bIsLeftHandBlow = true); // 점프공격 내려찍기
	void Create_IceManJumpAtkGuide(CTransform* pTransform, _bool* pParentDead);
	void Create_IceBall(CTransform* pTransform, _bool* pParentDead); // 아이스볼 생성
	void Create_IceBallParticle(CTransform* pTransform, _bool* pParentDead); // 아이스볼 파티클 생성
	void Create_IcicleEffect(CTransform* pTransform); // 고드름 생성, 제거
	void Create_IceManRoar(CTransform* pTransform); // 소리지르기
	HRESULT Create_IceBallBreak(CTransform* pTransform);

	//	Goat
	void Create_GoatTeleport(CTransform* pTransform);
	vector<CGameObject*> Create_GoatSpear(_float4x4* pBoneMatrix);
	void Create_GoatFlySwing(CTransform* pTransform);
	void Create_GoatDashSwing(CTransform* pTransform);
	void Create_GoatJumpDashSwing(CTransform* pTransform);
	void Create_GoatUnderSwing(CTransform* pTransform);
	vector<CGameObject*> Create_GoatSpinDashSwing(CTransform* pTransform);
	void Create_SpinDashSwingFinish(CTransform* pTransform);
	CTransform* Create_GoatPillarOfFire(CTransform* pTransform);
	void Create_GoatPillarOfFire_Short(CTransform* pTransform);
	void Create_GoatPillarOfFire_Short_Ready(CTransform* pTransform);
	void Create_GoatPillarOfFire_Rotation(CTransform* pTransform);
	void Create_GoatSmoke(_fvector vPosition, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval);
	CGameObject* Create_GoatParticle(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_Goat_FireBallRing(CTransform* pTransform, _bool* pParentDead);
	HRESULT Create_Goat_FireBallExplosion(CTransform* pTransform);
	void Create_Goat_FireBall(CTransform* pTransform, _bool* pParentDead);
	HRESULT Create_Goat_PillarWave(CTransform* pTransform);
	HRESULT Create_Goat_PillarWave_Rotation(CTransform* pTransform);

	//Player
	HRESULT Create_HitStar(CTransform* pTransform, _float fOffsetY = 0.f);
	HRESULT Create_HitStar(_float4* pPosition, _float fOffsetY = 0.f);
	HRESULT Create_AttackedEffect(_float4* pPosition, MONSTERTYPE eMonsterType, _float fOffsetY = 0.f);
	HRESULT Create_SummonParticle(CTransform* pTransform);

	HRESULT Create_SwordEffect(_float4x4* pTargetWorld);
	HRESULT Create_SwordEffectParticle(CTransform* pTransform);

	HRESULT Create_ReadyToAttackEffect(CTransform* pTransform);

	HRESULT Create_PlayerHealEffect(CTransform* pTransform);
	HRESULT Create_PlayerHealEffect_Full(CTransform* pTransform);
	HRESULT Create_PlayerHealEffect_MP(CTransform* pTransform);
	HRESULT Create_PlayerBuffEffect(CTransform* pTransform);
	HRESULT Create_PlayerBuffEffect_AttackSpeed(CTransform* pTransform);
	HRESULT Create_PlayerBuffEffect_MovingSpeed(CTransform* pTransform);

	// 4연타 공격시 나오는 이펙트
	HRESULT Create_TrailMesh(CTransform* pTransform);
	HRESULT Create_TrailMesh_Spin(_float4x4* pTargetWorld);

	HRESULT Create_Dash(CTransform* pTransform);
	HRESULT Create_AirWind(_float4x4* pTargetWorld);
	HRESULT Create_Guard(CTransform* pTransform);
	HRESULT Create_TrailMesh_Short(_float4x4* pTargetWorld, TRAIL_TYPE eTrailType = TRAIL_DEFAULT, _float4 * pPosition = nullptr);

	// HammerForm
	HRESULT Create_HammerForm_Gust(CTransform* pTransform, _fvector vPosition);

	HRESULT Create_HammerForm_Attack1(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_Attack2(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_Attack3(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_Attack4(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_Attack5(CTransform* pTransform, _fvector vOffset, _uint iSequence);

	HRESULT Create_HammerForm_LastAttack_Ground(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_LastAttack_Air(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_LastAttack_UpStream(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_HammerForm_LastAttack_Spike(CTransform* pTransform, _fvector vPoint, _fvector vLook);
	HRESULT Create_HammerForm_LastAttack_Circle(CTransform* pTransform, _fvector vPoint, _fvector vLook);

	HRESULT Create_HammerForm_AttackEffect_Beads(_fvector vPoint); // 파티클
	HRESULT Create_HammerForm_AttackEffect_Rings(CTransform* pTransform, _fvector vPoint);

	HRESULT Create_HammerForm_Effect_GroundHit_Glow(_fvector vPoint, _uint iAnimIndex); // 파티클
	HRESULT Create_HammerForm_Effect_GroundHit_Rock(_fvector vPoint, _uint iAnimIndex); // 파티클
	HRESULT Create_HammerForm_Effect_GroundHit_Wave(CTransform* pTransform, _fvector vPoint, _uint iAnimIndex);
	HRESULT Create_HammerForm_Effect_GroundHit_Crown(CTransform* pTransform, _fvector vPoint, _float fRotRange, _uint iAnimIndex);

	HRESULT Create_HammerForm_Effect_Swing_HeadStream(CTransform* pTransform, _float4x4* pMatrix, _uint iAnimIndex);
	HRESULT Create_HammerForm_Effect_Swing_HeadAirCut(CTransform* pTransform, _float4x4* pMatrix);
	CGameObject* Create_HammerForm_Effect_Swing_HeadEmbers(CTransform* pTransform, _float4* pPos);  // 파티클
	HRESULT Create_HammerForm_Effect_Swing_AirCut(CTransform* pTransform, _fvector vPoint, _fvector vLookDir);

	// RageForm
	HRESULT Create_RageForm_FormChange(CTransform* pTransform); // 확인용

	HRESULT Create_RageForm_RedPearl(_fvector vPosition);
	HRESULT Create_RageForm_Ripple(_fvector vPosition);
	HRESULT Create_RageForm_Marble(_fvector vPosition);
	HRESULT Create_RageForm_Particle(CTransform* pTransform);
	HRESULT Create_RageForm_Roaring(CTransform* pTransform);

	HRESULT Create_RageForm_Guard(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_RageForm_Guard_Success(CTransform* pTransform);

	HRESULT Create_RageForm_DoubleTap(_float4* pPos, _fvector vLookDir);
	HRESULT Create_RageForm_TripleTap(_float4* pPos, _fvector vLookDir);

	HRESULT Create_RageForm_RageAttack1(_float4* pPos, _fvector vLookDir, _uint iSequence);
	HRESULT Create_RageForm_RageAttack2(_float4* pPos, _fvector vLookDir, _uint iSequence);
	HRESULT Create_RageForm_RageAttack3(_float4* pPos, _fvector vLookDir);
	HRESULT Create_RageForm_RageAttack4(_matrix WorldMatrix);
	HRESULT Create_RageForm_RageAttack5(_float4* pPos, _fvector vLookDir, _uint iSequence);
	HRESULT Create_RageForm_RageAttack5_Finish(CTransform* pTransform, _fvector vOffset);

	HRESULT Create_RageForm_FinishAttack_Start(CTransform* pTransform);
	HRESULT Create_RageForm_FinishAttack(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_RageForm_FinishAttack_Wave(CTransform* pTransform);

	HRESULT Create_RageForm_CounterAttack_Weapon(_float4x4* pTargetWorld);

	HRESULT Create_RageForm_Effect_Attack_Texture(_fvector vPoint);

	// 소환 이펙트
	HRESULT Create_Summon(CTransform* pTransform);

	// Donald & Goofy
	HRESULT Create_DonaldBye(_fvector vPosition);
	HRESULT Create_GoofyBye(_fvector vPosition);
	HRESULT Create_GoofyHitEffect(_fvector vPosition, _fvector vLook);
	HRESULT Create_HitStarDonald(CTransform* pTransform, _float fOffsetY = 0.f);
	HRESULT Create_HitStarGoofy(CTransform* pTransform, _float fOffsetY = 0.f);

	// FrozenWolf Effect
	HRESULT Create_Wolf_Scratch(CTransform* pTransform, _uint iSequence);
	HRESULT Create_Wolf_SmokeWave(CTransform* pTransform);
	HRESULT Create_Wolf_Fire(CTransform* pTransform, _bool* pDead, _float fOffsetY = 0.f);
	HRESULT Create_Wolf_Fire_Dead(CTransform* pTransform);
	HRESULT Create_Wolf_SoaringRipple(CTransform* pTransform);
	HRESULT Create_Wolf_SpinAttackRing(_float4x4* pWorld, _bool* pDead);
	HRESULT Create_Wolf_SoaringRibbon(_float4x4* pTargetWorld);
	HRESULT Create_Wolf_Marble();
	HRESULT Create_Wolf_Rain(CTransform* pTransform, _bool* pDead);
	HRESULT Create_Wolf_Rain_Splash(CTransform* pTransform);
	HRESULT Create_Wolf_Rock(_fvector vPos);
	HRESULT Create_Wolf_Ptcl(CTransform* pTransform, _fvector vOffset);
	HRESULT Create_Wolf_SpawnEffect(CTransform* pTransform);
	HRESULT Create_Wolf_LandingEffect(CTransform* pTransform);
	HRESULT Create_Wolf_TailWind(CTransform* pTransform);
	HRESULT Create_Wolf_BlowingParticle(_float4* pPos, _fvector vOffset);

private:
	void	Ready_MeshDesc();
	void	Ready_ParticleDesc();
	void	Ready_TextureDesc();

	void	Ready_HammerFormMeshDesc_Trail();
	void	Ready_HammerFormMeshDesc_Hammer();
	void	Ready_HammerFormMeshDesc_Attack();

	void	Ready_HammerFormParticleDesc();

	void	Ready_GoatSmokeDesc();
	void	Ready_BigTreeSmokeDesc();

	void	Ready_RageFormTextureDesc_Attack();
public:
	static const _tchar*							m_pLayerTag;
	static const _tchar*							m_pMeshProtoTag;
	static const _tchar*							m_pTextureProtoTag;
	static const _tchar*							m_pParticleProtoTag;

private:
	ID3D11Device*									m_pDevice = nullptr;
	ID3D11DeviceContext*							m_pDeviceContext = nullptr;

	//Effect_Mesh 구조체 정보 담아놓는 벡터
	vector<CEffect_Mesh::EFFECT_MESHDESC>			m_MeshDesc;

	//Effect_Patricle 구조체 정보 담아놓는 리스트
	vector<CEffect_Particle::PARTICLEDESC>			m_PatricleDesc;

	//Effect_Texture 구조체 정보 담아놓는 리스트
	vector<CEffect_Texture::EFFECT_TEX_DESC>		m_TextureDesc;
};

END