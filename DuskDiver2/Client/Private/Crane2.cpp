#include "stdafx.h"
#include "..\Public\Crane2.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "Monster_Manager.h"

#include "ToolManager.h"

CCrane2::CCrane2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CCrane2::CCrane2(const CCrane2 & rhs)
	: CCreture(rhs)
{
}

HRESULT CCrane2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrane2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	Make_ImGui();

	XMVECTOR Pos = *(XMVECTOR*)pArg;
	

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 40.f, 136.f, 1.f));
	if (XMVectorGetW(Pos) == 0)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(Pos, 1.f));
		m_Side = SIDE_RIGHT;
	}
	else
		m_Side = SIDE_LEFT;

	 m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));



	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fHurtTime = 0.02f;

	return S_OK;
}

void CCrane2::Tick(_float fTimeDelta)
{
	Emp_Col();

	switch (m_State)
	{
	case STATE_NARROW:
		State_Narrow(fTimeDelta);
		break;
	case STATE_WIDEN:
		State_Widen(fTimeDelta);
		break;
	case STATE_END:
		break;
	default:
		break;

	}
	m_pTransformCom->Set_TurnSpeed(fTimeDelta);


	if (CK_DOWN(DIK_U))
	{
		m_State = STATE_NARROW;
	}
	else if (CK_DOWN(DIK_I))
	{
		m_State = STATE_WIDEN;
	}
	//else if (CK_DOWN(DIK_O))
	//{
	//	MM->Arcade_Event1();
	//}



	if (XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == 1.344f)
	{
		if (MM->Get_Arcade_Event1())
		{
			m_fAracadeTime += fTimeDelta;
			if (m_fAracadeTime < 3.f)
				m_State = STATE_WIDEN;

			if (m_fAracadeTime <= 6.4f && m_fAracadeTime > 3.6f)
			{
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y -= 0.13f;
					m_fTickTime = 0.f;
				}
			}
			else if (m_fAracadeTime > 5.8f)
				m_State = STATE_NARROW;

			if (m_fAracadeTime > 11.f && 13.8f > m_fAracadeTime)
			{
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y += 0.13f;
					m_fTickTime = 0.f;
				}
			}
		}
	}
	if (XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == 1.6f)
	{
		//if (CK_DOWN(DIK_P))
		//{
		//	MM->Arcade_Event2();
		//}
		if (MM->Get_Arcade_Event2())
		{
			m_fAracadeTime += fTimeDelta;
			if (m_fAracadeTime < 3.f)
				m_State = STATE_WIDEN;

			if (m_fAracadeTime <= 6.4f && m_fAracadeTime > 3.6f)
			{
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y -= 0.13f;
					m_fTickTime = 0.f;
				}
			}
			else if (m_fAracadeTime > 5.8f)
				m_State = STATE_NARROW;

			if (m_fAracadeTime > 11.f && 13.8f > m_fAracadeTime)
			{
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y += 0.13f;
					m_fTickTime = 0.f;
				}
			}
		}
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_OriginPos) + x, XMVectorGetY(m_OriginPos) + y, XMVectorGetZ(m_OriginPos) + z, 1));
}

void CCrane2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

}

HRESULT CCrane2::Render()
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


	_float fGama = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;


		if (MM->Get_Stage4Scene())
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CCrane2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

HRESULT CCrane2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_Claw2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CCrane2::State_Narrow(_float fTimeDelta)
{
	m_fAngleTime += fTimeDelta;

	if (m_fTure > -60.f && m_Side == SIDE_LEFT && m_fAngleTime > 0.015f)
	{
		m_fTure -= 0.25f;
		m_pTransformCom->TurnAngle(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-0.25f));
		m_fAngleTime = 0;

	}
	else if (m_fTure > -60.f && m_Side == SIDE_RIGHT && m_fAngleTime > 0.015f)
	{
		m_fTure -= 0.25f;
		m_pTransformCom->TurnAngle(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.25f));
		m_fAngleTime = 0;
	}
}

void CCrane2::State_Widen(_float fTimeDelta)
{
	m_fAngleTime += fTimeDelta;

	if (m_fTure <= 0.f && m_Side == SIDE_LEFT && m_fAngleTime > 0.01f)
	{
		m_fTure += 0.3f;
		m_pTransformCom->TurnAngle(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.3f));
		m_fAngleTime = 0;

	}
	else if (m_fTure <= 0.f && m_Side == SIDE_RIGHT && m_fAngleTime > 0.01f)
	{
		m_fTure += 0.3f;
		m_pTransformCom->TurnAngle(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-0.3f));
		m_fAngleTime = 0;
	}
}

CCrane2 * CCrane2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrane2*		pInstance = new CCrane2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrane2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CCrane2::Clone(void * pArg)
{
	CCrane2*		pInstance = new CCrane2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrane2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrane2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CCrane2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Tower")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Tower")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Tower")->Set_Size(vSize);
	Get_Colliders("Tower")->Set_Center(vCenter);



	ImGui::End();
}
