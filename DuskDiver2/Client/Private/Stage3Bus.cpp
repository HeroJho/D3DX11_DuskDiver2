#include "stdafx.h"
#include "..\Public\Stage3Bus.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Time_Manager.h"

CStage3Bus::CStage3Bus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CStage3Bus::CStage3Bus(const CStage3Bus & rhs)
	: CCreture(rhs)
{
}

HRESULT CStage3Bus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Bus::Initialize(void * pArg)
{
	m_BusDesc = *(BUSDESC*)pArg;

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage_Prop";

	Set_InitialState();

	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, _vector{ 0.f,0.f,1.f });
	m_fPushPower = 30.f;
	m_fAirPower = 10.f;
	m_eHitType = TYPE_SPIN;
	m_fDamage = 100.F;

	return S_OK;
}

void CStage3Bus::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;
	Update_Cars(fTimeDelta);

	/*if (!m_bAttackCol)
	{
		m_fAttackColAcc += 1.f * fTimeDelta;
		if (m_fAttackColAcc >= 0.5f)
		{
			m_fAttackColAcc = 0.f;
			m_bAttackCol = true;
		}
	}*/

	if (TM->Get_Stage3_Bool())
	{
		m_fDissolveAcc += 0.15f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			m_bRender = true;
		}

	}
	else
	{
		m_bRender = false;
		m_fDissolveAcc = 0.f;
	}

}

void CStage3Bus::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (!m_bRender)
	{
		//Choose_Col(nullptr);
		//Tick_Col(m_pTransformCom->Get_WorldMatrix());
		//Render_Col(m_pRendererCom);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CStage3Bus::Render()
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
void CStage3Bus::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	list<CCollider*>::iterator iter = m_Colliders.begin();
	IG->Debug_Col(*iter);


	ImGui::End();

}


HRESULT CStage3Bus::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Bus"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;




	return S_OK;
}

void CStage3Bus::Set_InitialState()
{

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 1.f, 1.f);
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Char_1");



	if (0 == m_BusDesc.iLaneIndex)
	{
		m_BusDesc.fResetX = -287.5f;
		m_BusDesc.fDestinationX = 16.8f;

		ColDesc.vCenter = _float3(-0.2f, 1.f, -2.9f);
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return;

	}
	else if (5 == m_BusDesc.iLaneIndex)
	{
		m_BusDesc.fResetX = 281.5f;
		m_BusDesc.fDestinationX = -21.5f;

		ColDesc.vCenter = _float3(-0.1f, 1.f, -3.f);
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return;

	}

	_float fStartX = 0.f;
	if (0 == m_BusDesc.iLaneIndex)
	{
		fStartX = -106.8f;
		m_fStartZ = 15.3f;
	}
	else if (5 == m_BusDesc.iLaneIndex)
	{
		fStartX = 146.6f;
		m_fStartZ = 36.6f;
	}
	_float fStartAngle = 180.f * ((m_BusDesc.iLaneIndex * 0.2f) - 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fStartX, 0.f, m_fStartZ, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fStartAngle));

	m_fCurSpeed = m_fMaxSpeed;
	m_fCurPosX = fStartX;

	m_fAcceleration = m_fMaxSpeed / m_fSlideTime;

	int a = 0;
}

void CStage3Bus::Update_Cars(_float fTimeDelta)
{
	// 갱신된 속도를 구한다
	if (false == m_bStopped) // 버스역 도착이전이라면 
	{
		if (m_fSlideLength < abs(m_fCurPosX - m_BusDesc.fDestinationX))
		{
			m_fCurSpeed += m_fAcceleration * fTimeDelta;

			if (m_fMaxSpeed < m_fCurSpeed)
			{
				m_fCurSpeed = m_fMaxSpeed;
			}
		}
		else
		{
			if (false == m_bDecline)
			{
				m_fSlideTime = (2.f * (abs(m_fCurPosX - m_BusDesc.fDestinationX))) / m_fMaxSpeed;
				m_fAcceleration = m_fMaxSpeed / m_fSlideTime;
				m_bDecline = true;
			}
			else
			{
				if (m_fSlideTime > m_fDecliningTime)
				{
					m_fDecliningTime += fTimeDelta;
					m_fCurSpeed -= m_fAcceleration * fTimeDelta;
					if (0.f > m_fCurSpeed)
					{
						m_fCurSpeed = 0.f;
						m_bStopped = true;
						m_fDecliningTime = 0.f;
						m_bDecline = false;
					}
					
				}
				else
				{
					m_fCurSpeed = 0.f;
					m_bStopped = true;
					m_fDecliningTime = 0.f;
					m_bDecline = false;
				}

			}
			
		}
	}

	// 버스역에 도착해서부터 그 이후의 움직임
	if (true == m_bStopped)
	{
		m_fWaitingTime += 1.f *fTimeDelta;

		if (5.f < m_fWaitingTime)
		{
			m_fCurSpeed += m_fAcceleration * fTimeDelta;

			if (m_fMaxSpeed < m_fCurSpeed)
			{
				m_fCurSpeed = m_fMaxSpeed;
			}
		}
	}

	if (0 == m_BusDesc.iLaneIndex)
	{
		MoveFront(fTimeDelta);
	}
	else if (5 == m_BusDesc.iLaneIndex)
	{
		MoveBack(fTimeDelta);
	}
}

void CStage3Bus::MoveBack(_float fTimeDelta)
{
	// 범위를 벗어났는지 체크
	if (-m_BusDesc.fResetX > m_fCurPosX)
	{
		m_fCurPosX = m_BusDesc.fResetX;
		m_bStopped = false;
		m_fWaitingTime = 0.f;
	}

	// 갱신된 위치를 구한다
	m_fCurPosX -= m_fCurSpeed * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, m_fStartZ, 1.f));
}

void CStage3Bus::MoveFront(_float fTimeDelta)
{
	// 범위를 벗어났는지 체크
	if (-m_BusDesc.fResetX < m_fCurPosX)
	{
		m_fCurPosX = m_BusDesc.fResetX;
		m_bStopped = false;
		m_fWaitingTime = 0.f;
	}

	// 갱신된 위치를 구한다
	m_fCurPosX += m_fCurSpeed * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fCurPosX, 0.f, m_fStartZ, 1.f));
}

CStage3Bus * CStage3Bus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Bus*		pInstance = new CStage3Bus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Bus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CStage3Bus::Clone(void * pArg)
{
	CStage3Bus*		pInstance = new CStage3Bus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Bus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage3Bus::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTexture);
}



// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CStage3Bus::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (m_bAttackCol)
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
			m_bAttackCol = false;
			m_fAttackColAcc = 0.f;
		}

	}


}