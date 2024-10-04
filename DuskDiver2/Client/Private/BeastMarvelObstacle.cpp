#include "stdafx.h"
#include "..\Public\BeastMarvelObstacle.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "BeastAura.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01Wind1.h"
#include "Particle_Manager.h"
#include "Effect_Manager.h"

CBeastMarvelObstacle::CBeastMarvelObstacle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastMarvelObstacle::CBeastMarvelObstacle(const CBeastMarvelObstacle & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastMarvelObstacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastMarvelObstacle::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	SetUp_AppearLimitTime();

	m_fLimitScale = 3.f;
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixIdentity());
	m_pTransformCom->Set_WorldMatrix(Matrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CretureInfo.vPosition));
	m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));

	CBeastAura::AURAINFO* Info = new CBeastAura::AURAINFO;
	Info->fAngle = 0.f;
	Info->fBlurPower = 1.f;
	Info->vColor = { 0.f, 0.f, 0.f, 1.f };
	Info->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info->vOriginRatio = { 1.f,2.5f };
	Info->vOriginScale = { 40.f,40.f,40.f };
	Info->vRatio = { 1.f,2.5f };
	Info->vRatioSpeed = { 2.f,2.f };
	Info->vScale = { 0.1f,0.1f,0.1f };
	Info->vScaleSpeed = { 1.f,1.f,1.f };
	Info->vUVFixSpeed = { 0.f, 0.5f };
	Info->fTurnSpeed = 4.f;
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_Aura.push_back(GI->Clone_GameObject(L"BeastAura", &Info));

	CBeastAura::AURAINFO* Info2 = new CBeastAura::AURAINFO;
	Info2->fAngle = 112.f;
	Info2->fBlurPower = 1.f;
	Info2->vColor = { 0.f, 0.f, 0.f, 1.f };
	Info2->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info2->vOriginRatio = { 1.f,2.5f };
	Info2->vOriginScale = { 35.f,35.f,35.f };
	Info2->vRatio = { 1.f,2.5f };
	Info2->vRatioSpeed = { 2.f,2.f };
	Info2->vScale = { 0.1f,0.1f,0.1f };
	Info2->vScaleSpeed = { 1.f,1.f,1.f };
	Info2->vUVFixSpeed = { 0.f, 0.5f };
	Info2->fTurnSpeed = 6.f;
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_Aura.push_back(GI->Clone_GameObject(L"BeastAura", &Info2));	

	return S_OK;
}

void CBeastMarvelObstacle::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	/*if (m_bAppear)
	{
		m_fAppearAcc += 1.f * fTimeDelta;

		if (m_fAppearAcc >= m_fDelayAppear)
		{
			m_bRender = true;
			m_bAppear = false;
			m_fAppearAcc = 0.f;
			
		}
	}*/


	if (m_bRender)
	{
		
		for (auto& iter : m_Aura)
			iter->Tick(fTimeDelta);

		if (EM->Get_MarbleCol() && !m_bMarbleColEffect)
		{
			m_fAppearAcc += 1.f * fTimeDelta;
			if (m_fAppearAcc >= m_fDelayAppear)
			{			
				for (auto& iter : m_Aura)
					((CBeastAura*)iter)->Set_Off();

				m_bMarbleColEffect = true;
			}		
		}

		if (EM->Get_MarbleCol() || EM->Get_MarbleEnd())
		{
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - _vector{ 2.f,2.f,2.f } * fTimeDelta);

			if (m_pTransformCom->Get_Scale().x <= 0.2f)
				m_bRender = false;
		}
		else
			Change_ObstacleSize(fTimeDelta);
	}

}

void CBeastMarvelObstacle::LateTick(_float fTimeDelta)
{
	//렌더 키고 끄기
	if (nullptr == m_pRendererCom)
		return;
	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	if (m_bRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Choose_Col("Monster_Push_Range");
		Choose_Col("Obstacle_Body");
		for (auto& iter : m_Aura)
			iter->LateTick(fTimeDelta);
	}
	Render_Col(m_pRendererCom);

}

HRESULT CBeastMarvelObstacle::Render()
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}
	return S_OK;
}

void CBeastMarvelObstacle::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//플레이어랑 충돌처리 할거는 미는거 밖에없는데 이거는 플레이어안에서 해주고있음
	//구슬이랑 충돌처리는 body로 
	//미는것도 구슬안에서 해줄것

}

