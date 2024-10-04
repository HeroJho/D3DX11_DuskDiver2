#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CEditor final : public CVIBuffer
{
public:
	typedef struct EditorDesc
	{
		VTXANIMMODEL* pVtxBuffer;
		_uint iVtxSize;

		FACEINDICES32* pIndiBuffer;
		_uint iIndiSize;
	}EDITORDESC;

private:
	CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor(const CEditor& rhs);
	virtual ~CEditor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


public:
	void Init_Buffers(class CModel* pModel);
	void Make_Buffer();

	void Update_Editor();

public:
	vector<EDITORDESC> m_VtxBuffers;
	vector<EDITORDESC> m_IndexBuffers;

public:
	static CEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END