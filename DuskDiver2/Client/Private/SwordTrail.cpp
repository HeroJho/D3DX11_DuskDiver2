#include "stdafx.h"
#include "SwordTrail.h"
#include "GameInstance.h"


CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pShaderCom(nullptr)
{
}

CSwordTrail::CSwordTrail(const CSwordTrail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwordTrail::Initialize(void * pArg)
{

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 22;
	m_iStride = sizeof(VTXTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC/*D3D11_USAGE_DEFAULT*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE/*0*/;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);
	
	for (int i = 0; i < 22; i += 2)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i].vTexture = _float2(0.f, 0.f);

		pVertices[i + 1].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i + 1].vTexture = _float2(0.f, 0.f);
	}
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 20;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	if (pArg == nullptr)
	{
		MSG_BOX(TEXT("Plz High and Low"));
		return E_FAIL;
	}
	
	TRAILINFO Info = *(TRAILINFO*)(pArg);
	m_HighAndLow = Info.HighAndLow;
	m_vColor = Info.vColor1;
	m_vColor2 = Info.vColor2;
	m_bFixColor = Info.bFixColor;
	m_vRatio = Info.vRatio;
	m_fBlurPower = Info.fBlurPower;
	m_iShaderPass = Info.iShaderPass;

	m_pShaderCom = (CShader*)(GI->Clone_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_SwordTrail")));
	m_pTextureCom = (CTexture*)(GI->Clone_Component(LEVEL_STATIC, Info.TexName));
	if (m_pShaderCom == nullptr)
	{
		MSG_BOX(TEXT("Clone Trail Shader"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSwordTrail::Render()
{
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	if (m_bFixColor)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
			return E_FAIL;

		
	}

	m_pShaderCom->Begin(m_iShaderPass);
	
	__super::Render();


	return S_OK;
}

void CSwordTrail::Tick(const _float& _fTimeDelta, _matrix _matWeapon)
{
	
	m_fTimeAcc += _fTimeDelta;

	if (m_fTimeAcc > m_fTimeLimit)
	{
		m_fTimeAcc = 0.f;
		if (m_iVtxCount >= (int)m_iNumVertices)
		{
			m_iVtxCount = 20;
			for (_uint i = 0; i < (_uint)m_iNumVertices - 2; ++i)
			{
				m_RealData[i].vPosition = m_RealData[i + 2].vPosition;
			}
			if (m_RealData[21].vPosition.x == m_RealData[1].vPosition.x &&
				m_RealData[21].vPosition.y == m_RealData[1].vPosition.y &&
				m_RealData[21].vPosition.z == m_RealData[1].vPosition.z)
			{
				m_iVtxCount = 0;
				m_bTrailOn = false;
				return;
			}
		}
		_fvector High = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vHigh)), _matWeapon);
		_fvector Low = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vLow)), _matWeapon);
		XMStoreFloat3(&(m_RealData[m_iVtxCount].vPosition), High);
		XMStoreFloat3(&(m_RealData[m_iVtxCount + 1].vPosition), Low);


		for (int i = 0; i < m_iVtxCount; i += 2)
		{
			_float _iVtxCount = 0.f;
			if (m_iVtxCount < 1)
			{
				_iVtxCount = 1.f;
			}
			else
			{
				_iVtxCount = (_float)m_iVtxCount;
			}
			m_RealData[i].vTexture = { ((_float)i) / ((_float)_iVtxCount),0.f };
			m_RealData[i + 1].vTexture = { ((_float)i) / ((_float)_iVtxCount),1.f };
		}
		m_iVtxCount += 2;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, m_RealData, sizeof(VTXTEX) * 22);
	m_pContext->Unmap(m_pVB, 0);
}

void CSwordTrail::TrailOn(_matrix _matWeapon)
{
	for (_uint i = 0; i < 22; i += 2)
	{
		_fvector High = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vHigh)), _matWeapon);
		_fvector Low = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vLow)), _matWeapon);
		XMStoreFloat3(&(m_RealData[i].vPosition), High);
		XMStoreFloat3(&(m_RealData[i + 1].vPosition), Low);
	}
	m_iVtxCount = 0;
	m_bTrailOn = true;
}

CSwordTrail * CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail*			pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CSwordTrail::Clone(void * pArg)
{
	CSwordTrail*			pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrail::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
