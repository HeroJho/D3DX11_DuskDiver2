#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Instance final : public CVIBuffer
{
private:
	CVIBuffer_Particle_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& rhs);
	virtual ~CVIBuffer_Particle_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_float fLifeAcc, _float fGravityAcc, _float fChargeRange, _float fTimeDelta);

public:
	virtual HRESULT Render();

private:
	_float4 Get_Look(_float3 vDirAngle, _float3 vRangeAngle1, _float3 vRangeAngle2);
private:
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;
	PTINSTANCEINFO*			m_pPtInstanceInfo = nullptr;
	_float3*				m_vDirs;
	_float*					m_fSpeeds;
	_float*					m_fLifeTimes;

public:
	static CVIBuffer_Particle_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END