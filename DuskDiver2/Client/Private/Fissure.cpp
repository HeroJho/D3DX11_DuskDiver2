#include "stdafx.h"
#include "..\Public\Fissure.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "PipeLine.h"
#include "Crack_Manager.h"

CFissure::CFissure(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFissure::CFissure(const CFissure & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFissure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFissure::Initialize(void * pArg)
{
	m_FissureDesc = *(FISSUREDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeZ = m_fSizeMax;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMLoadFloat4(&m_FissureDesc.vPos));
	//m_pTransformCom->Set_Scale(XMVectorSet(15.f, 15.f, 15.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeZ, 2.f, 2.f, 1.f));

	CRKM->BackUp_Fissure(this);

	return S_OK;
}

void CFissure::Tick(_float fTimeDelta)
{
	if (m_bTest)
	{
		if (m_FissureDesc.iIndex == 0)
		{
			// 테스트 코드, 아무일도 발생하지 않고 균열만 삭제한다.
		}
		else if (m_FissureDesc.iIndex == 1) // 지하철안의 균열
		{
			UM->Set_TalkIndex(17);
			m_bMonsterSpawn = true;
			MM->Spawn_Stage2();
			//GI->PlaySound_Linear(L"SubWayBattle.wav", SD_BGM, 0.3f, true, 0.5f);
			UM->Set_FrontUI(false);

		}
		else if (m_FissureDesc.iIndex == 2) // stage3 지하철 앞 균열
		{
			UM->Set_QuestPointOff();
			UM->Set_FrontUI(true);
			MM->Spawn_Stage3_1(); // stage3 시작후 첫전투의 몬스터 스폰
			//GI->PlaySound_Linear(L"CityBattle1.wav", SD_BGM, 0.3f, true, 0.4f);
			TM->Set_Stage3_Bool(true);

		}
		else if (m_FissureDesc.iIndex == 3) // 중간보스 생성 균열 (Disaster Boss) 
		{
			UM->Set_QuestPointOff();
			//중중간 보스 생성
			UM->Set_FrontUI(true);
			MM->Spawn_Stage3_2();
			//GI->PlaySound_Linear(L"CityBattle1.wav", SD_BGM, 0.3f, true, 0.4f);
			TM->Set_Stage3_Bool(true);
			UM->Set_TalkIndex(99);
		}
		else if (m_FissureDesc.iIndex == 4) // 충돌후 골렘 생성 컷신
		{
			GI->StopSound(SD_TRAININGBOT1); // 강아지 소리 끄기
			UM->Set_QuestPointOff();
			CRKM->Delete_Dog();
			MM->Spawn_Golem();
			//GI->PlaySound_Linear(L"BGM_Golem.wav", SD_BGM, 0.3f, true, 0.4f);
			TM->Set_Stage3_Bool(true);

		}
		else if (m_FissureDesc.iIndex == 5) // 충돌후 Stage5로 향함
		{
			UM->Set_FrontUI(false);
			UM->Set_QuestPointOff();
			UM->MoveBlind(true);
			//TM->Change_Level(LEVEL_STAGE5);
			CRKM->Set_GotoStage5(true);
		}


		Set_Dead();
		CRKM->Clear_Fissure();
	}

	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&GI->Get_CamPosition()));

	// 플레이어와의 거리를 잰다
	if (false == m_bChanged)
	{
		if (true == Check_OnSight())
		{
			m_CurCrackState = STATE_RIPPED;

			if (!m_bSoundOn)
			{
				GI->PlaySoundW(L"se_Gate_Vfx_Blue.wav", SD_MAP, 0.5f);
				m_bSoundOn = true;
			}
		}
		else
		{
			m_CurCrackState = STATE_CLOSED;
			m_bSoundOn = false;
		}
	}
	// 상태가 달라졌다면 움직인다
	if (m_LastCrackState != m_CurCrackState)
	{
		// 거리 판단을 멈추게 해준다.
		m_bChanged = true;
		// 실제 움직이는 함수
		Update_Crack(fTimeDelta);
	}

}

void CFissure::LateTick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;

	if (true == m_bRenderState)
	{
		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NOBLUR, this); //RENDER_NOBLUR //RENDER_ALPHABLEND
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
	
}

