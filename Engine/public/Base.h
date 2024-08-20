#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;
public:
	unsigned long	AddRef();	// 레퍼런스 증가
	unsigned long	Release();	// 감소

private:
	unsigned long	m_dwRefCnt = 0;	// 레퍼런스 카운트
public:
	virtual void	Free() = 0;
};

END