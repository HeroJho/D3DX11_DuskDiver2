#include "stdafx.h"
#include "..\Public\Stage3AzitEvent.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "CluePic.h"

CStage3AzitEvent::CStage3AzitEvent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage3AzitEvent::CStage3AzitEvent(const CStage3AzitEvent & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage3AzitEvent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3AzitEvent::Initialize(void * pArg)
{
	m_AzitEventDesc = *(AZITEVENTDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage3_Prop";

	Make_ImGui();

	InitStage();

	return S_OK;
}

void CStage3AzitEvent::Tick(_float fTimeDelta)
{
	Emp_Col();

	if (40 == UM->Get_TalkIndex())
		m_bActivate = true;

	//m_bActivate = true; // debug

	if (true == m_bOn)
	{
		// 0 번 트리거라면 
		if (0 == m_AzitEventDesc.iAzitEventIndex)
		{
			if (false == m_bClueFound)
			{
				// 카메라가 컷신을 시작

				// 사진이 움직이기 시작한다.
				m_bClueFound = true;
				m_pCluePic->Clue_Found();
			}

			// 컷신이 시작되면 움직인다 사진이 (임시로 그냥 움직이게)
			m_pCluePic->Tick(fTimeDelta);
		}
		else
		{
			// 카메라가 컷신을 진행한다
			if (false == m_bClueFound)
			{
				// 카메라가 컷신을 시작

				// 사진이 움직이기 시작한다.
				m_bClueFound = true;
			}
		}
	}

	// debug
	//Move();
}

void CStage3AzitEvent::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	if (0 == m_AzitEventDesc.iAzitEventIndex)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pCluePic->LateTick(fTimeDelta);
	}
		
}

HRESULT CStage3AzitEvent::Render()
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

	_float fGama = 2.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
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

void CStage3AzitEvent::Render_Debug()
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

void CStage3AzitEvent::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ((true == m_bActivate) && ("Tag_Player" == Desc.pOther->Get_Tag()))
	{
		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			if (CK_DOWN(DIK_C))
			{
				m_bOn = true;
			}
		}

	}

}



HRESULT CStage3AzitEvent::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_DXBook"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

void CStage3AzitEvent::InitStage()
{
	_float4 vInitPos[3] = {
		{ 20.2f,0.95f,158.5f,1.f }, // 책 좌표
		{ 16.33f,0.5f,164.95f,1.f }, // 1인1메뉴
		{ 24.15f,0.5f,174.44f,1.f } // 구슬 아이스
	};
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vInitPos[m_AzitEventDesc.iAzitEventIndex]));

	if (0 == m_AzitEventDesc.iAzitEventIndex)
		m_pCluePic = (CCluePic*)GI->Clone_GameObject(TEXT("CluePic"));
}

void CStage3AzitEvent::Move()
{
	//if (CK_DOWN(DIK_NUMPADENTER))
	//{
	//	fSpeed *= 10.f;
	//	if (10.f < fSpeed)
	//		fSpeed = 0.1f;
	//}
		

	if (CK_PRESS(DIK_RSHIFT))
	{
		if (CK_PRESS(DIK_UP))
		{
			fY += 0.1f;
		}
		if (CK_PRESS(DIK_DOWN))
		{
			fY -= 0.1f;
		}
	}
	else
	{
		if (CK_PRESS(DIK_UP))
		{
			fZ += 0.1f;
		}
		if (CK_PRESS(DIK_DOWN))
		{
			fZ -= 0.1f;
		}
	}
	if (CK_PRESS(DIK_LEFT))
	{
		fX -= 0.1f;
	}
	if (CK_PRESS(DIK_RIGHT))
	{
		fX += 0.1f;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.2f + fX, 0.95f + fY, 158.5f + fZ, 1.f));



}


CStage3AzitEvent * CStage3AzitEvent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3AzitEvent*		pInstance = new CStage3AzitEvent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3AzitEvent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage3AzitEvent::Clone(void * pArg)
{
	CStage3AzitEvent*		pInstance = new CStage3AzitEvent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3AzitEvent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage3AzitEvent::Free()
{
	__super::Free();

	Safe_Release(m_pCluePic);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
