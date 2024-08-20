#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSoundManager final : public CBase
{
	MAKE_SINGLETON(CSoundManager)
public:
	CSoundManager();
	virtual ~CSoundManager() = default;

public:
	void	Sound_Command(TCHAR* pSoundKey, _float _vol);
	void	Sound_ItemGet(TCHAR* pSoundKey, _float _vol);
	void	Sound_MoneyGet(TCHAR* pSoundKey, _float _vol);

private:
	_int	m_iSoundCommandCnt = 0;
	_int	m_iItemGetCnt = 0;
	_int	m_iMoneyGetCnt = 0;



public:
	virtual void	Free() override;
};

END