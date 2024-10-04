#include "stdafx.h"
#include "..\Public\Stage3Cars.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Time_Manager.h"

CStage3Cars::CStage3Cars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CStage3Cars::CStage3Cars(const CStage3Cars & rhs)
	: CCreture(rhs)
{
}

HRESULT CStage3Cars::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Cars::Initialize(void * pArg)
{
	m_CarsDesc = *(CARSDESC*)pArg;

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage_Prop";

	_float NewZ = 0.f;
	if (0 == m_CarsDesc.iLaneIndex % 2)
		NewZ = (2 - m_CarsDesc.iLaneIndex) * 100.f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, NewZ, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, _vector{ 0.f,0.f,-1.f });
	m_fPushPower = 30.f;
	m_fAirPower = 10.f;
	m_eHitType = TYPE_SPIN;
	m_fDamage = 100.F;

	return S_OK;
}

void CStage3Cars::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;
	Update_Cars(fTimeDelta);

	if (!m_bAttackCol)
	{
		m_fAttackColAcc += 1.f * fTimeDelta;
		if (m_fAttackColAcc >= 0.5f)
		{
			m_fAttackColAcc = 0.f;
			m_bAttackCol = true;
		}
	}

	//if (TM->Get_Stage3_Bool())
	//{
	//	//m_fDissolveAcc += 0.15f * fTimeDelta;
	//	//if (m_fDissolveAcc >= 1.f)
	//	//{
	//	//	m_fDissolveAcc = 1.f;
	//	//	//m_bRender = true;
	//	//}

	//}
	//else
	//{
	//	//m_bRender = false;
	//	//m_fDissolveAcc = 0.f;
	//}

}

void CStage3Cars::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	if (!TM->Get_Stage3_Bool())
	{
		Choose_Col(nullptr);
		Tick_Col(m_pTransformCom2->Get_WorldMatrix());
		Render_Col(m_pRendererCom);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStage3Cars::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float fGama = 1.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 20)))
			return E_FAIL;
	}

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CStage3Cars::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::End();


	if (3 == m_CarsDesc.iLaneIndex)
	{
		list<CCollider*>::iterator iter = m_Colliders.begin();

		ImGui::Begin("1");
		IG->Debug_Col(*iter);
		++iter;
		ImGui::End();
		ImGui::Begin("2");
		IG->Debug_Col(*iter);
		++iter;
		ImGui::End();
		ImGui::Begin("3");
		IG->Debug_Col(*iter);
		++iter;
		ImGui::End();
		ImGui::Begin("4");
		IG->Debug_Col(*iter);
		++iter;
		ImGui::End();
		ImGui::Begin("5");
		IG->Debug_Col(*iter);
		++iter;
		ImGui::End();
		ImGui::Begin("6");
		IG->Debug_Col(*iter);
		ImGui::End();
	}

}

HRESULT CStage3Cars::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;




	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;

	/* For.Com_Model */
	switch (m_CarsDesc.iLaneIndex)
	{
	case 0:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Vehicles0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;


		ColDesc.vCenter = _float3(-19.f, 1.f, 18.8f);
		strcpy(ColDesc.sTag, "Char_1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(85.8f, 1.f, 18.76f);
		strcpy(ColDesc.sTag, "Char_2");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-79.3f, 1.f, 18.76f);
		strcpy(ColDesc.sTag, "Char_3");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-104.f, 1.f, 18.76f);
		strcpy(ColDesc.sTag, "Char_4");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-217.3f, 1.f, 18.7f);
		strcpy(ColDesc.sTag, "Char_5");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;


	}
		break;
	case 1:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Vehicles1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColDesc.vCenter = _float3(16.8f, 1.f, 23.f);
		strcpy(ColDesc.sTag, "Char_1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(47.6f, 1.f, 23.6f);
		strcpy(ColDesc.sTag, "Char_2");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(107.6f, 1.f, 23.2f);
		strcpy(ColDesc.sTag, "Char_3");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-128.1f, 1.f, 23.3f);
		strcpy(ColDesc.sTag, "Char_4");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-254.f, 1.f, 23.f);
		strcpy(ColDesc.sTag, "Char_5");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-161.3f, 1.f, 23.5f);
		strcpy(ColDesc.sTag, "Char_6");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

	}
		break;
	case 2:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Vehicles2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColDesc.vCenter = _float3(-12.1f, 1.f, 28.6f);
		strcpy(ColDesc.sTag, "Char_1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(100.8f, 1.f, 28.5f);
		strcpy(ColDesc.sTag, "Char_2");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-36.7f, 1.f, 28.4f);
		strcpy(ColDesc.sTag, "Char_3");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-96.8f, 1.f, 28.5f);
		strcpy(ColDesc.sTag, "Char_4");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-202.1f, 1.f, 28.6f);
		strcpy(ColDesc.sTag, "Char_5");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;


	}
		break;
	case 3:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Vehicles3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColDesc.vCenter = _float3(28.8f, 1.f, 33.5f);
		strcpy(ColDesc.sTag, "Char_1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-62.9f, 1.f, 32.9f);
		strcpy(ColDesc.sTag, "Char_2");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-96.6f, 1.f, 33.3f);
		strcpy(ColDesc.sTag, "Char_3");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-241.8f, 1.f, 33.6f);
		strcpy(ColDesc.sTag, "Char_4");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-272.7f, 1.f, 32.8f);
		strcpy(ColDesc.sTag, "Char_5");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
		ColDesc.vCenter = _float3(-332.4f, 1.f, 33.3f);
		strcpy(ColDesc.sTag, "Char_6");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

	}
		break;
	default:
		break;
	}








	return S_OK;
}

