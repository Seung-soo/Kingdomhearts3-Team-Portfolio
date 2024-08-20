#include "stdafx.h"
#include "..\public\AssimpLoad.h"
#include "HierarchyNode.h"
#include "GameInstance.h"


CAssimpLoad::CAssimpLoad()
{
}


CAssimpLoad::~CAssimpLoad()
{
	Safe_Delete(m_AssimpDesc);
	m_Importer.FreeScene();
}

HRESULT CAssimpLoad::NonAnim_NativeConstruct(_tchar* pSearchPath)
{
	// 파일 목록 불러오기
	_tfinddata64_t fd;
	_tchar	wszFirstSearch[MAX_PATH] = TEXT("");
	lstrcpy(wszFirstSearch, pSearchPath);
	lstrcat(wszFirstSearch, TEXT("/"));
	lstrcat(wszFirstSearch, TEXT("*.*"));

	__int64 handle = _tfindfirst64(wszFirstSearch, &fd);
	if (handle == -1)
		return S_OK;

	int iResult = 0;

	if (handle != -1)
	{
		while (iResult == 0)
		{
			if (!lstrcmp(fd.name, TEXT(".")) || !lstrcmp(fd.name, TEXT("..")))
			{
				iResult = _tfindnext64(handle, &fd);
				continue;
			}
			if (!(fd.attrib & _A_SUBDIR))
			{
				iResult = _tfindnext64(handle, &fd);
				continue;
			}
			_tchar	wszSearch[MAX_PATH] = TEXT("");
			lstrcpy(wszSearch, pSearchPath);
			lstrcat(wszSearch, TEXT("/"));
			lstrcat(wszSearch, fd.name);
			CHECK_FAILED(NonAnim_NativeConstruct(wszSearch));
			iResult = _tfindnext64(handle, &fd);
		}
		_findclose(handle);
	}

	lstrcpy(wszFirstSearch, pSearchPath);
	lstrcat(wszFirstSearch, TEXT("/*.fbx"));

	handle = _tfindfirst64(wszFirstSearch, &fd);
	if (handle == -1 || handle == 0)
		return S_OK;

	iResult = 0;

	char szFilename[MAX_PATH];



	while (iResult != -1)
	{
		m_AssimpDesc = new ASSIMPDESC;

		m_AssimpDesc->iModelType = TYPE_NONANIM;
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		m_iNodeIndex = 0;

		char szDirectoryPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_UTF8, 0, pSearchPath, -1, szDirectoryPath, sizeof(szDirectoryPath), NULL, NULL);
		char szFullPath[MAX_PATH] = "";

		sprintf_s(szFilename, szFilename);
		strcat_s(szFullPath, szDirectoryPath);
		strcat_s(szFullPath, "/");
		strcat_s(szFullPath, szFilename);

		_uint	iFrag = 0;

		iFrag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

		m_pScene = m_Importer.ReadFile(szFullPath, iFrag);

		CHECK_NULL(m_pScene);

		char szMaterialPath[MAX_PATH] = "";

		strcpy_s(szMaterialPath, szDirectoryPath);

		CHECK_FAILED(Ready_Materials(szMaterialPath));

		CHECK_FAILED(Ready_HierarchyNode(m_pScene->mRootNode, -1, 0));

		sort(m_AssimpDesc->HierarchyNodes.begin(), m_AssimpDesc->HierarchyNodes.end(), [](HIERARCHYNODEDESC pSour, HIERARCHYNODEDESC pDest)
			{
				return pSour.iDepth < pDest.iDepth;
			});

		m_AssimpDesc->iNumNodes = (_uint)m_AssimpDesc->HierarchyNodes.size();

		CHECK_FAILED(Ready_MeshContainers());

		CHECK_FAILED(Ready_Animation());

		char	szMeshDataName[MAX_PATH] = "";
		char*	szMeshFileName = nullptr;
		char*	szContext = nullptr;

		strcat_s(szMeshDataName, szDirectoryPath);
		strcat_s(szMeshDataName, "/");
		szMeshFileName = strtok_s(szFilename, ".", &szContext);
		strcat_s(szMeshDataName, szMeshFileName);
		strcat_s(szMeshDataName, ".dat");

		_tchar	wszMeshDataName[MAX_PATH] = TEXT("");

		MultiByteToWideChar(CP_ACP, 0, szMeshDataName, (_int)strlen(szMeshDataName), wszMeshDataName, MAX_PATH);


		_ulong			dwByte = 0;
		HANDLE			hFile = CreateFile(wszMeshDataName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		WriteFile(hFile, m_AssimpDesc, sizeof(_uint) * 5, &dwByte, nullptr);

		m_AssimpDesc->Materials.resize(m_AssimpDesc->iNumMaterials);
		WriteFile(hFile, &m_AssimpDesc->Materials[0], sizeof(MATERIAL) * m_AssimpDesc->iNumMaterials, &dwByte, nullptr);

		m_AssimpDesc->MeshContainers.resize(m_AssimpDesc->iNumMeshContainers);
		for (_uint i = 0; i < m_AssimpDesc->iNumMeshContainers; i++)
		{
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i], sizeof(_uint) * 6 + sizeof(char) * MAX_PATH, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].NonAnimInfo.resize(m_AssimpDesc->MeshContainers[i].iNumVertices);
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].NonAnimInfo[0], sizeof(VTXMODEL) * m_AssimpDesc->MeshContainers[i].iNumVertices, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices.resize(m_AssimpDesc->MeshContainers[i].iNumBones);
			if (0 < m_AssimpDesc->MeshContainers[i].iNumBones)
				WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices[0], sizeof(_uint) * m_AssimpDesc->MeshContainers[i].iNumBones, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].Indices.resize(m_AssimpDesc->MeshContainers[i].iNumPrimitive);
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].Indices[0], sizeof(FACEINDICES32) * m_AssimpDesc->MeshContainers[i].iNumPrimitive, &dwByte, nullptr);
		}

		m_AssimpDesc->HierarchyNodes.resize(m_AssimpDesc->iNumNodes);
		if (0 < m_AssimpDesc->iNumNodes)
			WriteFile(hFile, &m_AssimpDesc->HierarchyNodes[0], sizeof(HIERARCHYNODEDESC) * m_AssimpDesc->iNumNodes, &dwByte, nullptr);

		//_uint iModelType = m_AssimpDesc->iModelType;

		//WriteFile(hFile, &iModelType, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumMaterials, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}
		//for (_uint i = 0; i < m_AssimpDesc->iNumMaterials; i++)
		//{
		//	for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->Materials[i].wszTexturePath[j], sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumNodes, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//for (_uint i = 0; i < m_AssimpDesc->iNumNodes; i++)
		//{
		//	WriteFile(hFile, &m_AssimpDesc->HierarchyNodes[i], sizeof(HIERARCHYNODEDESC), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumMeshContainers, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//for (_uint i = 0; i < m_AssimpDesc->iNumMeshContainers; i++)
		//{
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumVertices, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumVertices; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].AnimInfo[j], sizeof(VTXANIMMODEL), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumPrimitive; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].Indices[j], sizeof(FACEINDICES32), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumBones, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumBones; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices[j], sizeof(_uint), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iMeshIndex, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumIndicesPerPrimitive, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumVertexBuffers, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].szMeshName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}
		//}
		Safe_Delete(m_AssimpDesc);

		CloseHandle(hFile);

		iResult = _tfindnext64(handle, &fd);
	}


	MSGBOX("저장완료!");

	_findclose(handle);

	return S_OK;
}

