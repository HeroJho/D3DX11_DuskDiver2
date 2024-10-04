#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		float		fTime;
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;		
	}KEYFRAME;

	// ���� ������ ��Ƴ��� ���� ����ü.
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, 
			TYPE_END };
		TYPE			eType;

		XMFLOAT4		vDirection;

		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;

	typedef struct tagMaterialDesc
	{
		class CTexture*		pTexture[AI_TEXTURE_TYPE_MAX];
	}MATERIALDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;





	// D3DDECLUSAGE
	typedef struct tagVertex
	{
		XMFLOAT3		vPosition;
	}VTX;
	typedef struct ENGINE_DLL tagVertex_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTX_DECLARATION;


	// D3DDECLUSAGE
	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;		
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* �� ������ ������ �ִ� ���� �ε��� �װ�. */
		XMFLOAT4		vBlendWeight; /* ���⸣ �ְ� �ִ� �� ������ ���� ���� */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3        vNormal;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;


	typedef struct tagVertexColor
	{
		XMFLOAT3		vPosition;
		XMFLOAT4		vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertexColor_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCOL_DECLARATION;

	/* �ν��Ͻ����� �׷����� ������ �ν��Ͻ� ������ 
	���ÿ��������� ���¸� ǥ���ϱ����� ��������� �����Ѵ�. */
	typedef struct tagVertexInstance
	{		
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		/* ���� �׸����� �ߴ� ����(VTXTEX)�� �ش� ���� ���� ���Ÿ� ��ǥ���ϴ� 
		VTXINSTANCE�� ������ �Բ� ���̴��� ���޵Ǿ���Ѵ�. */
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE_DECLARATION;


	typedef struct tagVertexPoint
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexPoint_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINT_DECLARATION;

	typedef struct ENGINE_DLL tagVertexPointInstance_Declaration
	{
		/* ���� �׸����� �ߴ� ����(VTXTEX)�� �ش� ���� ���� ���Ÿ� ��ǥ���ϴ�
		VTXINSTANCE�� ������ �Բ� ���̴��� ���޵Ǿ���Ѵ�. */
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINTINSTANCE_DECLARATION;



	/* �ν��Ͻ����� �׷����� ������ �ν��Ͻ� ������
	���ÿ��������� ���¸� ǥ���ϱ����� ��������� �����Ѵ�. */
	typedef struct tagVertexModelInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
		float			fDisolveRatio;
	}VTXMODELINSTANCE;

	typedef struct ENGINE_DLL tagVertexModelInstance_Declaration
	{
		/* ���� �׸����� �ߴ� ����(VTXTEX)�� �ش� ���� ���� ���Ÿ� ��ǥ���ϴ�
		VTXINSTANCE�� ������ �Բ� ���̴��� ���޵Ǿ���Ѵ�. */
		static const unsigned int iNumElements = 11;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODELINSTANCE_DECLARATION;




	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		WINMODE eWinMode;

	}GRAPHICDESC;






	// ============================
	//		For. BinModelData
	// ============================

	typedef struct tagHeroHierarchyNode
	{

		char cName[MAX_PATH];
		char cParent[MAX_PATH];
		int  iDepth;
		XMFLOAT4X4 mTransform;

	}DATA_HERONODE;

	typedef struct tagHeroMaterial
	{

		char cNames[AI_TEXTURE_TYPE_MAX][MAX_PATH];

	}DATA_HEROMATERIAL;


	typedef struct tagHeroBone
	{
		char		cNames[MAX_PATH];
		XMFLOAT4X4	mOffsetTransform;
	}DATA_HEROBONE;

	typedef struct tagHeroMesh
	{
		char				cName[MAX_PATH];
		int					iMaterialIndex;

		int					NumVertices;
		VTXMODEL*			pNonAnimVertices;
		VTXANIMMODEL*		pAnimVertices;

		int					iNumPrimitives;
		FACEINDICES32*		pIndices;

		int					iNumBones;
		DATA_HEROBONE*		pBones;

	}DATA_HEROMETH;


	typedef struct tagHeroChannel
	{

		char				szName[MAX_PATH];
		int					iNumKeyFrames;
		KEYFRAME*			pKeyFrames;

	}DATA_HEROCHANNEL;
	typedef struct tagHeroAnim
	{

		bool				bLoop;
		char				szName[MAX_PATH];
		int					iNumChannels;
		float				fDuration;
		float				fTickPerSecond;
		DATA_HEROCHANNEL*	pHeroChannel;

		int					iNumTimeLimit;
		float*				pTimeLimits;

	}DATA_HEROANIM;

	typedef struct tagHeroScene
	{

		int iNodeCount;
		DATA_HERONODE* pHeroNodes;

		int iMaterialCount;
		DATA_HEROMATERIAL* pHeroMaterial;

		int iMeshCount;
		DATA_HEROMETH* pHeroMesh;

		int iNumAnimations;
		DATA_HEROANIM* pHeroAnim;

	}DATA_HEROSCENE;
	
	// ============================
	//		For. ParticleInstance
	// ============================

	typedef struct ENGINE_DLL tagParticleInstace
	{
		XMFLOAT4 vMinPos;
		XMFLOAT4 vMaxPos;
		XMFLOAT2 vMinSize;
		XMFLOAT2 vMaxSize;
		float fMinSpeed = 0.f;
		float fMaxSpeed = 0.f;
		float fMinLifeTime = 0.f;
		float fMaxLifeTime = 0.f;
		float fGravity = 0.f;
		XMFLOAT3 vDirAngle;
		XMFLOAT3 vRangeAngle1;
		XMFLOAT3 vRangeAngle2;
		int iNumInstance;	
		XMFLOAT2 vReleaseSpeed = { 0.f,0.f };
		bool bCharge = false;
		XMFLOAT4 vChargePos = { 0.f,1.f,0.f,1.f };
	
	}PTINSTANCEINFO;

	typedef struct tagParticleInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
		XMFLOAT2		vSize2;
	}PARTICLEINSTANCE;

	typedef struct ENGINE_DLL tagVertexParticleInstance_Declaration
	{
		/* ���� �׸����� �ߴ� ����(VTXTEX)�� �ش� ���� ���� ���Ÿ� ��ǥ���ϴ�
		VTXINSTANCE�� ������ �Բ� ���̴��� ���޵Ǿ���Ѵ�. */
		static const unsigned int iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}PARTICLEINSTANCE_DECLARATION;

}
