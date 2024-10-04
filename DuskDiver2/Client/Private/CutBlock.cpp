#include "stdafx.h"
#include "..\Public\CutBlock.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "MiniGame_Block.h"
#include "DumiBlock.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"

CCutBlock::CCutBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CCutBlock::CCutBlock(const CCutBlock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCutBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutBlock::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	Make_ImGui();
	m_sTag = "Tag_Cube";


	m_vDestPos = _float3(-130.17f, 33.934f, 103.556f);
	XMStoreFloat3(&m_vGoLook, XMVector3Normalize(XMLoadFloat3(&m_vDestPos) - XMVectorSet(-136.68f, 33.934f, 99.833f, 1.f)));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-136.68f, 34.193f, 99.833f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.25f, 0.5f, 1.f));

	m_bStart = false;

	return S_OK;
}



void CCutBlock::Start()
{
	m_bStart = true;
	// m_pTransformCom->Jump(10.f);
}



void CCutBlock::Tick(_float fTimeDelta)
{

	m_fTimeDeadAcc += fTimeDelta;
	if (10.f < m_fTimeDeadAcc)
	{
		Set_Dead();
		return;
	}

	// 던지는 시간
	if (!m_bCheckTime)
	{
		m_fTimeAcc += fTimeDelta;
		if (2.f < m_fTimeAcc)
		{
			m_bStart = true;
			m_bCheckTime = true;
		}
	}


	if (m_bStart)
	{
		m_pTransformCom->Go_Dir(XMVectorSetW(XMLoadFloat3(&m_vGoLook), 0.f), 13.5f, fTimeDelta);
	}
}

void CCutBlock::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_bStart)
	{

		_float fFloatY = 31.275f;

		m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, 30.f);
		_float4 vPos; XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (fFloatY > vPos.y)
		{
			vPos.y = fFloatY;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat4(&vPos), 1.f));
			m_pTransformCom->ResetGravity();
			GI->PlaySoundW(L"basic_throw.wav", SD_MAP, 1.f);
			m_bStart = false;
		}


	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCutBlock::Render()
{
	if (nullptr == m_pVIBuffer_Cube ||
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

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float4 vColor = { 1.f, 0.5f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBuffer_Cube->Render();

	return S_OK;
}

HRESULT CCutBlock::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_Model"), (CComponent**)&m_pVIBuffer_Cube)))
		return E_FAIL;




	return S_OK;
}

CCutBlock * CCutBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCutBlock*		pInstance = new CCutBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCutBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCutBlock::Clone(void * pArg)
{
	CCutBlock*		pInstance = new CCutBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCutBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutBlock::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Cube);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRigidCom);
}





void CCutBlock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	//IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}