HRESULT CAssimpLoad::Anim_NativeConstruct(_tchar* pSearchPath)
{
	// 파일 목록 불러오기
	_tfinddata64_t fd;
	_tchar	wszFirstSearch[MAX_PATH] = TEXT("");
	lstrcpy(wszFirstSearch, pSearchPath);
	lstrcat(wszFirstSearch, TEXT("/"));
	lstrcat(wszFirstSearch, TEXT("*.*"));

	__int64 handle = _tfindfirst64(wszFirstSearch, &fd);
	if (handle == -1)
		return S_OK;

	int iResult = 0;

	if (handle != -1)
	{
		while (iResult == 0)
		{
			if (!lstrcmp(fd.name, TEXT(".")) || !lstrcmp(fd.name, TEXT("..")))
			{
				iResult = _tfindnext64(handle, &fd);
				continue;
			}
			if (!(fd.attrib & _A_SUBDIR))
			{
				iResult = _tfindnext64(handle, &fd);
				continue;
			}
			_tchar	wszSearch[MAX_PATH] = TEXT("");
			lstrcpy(wszSearch, pSearchPath);
			lstrcat(wszSearch, TEXT("/"));
			lstrcat(wszSearch, fd.name);
			CHECK_FAILED(Anim_NativeConstruct(wszSearch));
			iResult = _tfindnext64(handle, &fd);
		}
		_findclose(handle);
	}

	lstrcpy(wszFirstSearch, pSearchPath);
	lstrcat(wszFirstSearch, TEXT("/*.fbx"));

	handle = _tfindfirst64(wszFirstSearch, &fd);
	if (handle == -1 || handle == 0)
		return S_OK;

	iResult = 0;

	char szFilename[MAX_PATH];




	while (iResult != -1)
	{
		m_AssimpDesc = new ASSIMPDESC;

		m_AssimpDesc->iModelType = TYPE_ANIM;
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		m_iNodeIndex = 0;

		char szDirectoryPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_UTF8, 0, pSearchPath, -1, szDirectoryPath, sizeof(szDirectoryPath), NULL, NULL);
		char szFullPath[MAX_PATH] = "";

		sprintf_s(szFilename, szFilename);
		strcat_s(szFullPath, szDirectoryPath);
		strcat_s(szFullPath, "/");
		strcat_s(szFullPath, szFilename);

		_uint	iFrag = 0;

		iFrag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

		m_pScene = m_Importer.ReadFile(szFullPath, iFrag);

		CHECK_NULL(m_pScene);

		char szMaterialPath[MAX_PATH] = "";

		strcpy_s(szMaterialPath, szDirectoryPath);

		CHECK_FAILED(Ready_Materials(szMaterialPath));

		CHECK_FAILED(Ready_HierarchyNode(m_pScene->mRootNode, -1, 0));

		sort(m_AssimpDesc->HierarchyNodes.begin(), m_AssimpDesc->HierarchyNodes.end(), [](HIERARCHYNODEDESC pSour, HIERARCHYNODEDESC pDest)
			{
				return pSour.iDepth < pDest.iDepth;
			});

		m_AssimpDesc->iNumNodes = (_uint)m_AssimpDesc->HierarchyNodes.size();

		CHECK_FAILED(Ready_MeshContainers());

		CHECK_FAILED(Ready_Animation());

		char	szMeshDataName[MAX_PATH] = "";
		char*	szMeshFileName = nullptr;
		char*	szContext = nullptr;

		strcat_s(szMeshDataName, szDirectoryPath);
		strcat_s(szMeshDataName, "/");
		szMeshFileName = strtok_s(szFilename, ".", &szContext);
		strcat_s(szMeshDataName, szMeshFileName);
		strcat_s(szMeshDataName, ".dat");

		_tchar	wszMeshDataName[MAX_PATH] = TEXT("");

		MultiByteToWideChar(CP_ACP, 0, szMeshDataName, (_int)strlen(szMeshDataName), wszMeshDataName, MAX_PATH);


		_ulong			dwByte = 0;
		HANDLE			hFile = CreateFile(wszMeshDataName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		WriteFile(hFile, m_AssimpDesc, sizeof(_uint) * 5, &dwByte, nullptr);

		m_AssimpDesc->Materials.resize(m_AssimpDesc->iNumMaterials);
		WriteFile(hFile, &m_AssimpDesc->Materials[0], sizeof(MATERIAL) * m_AssimpDesc->iNumMaterials, &dwByte, nullptr);

		m_AssimpDesc->MeshContainers.resize(m_AssimpDesc->iNumMeshContainers);
		for (_uint i = 0; i < m_AssimpDesc->iNumMeshContainers; i++)
		{
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i], sizeof(_uint) * 6 + sizeof(char) * MAX_PATH, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].AnimInfo.resize(m_AssimpDesc->MeshContainers[i].iNumVertices);
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].AnimInfo[0], sizeof(VTXANIMMODEL) * m_AssimpDesc->MeshContainers[i].iNumVertices, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices.resize(m_AssimpDesc->MeshContainers[i].iNumBones);
			if (0 < m_AssimpDesc->MeshContainers[i].iNumBones)
				WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices[0], sizeof(_uint) * m_AssimpDesc->MeshContainers[i].iNumBones, &dwByte, nullptr);

			m_AssimpDesc->MeshContainers[i].Indices.resize(m_AssimpDesc->MeshContainers[i].iNumPrimitive);
			WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].Indices[0], sizeof(FACEINDICES32) * m_AssimpDesc->MeshContainers[i].iNumPrimitive, &dwByte, nullptr);
		}

		if (TYPE_ANIM == m_AssimpDesc->iModelType)
		{
			// 애니메이션을 읽어온다.
			m_AssimpDesc->Animations.resize(m_AssimpDesc->iNumAnimations);
			for (_uint i = 0; i < m_AssimpDesc->iNumAnimations; i++)
			{
				WriteFile(hFile, &m_AssimpDesc->Animations[i], sizeof(_uint) * 2 + sizeof(char) * MAX_PATH + sizeof(_double) * 2, &dwByte, nullptr);
				ANIMATIONDESC tAnimation = {};
				memcpy(&tAnimation, &m_AssimpDesc->Animations[i], sizeof(_uint) * 2 + sizeof(char) * MAX_PATH + sizeof(_double) * 2);

				m_AssimpDesc->Animations[i].Channels.resize(m_AssimpDesc->Animations[i].iNumChannels);
				for (_uint j = 0; j < m_AssimpDesc->Animations[i].iNumChannels; j++)
				{
					WriteFile(hFile, &m_AssimpDesc->Animations[i].Channels[j], sizeof(_uint) + sizeof(char) * MAX_PATH, &dwByte, nullptr);

					m_AssimpDesc->Animations[i].Channels[j].KeyFrames.resize(m_AssimpDesc->Animations[i].Channels[j].iNumKeyFrames);

					WriteFile(hFile, &m_AssimpDesc->Animations[i].Channels[j].KeyFrames[0], sizeof(KEYFRAME) * m_AssimpDesc->Animations[i].Channels[j].iNumKeyFrames, &dwByte, nullptr);
				}
			}
		}

		m_AssimpDesc->HierarchyNodes.resize(m_AssimpDesc->iNumNodes);
		if (0 < m_AssimpDesc->iNumNodes)
			WriteFile(hFile, &m_AssimpDesc->HierarchyNodes[0], sizeof(HIERARCHYNODEDESC) * m_AssimpDesc->iNumNodes, &dwByte, nullptr);

		//_uint iModelType = m_AssimpDesc->iModelType;

		//WriteFile(hFile, &iModelType, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumMaterials, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}
		//for (_uint i = 0; i < m_AssimpDesc->iNumMaterials; i++)
		//{
		//	for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->Materials[i].wszTexturePath[j], sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumNodes, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//for (_uint i = 0; i < m_AssimpDesc->iNumNodes; i++)
		//{
		//	WriteFile(hFile, &m_AssimpDesc->HierarchyNodes[i], sizeof(HIERARCHYNODEDESC), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumMeshContainers, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//for (_uint i = 0; i < m_AssimpDesc->iNumMeshContainers; i++)
		//{
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumVertices, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumVertices; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].AnimInfo[j], sizeof(VTXANIMMODEL), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumPrimitive; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].Indices[j], sizeof(FACEINDICES32), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumBones, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumBones; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices[j], sizeof(_uint), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}
		//	}

		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iMeshIndex, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumIndicesPerPrimitive, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].iNumVertexBuffers, sizeof(_uint), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->MeshContainers[i].szMeshName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}
		//}

		//WriteFile(hFile, &m_AssimpDesc->iNumAnimations, sizeof(_uint), &dwByte, nullptr);
		//if (0 == dwByte)
		//{
		//	__debugbreak();
		//	CloseHandle(hFile);
		//	return E_FAIL;
		//}

		//for (_uint i = 0; i < m_AssimpDesc->iNumAnimations; i++)
		//{
		//	WriteFile(hFile, &m_AssimpDesc->Animations[i].Duration, sizeof(_double), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->Animations[i].TicksPerSecond, sizeof(_double), &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->Animations[i].szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//	WriteFile(hFile, &m_AssimpDesc->Animations[i].iNumChannels, sizeof(_uint), &dwByte, nullptr);
		//	if (0 == dwByte)
		//	{
		//		__debugbreak();
		//		CloseHandle(hFile);
		//		return E_FAIL;
		//	}

		//	for (_uint j = 0; j < m_AssimpDesc->Animations[i].iNumChannels; j++)
		//	{
		//		WriteFile(hFile, &m_AssimpDesc->Animations[i].Channels[j].szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		//		WriteFile(hFile, &m_AssimpDesc->Animations[i].Channels[j].iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
		//		if (0 == dwByte)
		//		{
		//			__debugbreak();
		//			CloseHandle(hFile);
		//			return E_FAIL;
		//		}

		//		for (_uint k = 0; k < m_AssimpDesc->Animations[i].Channels[j].iNumKeyFrames; k++)
		//		{
		//			WriteFile(hFile, &m_AssimpDesc->Animations[i].Channels[j].KeyFrames[k], sizeof(KEYFRAME), &dwByte, nullptr);
		//			if (0 == dwByte)
		//			{
		//				__debugbreak();
		//				CloseHandle(hFile);
		//				return E_FAIL;
		//			}
		//		}

		//	}
		//}

		Safe_Delete(m_AssimpDesc);

		CloseHandle(hFile);


		iResult = _tfindnext64(handle, &fd);
	}

	Safe_Delete(m_AssimpDesc);

	MSGBOX("저장완료!");

	_findclose(handle);

	return S_OK;
}

