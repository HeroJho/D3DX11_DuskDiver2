#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
	// _vector		vLightEye = XMVectorSet(24.f, 31.64f, -33.9f, 1.f);
	//_vector		vLightEye = XMVectorSet(13.f, 5.7f, -20.3f, 1.f);
	//_vector		vLightAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//XMStoreFloat4x4(&m_mShadowLightViewMatrixs, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));
}


HRESULT CLight_Manager::Init(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iMaxLevel)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_mShadowLightViewMatrixs = new _float4x4[iMaxLevel];

	for (_uint i = 0; i < iMaxLevel; ++i)
	{
		XMStoreFloat4x4(&m_mShadowLightViewMatrixs[i],XMMatrixIdentity());
	}

	return S_OK;;
}


const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();	
}

HRESULT CLight_Manager::Add_ShadowLightViewMatrix(_uint iLevel, _float4x4 mMatrix)
{
	m_mShadowLightViewMatrixs[iLevel] = mMatrix;

	return S_OK;
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix(_uint iLevel)
{
	_float4x4 mMatirx;
	XMStoreFloat4x4(&mMatirx, XMMatrixTranspose(XMLoadFloat4x4(&m_mShadowLightViewMatrixs[iLevel])));
	return mMatirx;
}

LIGHTDESC * CLight_Manager::Get_DirLightDesc()
{
	return m_DirLight->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const LIGHTDESC & LightDesc, CLight** pOut)
{
	CLight*		pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	if (tagLightDesc::TYPE_DIRECTIONAL != LightDesc.eType)
	{
		m_Lights.push_back(pLight);
		if (pOut)
			*pOut = pLight;
	}

	else
	{
		if (m_DirLight)
			Safe_Release(m_DirLight);
		m_DirLight = pLight;
	}



	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	Safe_Release(*iter);
	m_Lights.erase(iter);


	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(CLight * pLight)
{

	for (list<class CLight*>::iterator iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		if ((*iter) == pLight)
		{
			Safe_Release(*iter);
			m_Lights.erase(iter);
			return S_OK;
		}
		else
			++iter;
	}

	return S_OK;
}

HRESULT CLight_Manager::Reset_Lights()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	//if (m_DirLight)
	//{
	//	Safe_Release(m_DirLight);
	//	m_DirLight = nullptr;
	//}

	return S_OK;
}



HRESULT CLight_Manager::Render(CShader * pShader, 
	CVIBuffer_Rect * pVIBuffer)
{
	// 방향성 광원 연산
	if (m_DirLight)
		m_DirLight->Render(pShader, pVIBuffer);

	// 점광원 연산
	list<class CLight*>::iterator iter = m_Lights.begin();
	for (; iter != m_Lights.end();)
	{
		if ((*iter)->Get_Dead())
		{
			Safe_Release((*iter));
			iter = m_Lights.erase(iter);
		}
		else
		{
			(*iter)->Render(pShader, pVIBuffer);
			++iter;
		}
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);
	m_Lights.clear();
	Safe_Release(m_DirLight);

	Safe_Delete_Array(m_mShadowLightViewMatrixs);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