void CStage3Cars::Update_Cars(_float fTimeDelta)
{
	if (TM->Get_Stage3_Bool())
	{
		if (2 > m_CarsDesc.iLaneIndex)
		{
			MoveFrontFric(fTimeDelta);
		}
		else
		{
			MoveBackFric(fTimeDelta);
		}
	}
	else
	{
		Reset_Fric();
		if (2 > m_CarsDesc.iLaneIndex)
		{
			MoveFront(fTimeDelta);
		}
		else
		{
			MoveBack(fTimeDelta);
		}
	}
	
}

void CStage3Cars::MoveBack(_float fTimeDelta)
{
	if (2 < m_CarsDesc.iLaneIndex)
		m_fCurPosX -= m_fHighSpeed * fTimeDelta;
	else
		m_fCurPosX -= m_fLowSpeed * fTimeDelta;

	if (-450.f > m_fCurPosX)
		m_fCurPosX = 657.f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, 0.f, 1.f));
}

void CStage3Cars::MoveFront(_float fTimeDelta)
{
	if (1 > m_CarsDesc.iLaneIndex)
		m_fCurPosX += m_fLowSpeed * fTimeDelta;
	else
		m_fCurPosX += m_fHighSpeed * fTimeDelta;

	if (657.f < m_fCurPosX)
		m_fCurPosX = -450.f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, 0.f, 1.f));
}

void CStage3Cars::MoveBackFric(_float fTimeDelta)
{
	// 속도를 업데이트 해준다.
	if (0.f < m_fSpeedFric)
		m_fSpeedFric -= m_fFriction * fTimeDelta;
	else
		m_fSpeedFric = 0.f;

	// 위치를 이동시킨다
	if (2 < m_CarsDesc.iLaneIndex)
		m_fCurPosX -= m_fSpeedFric * fTimeDelta;
	else
		m_fCurPosX -= m_fSpeedFric * fTimeDelta;

	if (-450.f > m_fCurPosX)
		m_fCurPosX = 657.f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, 0.f, 1.f));
}

void CStage3Cars::MoveFrontFric(_float fTimeDelta)
{
	// 속도를 업데이트 해준다.
	if (0.f < m_fSpeedFric)
		m_fSpeedFric -= m_fFriction * fTimeDelta;
	else
		m_fSpeedFric = 0.f;

	// 위치를 이동시킨다
	if (1 > m_CarsDesc.iLaneIndex)
		m_fCurPosX += m_fSpeedFric * fTimeDelta;
	else
		m_fCurPosX += m_fSpeedFric * fTimeDelta;

	if (657.f < m_fCurPosX)
		m_fCurPosX = -450.f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, 0.f, 1.f));
}

void CStage3Cars::Reset_Fric()
{
	if ((0 == m_CarsDesc.iLaneIndex) || (3 == m_CarsDesc.iLaneIndex))
	{
		m_fSpeedFric = m_fLowSpeed;
	}
	else
		m_fSpeedFric = m_fHighSpeed;
}

CStage3Cars * CStage3Cars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Cars*		pInstance = new CStage3Cars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Cars"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CStage3Cars::Clone(void * pArg)
{
	CStage3Cars*		pInstance = new CStage3Cars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Cars"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage3Cars::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pTransformCom2);

}



// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CStage3Cars::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!m_bRender)
	{
		if ("Tag_Player" == Desc.pOther->Get_Tag())
		{
			if (m_bAttackCol)
			{
				((CCreture*)Desc.pOther)->On_Hit(this);
				m_bAttackCol = false;
				m_fAttackColAcc = 0.f;

				_uint iRandom = GI->Get_Random(0, 3);
				switch (iRandom)
				{
				case 0:
					GI->PlaySoundW(L"Amb_Speaker_01.wav", SD_MAP, 0.6f);
					break;
				case 1:
					GI->PlaySoundW(L"Amb_Speaker_07.wav", SD_MAP, 0.6f);
					break;
				case 2:
					GI->PlaySoundW(L"Amb_Speaker_07.wav", SD_MAP, 0.6f);
					break;
				case 3:
					GI->PlaySoundW(L"Amb_Speaker_07.wav", SD_MAP, 0.6f);
					break;
				}
			}

		}
	}
}