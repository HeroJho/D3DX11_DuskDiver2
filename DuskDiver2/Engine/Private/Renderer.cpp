#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "ValueManager.h"
#include "Level_Manager.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
#pragma region 중략

	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint		iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	// For.Target_ShadowDepth
	_float		iShadowMapCX = ViewportDesc.Width * 12.5f;
	_float		iShadowMapCY = ViewportDesc.Height * 12.5f;
	if (FAILED(m_pTarget_Manager->Ready_ShadowDepthStencilRenderTargetView(m_pDevice, (_uint)iShadowMapCX, (_uint)iShadowMapCY)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), (_uint)iShadowMapCX, (_uint)iShadowMapCY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
#pragma endregion
	// 타겟 생성
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), 
		(_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, 
		_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), 
		(_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, 
		_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), 
		(_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, 
		_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;
#pragma region 중략
	/* For.Target_Shader */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_TempBack */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_TempBack"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	/* For.Target_TempBack_Shadow */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_TempBack_Shadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Blur */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurDownSample */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurDownSample"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurHor */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurHor"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurVer */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurVer"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_BlurUpSample */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurUpSample"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Outline */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Outline"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Finish */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Finish"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;




	/* For.Target_Distortion */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Distortion"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Distortion_Finish */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Distortion_Finish"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_LUT */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LUT"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_PointLight */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_PointLight"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Temp */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Temp"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion
	// MRT 그룹 생성
	/* For.MRT_TempBack */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_TempBack"), TEXT("Target_TempBack"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_TempBack"), TEXT("Target_Normal"))))
		return E_FAIL;
	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_PointLight"))))
		return E_FAIL;