void CBeastMarvelObstacle::On_Hit(CCreture * pOther)
{
	//볼이랑 맞닿은거 판정
	if (m_bOneHit)
	{
		m_bOneHit = false;		
		//비스트에서 방해물 중에 하나라도 m_bExplosion이 true가 되면 나머지도 다 사라져야한다 
	}

}

void CBeastMarvelObstacle::Reset_State(_float4 vPos)
{
	m_bAppear = false;
	m_bRender = true;
	m_bOneHit = true;
	m_bSizeUp = false;
	m_bChangeScale = false;
	m_bMarbleColEffect = false;
	m_fObstacleScale = 0.1f;
	m_fTotalScale = 0.f;
	m_fDelayAcc = 0.f;
	m_fPlusScale = 0.f;
	m_fAppearAcc = 0.f;

	vPos.y += 1.f;
	for (auto& iter : m_Aura)
		((CBeastAura*)iter)->Set_On(vPos);

	vPos.y += 0.3f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	

}

HRESULT CBeastMarvelObstacle::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_BlastBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); //플레이어, 구슬 밀어내기 위함
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.9f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.9f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Obstacle_Body"); //구슬이랑 충돌

	//플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	//필요한 충돌체 - 플레이어 밀어내야하고, 구슬 밀어내야하고, 구슬이랑 충돌해야함



	return S_OK;
}

void CBeastMarvelObstacle::SetUp_AppearLimitTime()
{
	switch (m_CretureInfo.iIndex)
	{
	case 0:
		m_fDelayAppear = 0.2f;
		break;
	case 1:
		m_fDelayAppear = 0.5f;
		break;
	case 2:
		m_fDelayAppear = 1.f;
		break;
	case 3:
		m_fDelayAppear = 1.2f;;
		break;
	case 4:
		m_fDelayAppear = 0.3f;;
		break;
	case 5:
		m_fDelayAppear = 0.4f;;
		break;
	case 6:
		m_fDelayAppear = 0.6f;;
		break;
	case 7:
		m_fDelayAppear = 0.7f;;
		break;
	case 8:
		m_fDelayAppear = 0.9f;;
		break;
	}
}

void CBeastMarvelObstacle::Change_ObstacleSize(_float fTimeDelta)
{
	//처음 생성될때 되게 작았다가 일정사이즈되면 반복적으로 커졌다 줄어졌다 하기
	//충돌체는 사이즈 고정
	m_fObstacleScale += 3.7f * fTimeDelta;

	if (m_fObstacleScale >= m_fLimitScale)
	{
		m_bChangeScale = true;
		m_fObstacleScale = m_fLimitScale;
	}

	if (m_bChangeScale)
		m_fDelayAcc += 1.f * fTimeDelta;

	if (m_fDelayAcc > 1.f)
	{
		if (m_bSizeUp)
			m_fPlusScale += 15.f * fTimeDelta;
		else
			m_fPlusScale -= 15.f * fTimeDelta;

		if (m_fPlusScale >= 4.f)
		{
			m_bSizeUp = false;
			for (auto& iter : m_Aura)
				((CBeastAura*)iter)->Create_Effect();
		}
		if (m_fPlusScale <= 0.f)
		{
			m_bSizeUp = true;
			m_fDelayAcc = 0.f;
			
		}
		
	}

	m_fTotalScale = m_fObstacleScale + m_fPlusScale;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fTotalScale, m_fTotalScale, m_fTotalScale, 0.f));
}

CBeastMarvelObstacle * CBeastMarvelObstacle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastMarvelObstacle*		pInstance = new CBeastMarvelObstacle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastMarvelObstacle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastMarvelObstacle::Clone(void * pArg)
{
	CBeastMarvelObstacle*		pInstance = new CBeastMarvelObstacle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastMarvelObstacle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastMarvelObstacle::Free()
{
	__super::Free();
	for (auto& iter : m_Aura)
		Safe_Release(iter);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}




void CBeastMarvelObstacle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Obstacle_Body")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Obstacle_Body")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Obstacle_Body")->Set_Size(vSize);
	Get_Colliders("Obstacle_Body")->Set_Center(vCenter);



	ImGui::End();
}
