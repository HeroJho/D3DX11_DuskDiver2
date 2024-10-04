#include "stdafx.h"
#include "..\Public\Stage2Event2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Crack_Manager.h"

#include "Light.h"

CStage2Event2::CStage2Event2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage2Event2::CStage2Event2(const CStage2Event2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage2Event2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage2Event2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-0.5f, 0.f, -3.95f, 1.f));

	m_fRatio = 0.f;

	m_vOtherSpec = _float4(1.f, 0.3f, 0.3f, 1.f);
	//m_vOtherSpec = _float4(0.8f, 0.8f, 0.8f, 1.f);

	return S_OK;
}

void CStage2Event2::Tick(_float fTimeDelta)
{
	// 맵 이동
	if (CK_DOWN(DIK_C) && m_bOn)
	{
		UM->MoveBlind(true);
		return;
	}


	m_bOn = false;




	// 처음 크랙 사라질 때만 저세계
	if (!CRKM->Get_CrackExist() && !m_bFirstCrack)
	{
		if (!TM->Get_Stage2_2Bool())
		{
			TM->Set_Stage2_2Bool(true);
			list<CLight*>* pLights = GI->Get_Lights();
			m_vInitLightness = (*(pLights->begin()))->Get_LightDesc()->vDiffuse;
			m_bTurnBack = true;

			m_bFirstCrack = true;
		}
	}






	_float fMinRatio = 0.3f;
	if (fMinRatio > m_fRatio)
	{
		if (TM->Get_Stage2_2Bool())
		{
			list<CLight*>* pLights = GI->Get_Lights();
			for (auto& pLight : *pLights)
			{
				XMStoreFloat4(&(pLight->Get_LightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&m_vInitLightness) * m_fRatio, 1.f));
			}

			m_fRatio += fTimeDelta;
		}
	}
	else if(0 < m_iBringMaxCount)
	{
		m_fRatio = fMinRatio - 0.2f;
		--m_iBringMaxCount;
	}
	else
		m_fRatio = fMinRatio;



	if (0 >= m_iBringMaxCount)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeMax < m_fTimeAcc)
		{
			m_fTimeMax = GI->Get_FloatRandom(4.f, 7.f);
			m_iBringMaxCount = GI->Get_Random(2, 5);
			m_fTimeAcc = 0.f;
		}
	}


	if (TM->Get_Stage2_2Bool() && m_fSpecRatio < 1.f)
	{
		if (1.f < m_fSpecRatio)
			m_fSpecRatio = 1.f;

		m_fSpecRatio += fTimeDelta;

		list<CLight*>* pLights = GI->Get_Lights();
		for (auto& pLight : *pLights)
		{
			if (0.f >= pLight->Get_LightDesc()->vSpecular.x)
				continue;
			XMStoreFloat4(&(pLight->Get_LightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&m_vOtherSpec) * m_fSpecRatio, 1.f));
		}
	}




	if (m_bTurnBack && !TM->Get_Stage2_2Bool())
	{
		m_fRatio = 1.f;
		m_fSpecRatio = 1.f;
		m_vOtherSpec = _float4(0.8f, 0.8f, 0.8f, 1.f);
		list<CLight*>* pLights = GI->Get_Lights();
		for (auto& pLight : *pLights)
		{
			XMStoreFloat4(&(pLight->Get_LightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&m_vInitLightness) * m_fRatio, 1.f));

			if (0.f >= pLight->Get_LightDesc()->vSpecular.x)
				continue;
			XMStoreFloat4(&(pLight->Get_LightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&m_vOtherSpec) * m_fSpecRatio, 1.f));
		}

	}


}

void CStage2Event2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage2Event2::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CStage2Event2::Render_Debug()
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

void CStage2Event2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{

			if (19 < UM->Get_TalkIndex())
			{
				UM->Set_TalkButtonType(6);
				UM->Set_TalkButton(true);
				m_bOn = true;
			}


		}

	}
}



HRESULT CStage2Event2::Ready_Components()
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
	ColDesc.vSize = _float3(3.f, 2.f, 4.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CStage2Event2 * CStage2Event2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage2Event2*		pInstance = new CStage2Event2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage2Event1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage2Event2::Clone(void * pArg)
{
	CStage2Event2*		pInstance = new CStage2Event2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage2Event1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage2Event2::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