#pragma region 중략

	/* For.MRT_LightDepth*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;

	/* For.MRT_BlurDownSample*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurDownSample"), TEXT("Target_BlurDownSample"))))
		return E_FAIL;

	/* For.MRT_BlurHor*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurHor"), TEXT("Target_BlurHor"))))
		return E_FAIL;

	/* For.MRT_BlurVer*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurVer"), TEXT("Target_BlurVer"))))
		return E_FAIL;

	/* For.MRT_BlurUpSample*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurUpSample"), TEXT("Target_BlurUpSample"))))
		return E_FAIL;

	/* For.MRT_Outline*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Outline"), TEXT("Target_Outline"))))
		return E_FAIL;
	
	/* For.MRT_Finish*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Finish"), TEXT("Target_Finish"))))
		return E_FAIL;



	/* For.MRT_Distortion*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;
	/* For.MRT_Distortion*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Distortion_Finish"), TEXT("Target_Distortion_Finish"))))
		return E_FAIL;
	
	/* For.MRT_LUT*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_LUT"))))
		return E_FAIL;






#ifdef _DEBUG
	
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_TempBack"), 200.f, 200.f, 400.f, 400.f)))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Finish"), 100.f, 300.f, 200.f, 200.f)))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Depth"), 600.f, 200.f, 400.f, 400.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Outline"), 1000.f, 200.f, 400.f, 400.f)))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_TempBack_Shadow"), 300.f, 300.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_ShadowDepth"), 300.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Blur"), 500.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;



	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

#endif // _DEBUG

	m_pTextureLUT_Com = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/LUT/TX_FX_PPC_LUT_%d.png"), 55);
	m_pTextureNoise_Com = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Noise/T_Noise_02.png"), 1);
	
#pragma endregion
	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))      
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend())) 
		return E_FAIL;


	// 빛을 그린다
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Blend()))          
		return E_FAIL;
	if (FAILED(Render_NonLight()))        
		return E_FAIL;


	// 광원 깊이를 그린다, 그림자를 그린다.
	if (FAILED(Render_ShadowDepth()))
		return E_FAIL;
	if (FAILED(Render_Shadow()))           
		return E_FAIL;

	if (FAILED(Render_ComputeAlphaBlend()))       
		return E_FAIL;

	if (FAILED(Render_AlphaBlend()))       
		return E_FAIL;


	// 외각선을 딴다
	if (FAILED(Render_Toon()))
		return E_FAIL;


	// 블러
	if (FAILED(Render_BlurDownSample()))
		return E_FAIL;
	if (FAILED(Render_BlurHorizontal()))
		return E_FAIL;
	if (FAILED(Render_BlurVertical()))
		return E_FAIL;
	if (FAILED(Render_BlurUpSample()))
		return E_FAIL;


	// 외각선, 블러, 명암 적용된 Temp를 조합한다.
	if (FAILED(Render_Finish()))
		return E_FAIL;

	if (FAILED(Render_NoBlur()))
		return E_FAIL;

	//if (FAILED(Render_LUT()))
	//	return E_FAIL;

	// 최종 이미지의 굴곡지게한다.
	if (FAILED(Render_Distortion()))
		return E_FAIL;
	if (FAILED(Render_DistortionFinish()))
		return E_FAIL;




	if (FAILED(Render_WorldUI()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


#ifdef _DEBUG


	if (FAILED(Render_Debug()))
		return E_FAIL;


#endif
	

	return S_OK;
}

HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}








HRESULT CRenderer::Render_Priority()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTarget_Manager->Reset_SRV(TEXT("MRT_Deferred"), 0);
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"), false)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, 
		TEXT("MRT_LightAcc"))))
		return E_FAIL;

#pragma region 중략

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4			ViewMatrixInv;
	_float4x4			ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Set_RawValue("g_SpecPower", &CValueManager::Get_Instance()->Get_ValueDesc()->fSpacPower, sizeof(_float))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Normal"), 
		m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), 
		m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);
	
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack"), false)))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;



	m_pShader->Begin(3);

	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack"), false)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}



HRESULT CRenderer::Render_ShadowDepth()
{
	if(!CLight_Manager::Get_Instance()->Get_RenderLight())
		return S_OK;


	if (FAILED(m_pTarget_Manager->Begin_ShadowMRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;


	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_ShadowDepth();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOWDEPTH].clear();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	CLight_Manager::Get_Instance()->Set_RenderLight(false);

	return S_OK;
}


HRESULT CRenderer::Render_Shadow()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack_Shadow"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);
	_float4x4			ViewMatrixInv;
	_float4x4			ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pShader->Set_RawValue("g_ShadowPow", &CValueManager::Get_Instance()->Get_ValueDesc()->fShadowPow, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_ShadowDepth"), m_pShader, "g_ShadowDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_TempBack"), m_pShader, "g_TempBackTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_PointLight"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;


	_float4x4		LightViewMatrix;
	if (FAILED(m_pShader->Set_RawValue("g_ShadowViewMatrix", &CLight_Manager::Get_Instance()->Get_ShadowLightViewMatrix(CLevel_Manager::Get_Instance()->Get_Cur_LevelIndex()), sizeof(_float4x4))))
		return E_FAIL;

	_float4x4 mProj; XMStoreFloat4x4(&mProj, XMMatrixTranspose(pPipeLine->Get_InitProjTransform()));
	if (FAILED(m_pShader->Set_RawValue("g_CamProjMatrix", &mProj, sizeof(_float4x4))))
		return E_FAIL;


	RELEASE_INSTANCE(CPipeLine);




	m_pShader->Begin(4);


	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ComputeAlphaBlend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack_Shadow"), false)))
		return E_FAIL;

	m_RenderObjects[RENDER_COMPUTEALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_COMPUTEALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_COMPUTEALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_TempBack_Shadow_Alph"), false)))
		return E_FAIL;

	
	//m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	//{
	//	return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	//});


	for (auto& pRenderObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_ALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}








HRESULT CRenderer::Render_Toon()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Outline"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fLinePower", &CValueManager::Get_Instance()->Get_ValueDesc()->fLinePower, sizeof(_float))))
		return E_FAIL;



	m_pShader->Begin(9);

	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_DISTORTION].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_DistortionFinish()
{
	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Finish"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Distortion"), m_pShader, "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Specular"), m_pShader, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureNoise_Com->Set_SRV(m_pShader, "g_NoiseTexture")))
		return E_FAIL;

	m_pShader->Begin(12);

	m_pVIBuffer->Render();


	return S_OK;
}



HRESULT CRenderer::Render_BlurDownSample()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurDownSample"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width * 0.25f, ViewportDesc.Height * 0.25f, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Blur"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Toon"), m_pShader, "g_ToonTexture")))
	//	return E_FAIL;


	m_pShader->Begin(0);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurHorizontal()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurHor"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurDownSample"), m_pShader, "g_BlurTexture")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_ToonDownSample"), m_pShader, "g_ToonTexture")))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Outline"), m_pShader, "g_ToonTexture")))
		return E_FAIL;


	m_pShader->Begin(5);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurVertical()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurVer"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;



	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurHor"), m_pShader, "g_BlurTexture")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_ToonHor"), m_pShader, "g_ToonTexture")))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Outline"), m_pShader, "g_ToonTexture")))
		return E_FAIL;


	m_pShader->Begin(6);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurUpSample()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurUpSample"))))
		return E_FAIL;


	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width * 4.f, ViewportDesc.Height * 4.f, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurVer"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_ToonVer"), m_pShader, "g_ToonTexture")))
	//	return E_FAIL;


	m_pShader->Begin(0);


	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Finish()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Finish"))))
		return E_FAIL;
	

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_TempBack_Shadow"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurUpSample"), m_pShader, "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Outline"), m_pShader, "g_ToonTexture")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Alpa"), m_pShader, "g_AlphaTexture")))
	//	return E_FAIL;

	m_pShader->Begin(7);


	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NoBlur()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Finish"), false)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NOBLUR])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NOBLUR].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_LUT()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LUT"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Finish"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;


	if (CValueManager::Get_Instance()->Get_ValueDesc()->iLUT_Index != 100)
	{
		if (FAILED(m_pTextureLUT_Com->Set_SRV(m_pShader, "g_LUTTexture", CValueManager::Get_Instance()->Get_ValueDesc()->iLUT_Index)))
			return E_FAIL;
		m_pShader->Begin(11);
	}

	else
		m_pShader->Begin(0);


	m_pVIBuffer->Render();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	return S_OK;
}



HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_WorldUI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_WORLDUI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_WORLDUI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (m_bDebugTarget)
	{
		m_pTarget_Manager->Render_Debug(TEXT("MRT_TempBack"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"), m_pVIBuffer, m_pShader);

		m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurDownSample"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurHor"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurVer"), m_pVIBuffer, m_pShader);
		m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurUpSample"), m_pVIBuffer, m_pShader);

		m_pTarget_Manager->Render_Debug(TEXT("MRT_Outline"), m_pVIBuffer, m_pShader);

		m_pTarget_Manager->Render_Debug(TEXT("MRT_TempBack_Shadow"), m_pVIBuffer, m_pShader);

		m_pTarget_Manager->Render_Debug(TEXT("MRT_Finish"), m_pVIBuffer, m_pShader);

		m_pTarget_Manager->Render_Debug(TEXT("MRT_Alpa"), m_pVIBuffer, m_pShader);
	}

	if (m_bDebugCol)
	{
		for (auto& pDebugCom : m_DebugObject)
		{
			pDebugCom->Render();
			Safe_Release(pDebugCom);
		}
		m_DebugObject.clear();
	}
	else
	{
		for (auto& pDebugCom : m_DebugObject)
			Safe_Release(pDebugCom);
		m_DebugObject.clear();
	}


	return S_OK;
}
#endif // _DEBUG








CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*			pInstance = new CRenderer(pDevice, pContext);	

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& List : m_RenderObjects)
	{
		for (auto& pGameObject : List)
		{
			Safe_Release(pGameObject);
		}
		List.clear();
	}	

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	Safe_Release(m_pTextureLUT_Com);
	Safe_Release(m_pTextureNoise_Com);
}
