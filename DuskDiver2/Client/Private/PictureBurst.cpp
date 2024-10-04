#include "stdafx.h"
#include "..\Public\PictureBurst.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"
#include "Monster_Manager.h"


CPictureBurst::CPictureBurst(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPictureBurst::CPictureBurst(const CPictureBurst & rhs)
	: CUI(rhs)
{
}

HRESULT CPictureBurst::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPictureBurst::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CPictureBurst::Tick(_float fTimeDelta)
{

	if (UM->Get_PictureBurst())
	{
		m_fTime += fTimeDelta;
		if (m_fTime > 0.01f)
		{
			if (m_fAlpha <= 0.f)
			{
				m_fAlpha = 0.f;
				m_bPictureOn = true;
			}
			else
				m_fAlpha -= 0.005f;

			m_fTime = 0.f;
		}
	}
	else if (!UM->Get_PictureBurst() && m_bPictureOn)
	{
		MM->Control_Beta();
		MM->Control_Fusion();
		MM->Control_OldBombRobot();
		MM->Control_DisasterBoss();

		m_fTime += fTimeDelta;
		if (m_fTime > 0.01f)
		{
			if (m_fAlpha >= 0.3f)
			{
				// UI가 사라지고 몬스터들을 컨트롤 한다

				m_fAlpha = 0.3f;
				m_bPictureOn = true;
				m_bDead = true;

		
			}
			else
				m_fAlpha += 0.005f;

			m_fTime = 0.f;
		}
	}

	////UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPictureBurst::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (UM->Get_PictureBurst())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	
}

HRESULT CPictureBurst::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPictureBurst::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PictureBurst"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPictureBurst::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPictureBurst * CPictureBurst::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPictureBurst*		pInstance = new CPictureBurst(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPictureBurst"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPictureBurst::Clone(void * pArg)
{
	CPictureBurst*		pInstance = new CPictureBurst(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPictureBurst"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPictureBurst::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
