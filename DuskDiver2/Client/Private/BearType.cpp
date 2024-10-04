#include "stdafx.h"
#include "..\Public\BearType.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "ToolManager.h"
#include "Bear.h"


CBearType::CBearType(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBearType::CBearType(const CBearType & rhs)
	: CUI(rhs)
{
}

HRESULT CBearType::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBearType::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CBearType::Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (UM->Get_BearHunt())
	{
		m_fBearTime += fTimeDelta;
		if (m_fBearTime > 10.f)
		{
			m_fBearTime = 0;
			while (true)
			{
				int i = GI->Get_Random(0, 3);

				if (i != m_iBearType)
				{
					GI->PlaySoundW(L"minigame_common_button.wav", SD_BEAST, 1.f);

					m_iBearType = i;
					break;
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	//UI_InputDebug(fTimeDelta);

	UM->Set_BearType(m_iBearType);

	__super::Tick(fTimeDelta);



	if (CK_DOWN(DIK_I))
	{
		UM->Make_Ink();
	}
}

void CBearType::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBearType::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iBearType)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	//if (UM->Get_BearHunt())
	//{
	//	if (FAILED(m_pVIBufferCom->Render()))
	//		return E_FAIL;
	//}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CBearType::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BearType"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBearType::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBearType * CBearType::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBearType*		pInstance = new CBearType(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBearType"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBearType::Clone(void * pArg)
{
	CBearType*		pInstance = new CBearType(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBearType"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBearType::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
