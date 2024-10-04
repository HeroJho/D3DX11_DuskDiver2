#include "stdafx.h"
#include "..\Public\PunchTime.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"

CPunchText::CPunchText(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPunchText::CPunchText(const CPunchText & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPunchText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPunchText::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기

	m_fSizeX = 4.f;
	m_fSizeY = 4.f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(146.593f, -12.f, 81.278f, 1.f));

	return S_OK;
}

void CPunchText::Tick(_float fTimeDelta)
{
	m_bLight = UM->Get_PunchLight();

	if (m_bLight)
	{
		if (m_fTime <= 0.f)
			m_fTime = 0.f;
		else
			m_fTime -= fTimeDelta;
	}
	else
	{
		if (m_fTime >= 1.f)
			m_fTime = 1.f;
		else
			m_fTime += fTimeDelta;
	}

	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), 59.f);
}

void CPunchText::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPunchText::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fTime, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_int i = (15 - (_uint)UM->Get_PunchTime()) % 10;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
		return E_FAIL;

	if (15 - UM->Get_PunchTime() < 9)
	{
		if (FAILED(m_pShaderCom->Begin(8)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(10)))
			return E_FAIL;
	}

	if (m_bLight)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	
	return S_OK;
}

void CPunchText::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Potal")->Set_Size(vSize);
	Get_Colliders("Potal")->Set_Center(vCenter);
	ImGui::End();
}

HRESULT CPunchText::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}





CPunchText * CPunchText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPunchText*		pInstance = new CPunchText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPunchText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunchText::Clone(void * pArg)
{
	CPunchText*		pInstance = new CPunchText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPunchText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPunchText::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