HRESULT CAssimpLoad::Ready_Materials(const char* pModelFilePath)
{
	m_AssimpDesc->iNumMaterials = m_pScene->mNumMaterials;

	m_AssimpDesc->Materials.resize(m_AssimpDesc->iNumMaterials);

	for (_uint i = 0; i < m_AssimpDesc->iNumMaterials; i++)
	{
		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	TexturePath;

			if (FAILED(m_pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &TexturePath)))
			{
				_tchar		wszTextureFullPath[MAX_PATH] = TEXT("");
				lstrcpy(m_AssimpDesc->Materials[i].wszTexturePath[j], wszTextureFullPath);
				continue;
			}

			char	szTextureFullPath[MAX_PATH] = "";
			char	szTextureFileName[MAX_PATH] = "";
			char	szTextureExt[MAX_PATH] = "";

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szTextureExt, MAX_PATH);

			string str(pModelFilePath);

			string find_str;
			if (TYPE_NONANIM == m_AssimpDesc->iModelType)
				find_str = "NonAnimMeshes";
			else
				find_str = "AnimMeshes";
			string replace_str = "Models";

			str.replace(str.find(find_str), find_str.length(), replace_str);
			
			strcpy_s(szTextureFullPath, str.c_str());

			strcat_s(szTextureFullPath, "/");
			strcat_s(szTextureFullPath, szTextureFileName);
			strcat_s(szTextureFullPath, szTextureExt);

			_tchar		wszTextureFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTextureFullPath, (_int)strlen(szTextureFullPath), wszTextureFullPath, MAX_PATH);

			lstrcpy(m_AssimpDesc->Materials[i].wszTexturePath[j], wszTextureFullPath);
		}
	}

	return S_OK;
}

