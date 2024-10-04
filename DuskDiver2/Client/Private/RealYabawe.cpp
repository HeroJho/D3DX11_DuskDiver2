#include "stdafx.h"
#include "..\Public\RealYabawe.h"
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
#include "Camera_Main.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Bear_HP_Bar.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"

CRealYabawe::CRealYabawe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CYabawe(pDevice, pContext)
{
}

CRealYabawe::CRealYabawe(const CRealYabawe & rhs)
	: CYabawe(rhs)
{
}

HRESULT CRealYabawe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRealYabawe::Initialize(void * pArg)
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

	m_bRealYa = true;

	m_eFace = FACE_SMILE;

	return S_OK;
}

void CRealYabawe::Tick(_float fTimeDelta)
{

	if (TM->Get_MiniGameYabawe()->Get_End())
	{
		End_Tick(fTimeDelta);

		if (!m_bEndSound)
		{
			//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.4f);
			//GI->StopSound(SD_BGM);
			//GI->PlayBGM(L"Arcade.wav", 0.2f);
			m_bEndSound = true; 
		}
	}
	else
	{
		Game_Tick(fTimeDelta);
	}


	//if (m_bShake)
	//	Tick_Shake(fTimeDelta);

}

void CRealYabawe::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);



	if (0 != TM->Get_MiniGameYabawe()->Get_Level() && !TM->Get_MiniGameYabawe()->Get_End())
	{
		m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, 100.f);
		_float4 vPos = Get_WorldPos();
		if (m_vInitPos.y > Get_WorldPos().y)
		{
			vPos.y = m_vInitPos.y;
			Set_WorldPos(vPos);
			m_pTransformCom->ResetGravity();
			TM->Get_MiniGameYabawe()->Get_YabaweBall()->Set_Render(false);
			m_eFace = FACE_IDLE;
		}
	}



	// if(TM->Get_YavaweBallStart())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRealYabawe::Render()
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

	_uint		iNumMeshes = 0;
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_float fAlpha = 0.f;


	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;
	
	if (FACE_SMILE == m_eFace)
	{
		iNumMeshes = m_pModelCom1->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;


		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom1->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}
	else if(FACE_IDLE == m_eFace)
	{
		iNumMeshes = m_pModelCom->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;

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
	}
	else if (FACE_HURT == m_eFace)
	{
		iNumMeshes = m_pModelCom2->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom2->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}



	return S_OK;
}

void CRealYabawe::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Body1", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			m_bHurt = true;
			//m_bAttackCol = false;

			//m_eHitType = TYPE_SPIN;
			//m_fAirPower = 5.f;
			//m_fPushPower = 1.5f;
			//m_fDamage = 0.f;
			//((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}
}

void CRealYabawe::On_Hit(CCreture * pOther)
{
	if (m_bHurt)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.7f;
		pOther->Create_HitEffect(vPos);

		m_bHurt = false;
		m_bHit = true;
		m_eFace = FACE_HURT;
		TM->Get_CumaHpBar()->Deal_PunchHp(1000.f);
		GI->PlaySoundW(L"minigame2_hit_heart_energy.wav", SD_NPC, 1.f);
	}
}

HRESULT CRealYabawe::Ready_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAa"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom1)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAc"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom2)))
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

CRealYabawe * CRealYabawe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRealYabawe*		pInstance = new CRealYabawe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRealYabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CRealYabawe::Clone(void * pArg)
{
	CRealYabawe*		pInstance = new CRealYabawe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRealYabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRealYabawe::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CRealYabawe::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);



	ImGui::End();
}

void CRealYabawe::Start_CutScene()
{
	m_bJump = true;
}

void CRealYabawe::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	m_eFace = FACE_HURT;
	XMStoreFloat3(&m_vInitShakePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}





