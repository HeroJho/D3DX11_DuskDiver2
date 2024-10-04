#include "stdafx.h"
#include "..\Public\BeastTornadoSide.h"
#include "GameInstance.h"
#include "Status_Manager.h"
#include "Camera_Manager.h"
#include "Time_Manager.h"
#include "Particle_Manager.h"
#include "ToolManager.h"
#include "ViewAlpha.h"
#include "Player_Manager.h"
#include "Effect_Manager.h"

CBeastTornadoSide::CBeastTornadoSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CBeastTornadoSide::CBeastTornadoSide(const CBeastTornadoSide& rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastTornadoSide::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastTornadoSide::Initialize(void * pArg)
{
	m_pInfo = (*(TORNADOSIDEINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bDead = false;
	
	m_vTornadoScale.x = 1.f;
	m_vTornadoScale.y = 1.f;
	m_vTornadoScale.z = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfo->vLocalPos));

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	return S_OK;
}

void CBeastTornadoSide::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (EM->Get_BeastTornadoEnd())
	{
		m_bDeadStart = true;
		m_fDeadAcc = 3.f;
	}

	Check_Death(fTimeDelta);
	if (!m_bDeadStart)
		Change_TornadoSize(fTimeDelta); //토네이도 사이즈 점점 커지게
	Chase_Player(); // 플레이어 추격
	Instant_ChangeSize(fTimeDelta); //정해진 사이즈 됐을때 순간적으로 커졌다가 마지막 본 곳으로 날라감
	
	m_pInfo->vUVFixAcc.x += m_pInfo->vUVFixSpeed.x * fTimeDelta;
	m_pInfo->vUVFixAcc.y += m_pInfo->vUVFixSpeed.y * fTimeDelta;

	//토네이도 회전, 이동
	m_pTransformCom->Turn(_vector{ 0.f,1.f,0.f }, m_fTurnSpeed, fTimeDelta);

	if (!m_bChange)
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta);
}

void CBeastTornadoSide::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	


}

HRESULT CBeastTornadoSide::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_pInfo->vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastTornadoSide::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::End();
}


HRESULT CBeastTornadoSide::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Storm_Side"), TEXT("Effect_Storm_Side"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}


void CBeastTornadoSide::Check_Death(_float fTimeDelta)
{
	if (m_bDeadStart)
	{
		m_fDeadAcc += 1.f * fTimeDelta;
		if (m_fDeadAcc >= 3.f)
		{
			//m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ 400.f*fTimeDelta, 200.f*fTimeDelta,400.f*fTimeDelta });
			m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
			m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;

			if (m_pInfo->vRatio.x <= 0.f)
			{
				Set_Dead();

			}
		}
	}
}


void CBeastTornadoSide::Change_TornadoSize(_float fTimeDelta)
{
	if (m_fLifeTimeAcc >= 10.f && !m_bScaleEnd)
	{
		m_bScaleEnd = true;
		m_bChange = true;
		m_bPlayerChase = false;
		m_pInfo->vColor = { 1.f,1.f,1.f,1.f };
		m_pInfo->vColor2 = { 1.f,0.f,0.f,1.f };
		m_pInfo->fBlurPower = 1.f;
	}
	else if (m_vTornadoScale.y <= 15.f)
	{
		m_vTornadoScale.x += 7.f * fTimeDelta;
		m_vTornadoScale.y += 9.f * fTimeDelta;
		m_vTornadoScale.z += 7.f * fTimeDelta;

		m_vColScale.x += 0.17f * fTimeDelta;
		m_vColScale.y += 0.3f * fTimeDelta;
		m_vColScale.z += 0.17f * fTimeDelta;
	}

	//mesh, collider 사이즈 변경
	m_pTransformCom->Set_Scale(XMVectorSet(m_vTornadoScale.x + m_fPlusScale, m_vTornadoScale.y, m_vTornadoScale.z + m_fPlusScale, 0.f));

	if (m_fTurnSpeed <= 10.f)
		m_fTurnSpeed += 0.8f * fTimeDelta;

	if (m_fSpeed <= 5.f)
		m_fSpeed += 0.8f * fTimeDelta;
}

void CBeastTornadoSide::Instant_ChangeSize(_float fTimeDelta)
{

	if (m_bChange)
	{
		m_fScaleTimeAcc += 1.f * fTimeDelta;
		if (m_bInstantSize)
			m_fPlusScale += 5.f * fTimeDelta;
		else
		{
			m_fPlusScale -= 5.f * fTimeDelta;

			if (m_fPlusScale <= 0.f && !m_bDeadStart)
			{
				m_bChange = false;
				m_bDeadStart = true;
				m_fDeadLimitTime = 5.f;
				m_fSpeed = 22.f;
				m_vPlayerPos = PM->Get_PlayerPos();
				m_vPlayerPos.y = 0.02f;
				_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat3(&m_vTargetLook, vLook);
			}
		}

		if (m_fScaleTimeAcc >= 1.5f)
		{
			m_bInstantSize = false;

		}

	}
}

void CBeastTornadoSide::Chase_Player()
{
	if (m_bPlayerChase)
	{
		m_vPlayerPos = PM->Get_PlayerPos();

		m_vPlayerPos.y = 0.02f; //네비 안태우는 대신에 y는 고정

		_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		XMStoreFloat3(&m_vTargetLook, vLook);
	}
}

CCreture * CBeastTornadoSide::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastTornadoSide*		pInstance = new CBeastTornadoSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastTornadoSide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeastTornadoSide::Clone(void * pArg)
{
	CBeastTornadoSide*		pInstance = new CBeastTornadoSide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastTornadoSide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastTornadoSide::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
