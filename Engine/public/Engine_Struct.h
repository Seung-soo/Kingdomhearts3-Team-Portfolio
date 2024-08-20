#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		_float3		vScale;
		_float4		vRotation;
		_float3		vPosition;
		_double		Time;		// 적용되어야 하는 시간
	}KEYFRAME;

	typedef struct tagMeshMaterialDesc
	{
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIALDESC;
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_POINTCHASE, TYPE_END };

		TYPE		eLightType = TYPE_END;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		XMFLOAT4*	pPosition = nullptr;
		bool*		pDead = nullptr;
		float		fRange = 0.f;
	}LIGHTDESC;

	typedef struct tagVertex_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct tagVertex_Animation_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertex_AnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;

	typedef struct tagVertex_Point
	{
		XMFLOAT3		vPosition;
		float			fPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Matrix_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT_MATRIX_DECLARATION;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
	}VTXMATRIX;

	typedef struct tagVertex_Position
	{
		XMFLOAT4 vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS_DECLARATION;

	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertex_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertex_Texture_Color
	{
		XMFLOAT3			vPosition;
		DWORD				dwColor;
		XMFLOAT2			vTexUV;
	}VTXTEX_COLOR;

	typedef struct tagVertex_CubeTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUV;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Cube_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;

	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Texture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORMALTEX_DECLARATION;

	typedef struct tagVertex_Normal_Multy_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT4		vSplatting;
	}VTXNORMULTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Multy_Texture_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORMALMULTEX_DECLARATION;

	typedef struct tagVertex_NonAnimModel_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXNONANIMMODELTEX;

	typedef struct ENGINE_DLL tagVertex_NonAnimModel_Texture_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNONANIMMODELTEX_DECLARATION;

	typedef struct tagVertex_Color
	{
		XMFLOAT3	vPosition;
		XMFLOAT4	vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertex_Color_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCOL_DECLARATION;

	typedef struct tagLineIndices32
	{
		unsigned long	_0, _1;
	}LINEINDICES32;


	typedef struct tagLineIndices16
	{
		unsigned short	_0, _1;
	}LINEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long	_0, _1, _2;
	}FACEINDICES32;


	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;
	}FACEINDICES16;


	// 어심프 로드 관련
	typedef struct tagHierarchyNodeDesc
	{
		_int		iParentIndex = -1;
		_int		iNodeIndex = 0;
		_uint		iDepth = 0;
		char		m_szName[MAX_PATH] = "";
		_float4x4	m_OffsetMatrix = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};
		_float4x4	m_TransformationMatrix;
	}HIERARCHYNODEDESC;

	typedef struct tagChannelDesc
	{
		char				szName[MAX_PATH] = "";
		_uint				iNumKeyFrames = 0;
		vector<KEYFRAME>	KeyFrames;
	}CHANNELDESC;

	typedef struct tagAnimationDesc
	{
		char				szName[MAX_PATH] = "";
		_double				Duration;
		_double				TicksPerSecond;
		_uint				iNumChannels;
		vector<CHANNELDESC>	Channels;
	}ANIMATIONDESC;

	typedef struct tagMeshContainerDesc
	{
		char					szMeshName[MAX_PATH] = "";
		_uint					iMeshIndex = 0;			// 매쉬 인덱스
		_uint					iNumBones = 0;			// 뼈 갯수
		_uint					iNumIndicesPerPrimitive = 0;
		_uint					iNumVertices = 0;
		_uint					iNumVertexBuffers = 0;
		_uint					iNumPrimitive;

		vector<VTXANIMMODEL>	AnimInfo;
		vector<VTXMODEL>		NonAnimInfo;
		vector<_uint>			HierarchyNodeIndices;
		vector<FACEINDICES32>	Indices;
	}MESHCONTAINERDESC;

	typedef struct tagMaterial
	{
		_tchar	wszTexturePath[AI_TEXTURE_TYPE_MAX][MAX_PATH];
	}MATERIAL;

	typedef struct tagAssimpDesc
	{
		_uint						iModelType = 0;
		_uint						iNumMaterials;
		_uint						iNumMeshContainers;
		_uint						iNumAnimations;
		_uint						iNumNodes = (_uint)HierarchyNodes.size();

		vector<MATERIAL>			Materials;
		vector<MESHCONTAINERDESC>	MeshContainers;
		vector<ANIMATIONDESC>		Animations;
		vector<HIERARCHYNODEDESC>	HierarchyNodes;
	}ASSIMPDESC;
}
