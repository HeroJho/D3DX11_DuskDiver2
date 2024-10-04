#include "stdafx.h"
#include "..\Public\BossBase.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CBossBase::CBossBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBossBase::CBossBase(const CBossBase & rhs)
	: CUI(rhs)
{
}

HRESULT CBossBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, -99.f, 1280.f, 135.33f, TEXT("Prototype_GameObject_ThunderBase0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -105.83f, 1280.f, 86.f, TEXT("Prototype_GameObject_Thunder0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-436.88f, 18.34f, 410.f, 135.33f, TEXT("Prototype_GameObject_ThunderBase1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-457.7f, 15.f, 366.f, 116.66f, TEXT("Prototype_GameObject_Thunder1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-367.7f, 112.6f, 508.f, 124.66f, TEXT("Prototype_GameObject_ThunderBase2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-367.7f, 117.5f, 426.66f, 73.33f, TEXT("Prototype_GameObject_Thunder2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(450.f,36.67f, 380.66f, 129.66f, TEXT("Prototype_GameObject_ThunderBase3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(472.f, 36.67f, 336.f, 132.f, TEXT("Prototype_GameObject_Thunder3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(297.6f, 67.5f, 546.66f, 192.66f, TEXT("Prototype_GameObject_ThunderBase5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(277.62f, 85.f, 440.66f, 96.66f, TEXT("Prototype_GameObject_Thunder5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 320.f, 257.3f, TEXT("Prototype_GameObject_SkullBase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 83.33f, 75.33f, TEXT("Prototype_GameObject_SkullLight"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 163.3f, 142.f, TEXT("Prototype_GameObject_SkullVanish"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(0.f, 0.f, 610.f, 136.6f, TEXT("Prototype_GameObject_Warning"))))
		return E_FAIL;



	return S_OK;
}

void CBossBase::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_I) && !UM->Get_Warning())
	//{
	//	UM->Warning_On();
	//}
	//else if (CK_DOWN(DIK_I) && UM->Get_Warning())
	//{
	//	UM->Warning_Off();
	//}

	if (UM->Get_Warning() && !m_bRender)
	{
		m_bRender = true;
		GI->PlaySoundW(L"UI_Boss_Warning.wav", SD_UI, 0.6f);
	}
	else
		m_bRender = false;
	//UI_InputDebug(fTimeDelta);

	if(m_bRender)
		__super::Tick(fTimeDelta);
}

void CBossBase::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBossBase::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (m_bRender)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}



	//UI_RenderDebug();

	// 자식들의 Render 호출

	if (m_bRender)
		__super::Render();

	return S_OK;
}






HRESULT CBossBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossBase"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBossBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBossBase * CBossBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossBase*		pInstance = new CBossBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossBase::Clone(void * pArg)
{
	CBossBase*		pInstance = new CBossBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBossBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
