#include "stdafx.h"
#include "..\Public\NPCKwon.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"

CNPCKwon::CNPCKwon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CNPCKwon::CNPCKwon(const CNPCKwon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPCKwon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCKwon::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.5f, 0.95f, 158.5f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.95f, 10.5f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	return S_OK;
}

void CNPCKwon::Tick(_float fTimeDelta)
{
	m_bAction = PlayerNearby();
	//m_bAction = true;
}

void CNPCKwon::LateTick(_float fTimeDelta)
{
	if (true == m_bAction)
	{
		if (nullptr == m_pRendererCom)
			return;

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
}

HRESULT CNPCKwon::Render()
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

void CNPCKwon::Render_Debug()
{
}

void CNPCKwon::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_Kwon", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			UM->Set_TalkButtonType(5);
			UM->Set_TalkButton(true);
			if (CK_DOWN(DIK_C))
			{
				UM->Set_TalkIndex(29);
				UM->Set_FrontUI(false);
			}
		}

	}
}



HRESULT CNPCKwon::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCKwon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_Kwon");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

_bool CNPCKwon::PlayerNearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	if ((185.f > vPlayerPos.z) && (150.f < vPlayerPos.z) && ((10.f < vPlayerPos.x) && (70.f > vPlayerPos.x)))
		return true;

	return false;
}





CNPCKwon * CNPCKwon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCKwon*		pInstance = new CNPCKwon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCKwon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCKwon::Clone(void * pArg)
{
	CNPCKwon*		pInstance = new CNPCKwon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCKwon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCKwon::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
