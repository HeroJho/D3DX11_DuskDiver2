#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ZRect final : public CVIBuffer
{
private:
	CVIBuffer_ZRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_ZRect(const CVIBuffer_ZRect& rhs);
	virtual ~CVIBuffer_ZRect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_ZRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END