HRESULT CAssimpLoad::Ready_HierarchyNode(aiNode* pNode, _int iParentIndex, _uint iDepth)
{
	CHECK_NULL(pNode)

		_float4x4	TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	HIERARCHYNODEDESC	tHierarchyNodeDesc;
	tHierarchyNodeDesc.iDepth = iDepth;
	tHierarchyNodeDesc.iNodeIndex = m_iNodeIndex;
	tHierarchyNodeDesc.iParentIndex = iParentIndex;
	strcpy_s(tHierarchyNodeDesc.m_szName, pNode->mName.data);
	memcpy(&tHierarchyNodeDesc.m_TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&tHierarchyNodeDesc.m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&tHierarchyNodeDesc.m_TransformationMatrix)));	// 전치해서 저장해준다.

	m_AssimpDesc->HierarchyNodes.push_back(tHierarchyNodeDesc);
	m_iNodeIndex++;

	for (_uint i = 0; i < pNode->mNumChildren; i++)
	{
		Ready_HierarchyNode(pNode->mChildren[i], tHierarchyNodeDesc.iNodeIndex, iDepth + 1);
	}

	return S_OK;
}

HRESULT CAssimpLoad::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
	{
		MSGBOX("m_pScene is nullptr in CModel::Ready_MeshContainers");
		return E_FAIL;
	}
	m_AssimpDesc->iNumMeshContainers = m_pScene->mNumMeshes;
	m_AssimpDesc->MeshContainers.resize(m_AssimpDesc->iNumMeshContainers);

	for (_uint i = 0; i < m_AssimpDesc->iNumMeshContainers; i++)
	{
		strcpy_s(m_AssimpDesc->MeshContainers[i].szMeshName, m_pScene->mMeshes[i]->mName.data);
		m_AssimpDesc->MeshContainers[i].iMeshIndex = m_pScene->mMeshes[i]->mMaterialIndex;	// 매쉬 인덱스
		m_AssimpDesc->MeshContainers[i].iNumBones = m_pScene->mMeshes[i]->mNumBones;			// 뼈 갯수

		m_AssimpDesc->MeshContainers[i].iNumIndicesPerPrimitive = 3;
		m_AssimpDesc->MeshContainers[i].iNumVertices = m_pScene->mMeshes[i]->mNumVertices;
		m_AssimpDesc->MeshContainers[i].iNumVertexBuffers = 1;

		m_AssimpDesc->MeshContainers[i].AnimInfo.reserve(m_AssimpDesc->MeshContainers[i].iNumVertices);

		for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumVertices; j++)
		{
			if (TYPE_ANIM == m_AssimpDesc->iModelType)
			{
				VTXANIMMODEL	tVtxAnimModel;
				ZeroMemory(&tVtxAnimModel, sizeof(VTXANIMMODEL));
				memcpy(&tVtxAnimModel.vPosition, &m_pScene->mMeshes[i]->mVertices[j], sizeof(_float3));
				memcpy(&tVtxAnimModel.vNormal, &m_pScene->mMeshes[i]->mNormals[j], sizeof(_float3));
				memcpy(&tVtxAnimModel.vTexUV, &m_pScene->mMeshes[i]->mTextureCoords[0][j], sizeof(_float2));
				memcpy(&tVtxAnimModel.vTangent, &m_pScene->mMeshes[i]->mTangents[j], sizeof(_float3));
				m_AssimpDesc->MeshContainers[i].AnimInfo.push_back(tVtxAnimModel);
			}
			else
			{
				VTXMODEL	tVtxModel;
				ZeroMemory(&tVtxModel, sizeof(VTXMODEL));
				memcpy(&tVtxModel.vPosition, &m_pScene->mMeshes[i]->mVertices[j], sizeof(_float3));
				memcpy(&tVtxModel.vNormal, &m_pScene->mMeshes[i]->mNormals[j], sizeof(_float3));
				memcpy(&tVtxModel.vTexUV, &m_pScene->mMeshes[i]->mTextureCoords[0][j], sizeof(_float2));
				memcpy(&tVtxModel.vTangent, &m_pScene->mMeshes[i]->mTangents[j], sizeof(_float3));
				m_AssimpDesc->MeshContainers[i].NonAnimInfo.push_back(tVtxModel);
			}
		}
		m_AssimpDesc->MeshContainers[i].iNumPrimitive = m_pScene->mMeshes[i]->mNumFaces;
		m_AssimpDesc->MeshContainers[i].Indices.resize(m_AssimpDesc->MeshContainers[i].iNumPrimitive);

		for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumPrimitive; j++)
		{
			m_AssimpDesc->MeshContainers[i].Indices[j]._0 = m_pScene->mMeshes[i]->mFaces[j].mIndices[0];
			m_AssimpDesc->MeshContainers[i].Indices[j]._1 = m_pScene->mMeshes[i]->mFaces[j].mIndices[1];
			m_AssimpDesc->MeshContainers[i].Indices[j]._2 = m_pScene->mMeshes[i]->mFaces[j].mIndices[2];
		}

		for (_uint j = 0; j < m_AssimpDesc->MeshContainers[i].iNumBones; j++)	// 해당 매쉬의 뼈의 갯수만큼 루프
		{
			aiBone* pAIBone = m_pScene->mMeshes[i]->mBones[j];
			for (_uint k = 0; k < pAIBone->mNumWeights; k++)
			{
				if (0.0f == m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.x)
				{
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendIndex.x = j;
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.x = pAIBone->mWeights[k].mWeight;
				}

				else if (0.0f == m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.y)
				{
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendIndex.y = j;
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.y = pAIBone->mWeights[k].mWeight;

				}

				else if (0.0f == m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.z)
				{
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendIndex.z = j;
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.z = pAIBone->mWeights[k].mWeight;

				}

				else if (0.0f == m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.w)
				{
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendIndex.w = j;
					m_AssimpDesc->MeshContainers[i].AnimInfo[pAIBone->mWeights[k].mVertexId].vBlendWeight.w = pAIBone->mWeights[k].mWeight;

				}
			}
			// 해당 뼈의 이름으로 노드의 객체와 인덱스값을 받아온다.
			_uint	iIndex = 0;

			m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices.resize(m_AssimpDesc->MeshContainers[i].iNumBones);

			for (auto& HierarchyNode : m_AssimpDesc->HierarchyNodes)		// 모든 하이어라키 노드를 순회
			{
				if (!strcmp(HierarchyNode.m_szName, m_pScene->mMeshes[i]->mBones[j]->mName.data))	// 해당 노드의 이름과 찾는 이름이 같으면
				{
					m_AssimpDesc->MeshContainers[i].HierarchyNodeIndices[j] = iIndex;
					memcpy(&HierarchyNode.m_OffsetMatrix, &m_pScene->mMeshes[i]->mBones[j]->mOffsetMatrix, sizeof(_float4x4));
					break;
				}
				iIndex++;
			}
		}
	}

	return S_OK;
}