void CRealYabawe::Game_Tick(_float fTimeDelta)
{
	//움직이기 시작
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
		//도착하고 다시 움직이기
		else if (m_bCheckRealDestPos)
		{
			vDir = XMLoadFloat3(&m_vRealDestPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			fDis = XMVectorGetX(XMVector3Length(vDir));

			if (m_bMoveSound)
			{
				GI->PlaySoundW(L"minigame1_move.wav", SD_MAP, 0.7f);
				m_bMoveSound = false;
			}

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
				m_pTransformCom->Jump(12.f);
				m_bHit = false;

				_float4 vPos; XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				vPos.y = TM->Get_MiniGameYabawe()->Get_YabaweBall()->Get_WorldPos().y;
				TM->Get_MiniGameYabawe()->Get_YabaweBall()->Set_WorldPos(vPos);

				TM->Get_MiniGameYabawe()->Get_YabaweBall()->Set_Render(true);

				m_bNext = true;
			}


			if (m_bNext)
			{
				m_fNextTimeAcc += fTimeDelta;

				if (2.f < m_fNextTimeAcc)
				{
					TM->Get_MiniGameYabawe()->Level_Up();
					m_fNextTimeAcc = 0.f;
					m_bNext = false;
				}

			}


		}
	}




	// 컷신 시작
	if (m_bJump)
	{
		m_fSpinTimeAcc += fTimeDelta;

		if (2.f < m_fSpinTimeAcc && !m_bOnce_1)
		{
			_float3 vLookPos; XMStoreFloat3(&vLookPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vLookPos.y += 2.f;

			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
			CamDesc.vLook = vLookPos;
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 1.f;
			CamDesc.fLookLerpSpeed = 2.f;
			CRM->Get_Cam()->Show_Target(CamDesc);

			m_bOnce_1 = true;
		

		}

		if (6.f < m_fSpinTimeAcc)
		{
			if (!m_bStartShakeJump)
			{

				CCamera_Main::CUTDESC CamDesc;
				CamDesc.vPos = CRM->Get_Cam()->Get_CamBackPos(2.f, 0.f, -10.f);
				CamDesc.vPos.y += 5.f;
				CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
				CamDesc.bLerp = true;
				CamDesc.bLookLerp = true;
				CamDesc.fLerpSpeed = 10.f;
				CamDesc.fLookLerpSpeed = 10.f;
				CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
				CamDesc.pTran = nullptr;
				CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
				CamDesc.bPixLocal = false;
				CRM->Get_Cam()->Show_Target(CamDesc);

				CRM->Get_Cam()->Shake(0.05f, 0.1f);
				m_bStartShakeJump = true;
				GI->PlaySoundW(L"GGST_105.ogg", SD_MAP, 1.f);


				// 스모크
				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 1.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 250.f,250.f,250.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info2->fBlurPower = 1.f;
				Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info2->vRatio = { 1.f,1.f };
				Info2->vRatioSpeed = { 2.f,2.f };
				Info2->vScale = { 0.1f,0.1f,0.1f };
				Info2->vScaleSpeed = { 300.f,300.f,300.f };
				Info2->vUvFixSpeed = { -1.f,-3.f };
				Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);


			}


			if (!m_bInvJump)
				m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 60.f, fTimeDelta);
			else
				m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 60.f, fTimeDelta);

			if (m_vInitPos.y + 100.f < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			{
				_float4 vPos = TM->Get_MiniGameYabawe()->Get_YabaweBall()->Get_WorldPos();
				vPos.y = 10.f;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
				m_bInvJump = true;
			}

			if (m_vInitPos.y > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			{
				_float4 vPos = TM->Get_MiniGameYabawe()->Get_YabaweBall()->Get_WorldPos();
				vPos.y = m_vInitPos.y;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

				m_bJump = false;
				m_bTurnChange = true;
				m_fSpinTimeAcc = 0.f;
				CRM->Get_Cam()->Shake(0.15f, 0.15f, false, 0.1f);
				TM->Get_MiniGameYabawe()->Get_YabaweBall()->Set_Render(false); 
				GI->PlaySoundW(L"ADV_075.ogg", SD_MAP, 1.f);
				// 야바위 게임 시작

			}
		}
	}



	if (m_bTurnChange)
	{
		m_fSpinTimeAcc += fTimeDelta;

		if (1.f < m_fSpinTimeAcc)
		{
			m_fAngle += 800.f * fTimeDelta;
			m_pTransformCom->TurnAngle(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(800.f * fTimeDelta));

			if (180.f < m_fAngle && !m_bTempOne)
			{
				if (m_eFace == FACE_SMILE)
					m_eFace = FACE_IDLE;
				else
					m_eFace = FACE_SMILE;

				GI->PlaySoundW(L"SE_BTL_021.wav", SD_MAP, 1.f);
				m_bTempOne = true;
			}

			if (360.f < m_fAngle)
			{
				m_fAngle = 0.f;
				m_bTurnChange = false;
				m_bTempOne = false;
				m_pTransformCom->Set_Look(XMVectorSetW(XMLoadFloat4x4(&m_Desc.mMatrix).r[2], 0.f));

				m_fSpinTimeAcc = 0.f;
				m_bStartShake = true;
			}
		}
	}


	if (m_bStartShake)
	{
		m_fSpinTimeAcc += fTimeDelta;

		if (!m_bStartEndSound)
		{
			GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_0000001B.wav", SD_NPC, 1.f);
			m_bStartEndSound = true;
		}

		if (1.f < m_fSpinTimeAcc)
		{
			_vector vDestPos = XMLoadFloat3(&m_vInitPos);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vGoDir = vDestPos - vPos;
			_float fDis = XMVectorGetX(XMVector3Length(vGoDir));

			if (0.2f > fDis)
			{
				m_fTimeAccTemp += fTimeDelta;

				if (1.f < m_fTimeAccTemp)
				{

					// (CTransform*)TM->Get_MiniGameYabawe()->Get_ComponentPtr(TEXT("Com_Transform"))
					CRM->Get_Cam()->Hold_Pos(CRM->Get_Cam()->Get_CamPos(), PM->Get_Transform(), nullptr, _float3(-99.6f, -3.9f, 130.2f), 0.7f);
					TM->Get_MiniGameYabawe()->Level_Up();
					m_bStartShake = false;
					UM->Set_BattleTalkIndex(17);
					UM->Set_FrontUI(true);
					GI->PlaySoundW(L"minigame_common_start.wav", SD_MAP, 1.f);
					return;
				}
			}
			else
				m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(vGoDir), 0.f), 15.f, fTimeDelta);
		}

	}
}

