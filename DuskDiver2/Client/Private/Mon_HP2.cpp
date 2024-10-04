#include "stdafx.h"
#include "..\Public\Mon_HP2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"

CMon_HP2::CMon_HP2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMon_HP2::CMon_HP2(const CMon_HP2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMon_HP2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_HP2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기
	m_pMonster = (CCreture*)pArg;

	m_fSizeX = 11.6f;
	m_fSizeY = 1.8f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX * 0.2f, m_fSizeY * 0.1f, 0.f, 0.f));
	m_vMonsterPos = m_pMonster->Get_WorldPos();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vMonsterPos.x, m_vMonsterPos.y + 2.5f, m_vMonsterPos.z, m_vMonsterPos.w));

	return S_OK;
}

void CMon_HP2::Tick(_float fTimeDelta)
{

	if (!m_pMonster->Get_Dead())
	{
		m_vMonsterPos = m_pMonster->Get_WorldPos();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vMonsterPos.x, m_vMonsterPos.y + 2.5f, m_vMonsterPos.z, m_vMonsterPos.w));
	}
	else
		Set_Dead();
}

void CMon_HP2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_pMonster != nullptr)
	{

		m_fNowHp = m_pMonster->Get_NowHp();

		if (!m_bRender && m_pMonster->Get_Spawn())
		{
			Billboard();
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
		}
	}


	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

}

HRESULT CMon_HP2::Render()
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

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;
}

void CMon_HP2::Render_Debug()
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

HRESULT CMon_HP2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMon_HP2::Billboard()
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





CMon_HP2 * CMon_HP2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMon_HP2*		pInstance = new CMon_HP2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMon_HP2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMon_HP2::Clone(void * pArg)
{
	CMon_HP2*		pInstance = new CMon_HP2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMon_HP2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMon_HP2::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
