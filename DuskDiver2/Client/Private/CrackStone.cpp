#include "stdafx.h"
#include "..\Public\CrackStone.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
//#include "Crack.h"
#include "Crack_Manager.h"

CCrackStone::CCrackStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCrackStone::CCrackStone(const CCrackStone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCrackStone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrackStone::Initialize(void * pArg)
{

	if (nullptr == pArg)
		return E_FAIL;
		
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";


	m_Desc = *((CRACKDESC*)pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_Desc.vPos), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));

	CRKM->BackUp_CrackStone(this);

	return S_OK;
}

void CCrackStone::Tick(_float fTimeDelta)
{
	m_bRenderState = Update_RenderState();
	if (true == m_bRenderState)
	{
		//m_fAlpha += 0.5f * fTimeDelta;
		m_fAlpha += 0.25f * fTimeDelta;
		if (1.f < m_fAlpha)
			m_fAlpha = 1.f;
	}
	else
	{
		m_fAlpha -= 0.5f * fTimeDelta;
		if (0.f > m_fAlpha)
			m_fAlpha = 0.f;
	}
	
	Update_Turn(fTimeDelta);
}

void CCrackStone::LateTick(_float fTimeDelta)
{
	if (0.f < m_fAlpha)
	{
		if (nullptr == m_pRendererCom)
			return;

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
	
}

HRESULT CCrackStone::Render()
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
		/*_float fBlurPower = 0.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
			return E_FAIL;*/
		if (FAILED(m_pShaderCom->Set_RawValue("g_MeshAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 22)))
				return E_FAIL;
		}


	return S_OK;
}

void CCrackStone::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Potal")->Set_Size(vSize);
	Get_Colliders("Potal")->Set_Center(vCenter);
	ImGui::End();
}

void CCrackStone::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("CrackStone", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{	
			//중중간 보스 생성

			UM->Set_QuestPointOff();
			Set_Dead();			
			CRKM->Clear_CrackStone();
		}

	}
}



HRESULT CCrackStone::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_CrackStone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "CrackStone");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

void CCrackStone::Update_Turn(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f, fTimeDelta);
}

_bool CCrackStone::Update_RenderState()
{
	_float4 fPlayerPos = PM->Get_PlayerPos();

	_float fDistance = sqrt((fPlayerPos.x - m_Desc.vPos.x) * (fPlayerPos.x - m_Desc.vPos.x) + (fPlayerPos.z - m_Desc.vPos.z) * (fPlayerPos.z - m_Desc.vPos.z));

	if (m_Desc.fRange > fDistance)
		return true;

	return false;
}


CCrackStone * CCrackStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrackStone*		pInstance = new CCrackStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrackStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrackStone::Clone(void * pArg)
{
	CCrackStone*		pInstance = new CCrackStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrackStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCrackStone::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