void CRealYabawe::End_Tick(_float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;

	if (0.1f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);
		GI->PlaySoundW(L"GGST_254.wav", SD_MAP, 1.f);

		m_fTimeAcc = 10.f;

		m_bCutShake = true;
		m_bBoom = true;

		UM->Set_FrontUI(false);
	}
	else if (13.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		m_bEndJump = true;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);

		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 3.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);




		// 스모크
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 1.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 250.f,250.f,250.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vScale = { 0.1f,0.1f,0.1f };
		Info2->vScaleSpeed = { 300.f,300.f,300.f };
		Info2->vUvFixSpeed = { -1.f,-3.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);







		m_bCutShake = false;
		m_bBoom = false;

		m_fTimeAcc = 20.f;
	}
	else if (22.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{
		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-129.89f, 58.566f, 66.934f);
		CamDesc.vLook = _float3(-136.156f, 31.276f, 96.48f);
		CamDesc.vLook.y += 5.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTimeAcc = 30.f;
	}
	else if (31.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{
		m_bEndJump = false;
		m_bDonw = true;
		GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 0.9f);

		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-136.156f, fY, 96.48f, 1.f));

		m_fTimeAcc = 40.f;
	}




	if (m_bEndJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 150.f, fTimeDelta);
	}
	else if (m_bDonw)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (31.276f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 120.f, fTimeDelta);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-136.156f, 31.276f, 96.48f, 1.f));

			m_bDonw = false;
			m_bCutEnd = true;

			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
			GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);




			// 스모크
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 1.f;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,2.f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 250.f,250.f,250.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 1.f;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 2.f,2.f };
			Info2->vScale = { 0.1f,0.1f,0.1f };
			Info2->vScaleSpeed = { 300.f,300.f,300.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);




		}

	}





	if (m_bCutShake)
	{
		m_fCutShakeTimeAcc += fTimeDelta;

		if (0.04f < m_fCutShakeTimeAcc)
		{
			Shake();
			m_fCutShakeTimeAcc = 0.f;
		}

	}



	if (m_bCutEnd)
	{
		m_fTimeCutCut += fTimeDelta;
		if (2.f < m_fTimeCutCut)
		{
			Set_Dead();
			CRM->Get_Cam()->End_Target();
			TM->Open_Stage4Trigger(6);
			// UM->Set_FrontUI(false);
			return;
		}
	}




	if (m_bBoom)
	{
		m_fBoomTimeAcc += fTimeDelta;
		if (0.25f < m_fBoomTimeAcc)
		{

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vRanPos = XMVectorSet(GI->Get_FloatRandom(-2.5f, 2.5f), -2.f + GI->Get_FloatRandom(0.5f, 2.5f), GI->Get_FloatRandom(-2.5f, 2.5f), 1.f);
			_matrix vvM = XMMatrixIdentity();
			vvM.r[3] = vPos + vRanPos;
			_float4x4 vM; XMStoreFloat4x4(&vM, vvM);

			_int iIndex = GI->Get_Random(0, 3);

			if (0 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
			}
			else if (1 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}
			else if (3 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}
			else if (4 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}


			m_fBoomTimeAcc = 0.f;
		}

	}





}

void CRealYabawe::Tick_Shake(_float fTimeDelta)
{

	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bCutShake = false;
		// m_eFace = FACE_IDLE;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitShakePos), 1.f));
		return;
	}


	m_fShakeTimeAcc += fTimeDelta;
	if (0.03f < m_fShakeTimeAcc)
	{
		m_bShakeTurn = !m_bShakeTurn;
		m_fShakeTimeAcc = 0.f;
	}

	if (m_bShakeTurn)
	{
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 5.f, fTimeDelta);
	}
	else
	{
		m_pTransformCom->Go_Dir(-1.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 5.f, fTimeDelta);
	}

}
