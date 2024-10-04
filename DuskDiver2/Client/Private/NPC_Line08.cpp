#include "stdafx.h"
#include "..\Public\NPC_Line08.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"

CNPC_Line08::CNPC_Line08(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CNPC_Line08::CNPC_Line08(const CNPC_Line08 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPC_Line08::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Line08::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Line";
	
	Make_ImGui();

	m_fSizeX = 3.745f;
	m_fSizeY = 1.f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX * 0.4f, m_fSizeY * 0.4f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(34.7f, 1.65f, 8.3f, 1.f));

	return S_OK;
}

void CNPC_Line08::Tick(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();
	m_bOn = m_pTransformCom->Check_LimitDistance(XMLoadFloat4(&m_fPlayerPos), 5.f);

	if (!m_bOn)
		m_fOnAcc = 0.f;

	else
	{
		m_fOnAcc += 2.5f * fTimeDelta;

		if (1.f <= m_fOnAcc)
		{
			m_fOnAcc = 1.f;
		}
	}
}

void CNPC_Line08::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && m_bOn && !TM->Get_Stage3_Bool())
	{
		Billboard();
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
	}
}

HRESULT CNPC_Line08::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fOnAcc, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 8)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;
}

void CNPC_Line08::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::End();
}

HRESULT CNPC_Line08::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Line"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Line08::Billboard()
{
	_vector vRight = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_RIGHT));
	_vector vUp = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_UP));
	_vector vLook = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_LOOK));

	vRight = m_pTransformCom->Get_Scale().x * XMVector3Normalize(vRight);
	vUp = m_pTransformCom->Get_Scale().y * XMVector3Normalize(vUp);
	vLook = m_pTransformCom->Get_Scale().z * XMVector3Normalize(vLook);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}





CNPC_Line08 * CNPC_Line08::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPC_Line08*		pInstance = new CNPC_Line08(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPC_Line08"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC_Line08::Clone(void * pArg)
{
	CNPC_Line08*		pInstance = new CNPC_Line08(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPC_Line08"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPC_Line08::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