HRESULT CFissure::Render()
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

	m_pTextureCom[MASK_UV]->Set_SRV(m_pShaderCom, "g_EmMask", 0);
	m_pTextureCom[MASK_RANGE]->Set_SRV(m_pShaderCom, "g_Mask2", 0);
	m_pTextureCom[MASK_BORDER]->Set_SRV(m_pShaderCom, "g_Mask3", 0);

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_MeshAlpha", &m_fMeshAlpha, sizeof(_float))))
		return E_FAIL;
		
	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMovev", &m_MoveUV.y, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{ 
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPass)))
			return E_FAIL;
	}


	return S_OK;
}

void CFissure::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Fissure")->Set_Size(vSize);
	Get_Colliders("Fissure")->Set_Center(vCenter);
	ImGui::End();
}

void CFissure::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Fissure", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bTest = true;
		}

	}
	
}

_float4 CFissure::Get_FissurePos()
{
	_float4 vFissurePos = { 0.f,0.f,0.f,1.f };
	XMStoreFloat4(&vFissurePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vFissurePos;
}

HRESULT CFissure::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Crack"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_PR_DimenCrackA_UVMask"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom[MASK_UV])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_PR_DimenCrackA_Mask2"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom[MASK_RANGE])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_PR_DimenCrackA_Mask3"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureCom[MASK_BORDER])))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, -0.1f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Fissure");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

_bool CFissure::Check_OnSight()
{
	_float4 fPlayerPos = PM->Get_PlayerPos();

	_float fDistance = sqrt((fPlayerPos.x - m_FissureDesc.vPos.x) * (fPlayerPos.x - m_FissureDesc.vPos.x) + (fPlayerPos.z - m_FissureDesc.vPos.z) * (fPlayerPos.z - m_FissureDesc.vPos.z));

	if (m_FissureDesc.fRange > fDistance)
	{
		if ((!m_bSpecialEvent) && (4 == m_FissureDesc.iIndex))
		{
			m_bSpecialEvent = true;
			UM->Set_BattleTalkIndex(2);
		}
		return true;
	}

	return false;
}

void CFissure::Update_Crack(_float fTimeDelta)
{
	switch (m_LastCrackState)
	{
	case STATE_CLOSED:
		Rip_Crack(fTimeDelta);
		break;
	case STATE_RIPPED:
		Close_Crack(fTimeDelta);
		break;
	default:
		break;
	}

}

void CFissure::Rip_Crack(_float fTimeDelta)
{
	m_iShaderPass = 24;
	m_bRenderState = true;

	// 멀어졌을경우를 대비하여 parameter를 리셋한다.
	m_MoveUV.y = -0.6f;

	m_fMeshAlpha += 0.35f * fTimeDelta;
	if (1.f < m_fMeshAlpha)
	{
		m_LastCrackState = m_CurCrackState;
		m_bChanged = false;
	}
		
}

void CFissure::Close_Crack(_float fTimeDelta)
{
	m_iShaderPass = 25;

	if (m_fSizeMin < m_fSizeZ)
	{
		m_fSizeZ -= m_fSizeSpeed * fTimeDelta;
	}
	else
	{
		m_fSizeZ = m_fSizeMin;

		if (0.f > m_MoveUV.y)
		{
			m_MoveUV.y += m_fUVSpeed *fTimeDelta;
			if (0.f < m_MoveUV.y)
				m_MoveUV.y = 0.f;
		}
		else
		{
			// 다음번 열릴 상태를 초기화
			m_fMeshAlpha = 0.f;
			m_MoveUV.y = 0.f;
			m_fSizeZ = m_fSizeMax;
			// 움직임이 끝났나면 지난상태를 바꾸어준다.
			m_LastCrackState = m_CurCrackState;
			m_bChanged = false;
			m_bRenderState = false;
		}

	}
	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeZ, 2.f, 2.f, 1.f));
}



CFissure * CFissure::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFissure*		pInstance = new CFissure(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFissure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFissure::Clone(void * pArg)
{
	CFissure*		pInstance = new CFissure(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFissure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CFissure::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	for (_uint i = 0; MASK_END > i; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}
