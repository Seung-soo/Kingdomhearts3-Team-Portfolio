#pragma once

#include "Base.h"
#include "Renderer.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider.h"
#include "StateMachine.h"
#include "Navigation.h"
#include "Status.h"
#include "MonsterCooler.h"

// VIBuffer
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Axis.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Triangle.h"
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	MAKE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT			Reserve_Container(_uint iNumLevels);
	HRESULT			Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	HRESULT			Rmv_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

	CComponent*		Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

	HRESULT			Clear_LevelComponent(_uint iLevelIndex);
	void			Clear_All_Components(); // LEVEL_STATIC 을 제외한 모든 레벨의 컴포넌트 원본을 지움

public:
	CComponent*		Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);

private:
	map<const _tchar*, CComponent*>*		m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>	PROTOTYPES;

private:
	_uint			m_iNumLevels = 0;

public:
	virtual void	Free() override;
};

END