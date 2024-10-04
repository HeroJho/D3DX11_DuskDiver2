#include "stdafx.h"
#include "..\Public\NPCWalk.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Route_Manager.h"
#include "Player_Manager.h"
#include "Monster_Manager.h"

CNPCWalk::CNPCWalk(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCWalk::CNPCWalk(const CNPCWalk & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCWalk::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCWalk::Initialize(void * pArg)
{
	m_NPCWalkDesc = *(NPCWALKDESC*)pArg;

	m_iNodeIndex = m_NPCWalkDesc.iStartNodeIndex;

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // °´Ã¼tag

	m_pModelCom->Set_AnimIndex(0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_NPCWalkDesc.vPosition.x, m_NPCWalkDesc.vPosition.y, m_NPCWalkDesc.vPosition.z, 1.f));

	return S_OK;
}

void CNPCWalk::Tick(_float fTimeDelta)
{
	RTM->Move_Route(m_NPCWalkDesc.iRouteIndex, m_pTransformCom, 1.f, fTimeDelta, &m_iNodeIndex);
	Check_Playernearby();

	if (TM->Get_Stage3_Bool())
	{
		m_fDissolveAcc += 0.15f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			m_bRender = true;
		}
		m_iShaderPass = 6;
	}
	else
	{
		m_bRender = false;
		m_fDissolveAcc = 0.f;
		m_iShaderPass = 0;
	}

}

void CNPCWalk::LateTick(_float fTimeDelta)
{
	if ((true == m_bRenderState) && (false == m_bRender))
	{
		if (nullptr == m_pRendererCom)
			return;

		m_pModelCom->Play_Animation_NoMove(fTimeDelta);

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
}

HRESULT CNPCWalk::Render()
{
	if (true == m_bRenderState)
	{
		if (nullptr == m_pModelCom ||
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

		//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

		// For.Blur
		_float fBlurPower = 0.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPass)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CNPCWalk::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("NPC_Metro")->Set_Size(vSize);
	Get_Colliders("NPC_Metro")->Set_Center(vCenter);
	ImGui::End();
}

void CNPCWalk::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_Walk", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_fNPCAlpha = 0.5f;
		}
		else
			m_fNPCAlpha = 1.f;

	}
}



HRESULT CNPCWalk::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	switch (m_NPCWalkDesc.iModelIndex)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon5"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 6:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon6"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 7:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon7"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 8:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon8"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 9:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon9"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 10:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon10"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 11:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon11"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 12:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon12"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 13:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon13"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);
	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_Walk"); // Ãæµ¹Ã¼tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

void CNPCWalk::Check_Playernearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();
	_float4 vDir = { 0.f,0.f,0.f,1.f };

	XMStoreFloat4(&vDir, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&vPlayerPos));

	_float fLength = XMVector4Length(XMLoadFloat4(&vDir)).m128_f32[0];
	if (45.f < fLength)
		m_bRenderState = false;
	else
		m_bRenderState = true;


}


CNPCWalk * CNPCWalk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCWalk*		pInstance = new CNPCWalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCWalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCWalk::Clone(void * pArg)
{
	CNPCWalk*		pInstance = new CNPCWalk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCWalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCWalk::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pRendererCom);
	//Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTexture);
}