HRESULT CAssimpLoad::Ready_Animation()
{
	m_AssimpDesc->iNumAnimations = m_pScene->mNumAnimations;
	m_AssimpDesc->Animations.reserve(m_AssimpDesc->iNumAnimations);

	for (_uint i = 0; i < m_AssimpDesc->iNumAnimations; i++)	// 애니메이션 갯수만큼 루프
	{
		aiAnimation* pAIAnimation = m_pScene->mAnimations[i];	// i번째 인덱스 애니메이션을 받아옴

		ANIMATIONDESC	tAnimationDesc;
		strcpy_s(tAnimationDesc.szName, pAIAnimation->mName.data);
		tAnimationDesc.Duration = pAIAnimation->mDuration;
		tAnimationDesc.TicksPerSecond = pAIAnimation->mTicksPerSecond;
		tAnimationDesc.iNumChannels = pAIAnimation->mNumChannels;

		tAnimationDesc.Channels.reserve(tAnimationDesc.iNumChannels);

		for (_uint j = 0; j < pAIAnimation->mNumChannels; j++)	// 뼈의 갯수만큼 루프
		{
			aiNodeAnim* pAIChannel = pAIAnimation->mChannels[j];// j번째 인덱스에 있는 뼈를 받아온다

			CHANNELDESC	tChannelDesc;
			strcpy_s(tChannelDesc.szName, pAIChannel->mNodeName.data);

			// 키 프레임의 총 갯수는 스케일, 로테이션, 포지션중에 가장 많은 프레임 수이다.
			// 보통 로테이션, 포지션은 거의 가지고 있고 스케일은 맨 앞에 하나정도 올 때도 많다.
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			tChannelDesc.KeyFrames.reserve(iNumKeyFrames);
			tChannelDesc.iNumKeyFrames = iNumKeyFrames;

			_float3	vScale;
			_float4 vRotation;
			_float3 vPosition;
			_double Time;

			for (_uint k = 0; k < iNumKeyFrames; k++)	// 키 프레임 수만큼 루프
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;	// 키 프레임 정보 구조체 동적 할당
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pAIChannel->mNumScalingKeys > k)	// k번째에 스케일 키 프레임이 존재하면(스케일 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));	// 스케일 키 프레임 값 채움
					Time = pAIChannel->mScalingKeys[k].mTime;	// 이 키 프레임이 재생되어야할 시간
				}

				if (pAIChannel->mNumRotationKeys > k)	// k번째에 로테이션 키 프레임이 존재하면(로테이션 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					vRotation.x = pAIChannel->mRotationKeys[k].mValue.x;	// 로테이션은 쿼터니언 단위이다.
					vRotation.y = pAIChannel->mRotationKeys[k].mValue.y;	// 쿼터니언은 wxyz순이기 때문에 memcpy를 하면 잘못 들어간다.
					vRotation.z = pAIChannel->mRotationKeys[k].mValue.z;
					vRotation.w = pAIChannel->mRotationKeys[k].mValue.w;
					Time = pAIChannel->mRotationKeys[k].mTime;				// 이 키 프레임이 재생되어야할 시간
				}

				if (pAIChannel->mNumPositionKeys > k)	// k번째에 포지션 키 프레임이 존재하면(포지션 키 프레임 갯수보다 현재 인덱스가 작으면)
				{
					memcpy(&vPosition, &pAIChannel->mPositionKeys[k].mValue, sizeof(_float3));	// 포지션 키 프레임 값 채움
					Time = pAIChannel->mPositionKeys[k].mTime;	// 이 키 프레임이 재생되어야할 시간
				}

				pKeyFrame->vScale = vScale;				// 키 프레임 구조체에 값 채워줌
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;
				pKeyFrame->Time = Time;

				tChannelDesc.KeyFrames.push_back(*pKeyFrame);		// 키 프레임 컨테이너에 추가
				memcpy(&tChannelDesc.KeyFrames.back(), pKeyFrame, sizeof(KEYFRAME));

				Safe_Delete(pKeyFrame);
			}
			tAnimationDesc.Channels.push_back(tChannelDesc);
		}
		m_AssimpDesc->Animations.push_back(tAnimationDesc);				// 애니메이션 컨테이너에 추가
	}

	return S_OK;
}

