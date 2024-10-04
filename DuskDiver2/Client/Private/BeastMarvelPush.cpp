#include "stdafx.h"
#include "..\Public\BeastMarvelPush.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Camera_Manager.h"
#include "Player01Pillar3.h"
#include "Player01Wind1.h"

CBeastMarvelPush::CBeastMarvelPush(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastMarvelPush::CBeastMarvelPush(const CBeastMarvelPush & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastMarvelPush::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastMarvelPush::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_pInfo = (*(PUSHINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (FAILED(Create_Navigation(m_pInfo->iNaviNum)))
		return E_FAIL;

	m_sTag = "Tag_PushObj";
	Make_ImGui();
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = XMLoadFloat4(&m_pInfo->vPosition);
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Identity);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_Scale(_vector{ 0.01f,0.01f,0.01f });

	return S_OK;
}

void CBeastMarvelPush::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
		fTimeDelta *= m_fTMRatio;
		Emp_Col();

		m_fParticleAcc += 1.f * fTimeDelta;
		if (m_fParticleAcc >= 0.1f)
		{
			Create_Particle();
			m_fParticleAcc = 0.f;
		}

		if (m_bStartFix)
		{
			m_pInfo->vRatio.x += m_pInfo->vRatioSpeed.x * fTimeDelta;
			if (m_pInfo->vRatio.x >= 0.5f)
				m_pInfo->vRatio.x = 0.5f;

			m_pInfo->vRatio.y += m_pInfo->vRatioSpeed.y * fTimeDelta;
			if (m_pInfo->vRatio.y >= 1.f)
				m_pInfo->vRatio.y = 1.f;


			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + XMLoadFloat3(&m_pInfo->vScaleSpeed) * fTimeDelta);

			if (m_pTransformCom->Get_Scale().x >= 1.f)
			{
				m_pTransformCom->Set_Scale(_vector{ 1.f,1.f,1.f });
				m_bStartFix = false;
			}
		}

		if (EM->Get_MarbleCol() && !m_bColOff)
		{
			m_bColOff = true;
		}

		if (EM->Get_MarbleEnd() && !m_bEndOff)
		{
			m_bEndOff = true;
		}

		if (m_bColOff || m_bEndOff)
		{
			m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
			if (m_pInfo->vRatio.x <= 0.f)
				m_pInfo->vRatio.x = 0.f;

			m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;
			if (m_pInfo->vRatio.y <= 0.f)
				m_pInfo->vRatio.y = 0.f;

			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - XMLoadFloat3(&m_pInfo->vScaleSpeed) *fTimeDelta);

			if (m_pTransformCom->Get_Scale().x <= 0.2f)
			{
				m_pTransformCom->Set_Scale(_vector{ 0.01f,0.01f,0.01f });
				Set_Off();
				m_bRender = false;
			}

		}

	}


}

void CBeastMarvelPush::LateTick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (nullptr == m_pRendererCom)
			return;

		Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

		Choose_Col("Obj_Push_Range");
		Render_Col(m_pRendererCom);

	}
}

HRESULT CBeastMarvelPush::Render()
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

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		_float Zero = 0.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastMarvelPush::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Obj_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Push_Range", Desc.pOtherDesc->sTag))
		{
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}
	}

	else if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Obj_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
		{
			//밀어내기
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}
	}

}


void CBeastMarvelPush::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	((COBB*)Get_Colliders("Monster_Attack"))->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}


void CBeastMarvelPush::Reset_Position()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfo->vPosition));
}

void CBeastMarvelPush::Set_On()
{
	Reset_Position();
	m_bRender = true;
	m_bStartFix = true;
	m_bColOff = false;
	m_bEndOff = false;
	m_fParticleAcc = 0.f;
	Create_OnParticle();

}

void CBeastMarvelPush::Set_Off()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.1f);
	switch (m_pInfo->iIndex)
	{
	case 1:
	{

		PTM->Create_InstanceParticle(L"BeastPushOff", Matrix);

		PTM->Create_InstanceParticle(L"BeastPillarDistortion", m_pTransformCom->Get_WorldFloat4x4());
		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 1.f,0.f,0.f,1.f };
		Info3->vColor2 = { 0.f,0.f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 1.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 100.f,30.f,100.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);
	}
	break;
	case 2:
	{
		PTM->Create_InstanceParticle(L"BeastPushOff_2", Matrix);

		PTM->Create_InstanceParticle(L"BeastPillarDistortion", m_pTransformCom->Get_WorldFloat4x4());
		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 1.f,1.f,0.f,1.f };
		Info3->vColor2 = { 1.f,0.f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 1.f,1.f,0.f,1.f };
		Info5->vColor2 = { 1.f,0.f,0.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 100.f,30.f,100.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);
	}
	break;
	case 3:
	{
		PTM->Create_InstanceParticle(L"BeastPushOff_3", Matrix);

		PTM->Create_InstanceParticle(L"BeastPillarDistortion", m_pTransformCom->Get_WorldFloat4x4());
		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 0.f,0.f,1.f,1.f };
		Info3->vColor2 = { 0.f,0.5f,1.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 0.f,0.f,1.f,1.f };
		Info5->vColor2 = { 0.f,0.5f,1.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 100.f,30.f,100.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);
	}
	break;
	case 4:
	{
		PTM->Create_InstanceParticle(L"BeastPushOff_4", Matrix);

		PTM->Create_InstanceParticle(L"BeastPillarDistortion", m_pTransformCom->Get_WorldFloat4x4());
		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,1.f,1.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 1.f,1.f,1.f,1.f };
		Info5->vColor2 = { 0.f,1.f,1.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 100.f,30.f,100.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);
	}
	break;
	}


}

void CBeastMarvelPush::Create_Particle()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	switch (m_pInfo->iIndex)
	{
	case 1:
		PTM->Create_InstanceParticle(L"BeastPush", Matrix);
		break;
	case 2:
		PTM->Create_InstanceParticle(L"BeastPush_2", Matrix);
		break;
	case 3:
		PTM->Create_InstanceParticle(L"BeastPush_3", Matrix);
		break;
	case 4:
		PTM->Create_InstanceParticle(L"BeastPush_4", Matrix);
		break;
	}
}

void CBeastMarvelPush::Create_OnParticle()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	switch (m_pInfo->iIndex)
	{
	case 1:
		PTM->Create_InstanceParticle(L"BeastPushOn", Matrix);
		break;
	case 2:
		PTM->Create_InstanceParticle(L"BeastPushOn_2", Matrix);
		break;
	case 3:
		PTM->Create_InstanceParticle(L"BeastPushOn_3", Matrix);
		break;
	case 4:
		PTM->Create_InstanceParticle(L"BeastPushOn_4", Matrix);
		break;
	}
}

HRESULT CBeastMarvelPush::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast_Crystal"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Obj_Push_Range"); //플레이어한테 밀려야하고 방해물한테는 밀려나면 안됨
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeastMarvelPush::Create_Navigation(_uint iNaviNum)
{
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	switch (iNaviNum)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfo->vPosition));
	_uint iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}

CBeastMarvelPush * CBeastMarvelPush::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastMarvelPush*		pInstance = new CBeastMarvelPush(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastMarvelPush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastMarvelPush::Clone(void * pArg)
{
	CBeastMarvelPush*		pInstance = new CBeastMarvelPush(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastMarvelPush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastMarvelPush::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
}




void CBeastMarvelPush::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	//_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	//_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	//ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	//ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	//Get_Colliders("Monster_Attack")->Set_Size(vSize);
	//Get_Colliders("Monster_Attack")->Set_Center(vCenter);



	ImGui::End();
}
