#include "stdafx.h"
#include "..\Public\Yabawe.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "MiniGame_Yabawe.h"
#include "YabaweBall.h"
#include "Camera_Manager.h"

CYabawe::CYabawe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CYabawe::CYabawe(const CYabawe & rhs)
	: CCreture(rhs)
{
}



HRESULT CYabawe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYabawe::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *(YABAWEDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_fHurtTime = 0.02f;
	m_eHitType = TYPE_HIT;
	m_fAirPower = 2.f;
	m_fPushPower = 4.f;
	m_fDamage = 0.f;

	_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_Desc.vPos), XMLoadFloat4x4(&m_Desc.mMatrix));

	XMStoreFloat3(&m_vInitPos, vPos);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Look(XMVectorSetW(XMLoadFloat4x4(&m_Desc.mMatrix).r[2], 0.f));
	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));

	
	
	return S_OK;
}





void CYabawe::Set_Root(_float3 * vRoot, _bool bInv, _float fSpeed, _float fSens, _float fStopDis)
{
	m_BasiRoots.clear();
	m_fT = 0.f;
	m_fSpeed = fSpeed;
	m_fSens = fSens;
	m_fStopDis = fStopDis;

	while (1.f >= m_fT)
	{
		if (bInv)
		{
			m_vRealDestPos = vRoot[0];
			m_BasiRoots.push_back(TM->GetBesierPos(vRoot[2], vRoot[1], vRoot[0], m_fT));
		}

		else
		{
			m_vRealDestPos = vRoot[2];
			m_BasiRoots.push_back(TM->GetBesierPos(vRoot[0], vRoot[1], vRoot[2], m_fT));
		}
		m_fT += m_fSens;
	}

	m_bMove = true;

	// 처음 위치는 같을거다
	m_vDestPos = m_BasiRoots.front();
	m_BasiRoots.pop_front();
}





void CYabawe::Tick(_float fTimeDelta)
{

	if (TM->Get_MiniGameYabawe()->Get_End())
	{
		Tick_End(fTimeDelta);
	}
	else
	{
		Tick_Game(fTimeDelta);
	}

}

void CYabawe::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	//if (TM->Get_YavaweBallStart())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CYabawe::Render()
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
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;



		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CYabawe::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Body1", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (!m_bAttackCol)
				return;

			m_eHitType = TYPE_SPIN;
			m_fAirPower = 5.f;
			m_fPushPower = 1.5f;
			m_fDamage = 150.f;
			((CCreture*)Desc.pOther)->On_Hit(this);

			m_bAttackCol = false;
		}
	}
}

void CYabawe::On_Hit(CCreture * pOther)
{
	if (m_bHurt)
	{
		m_bHurt = false;
		m_bAttackCol = true;
		m_bHit = true;
		CRM->Get_Cam()->Shake(0.05f, 0.1f);
		XMStoreFloat3(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.7f;
		pOther->Create_HitEffect(vPos);
		GI->PlaySoundW(L"minigame2_hit_pet.wav", SD_NPC, 1.f);
	}
}

HRESULT CYabawe::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CYabawe * CYabawe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYabawe*		pInstance = new CYabawe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CYabawe::Clone(void * pArg)
{
	CYabawe*		pInstance = new CYabawe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYabawe::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CYabawe::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

void CYabawe::Tick_Game(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	Check_Hurt(fTimeDelta);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	//m_pTransformCom->Set_TurnSpeed(fTimeDelta);



	if (m_bMove)
	{

		_vector vDir = XMLoadFloat3(&m_vDestPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fDis = XMVectorGetX(XMVector3Length(vDir));

		// 도착, 다음 베지 이동
		if (m_fStopDis > fDis && !m_bCheckRealDestPos)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vDestPos), 1.f));
			// 완전 도착
			if (m_BasiRoots.empty())
			{

				m_bCheckRealDestPos = true;
				m_fSpeed = 10.f;
				return;
			}

			if (!m_bMoveSound)
			{
				GI->PlaySoundW(L"minigame1_move.wav", SD_MAP, 0.7f);
				m_bMoveSound = true;
			}

			m_vDestPos = m_BasiRoots.front();
			m_BasiRoots.pop_front();
			return;
		}
		else if (m_bCheckRealDestPos)
		{
			vDir = XMLoadFloat3(&m_vRealDestPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			fDis = XMVectorGetX(XMVector3Length(vDir));
			if (0.1f > fDis)
			{
				m_bMove = false;
				m_bCheckRealDestPos = false;
				TM->Get_MiniGameYabawe()->Set_IsShake(false);
				return;
			}
		}


		// 이동
		m_pTransformCom->Go_Dir(XMVectorSetW(vDir, 0.f), m_fSpeed, fTimeDelta);


	}
	else
	{

		if (0 != TM->Get_MiniGameYabawe()->Get_Level() && !TM->Get_MiniGameYabawe()->Get_IsShake())
		{
			if (m_bHit)
			{
				m_fHitTimeAcc += fTimeDelta;
				if (0.2f < m_fHitTimeAcc)
				{
					m_fHitTimeAcc = 0.f;
					m_fHitTimeTurnAcc = 0.f;
					m_bHit = false;
					_float4 vPos; vPos.x = m_vInitPos.x; vPos.y = m_vInitPos.y; vPos.z = m_vInitPos.z; vPos.w = 1.f;
					Set_WorldPos(vPos);
				}


				m_fHitTimeTurnAcc += fTimeDelta;
				if (0.01f < m_fHitTimeTurnAcc)
				{
					m_bHitTurn = !m_bHitTurn;
					m_fHitTimeTurnAcc = 0.f;
				}

				if (m_bHitTurn)
					m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), 0.f), 8.f, fTimeDelta);
				else
					m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), 0.f), -8.f, fTimeDelta);

			}
		}

	}


}

void CYabawe::Tick_End(_float fTimeDelta)
{
}
