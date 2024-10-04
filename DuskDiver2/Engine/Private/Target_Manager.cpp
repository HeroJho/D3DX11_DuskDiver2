#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}


HRESULT CTarget_Manager::Ready_ShadowDepthStencilRenderTargetView(ID3D11Device * pDevice, _uint iWinCX, _uint iWinCY)
{
	if (nullptr == pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;


	if (FAILED(pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDeptheStencil)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	m_gShodowDepthSize.x = (_float)iWinCX;
	m_gShodowDepthSize.y = (_float)iWinCY;

	return S_OK;
}


HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, 
	_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 pClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if(nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_SRV(pShader, pConstantName);	
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag, _bool bIsClear)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);
		
	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{		
		if(bIsClear)
			pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

		

	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_ShadowMRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	// 끼워져 있는 8개의 랜더 타겟와 스텐실 하나를 보관한다.
	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}


	pContext->ClearDepthStencilView(m_pShadowDeptheStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pShadowDeptheStencil);
	// pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = m_gShodowDepthSize.x;
	ViewPortDesc.Height = m_gShodowDepthSize.y;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}


HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext)
{
	_uint		iNumRTVs = 8;	

	pContext->OMSetRenderTargets(iNumRTVs, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pOldRenderTargets[i]);

	Safe_Release(m_pOldDepthStencil);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.f;
	ViewPortDesc.Height = 720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;
	pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CTarget_Manager::Reset_SRV(const _tchar * pMRTTag, _uint iIndex)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint iCount = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		if (iIndex == iCount)
		{
			pRenderTarget->Clear();
			break;
		}

		++iCount;
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Initialize_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget =  Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Initialize_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug(const _tchar * pMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));
	if(iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{

	Safe_Release(m_pShadowDeptheStencil);


	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

}
