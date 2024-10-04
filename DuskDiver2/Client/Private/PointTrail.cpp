#include "stdafx.h"
#include "PointTrail.h"
#include "GameInstance.h"


CPointTrail::CPointTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pShaderCom(nullptr)
{
}

CPointTrail::CPointTrail(const CPointTrail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CPointTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPointTrail::Initialize(void * pArg)
{

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOINT*		pVertices = new VTXPOINT;

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices->vSize = _float2(0.5f, 0.5f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 1;
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

	if (pArg == nullptr)
	{
		MSG_BOX(TEXT("Plz High and Low"));
		return E_FAIL;
	}
	
	TRAILCREATEINFO Info = *(TRAILCREATEINFO*)(pArg);
	
	m_vColor = Info.vColor;
	m_vColor2 = Info.vColor2;
	m_vRatio = Info.vRatio;
	m_fBlurPower = Info.fBlurPower;
	m_bFixColor = Info.bFixColor;
	m_iShaderPass = Info.iShaderPass;
	m_fSize = Info.fSize;
	m_iMaxIndex = Info.iMaxIndex;
	m_pShaderCom = (CShader*)(GI->Clone_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_PointTrail")));
	m_pTextureCom = (CTexture*)(GI->Clone_Component(LEVEL_STATIC, Info.TexName));
	if (m_pShaderCom == nullptr)
	{
		MSG_BOX(TEXT("Clone Trail Shader"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPointTrail::Render()
{
	if (m_bTrailOn == false || m_TrailInfos.size() < 1)
	{
		return S_OK;
	}
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fSize", &m_fSize, sizeof(_float))))
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

	for (int i = 0; i < m_TrailInfos.size() -1; ++i)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vPos1", &m_TrailInfos[i].vPos, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vPos2", &m_TrailInfos[i + 1].vPos, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vRight1", &m_TrailInfos[i].vRight, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vRight2", &m_TrailInfos[i + 1].vRight, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vUp1", &m_TrailInfos[i].vUp, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vUp2", &m_TrailInfos[i + 1].vUp, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUv1", &m_TrailInfos[i].fUV, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUv2", &m_TrailInfos[i + 1].fUV, sizeof(_float))))
			return E_FAIL;	

		m_pShaderCom->Begin(m_iShaderPass);

		__super::Render();
	}

	return S_OK;
}

void CPointTrail::Tick(const _float& _fTimeDelta)
{	
	if (!m_bTrailOn)
		return;

	m_fTimeAcc += _fTimeDelta;
	if (m_fTimeAcc >= m_fTimeLimit)
	{
		m_fTimeAcc = 0.f;
		//if(m_bStart)
		if(m_TrailInfos.size() > 0)
			m_TrailInfos.erase(m_TrailInfos.begin());
	}

	if (m_TrailInfos.size() < 1 && m_bTrailOn)
	{
		TrailOff();
		return;
	}

	for (_uint i = 0; i < m_TrailInfos.size(); ++i)
	{
		_float InfoSize = 0.f;
		if (m_TrailInfos.size() < 1)
		{
			InfoSize = 1.f;
		}
		else
		{
			InfoSize = (_float)m_TrailInfos.size() - 1.f;
		}
		m_TrailInfos[i].fUV = _float(i) / ((_float)InfoSize);
	}
	

}

void CPointTrail::TrailOff()
{
	m_bTrailOn = false;
	m_bStart = false;
	m_TrailInfos.clear();
	m_fTimeAcc = 0.f;
}

void CPointTrail::Add_Point(_float4 vPos)
{
	if (m_TrailInfos.size() > m_iMaxIndex)
		return;

	if (m_TrailInfos.size() > 0)
	{
		if (m_TrailInfos.back().vPos.x == vPos.x && m_TrailInfos.back().vPos.y == vPos.y && m_TrailInfos.back().vPos.z == vPos.z)
			return;
	}

	TRAILINFO Info;
	Info.vPos = vPos;
	if (m_TrailInfos.size() == 0)
		Info.vRight = { 0.f,0.f,0.f };
	else
	{
		_float3 vLook;
		XMStoreFloat3(&vLook, XMVector3Normalize(XMLoadFloat4(&vPos) - XMLoadFloat4(&m_TrailInfos[m_TrailInfos.size() - 1].vPos)));
		XMStoreFloat3(&Info.vRight, XMVector3Cross(_vector{ 0.f,1.f,0.f }, XMLoadFloat3(&vLook)));
	}
	
	Info.vUp = { 0.f,1.f,0.f };
	m_TrailInfos.push_back(Info);

	if (!m_bStart && m_TrailInfos.size() >= 5)
		m_bStart = true;
		

}

CPointTrail * CPointTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPointTrail*			pInstance = new CPointTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPointTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CPointTrail::Clone(void * pArg)
{
	CPointTrail*			pInstance = new CPointTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPointTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPointTrail::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
