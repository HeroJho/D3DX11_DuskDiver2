#include "stdafx.h"
#include "..\Public\NPCZengbo.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CNPCZengbo::CNPCZengbo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCZengbo::CNPCZengbo(const CNPCZengbo & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCZengbo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCZengbo::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // 객체tag

	m_pModelCom->Set_AnimIndex(STATE_IDLE);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(8.5f, 0.f, 173.5f, 1.f)); //정확한 위치
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f)); // 디버깅 위치
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	return S_OK;
}

void CNPCZengbo::Tick(_float fTimeDelta)
{
	m_bAction = PlayerNearby();

	if ((3 < m_iTalkCnt) && (false == m_bAction))
		m_bDead = true;

	//================================
	/*m_bAction = true;
	Move(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f + NewX, 0.f, 0.f + NewZ, 1.f));*/
	//================================

}

void CNPCZengbo::LateTick(_float fTimeDelta)
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

HRESULT CNPCZengbo::Render()
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

void CNPCZengbo::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("NPC_Zengbo")->Set_Size(vSize);
	Get_Colliders("NPC_Zengbo")->Set_Center(vCenter);
	ImGui::End();
}

void CNPCZengbo::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_Zengbo", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			UM->Set_TalkButtonType(0);
			UM->Set_TalkButton(true);
			if ((CK_UP(DIK_C)) && (!UM->Get_TalkOnOff()))
			{
				CCamera_Main::CUTDESC CamDesc;
				CamDesc.vPos = _float3(11.9f, 2.5f, 173.3f);
				CamDesc.vPos.z -= 1.f;
				CamDesc.vPos.y -= 1.f;
				CamDesc.vLook = _float3(10.42f, 1.3f, 172.5f);
				//CamDesc.vLook.y += 1.f;
				CamDesc.bLerp = true;
				CamDesc.bLookLerp = true;
				CamDesc.fLerpSpeed = 3.f;
				CamDesc.fLookLerpSpeed = 3.f;
				CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
				CamDesc.pTran = nullptr;
				CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
				CamDesc.bPixLocal = false;
				CRM->Get_Cam()->Show_Target(CamDesc);
				// 대화를 이어 나가는 함수
				switch (m_iTalkCnt)
				{
				case 0:
					GI->PlaySoundW(L"NpcJ_03.wav", SD_MAP, 0.8f);
					UM->Set_TalkIndex(32);
					UM->Set_FrontUI(false);
					break;
				case 1:
					GI->PlaySoundW(L"NpcJ_00.wav", SD_MAP, 0.8f);
					UM->Set_TalkIndex(37);
					UM->Set_FrontUI(false);
					UM->Set_QuestPointOff();
					break;
				case 2:
					GI->PlaySoundW(L"NpcJ_01.wav", SD_MAP, 0.8f);
					UM->Set_TalkIndex(45);
					UM->Set_CoinOn(false); // 수집 UI off
					UM->Set_FrontUI(false);
					break;
				case 3:
					GI->PlaySoundW(L"Hero03_ba_71.wav", SD_MAP, 0.8f);
					UM->Set_QuestPointOff();
					UM->Set_TalkIndex(55);
					UM->Set_FrontUI(false);
					break;
				default:
					break;
				}
				++m_iTalkCnt;
			}
		}

	}
}



HRESULT CNPCZengbo::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCZengbo"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);
	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.7f, 1.7f, 1.7f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_Zengbo"); // 충돌체tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

_bool CNPCZengbo::PlayerNearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	if ((185.f > vPlayerPos.z) && (150.f < vPlayerPos.z) && ((6.f < vPlayerPos.x) && (50.f > vPlayerPos.x)))
		return true;

	return false;
}

void CNPCZengbo::Move(_float fTimeDelta)
{
	if (CK_PRESS(DIK_UP))
	{
		NewX += 0.5f;
	}
	else if (CK_PRESS(DIK_DOWN))
	{
		NewX -= 0.5f;
	}
	if (CK_PRESS(DIK_LEFT))
	{
		NewZ += 0.5f;
	}
	else if (CK_PRESS(DIK_RIGHT))
	{
		NewZ -= 0.5f;
	}

}


CNPCZengbo * CNPCZengbo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCZengbo*		pInstance = new CNPCZengbo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCZengbo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCZengbo::Clone(void * pArg)
{
	CNPCZengbo*		pInstance = new CNPCZengbo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCZengbo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCZengbo::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
