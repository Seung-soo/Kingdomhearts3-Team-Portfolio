#pragma once

#include "Base.h"
#include "Level.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	MAKE_SINGLETON(CLevel_Manager)
public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;
public:
	_uint			Get_LevelIndex() { return m_iCurrentLevelIndex; }
public:
	HRESULT			OpenLevel(_uint iNextLevelIndex, CLevel* pNextLevel);
	_int			Tick(_double dTimeDelta);
	_int			LateTick(_double dTimeDelta);
	HRESULT			Render();
private:
	_uint			m_iCurrentLevelIndex = 0;
	CLevel*			m_pCurrentLevel = nullptr;
public:
	virtual void	Free() override;
};

END