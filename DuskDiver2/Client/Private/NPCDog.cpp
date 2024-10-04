#include "stdafx.h"
#include "..\Public\NPCDog.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Dogbox.h"
#include "Crack_Manager.h"
#include "Player_Manager.h"
#include "Monster_Manager.h"

CNPCDog::CNPCDog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCDog::CNPCDog(const CNPCDog & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCDog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCDog::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // 객체tag

	_float4 vDogPos = { -2.43f, 0.f, 122.62f, 1.f };

	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vDogPos));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));

	CDogbox::DOGBOXDESC DogboxDesc;
	DogboxDesc.vPosition = vDogPos;
	m_Dogbox = (CDogbox*)GI->Clone_GameObject(TEXT("Dogbox"), &DogboxDesc);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CNPCDog::Tick(_float fTimeDelta)
{
	// stage3에서 전투를 한다면 m_bAction을 false로 아니면 true로 하는 기능 바로 밑에 추가
	m_bAction = !(TM->Get_Stage3_Bool());

	// 이상이 없다면 정상적인 틱을 돌린다.
	if (true == m_bAction)
	{
		if (nullptr != m_Dogbox)
			m_Dogbox->Tick(fTimeDelta);

		Update_Dog(fTimeDelta);
	}

}

void CNPCDog::LateTick(_float fTimeDelta)
{
	if (102 == UM->Get_TalkIndex())
	{
		if (CK_DOWN(DIK_NUMPADENTER))
		{
			if (false == m_bBurstUIOn)
			{
				UM->Set_PictureBurst(true);
				UM->Set_FrontUI(false);
				m_bBurstUIOn = true;
			}
			else if (true == m_bBurstUIOn)
			{
				UM->Set_PictureBurst(false);
				UM->Set_FrontUI(true);
			}
		}	
	}

	//// 3-2 중중간 보스 죽음 판정
	//if (false == m_bMiddleBoss)
	//{
	//	if (MM->Get_Stage3_2Count() == 4) // Disaster Boss 죽음
	//	{
	//		m_bMiddleBoss = true;
	//		UM->Set_FrontUI(false);
	//		UM->Set_BattleIndex(2);
	//		UM->Make_BattleEnd();
	//	}
	//}
	

	if (true == m_bAction)
	{
		if (nullptr == m_pRendererCom)
			return;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, nullptr);

		if (true == m_bRenderState)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		if (nullptr != m_Dogbox)
			m_Dogbox->LateTick(fTimeDelta);
	}
	
}

HRESULT CNPCDog::Render()
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

	if (nullptr != m_Dogbox)
		m_Dogbox->Render();

	return S_OK;
}

void CNPCDog::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	ImGui::End();
}

void CNPCDog::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}


HRESULT CNPCDog::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCDog"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Navigation */
	/*CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);*/

	return S_OK;
}

void CNPCDog::Update_Dog(_float fTimeDelta)
{
	// 균열 주위에 있는지 판단
	m_bFindCrack = Check_CrackNearby();
	// 박스를 쳐냈다면	
	if ((nullptr != m_Dogbox) && (true == m_Dogbox->Get_Delete()))
	{
		Safe_Release(m_Dogbox);
	}
	if (nullptr != m_Dogbox)
		m_bChase = m_Dogbox->Get_Hited();


	if (true == m_bChase)
	{
		if (true == m_bFindCrack)
			Barking(fTimeDelta);

		if (false == m_bFindCrack)
		{
			Chase_Player(fTimeDelta);
		}
	}

	else
	{
		m_fPlayerPos = PM->Get_PlayerPos();
		m_bOn = m_pTransformCom->Check_LimitDistance(XMLoadFloat4(&m_fPlayerPos), 5.f);
		
		if (m_bOn)
		{
			if (!m_bWhimperOn)
			{
				GI->PlaySoundW(L"Puppy_Wimper.wav", SD_TRAININGBOT1, 0.5f);
				m_bWhimperOn = true;
			}
		}

		else
		{
			GI->StopSound(SD_TRAININGBOT1);
			m_bWhimperOn = false;
		}
	}
	m_bDead = CRKM->Get_DeleteDog();

}

