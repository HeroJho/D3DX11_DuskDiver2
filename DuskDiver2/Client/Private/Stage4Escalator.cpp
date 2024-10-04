#include "stdafx.h"
#include "..\Public\Stage4Escalator.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"

CStage4Escalator::CStage4Escalator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CStage4Escalator::CStage4Escalator(const CStage4Escalator & rhs)
	: CCreture(rhs)
{
}

HRESULT CStage4Escalator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Escalator::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Escalator"; // °´Ã¼tag
		return S_OK;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.4f + NewX, -7.8f, -4.9f + NewZ, 1.f));
}

void CStage4Escalator::Tick(_float fTimeDelta)
{
	m_iModelIndex = PlayerNearby();
	Set_Position(m_iModelIndex);
	Move_Player(fTimeDelta);

	//================================
	/*Move(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.12f + NewX, 15.65f , 263.4f + NewZ, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.82f, 1.f, 1.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));*/
	//================================

	/*m_fPlayerPos = PM->Get_PlayerPos();
	m_bOn = m_pTransformCom->Check_LimitDistance(XMLoadFloat4(&m_fPlayerPos), 5.f);

	if (m_bOn)
	{
		if (!m_bEscalator)
		{
			GI->PlaySoundW(L"se_DVEnergy_Open.wav", SD_TRAININGBOT1, 0.2f);
			m_bEscalator = true;
		}
	}

	else
	{
		GI->StopSound(SD_TRAININGBOT1);
		m_bEscalator = false;
	}*/
}

void CStage4Escalator::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(fTimeDelta * 0.1f, m_pTransformCom, nullptr);

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
	
}

HRESULT CStage4Escalator::Render()
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
	

	return S_OK;
}

void CStage4Escalator::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Stage4Escalator")->Set_Size(vSize);
	Get_Colliders("Stage4Escalator")->Set_Center(vCenter);
	ImGui::End();
}

void CStage4Escalator::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Stage4Escalator", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			
		}

	}
}



HRESULT CStage4Escalator::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_Escalator"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);
	

	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	//ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.7f, 1.7f, 1.7f);
	//ColDesc.pOwner = this;
	//ColDesc.iFlag = 0;
	//strcpy(ColDesc.sTag, "Stage4Escalator"); // Ãæµ¹Ã¼tag
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;


	return S_OK;
}

_uint CStage4Escalator::PlayerNearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	if (80.f > vPlayerPos.z)
		return 0;

	return 1;
}

void CStage4Escalator::Set_Position(_uint iModelIndex)
{
	if (0 == iModelIndex)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.41f, -7.8f, -4.9f, 1.f));
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1.82f, 1.f, 1.f, 1.f));
	}
	else if (1 == iModelIndex)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.12f, 15.65f, 263.4f, 1.f));
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1.82f, 1.f, 1.f, 1.f));
	}

}

void CStage4Escalator::Move_Player(_float fTImeDelta)
{
	_int iNaviIndex = PM->Get_NaviIndex(PM->Get_SelectPlayer());

	if ((1747 == iNaviIndex) || (1748 == iNaviIndex) || (1749 == iNaviIndex) || (1750 == iNaviIndex) || (1751 == iNaviIndex) || (1752 == iNaviIndex))
	{
		_float4 vPositionChanges = { 0.f,0.f,0.f,1.f };
		vPositionChanges.z -= 1.f * fTImeDelta;

		PM->Change_PlayerPos(PM->Get_SelectPlayer(), vPositionChanges);
	}
	else if ((1772 == iNaviIndex) || (1773 == iNaviIndex) || (1774 == iNaviIndex) || (1775 == iNaviIndex) || (1776 == iNaviIndex) || (1777 == iNaviIndex))
	{
		_float4 vPositionChanges = { 0.f,0.f,0.f,1.f };
		vPositionChanges.z += 1.f * fTImeDelta;

		PM->Change_PlayerPos(PM->Get_SelectPlayer(), vPositionChanges);
	}
}

void CStage4Escalator::Move(_float fTimeDelta)
{
	if (CK_PRESS(DIK_UP))
	{
		NewX += 0.1f;
	}
	else if (CK_PRESS(DIK_DOWN))
	{
		NewX -= 0.1f;
	}
	if (CK_PRESS(DIK_LEFT))
	{
		NewZ += 1.f;
	}
	else if (CK_PRESS(DIK_RIGHT))
	{
		NewZ -= 0.1f;
	}

}


CStage4Escalator * CStage4Escalator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Escalator*		pInstance = new CStage4Escalator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Escalator"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Escalator::Clone(void * pArg)
{
	CStage4Escalator*		pInstance = new CStage4Escalator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Escalator"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage4Escalator::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
