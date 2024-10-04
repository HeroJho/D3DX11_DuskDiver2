#include "..\Public\VIBuffer_Particle_Instance.h"
#include "GameInstance.h"
CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance & rhs)
	: CVIBuffer(rhs)	
{
	
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Initialize(void * pArg)
{
	m_pPtInstanceInfo = (*(PTINSTANCEINFO**)pArg);

#pragma region VERTEXBUFFER

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOINT*		pVertices = new VTXPOINT;

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices->vSize = _float2(1.f, 1.f);
	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = m_pPtInstanceInfo->iNumInstance;
	m_iIndexSizeofPrimitive = sizeof(_ushort);
	m_iNumIndicesofPrimitive = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitives);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


#pragma region INSTANCEBUFFER
	m_iInstanceStride = sizeof(PARTICLEINSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_pPtInstanceInfo->iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	PARTICLEINSTANCE*		pInstanceParticle = new PARTICLEINSTANCE[m_pPtInstanceInfo->iNumInstance];
	ZeroMemory(pInstanceParticle, sizeof(PARTICLEINSTANCE) * m_pPtInstanceInfo->iNumInstance);

	m_vDirs = new _float3[m_pPtInstanceInfo->iNumInstance];

	for (int i = 0; i < m_pPtInstanceInfo->iNumInstance; ++i)
	{
		pInstanceParticle[i].vPosition = _float4{ CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->vMinPos.x, m_pPtInstanceInfo->vMaxPos.x),
			CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->vMinPos.y, m_pPtInstanceInfo->vMaxPos.y),
			CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->vMinPos.z, m_pPtInstanceInfo->vMaxPos.z), 1.f };
		pInstanceParticle[i].vSize2 = _float2{ CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->vMinSize.x, m_pPtInstanceInfo->vMaxSize.x),
			CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->vMinSize.y, m_pPtInstanceInfo->vMaxSize.y) };

		if (!m_pPtInstanceInfo->bCharge)
		{
			pInstanceParticle[i].vLook = Get_Look(m_pPtInstanceInfo->vDirAngle, m_pPtInstanceInfo->vRangeAngle1, m_pPtInstanceInfo->vRangeAngle2);
			m_vDirs[i] = { pInstanceParticle[i].vLook.x, pInstanceParticle[i].vLook.y, pInstanceParticle[i].vLook.z };
			if (pInstanceParticle[i].vLook.x == 0.f && pInstanceParticle[i].vLook.y == 1.f && pInstanceParticle[i].vLook.z == 0.f)
			{
				pInstanceParticle[i].vRight = { 1.f,0.f,0.f,0.f };
				pInstanceParticle[i].vUp = { 0.f,0.f,-1.f,0.f };
			}
			else
			{
				XMStoreFloat4(&pInstanceParticle[i].vRight, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&pInstanceParticle[i].vLook)));
				XMStoreFloat4(&pInstanceParticle[i].vUp, XMVector3Cross(XMLoadFloat4(&pInstanceParticle[i].vLook), XMLoadFloat4(&pInstanceParticle[i].vRight)));
			}
		}
		else
		{
			XMStoreFloat4(&pInstanceParticle[i].vLook, XMVector4Normalize(XMLoadFloat4(&m_pPtInstanceInfo->vChargePos) - XMLoadFloat4(&pInstanceParticle[i].vPosition)));
			XMVectorSetW(XMLoadFloat4(&pInstanceParticle[i].vLook), 0.f);
			m_vDirs[i] = { pInstanceParticle[i].vLook.x, pInstanceParticle[i].vLook.y, pInstanceParticle[i].vLook.z };
			if (pInstanceParticle[i].vLook.x == 0.f && pInstanceParticle[i].vLook.y == 1.f && pInstanceParticle[i].vLook.z == 0.f)
			{
				pInstanceParticle[i].vRight = { 1.f,0.f,0.f,0.f };
				pInstanceParticle[i].vUp = { 0.f,0.f,-1.f,0.f };
			}
			else
			{
				XMStoreFloat4(&pInstanceParticle[i].vRight, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&pInstanceParticle[i].vLook)));
				XMStoreFloat4(&pInstanceParticle[i].vUp, XMVector3Cross(XMLoadFloat4(&pInstanceParticle[i].vLook), XMLoadFloat4(&pInstanceParticle[i].vRight)));
			}
		}
		
		
	}
	
	
	m_fSpeeds = new _float[m_pPtInstanceInfo->iNumInstance];
	m_fLifeTimes = new _float[m_pPtInstanceInfo->iNumInstance];

	for (int i = 0; i < m_pPtInstanceInfo->iNumInstance; ++i)
	{
		m_fSpeeds[i] = CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->fMinSpeed, m_pPtInstanceInfo->fMaxSpeed);
		m_fLifeTimes[i] = CGameInstance::Get_Instance()->Get_FloatRandom(m_pPtInstanceInfo->fMinLifeTime, m_pPtInstanceInfo->fMaxLifeTime);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceParticle;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceParticle);

	

