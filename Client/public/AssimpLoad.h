#pragma once

#include "Client_Defines.h"
#include "Model.h"

class CAssimpLoad
{
public:
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
public:


public:
	CAssimpLoad();
	~CAssimpLoad();

public:
	HRESULT	NonAnim_NativeConstruct(_tchar* pSearchPath);
	HRESULT Anim_NativeConstruct(_tchar* pSearchPath);

private:
	const aiScene*		m_pScene = nullptr;
	Importer			m_Importer;

	ASSIMPDESC*			m_AssimpDesc = nullptr;
	_uint				m_iNodeIndex = 0;
private:
	HRESULT	Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNode(aiNode * pNode, _int iParentIndex, _uint iDepth);
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Animation();
};

