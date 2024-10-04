#include "stdafx.h"
#include "..\Public\NPCIdle.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"

CNPCIdle::CNPCIdle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCIdle::CNPCIdle(const CNPCIdle & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCIdle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCIdle::Initialize(void * pArg)
{
	//{ 58.f, 0.f, 170.4f, 1.f }, // 감자
	//{ 58.f,0.f,166.8f,1.f }, // 소세지
	//{ 58.f,0.f,163.2f,1.f }, // 와플
	//{ 58.f,0.f,156.f,1.f }, // 지파이
	//{ 58.f,0.f,174.f,1.f }, // 오징어
	//{ 58.f,0.f,159.5f,1.f }  // 과일
	_float3 vPosition[6] = {
		{ 58.9f,0.f,170.4f }, // 감자
		{ 58.9f,0.f,166.8f }, // 소세지
		{ 58.9f,0.f,163.2f }, // 와플
		{ 58.f,0.f,156.8f }, // 지파이
		{ 58.f,0.f,173.2f }, // 오징어
		{ 58.9f,0.f,159.5f } // 과일
	};
	_float fRotationAngle[6] = { -90.f, -90.f, -90.f, -180.f, 0.f, -90.f};

	m_NPCIdleDesc = *(NPCIDLEDESC*)pArg;

	if ((1 == m_NPCIdleDesc.iNPCIndex) || (4 == m_NPCIdleDesc.iNPCIndex))
		m_bDead = true;

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // 객체tag

	m_pModelCom->Set_AnimIndex(m_NPCIdleDesc.iIdleIndex);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPosition[m_NPCIdleDesc.iNPCIndex].x, vPosition[m_NPCIdleDesc.iNPCIndex].y, vPosition[m_NPCIdleDesc.iNPCIndex].z, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRotationAngle[m_NPCIdleDesc.iNPCIndex]));


	return S_OK;
}

void CNPCIdle::Tick(_float fTimeDelta)
{
	m_bAction = true; // 몬스터의 갯수를 판단.

	if (true == m_bAction)
		Check_Playernearby();



}

void CNPCIdle::LateTick(_float fTimeDelta)
{
	if ((true == m_bRenderState) && (true == m_bAction))
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

HRESULT CNPCIdle::Render()
{
	if ((true == m_bRenderState) && (true == m_bAction))
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

void CNPCIdle::Render_Debug()
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

void CNPCIdle::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_Idle", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_fNPCAlpha = 0.5f;
		}
		else
			m_fNPCAlpha = 1.f;

	}
}



HRESULT CNPCIdle::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	switch (m_NPCIdleDesc.iModelIndex)
	{
	case 11:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon11"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 13:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCCommon13"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	m_pModelCom->Set_RootHierarchy(1);
	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this; 
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_Idle"); // 충돌체tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

void CNPCIdle::Check_Playernearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();
	_float4 vDir = { 0.f,0.f,0.f,1.f };

	XMStoreFloat4(&vDir, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&vPlayerPos));

	_float fLength = XMVector4Length(XMLoadFloat4(&vDir)).m128_f32[0];
	if (30.f < fLength)
		m_bRenderState = false;
	else
		m_bRenderState = true;
}





CNPCIdle * CNPCIdle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCIdle*		pInstance = new CNPCIdle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCIdle::Clone(void * pArg)
{
	CNPCIdle*		pInstance = new CNPCIdle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCIdle::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pRendererCom);
	//Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