#pragma endregion

	return S_OK;
}

void CVIBuffer_Particle_Instance::Update(_float fLifeAcc, _float fGravityAcc, _float fChargeRange, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	if (!m_pPtInstanceInfo->bCharge)
	{
		for (int i = 0; i < m_pPtInstanceInfo->iNumInstance; ++i)
		{
			XMStoreFloat4(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition,
				XMLoadFloat4(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition) += XMVector3Normalize(XMLoadFloat3(&m_vDirs[i])) * (m_fSpeeds[i] * fTimeDelta));

			XMStoreFloat2(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2, XMLoadFloat2(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2)
				+ _vector{ m_pPtInstanceInfo->vReleaseSpeed.x, m_pPtInstanceInfo->vReleaseSpeed.y } *fTimeDelta);

			if (m_fLifeTimes[i] <= fLifeAcc)
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2 = _float2{ 0.f, 0.f };

			if (((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.x <= 0.f)
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.x = 0.f;

			if (((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.y <= 0.f)
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.y = 0.f;


			((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition.y -= fGravityAcc * fTimeDelta;
		}
	}
	else
	{
		for (int i = 0; i < m_pPtInstanceInfo->iNumInstance; ++i)
		{
			XMStoreFloat4(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition,
				XMLoadFloat4(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition) += XMVector3Normalize(XMLoadFloat3(&m_vDirs[i])) * m_fSpeeds[i] * fTimeDelta);

			if (m_fLifeTimes[i] <= fLifeAcc)
			{
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2 = _float2{ 0.f, 0.f };
			}
			else if (fChargeRange >= XMVector3Length(XMLoadFloat4(&((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition) - XMLoadFloat4(&m_pPtInstanceInfo->vChargePos)).m128_f32[0])
			{
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2 = _float2{ 0.f, 0.f };
			}

			else
			{
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.x += m_pPtInstanceInfo->vReleaseSpeed.x * fTimeDelta;
				((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vSize2.y += m_pPtInstanceInfo->vReleaseSpeed.y * fTimeDelta;
			}


			((PARTICLEINSTANCE*)MappedSubResource.pData)[i].vPosition.y -= fGravityAcc * fTimeDelta;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Particle_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance, 
	};

	_uint			iStrides[] = {
		m_iStride, 
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(1, m_pPtInstanceInfo->iNumInstance, 0, 0, 0);

	return S_OK;
}

_float4 CVIBuffer_Particle_Instance::Get_Look(_float3 vDirAngle, _float3 vRangeAngle1, _float3 vRangeAngle2)
{
	_vector OriginDir = { 0.f, 1.f, 0.f };

	_matrix mRX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vDirAngle.x));
	_matrix mRY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vDirAngle.y));
	_matrix mRZ = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(vDirAngle.z));

	OriginDir = XMVector3TransformNormal(OriginDir, mRX);
	OriginDir = XMVector3TransformNormal(OriginDir, mRY);
	OriginDir = XMVector3TransformNormal(OriginDir, mRZ);

	_float fRandAnlge_X = CGameInstance::Get_Instance()->Get_FloatRandom(vRangeAngle1.x, vRangeAngle2.x);
	_float fRandAnlge_Y = CGameInstance::Get_Instance()->Get_FloatRandom(vRangeAngle1.y, vRangeAngle2.y);
	_float fRandAnlge_Z = CGameInstance::Get_Instance()->Get_FloatRandom(vRangeAngle1.z, vRangeAngle2.z);

	_matrix mRX2 = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_X));
	_matrix mRY2 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_Y));
	_matrix mRZ2 = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRandAnlge_Z));

	OriginDir = XMVector3TransformNormal(OriginDir, mRX2);
	OriginDir = XMVector3TransformNormal(OriginDir, mRY2);
	OriginDir = XMVector3TransformNormal(OriginDir, mRZ2);

	_float4 vLook;
	XMStoreFloat4(&vLook, XMVector3Normalize(OriginDir));
	return vLook;
}




CVIBuffer_Particle_Instance * CVIBuffer_Particle_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Particle_Instance*			pInstance = new CVIBuffer_Particle_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Particle_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Particle_Instance::Clone(void * pArg)
{
	CVIBuffer_Particle_Instance*			pInstance = new CVIBuffer_Particle_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Particle_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Instance::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Delete_Array(m_vDirs);
		Safe_Delete_Array(m_fLifeTimes);
		Safe_Delete_Array(m_fSpeeds);	
		Safe_Delete(m_pPtInstanceInfo);
		Safe_Release(m_pVBInstance);
	}

	
	
}
