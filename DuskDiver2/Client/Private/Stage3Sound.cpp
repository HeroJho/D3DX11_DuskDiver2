#include "stdafx.h"
#include "..\Public\Stage3Sound.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "CrackStone.h"

CStage3Sound::CStage3Sound(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage3Sound::CStage3Sound(const CStage3Sound & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage3Sound::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Sound::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(18.3f, 0.f, 25.4f, 1.f));

	return S_OK;
}

void CStage3Sound::Tick(_float fTimeDelta)
{
	if (!TM->Get_Stage3_Bool() && m_bOnce)
	{ 
		m_fTimeAcc += 1.f * fTimeDelta;

		if (10.f <= m_fTimeAcc)
		{
			m_bOnce = false;
			m_fTimeAcc = 0.f;
		}
	}
}

void CStage3Sound::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage3Sound::Render()
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

void CStage3Sound::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);
	_float3 vSize = Get_Colliders("Potal")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Potal")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Potal")->Set_Size(vSize);
	Get_Colliders("Potal")->Set_Center(vCenter);
	ImGui::End();
}

void CStage3Sound::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			if (!TM->Get_Stage3_Bool())
			{
				if (!m_bOnce)
				{
					GI->PlaySoundW(L"Car Passing.wav", SD_MAP, 0.4f);
					m_bOnce = true;
				}
			}

			//if (60 < UM->Get_TalkIndex())
			if (4 < UM->Get_ClueCnt()) // 단서가 4개 이상이면 IT밸리로 갈수 있는 타이밍
			{
				//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.1f);
				GI->StopSound(SD_NPC);
			}
		}
	}
}



HRESULT CStage3Sound::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Face"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(1.5f, 1.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(130.f, 4.f, 34.8f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CStage3Sound * CStage3Sound::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Sound*		pInstance = new CStage3Sound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Sound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage3Sound::Clone(void * pArg)
{
	CStage3Sound*		pInstance = new CStage3Sound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Sound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage3Sound::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
