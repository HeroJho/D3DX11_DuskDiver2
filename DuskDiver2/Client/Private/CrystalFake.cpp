#include "stdafx.h"
#include "..\Public\CrystalFake.h"
#include "GameInstance.h"
#include "CrystalPart.h"
#include "ToolManager.h"

#include "Player_Manager.h"

CCrystalFake::CCrystalFake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CCrystalFake::CCrystalFake(const CCrystalFake & rhs)
	: CCreture(rhs)
{
}

HRESULT CCrystalFake::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrystalFake::Initialize(void * pArg)
{
	m_FakeDesc = *(FAKEDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_FakeDesc.vPosition));

	return S_OK;
}

void CCrystalFake::Tick(_float fTimeDelta)
{
	Update_RenderState();

	if (true == m_bHited)
	{
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 100.f, 0.f, 0.f)); // 충돌체를 이동시켜 충돌 안시키는 용도
		Decrease_Emissive(fTimeDelta);
		CrystalFake_Reset(fTimeDelta);
	}

		
}

void CCrystalFake::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	
	if (false == m_bHited)
	{
		Tick_Col(m_pTransformCom->Get_WorldMatrix());	

		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}
}

HRESULT CCrystalFake::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_EmissivePower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint		iPassIndex = 19;
	if (true == m_bHited)
		iPassIndex = 26;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CCrystalFake::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	
}

void CCrystalFake::On_Hit(CCreture * pOther)
{
	m_bHited = true;
}

HRESULT CCrystalFake::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_CrystalBody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.8f, 0.8f, 0.8f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 1.1f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CCrystalFake::Update_RenderState()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	// 조건1 : 거리가 maximum이내인가?
	_float fLength = 0.f;
	fLength = sqrt((vPlayerPos.x - m_FakeDesc.vPosition.x) * (vPlayerPos.x - m_FakeDesc.vPosition.x) + (vPlayerPos.z - m_FakeDesc.vPosition.z) * (vPlayerPos.z - m_FakeDesc.vPosition.z));
	// 조건2 : 절두체 컬링범위내에 있는가?
	_bool bisInFrustum = GI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f);

	if ((65.f > fLength) && ((true == m_bHited) || (true == bisInFrustum)))
		m_bRender = true;
	else
		m_bRender = false;
}

void CCrystalFake::Decrease_Emissive(_float fTimeDelta)
{
	if (false == m_bReset)
	{
		m_fBlurPower -= 0.25f * fTimeDelta;

		if (0.f > m_fBlurPower)
		{
			m_bReset = true;
			m_fBlurPower = 0.f;
		}
			
	}
}

void CCrystalFake::CrystalFake_Reset(_float fTimeDelta)
{
	m_fDeadTime += 0.5f * fTimeDelta;

	if (5.f < m_fDeadTime)
	{
		m_fBlurPower += 0.25f * fTimeDelta;

		if (0.5f < m_fBlurPower)
		{
			m_bReset = false;
			m_bHited = false;
			m_fDeadTime = 0.f;
			m_fBlurPower = 0.5f;
		}
		
	}
}

CCrystalFake * CCrystalFake::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrystalFake*		pInstance = new CCrystalFake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrystalFake"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrystalFake::Clone(void * pArg)
{
	CCrystalFake*		pInstance = new CCrystalFake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrystalFake"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrystalFake::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
