#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CMiniGame_Manager final : public CBase
{
	MAKE_SINGLETON(CMiniGame_Manager)
public:
	CMiniGame_Manager();
	virtual ~CMiniGame_Manager() = default;

public:
	_int	Tick(_double TimeDelta);

	/* =============== */
	
	_int	Get_Boss() { return Boss_Cnt; }
	void	Set_Boss(_int _Cnt) { Boss_Cnt = _Cnt; }
	_int	Boss_Cnt = 0; /* 보스 레벨 */

	_int	Get_Boss1_Pattern() { return Boss1_Pattern; }
	void	Set_Boss1_Pattren(_int _Cnt) { Boss1_Pattern = _Cnt; }
	_int	Boss1_Pattern = 0;

	_int	Get_Boss2_Pattern() { return Boss2_Pattern; }
	void	Set_Boss2_Pattren(_int _Cnt) { Boss2_Pattern = _Cnt; }
	_int	Boss2_Pattern = 0;

	_int	Get_Boss3_Pattern() { return Boss3_Pattern; }
	void	Set_Boss3_Pattren(_int _Cnt) { Boss3_Pattern = _Cnt; }
	_int	Boss3_Pattern = 0;

	_int	Get_Boss4_Pattern() { return Boss4_Pattern; }
	void	Set_Boss4_Pattren(_int _Cnt) { Boss4_Pattern = _Cnt; }
	_int	Boss4_Pattern = 0;

	_int	Get_Boss5_Pattern() { return Boss5_Pattern; }
	void	Set_Boss5_Pattren(_int _Cnt) { Boss5_Pattern = _Cnt; }
	_int	Boss5_Pattern = 0;

	/* =============== */

	_bool	Get_PlayerRender() { return Player_Render; }
	void	Set_PlayerRender(_bool _Cnt) { Player_Render = _Cnt; }
	_bool	Player_Render = true;

	/* =============== */

	_float4	Get_Bullet2_Position() { return Bullet2_Position; }
	void	Set_Bullet2_Position(_float4 _Cnt) { Bullet2_Position = _Cnt; }
	_float4 Bullet2_Position = _float4{ 0.f, 0.f, 0.f, 0.f };

	_float4	Get_Bullet3_Position() { return Bullet3_Position; }
	void	Set_Bullet3_Position(_float4 _Cnt) { Bullet3_Position = _Cnt; }
	_float4 Bullet3_Position = _float4{ 0.f, 0.f, 0.f, 0.f };

	/* =============== */

	_float4	Get_PlayerPosition() { return PlayetPosition; }
	void	Set_PlayerPosition(_float4 _Cnt) { PlayetPosition = _Cnt; }
	_float4	PlayetPosition = _float4{ 0.f, 0.f, 0.f, 0.f };

	_float4	Get_BossPosition() { return BossPosition; }
	void	Set_BossPosition(_float4 _Cnt) { BossPosition = _Cnt; }
	_float4	BossPosition = _float4{ 0.f, 0.f, 0.f, 0.f };

	_int	Get_Damage() { return Damage; }
	_int	Damage = 1;

	/* HP */

	_int	Get_BOSS1_HP() { return BOSS1_HP; }
	_int	Get_BOSS2_HP() { return BOSS2_HP; }
	_int	Get_BOSS3_HP() { return BOSS3_HP; }
	_int	Get_BOSS4_HP() { return BOSS4_HP; }
	_int	Get_BOSS5_HP() { return BOSS5_HP; }
	void	Math_BOSS1_HP(_int _Cnt) { BOSS1_HP += _Cnt; }
	void	Math_BOSS2_HP(_int _Cnt) { BOSS2_HP += _Cnt; }
	void	Math_BOSS3_HP(_int _Cnt) { BOSS3_HP += _Cnt; }
	void	Math_BOSS4_HP(_int _Cnt) { BOSS4_HP += _Cnt; }
	void	Math_BOSS5_HP(_int _Cnt) { BOSS5_HP += _Cnt; }

	_int	BOSS1_HP = 35;
	_int	BOSS2_HP = 50;
	_int	BOSS3_HP = 50;
	_int	BOSS4_HP = 50;
	_int	BOSS5_HP = 50;

	/* === */
	
	_bool	Get_Create_Ending() { return Create_Ending; }
	void	Set_Create_Ending(_bool _Cnt) { Create_Ending = _Cnt; }
	_bool	Create_Ending = false;


	_bool	Get_EndTexture() { return END_Texture; }
	void	Set_EndTexture(_bool _Cnt) { END_Texture = _Cnt; }
	_bool	END_Texture = false;

	_bool	Get_ENDLoading() { return END_Loading; }
	void	Set_ENDLoading(_bool _Cnt) { END_Loading = _Cnt; }
	_bool	END_Loading = false;



public:
	virtual void	Free() override;
};

END