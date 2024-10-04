#include "stdafx.h"
#include "..\Public\Mon_HP_Bar.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "MechanicalOctopusLeg.h"

CMon_HP_Bar::CMon_HP_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMon_HP_Bar::CMon_HP_Bar(const CMon_HP_Bar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMon_HP_Bar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_HP_Bar::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기
	m_pMonster = (CCreture*)pArg;

	m_fSizeX = 9.4f;
	m_fSizeY = 1.f;

	if (2000.f == m_pMonster->Get_MaxHp())
		m_bBoss = true;
	else
	{
		m_fY = 2.5f;
		m_bBoss = false;
	}

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX * 0.2f, m_fSizeY * 0.1f, 0.f, 0.f));
	m_vMonsterPos = m_pMonster->Get_WorldPos();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vMonsterPos.x, m_vMonsterPos.y + m_fY, m_vMonsterPos.z + m_fZ, m_vMonsterPos.w));

	m_fMaxHp = m_pMonster->Get_MaxHp();

	return S_OK;
}

void CMon_HP_Bar::Tick(_float fTimeDelta)
{

	if (!m_bBoss)
	{
		m_vMonsterPos = m_pMonster->Get_WorldPos();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vMonsterPos.x, m_vMonsterPos.y + m_fY, m_vMonsterPos.z + m_fZ, m_vMonsterPos.w));
	}
	else
	{
		_vector vRight = *(_vector*)static_cast<CMechanicalOctopusLeg*>(m_pMonster)->Get_WorldMatrix().m[0];
		_vector vUP = *(_vector*)static_cast<CMechanicalOctopusLeg*>(m_pMonster)->Get_WorldMatrix().m[1];
		_vector vPos = *(_vector*)static_cast<CMechanicalOctopusLeg*>(m_pMonster)->Get_WorldMatrix().m[3];

		vUP = XMVector3Normalize(vUP) * 1.2f;
		vPos += vUP;
		vRight = XMVector3Normalize(vRight) * -0.2f;
		vPos += vRight;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CMon_HP_Bar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_fNowHp = m_pMonster->Get_NowHp();
	m_fHpGauge = m_fNowHp / m_fMaxHp;
	
	m_fPreHp = m_pMonster->Get_PreHp();
	m_fPreHpGauge = m_fPreHp / m_fMaxHp;

	if (0 >= m_fNowHp)
		Set_Dead();

	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && m_pMonster->Get_Spawn())
	{
		Billboard();
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
	}
}

HRESULT CMon_HP_Bar::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_MonsterHP", &m_fHpGauge, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_MonsterPreHP", &m_fPreHpGauge, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;
}

void CMon_HP_Bar::Render_Debug()
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

HRESULT CMon_HP_Bar::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMon_HP_Bar::Billboard()
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





CMon_HP_Bar * CMon_HP_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMon_HP_Bar*		pInstance = new CMon_HP_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMon_HP_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMon_HP_Bar::Clone(void * pArg)
{
	CMon_HP_Bar*		pInstance = new CMon_HP_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMon_HP_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMon_HP_Bar::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