void CNPCDog::Chase_Player(_float fTimeDelta)
{
	// 일단 플레이어를 바라본다
	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&PM->Get_PlayerPos()));

	// 플레이어와의 거리를 재서 가까우면 idle(1)
	// 멀리에 있으면 walk(2)애니메이션을 돌린다
	_float fLength = 0.f;
	_float4 vDir = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vDir, XMVector4Normalize(XMLoadFloat4(&PM->Get_PlayerPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	XMStoreFloat(&fLength, XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&PM->Get_PlayerPos())));

	if (false == m_bRenderState)
	{
		if (3.8f < fLength)
			m_bRenderState = true;
	}

	if ((1.5f < fLength) && (15.f >= fLength))// 플레이어와의 거리가 멀다면
	{
		if (2 != m_iLastAnimIndex)
		{
			m_iLastAnimIndex = 2;
			m_pModelCom->Set_AnimIndex(2);
		}
		m_pTransformCom->Go_Dir(XMLoadFloat4(&vDir), m_fSpeed, fTimeDelta);
	}
	else if (15.f < fLength)//거리가 너무 멀다면
	{
		if (1 != m_iLastAnimIndex)
		{
			m_iLastAnimIndex = 1;
			m_pModelCom->Set_AnimIndex(1);
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PM->Get_PlayerPos()));
		m_bRenderState = false;
	}
	else if ((1.f < fLength) && (1.5f >= fLength))
	{
		if (1 != m_iLastAnimIndex)
		{
			m_iLastAnimIndex = 1;
			m_pModelCom->Set_AnimIndex(1);
		}
	}

	Fix_State();

	if (!m_bFirstMeet)
	{
		GI->StopSound(SD_TRAININGBOT1);
		GI->PlaySoundW(L"Puppy_Welcome.wav", SD_TRAININGBOT1, 0.5f);
		
		m_fBeNormalAcc += 1.f * fTimeDelta;

		//몇초 지나고
		if (3.f <= m_fBeNormalAcc)
			m_bFirstMeet = true;
	}

	if (m_bFirstMeet)
	{
		if (!m_bIdle)
		{
			GI->StopSound(SD_TRAININGBOT1);
			GI->PlaySoundW(L"Puppy_Normal.wav", SD_TRAININGBOT1, 0.5f);
			m_bIdle = true;
		}

		else
		{
			m_fIdleAcc += 1.f * fTimeDelta;

			if (19.f <= m_fIdleAcc)
			{
				m_bIdle = false;
				m_fIdleAcc = 0.f;
			}
		}
	}
}

void CNPCDog::Barking(_float fTimeDelta)
{
	// 균열을 바라본다.
	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&CRKM->Get_CrackPos()));
	m_bRenderState = true;

	// 균열까지의 거리가 1.5f보다 크다면 균열 쪽으로 걸어가고 작다면 짖는다
	if (1.5f < m_fCrackLength)
	{
		if (2 != m_iLastAnimIndex)
		{
			m_iLastAnimIndex = 2;
			m_pModelCom->Set_AnimIndex(2);
		}

		m_pTransformCom->Go_Dir(XMVector4Normalize(XMLoadFloat4(&CRKM->Get_CrackPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)), m_fSpeed, fTimeDelta);

	}
	else
	{
		if (0 != m_iLastAnimIndex)
		{
			m_iLastAnimIndex = 0;
			m_pModelCom->Set_AnimIndex(0);
		}

		if (!m_bBark)
		{
			GI->StopSound(SD_TRAININGBOT1);
			GI->PlaySoundW(L"Puppy_Bark1.wav", SD_TRAININGBOT1, 0.5f);
			m_bBark = true;
		}

		else
		{
			m_fBarkAcc += 1.f * fTimeDelta;

			if (8.f <= m_fBarkAcc)
			{
				m_bBark = false;
				m_fBarkAcc = 0.f;
			}
		}
	}

	
	Fix_State();
}

void CNPCDog::Fix_State()
{
	// 높이를 0으로 고정
	_float4 ZeroYPos = { 0.f,0.f,0.f,1.f };
	XMStoreFloat4(&ZeroYPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	ZeroYPos.y = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&ZeroYPos));

	// 방향을 수정
	_float4 NewLook = { 0.f,0.f,0.f,0.f };
	_float4 NewRight = { 0.f,0.f,0.f,0.f };
	_float4 NewUp = { 0.f,1.f,0.f,0.f };

	XMStoreFloat4(&NewLook, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	XMStoreFloat4(&NewRight, XMVector3Cross(XMLoadFloat4(&NewUp), XMLoadFloat4(&NewLook)));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&NewRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&NewUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&NewLook));
	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
}

_bool CNPCDog::Check_CrackNearby()
{
	// 균열이 근처에 있는지 판단
	if (true == CRKM->Get_CrackExist())
	{
		XMStoreFloat(&m_fCrackLength, XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&CRKM->Get_CrackPos())));

		if (10.f > m_fCrackLength)
			return true;

	}

	return false;
}



CNPCDog * CNPCDog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCDog*		pInstance = new CNPCDog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCDog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCDog::Clone(void * pArg)
{
	CNPCDog*		pInstance = new CNPCDog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCDog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPCDog::Free()
{
	__super::Free();

	Safe_Release(m_Dogbox);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNavigationCom);
}
