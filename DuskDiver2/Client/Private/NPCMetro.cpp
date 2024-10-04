#include "stdafx.h"
#include "..\Public\NPCMetro.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"

CNPCMetro::CNPCMetro(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCMetro::CNPCMetro(const CNPCMetro & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCMetro::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCMetro::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // °´Ã¼tag

	m_pModelCom->Set_AnimIndex(0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-1.f, 0.f, -7.5f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));

	return S_OK;
}

void CNPCMetro::Tick(_float fTimeDelta)
{
	if (25 == UM->Get_TalkIndex())
		m_bAction = true;
}

void CNPCMetro::LateTick(_float fTimeDelta)
{
	if (true == m_bAction)
	{
		if (nullptr == m_pRendererCom)
			return;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, nullptr);

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
}

HRESULT CNPCMetro::Render()
{
	if (true == m_bAction)
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


		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CNPCMetro::Render_Debug()
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

void CNPCMetro::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_Metro", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			UM->Set_TalkButtonType(0);
			UM->Set_TalkButton(true);
			if (CK_UP(DIK_C))
			{
				UM->Set_QuestPointOff();
				UM->Set_TalkIndex(26);
				UM->Set_FrontUI(false);
			}
		}

	}
}



HRESULT CNPCMetro::Ready_Components()
{
	///* For.Com_Transform */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
	//	return E_FAIL;

	///* For.Com_Renderer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCMetro"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_Metro"); // Ãæµ¹Ã¼tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CNPCMetro * CNPCMetro::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCMetro*		pInstance = new CNPCMetro(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCMetro"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCMetro::Clone(void * pArg)
{
	CNPCMetro*		pInstance = new CNPCMetro(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCMetro"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCMetro::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pRendererCom);
	//Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
