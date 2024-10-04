#include "stdafx.h"
#include "..\Public\EffectShield.h"
#include "GameInstance.h"
#include "Creture.h"

CEffectShield::CEffectShield(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffectShield::CEffectShield(const CEffectShield & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectShield::Initialize(void * pArg)
{
	m_EffectShieldDesc = *(EFFECTSHIELDDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_EffectShield";

	m_EffectShieldDesc.vColor = {1.f, 0.497f, 0.288f, 1.f};
	m_EffectShieldDesc.vColor2 = { 1.f, 0.f, 0.f, 1.f };
	m_EffectShieldDesc.vRatio = {0.1f, 0.1f};
	m_EffectShieldDesc.fBlurPower = 1.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EffectShieldDesc.vPosition));
	m_pTransformCom->Set_Scale(XMVectorSet(m_EffectShieldDesc.fSize, m_EffectShieldDesc.fSize, m_EffectShieldDesc.fSize, 0.f));

	return S_OK;
}

void CEffectShield::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (m_bOff)
		{
			m_EffectShieldDesc.vRatio.x -= 1.2f * fTimeDelta;
			m_EffectShieldDesc.vRatio.y -= 1.2f * fTimeDelta;


			if (m_EffectShieldDesc.vRatio.x <= 0.1f || m_EffectShieldDesc.vRatio.y <= 0.1f)
			{
				m_EffectShieldDesc.vRatio.x = 0.1f;
				m_EffectShieldDesc.vRatio.y = 0.1f;
				m_bRender = false;
				m_bOff = false;
			}
		}
		else
		{
			m_EffectShieldDesc.vRatio.x += 1.2f * fTimeDelta;
			m_EffectShieldDesc.vRatio.y += 1.2f * fTimeDelta;
			if (m_EffectShieldDesc.vRatio.x >= 1.f || m_EffectShieldDesc.vRatio.y >= 1.f)
			{
				m_EffectShieldDesc.vRatio.x = 1.f;
				m_EffectShieldDesc.vRatio.y = 1.f;
			}

		}

		//Update_ShiledState(fTimeDelta, m_EffectShieldDesc.pDesc->Get_WorldPos()); // 여기서 누구의 실드이면 그 객체의 위치를 계속 받아온다
	}
}

void CEffectShield::LateTick(_float fTimeDelta)
{
	if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffectShield::Render()
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


	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_EffectShieldDesc.vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_EffectShieldDesc.vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_EffectShieldDesc.vRatio.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_EffectShieldDesc.vRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_EffectShieldDesc.fBlurPower, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffectShield::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Effect_Shield", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			//타격을 막는 이펙트 생성??

		}

	}
}

void CEffectShield::Reset_Ratio()
{
	m_EffectShieldDesc.vRatio = { 0.1f, 0.1f };

}

HRESULT CEffectShield::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Shield2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Tile_Distort_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;*/

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Effect_Shield");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CEffectShield::Update_ShiledState(_float fTimeDelta, _float4 NewPosition)
{
	Update_ShieldAlpha(fTimeDelta);
	Update_ShieldPosition(NewPosition);
}

void CEffectShield::Update_ShieldAlpha(_float fTimeDelta)
{
	if (false == m_bAlphaReverse)
		m_fAlpha -= 0.3f * fTimeDelta;
	else
		m_fAlpha += 0.3f * fTimeDelta;

	if ((0.4f > m_fAlpha) || (1.f < m_fAlpha))
	{
		m_bAlphaReverse = !m_bAlphaReverse;
	}
}

void CEffectShield::Update_ShieldPosition(_float4 NewPosition)
{
	XMStoreFloat4(&NewPosition, XMVectorSetY(XMLoadFloat4(&NewPosition), NewPosition.y += 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NewPosition));
}

CEffectShield * CEffectShield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectShield*		pInstance = new CEffectShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectShield::Clone(void * pArg)
{
	CEffectShield*		pInstance = new CEffectShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectShield::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
