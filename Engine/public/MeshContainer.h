#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMeshContainer final : public CVIBuffer
{
private:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;
public:
	_uint	Get_NumBones() const { return m_iNumBones; }	// 뼈 갯수 반환

	const char* Get_ContainerName() { return m_szName; }
	const _uint Get_ContainerIndex() { return m_iMeshIndex; }

public:
	virtual HRESULT			NativeConstruct_Prototype(MESHCONTAINERDESC* pMeshContainer, _uint iIndex);
	virtual HRESULT			NativeConstruct(void* pArg);
public:
	HRESULT					Ready_VertexIndexBuffer(CModel::MODELTYPE eMeshType, _fmatrix TransformMatrix);
	HRESULT					Add_HierarChyNode(_uint iNodeIndex);		// 하이어라키 컨테이너에 추가해주는 함수
	HRESULT					SetUp_Matrices(_float4x4* pBoneMatrices, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix);
	_float4x4*				Ready_SetUp_Matrices(_int iSize, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix);
private:
	HRESULT					Ready_NonAnimMeshContainer(vector<VTXMODEL>* pModelInfo, _fmatrix TransformMatrix);
	HRESULT					Ready_AnimMeshContainer(vector<VTXANIMMODEL>* pModelInfo, _bool b);
private:
	char					m_szName[MAX_PATH] = "";

	_uint					m_iMaterialIndex = 0;

	_uint					m_iNumBones = 0;		// 뼈 갯수

	_uint					m_iMeshIndex = 0;

	MESHCONTAINERDESC*		m_pMeshContainerDesc = nullptr;

	vector<_uint>			m_HierarchyNodeIndices;	// 현재 매쉬에 영향을 주는 노드 인덱스들이 저장된 컨테이너
public:
	static CMeshContainer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MESHCONTAINERDESC* pMeshContainer, _uint iIndex);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END