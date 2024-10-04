#include "stdafx.h"
#include "..\Public\NPCPartTime.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CNPCPartTime::CNPCPartTime(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CNPCPartTime::CNPCPartTime(const CNPCPartTime & rhs)
	: CCreture(rhs)
{
}

HRESULT CNPCPartTime::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCPartTime::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC"; // 객체tag

	m_pModelCom->Set_AnimIndex(STATE_IDLE);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(24.f, 0.f, 157.5f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fFirstRotationAngle));

	return S_OK;
}

void CNPCPartTime::Tick(_float fTimeDelta)
{
	m_bAction = PlayerNearby();
	//m_bAction = true;

	if (true == m_bAction)
	{
		Update_NPC(fTimeDelta);
	}
	
	if (UM->Get_ShopState() && !UM->Get_ShopSelectBar())
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(24.1f, 1.5f, 159.f);
		CamDesc.vLook = _float3(25.1f, 1.3f, 156.f);
		//CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 4.f;
		CamDesc.fLookLerpSpeed = 4.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
	}
	else if (UM->Get_ShopState() && UM->Get_ShopSelectBar())
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(24.f, 1.5f, 159.f);
		CamDesc.vLook = _float3(21.f, 1.2f, 151.f);
		//CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 4.f;
		CamDesc.fLookLerpSpeed = 4.f;
		CamDesc.vPivotLookPos = _float3(-3.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
	}

	//================================
	//Move(fTimeDelta);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.f + NewX, 0.f, 160.f + NewZ, 1.f));
	//================================
}

void CNPCPartTime::LateTick(_float fTimeDelta)
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

HRESULT CNPCPartTime::Render()
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

void CNPCPartTime::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("NPC_Metro")->Set_Size(vSize);
	Get_Colliders("NPC_Metro")->Set_Center(vCenter);
	ImGui::End();
}

void CNPCPartTime::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("NPC_PartTIme", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			UM->Set_TalkButtonType(3);
			UM->Set_TalkButton(true);
			if (CK_UP(DIK_C))
			{
				if ((true == On_LeftSide()) && (true == m_bTurnable))
				{
					m_pModelCom->Set_AnimIndex(STATE_TURNL);
					m_eCurState = STATE_TURNL;
				}
				UM->Shop_On();
				GI->PlaySoundW(L"NA24_com_03.wav", SD_MAP, 0.8f);
				// 대화를 이어 나가는 함수

				//UM->Set_TalkIndex(27);
				//UM->Set_FrontUI(false);
			}
		}

	}
}



HRESULT CNPCPartTime::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCPartTime"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);
	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.7f, 1.7f, 1.7f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "NPC_PartTIme"); // 충돌체tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

_bool CNPCPartTime::PlayerNearby()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	if ((185.f > vPlayerPos.z) && (150.f < vPlayerPos.z) && ((6.f < vPlayerPos.x) && (50.f > vPlayerPos.x)))
		return true;

	return false;
}

void CNPCPartTime::Update_NPC(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CNPCPartTime::STATE_IDLE:
		Update_Ani_Idle(fTimeDelta);
		break;
	case Client::CNPCPartTime::STATE_TURNL:
		Update_Ani_TurnL(fTimeDelta);
		break;
	case Client::CNPCPartTime::STATE_TURNR:
		Update_Ani_TurnR(fTimeDelta);
		break;
	case Client::CNPCPartTime::STATE_WALK:
		break;
	default:
		break;
	}


}

void CNPCPartTime::Update_Ani_Idle(_float fTimeDelta)
{
	m_bTurnable = true;

	// NPC와의 대화가 삭제되어 코드가 무효 -> 다시 살리려면 NPC와의 대화종료 인덱스를 여기에 넣어주면 주석 풀면됨
	/*if (28 == UM->Get_TalkIndex())
	{
		if (true == m_bTurned)
		{
			m_pModelCom->Set_AnimIndex(STATE_TURNR);
			m_eCurState = STATE_TURNR;
		}
	}*/
}

void CNPCPartTime::Update_Ani_TurnL(_float fTimeDelta)
{
	m_bTurnable = false;
	
	if (true == m_pModelCom->GetAniEnd())
	{
		m_bTurned = true;
		m_pModelCom->Set_AnimIndex(STATE_IDLE);
		m_eCurState = STATE_IDLE;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fFirstRotationAngle - 90.f));
	}
}

void CNPCPartTime::Update_Ani_TurnR(_float fTimeDelta)
{
	m_bTurnable = false;

	if (true == m_pModelCom->GetAniEnd())
	{
		m_bTurned = false;
		m_pModelCom->Set_AnimIndex(STATE_IDLE);
		m_eCurState = STATE_IDLE;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fFirstRotationAngle));
	}
}

_bool CNPCPartTime::On_LeftSide()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();
	_float4 vDir = { 0.f,0.f,0.f,0.f }; // NPC위치에서 플레이어 위치로의 벡터

	XMStoreFloat4(&vDir, XMVector4Normalize(XMLoadFloat4(&vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	vDir.y = 0.f;

	_float4 vNPCLook = { 0.f,0.f,1.f,0.f };
	XMStoreFloat4(&vNPCLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float fCos = 0.f; // vDir과 vNPCLook의 내적의 결과를 저장할 변수
	fCos = vDir.x * vNPCLook.x + vDir.y * vNPCLook.y + vDir.z * vNPCLook.z;

	if (0.5f > fCos)
	{
		return true;
	}

	return false;
}

void CNPCPartTime::Move(_float fTimeDelta)
{
	if (CK_DOWN(DIK_UP))
	{
		NewX += 0.5f;
	}
	else if (CK_DOWN(DIK_DOWN))
	{
		NewX -= 0.5f;
	}
	if (CK_DOWN(DIK_LEFT))
	{
		NewZ += 0.5f;
	}
	else if (CK_DOWN(DIK_RIGHT))
	{
		NewZ -= 0.5f;
	}


}


CNPCPartTime * CNPCPartTime::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPCPartTime*		pInstance = new CNPCPartTime(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCPartTime"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPCPartTime::Clone(void * pArg)
{
	CNPCPartTime*		pInstance = new CNPCPartTime(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCPartTime"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CNPCPartTime::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
