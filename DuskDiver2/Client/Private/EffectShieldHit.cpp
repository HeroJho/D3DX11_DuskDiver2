#include "stdafx.h"
#include "..\Public\EffectShieldHit.h"
#include "GameInstance.h"

CEffectShieldHit::CEffectShieldHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffectShieldHit::CEffectShieldHit(const CEffectShieldHit & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectShieldHit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectShieldHit::Initialize(void * pArg)
{
	m_EffectShieldHitDesc = *(EFFECTSHIELDHITDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_EffectShield";

	Set_InitState(m_EffectShieldHitDesc.vPosition, m_EffectShieldHitDesc.vLook);


	return S_OK;
}

void CEffectShieldHit::Tick(_float fTimeDelta)
{

	m_bRender = true;

	//if (true == m_bRender)
		Update_ShiledState(fTimeDelta, _float4(0.f,2.f,0.f,1.f)); // 여기서 누구의 실드이면 그 객체의 위치를 계속 받아온다
}

void CEffectShieldHit::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CEffectShieldHit::Render()
{
	if (true == m_bRender)
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

		// For.Blur
		/*_float fBlurPower = 0.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;*/

		if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveV", &m_fMoveV, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 11)))
				return E_FAIL;
		}
	}
	

	return S_OK;
}

HRESULT CEffectShieldHit::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectSpin"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_ShieldHit"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Tile_Distort_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;*/


	return S_OK;
}

void CEffectShieldHit::Set_InitState(_float4 NewPosition, _float4 NewLook)
{
	_float4 NewUp = { 0.f,0.f,0.f,0.f };
	_float4 NewRight = { 0.f,0.f,0.f,0.f };

	XMStoreFloat4(&NewRight, XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&NewLook))));
	XMStoreFloat4(&NewUp, XMVector4Normalize(XMVector3Cross(XMLoadFloat4(&NewLook), XMLoadFloat4(&NewRight))));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&NewRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&NewUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(XMLoadFloat4(&m_EffectShieldHitDesc.vLook)));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NewPosition));
	m_pTransformCom->Set_Scale(XMVectorSet(m_EffectShieldHitDesc.fSize, m_EffectShieldHitDesc.fSize, m_EffectShieldHitDesc.fSize, 0.f));
}

void CEffectShieldHit::Update_ShiledState(_float fTimeDelta, _float4 NewPosition)
{
	Update_ShieldAlpha(fTimeDelta);
	Update_ShieldPosition(NewPosition);
}

void CEffectShieldHit::Update_ShieldAlpha(_float fTimeDelta)
{
	// uv를 움직인다
	m_fMoveV += 0.5f * fTimeDelta;

	if (0.6f < m_fMoveV)
	{
		//Set_Dead();
		m_fAlpha -= 3.f * fTimeDelta;
	}

	if (1.f < m_fMoveV)
	{
		m_fMoveV = 0.f;
		m_fAlpha = 1.f;
	}
}

void CEffectShieldHit::Update_ShieldPosition(_float4 NewPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NewPosition));
}

CEffectShieldHit * CEffectShieldHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectShieldHit*		pInstance = new CEffectShieldHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectShieldHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectShieldHit::Clone(void * pArg)
{
	CEffectShieldHit*		pInstance = new CEffectShieldHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectShieldHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectShieldHit::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
