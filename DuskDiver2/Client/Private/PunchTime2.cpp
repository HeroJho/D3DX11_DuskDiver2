#include "stdafx.h"
#include "..\Public\PunchTime2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "UI_Manager.h"

CPunchTime2::CPunchTime2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPunchTime2::CPunchTime2(const CPunchTime2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPunchTime2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPunchTime2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기

	m_fSizeX = 4.f;
	m_fSizeY = 4.f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(149.093f, -12.f, 82.278f, 1.f));

	return S_OK;
}

void CPunchTime2::Tick(_float fTimeDelta)
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

	_uint iTime = 15 - (_uint)UM->Get_PunchTime();

	if (3 == iTime && !m_bSound)
	{
		GI->PlaySoundW(L"Timer.wav", SD_MAP, 1.f);
		m_bSound = true;
	}

	else if (2 == iTime && m_bSound)
	{
		GI->PlaySoundW(L"Timer.wav", SD_MAP, 1.f);
		m_bSound = false;
	}

	else if (1 == iTime && !m_bSound)
	{
		GI->PlaySoundW(L"Timer.wav", SD_MAP, 1.f);
		m_bSound = true;
	}

	else if (0 == iTime && m_bSound)
		m_bSound = false;

	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), 59.f);
}

void CPunchTime2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPunchTime2::Render()
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

	_int i = (15 - (_uint)UM->Get_PunchTime() )/ 10;
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
			return E_FAIL;
	}

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

	if (i > 0)
	{
		if (m_bLight)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}
	
	return S_OK;
}

void CPunchTime2::Render_Debug()
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

HRESULT CPunchTime2::Ready_Components()
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





CPunchTime2 * CPunchTime2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPunchTime2*		pInstance = new CPunchTime2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPunchTime2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunchTime2::Clone(void * pArg)
{
	CPunchTime2*		pInstance = new CPunchTime2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPunchTime2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPunchTime2::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
