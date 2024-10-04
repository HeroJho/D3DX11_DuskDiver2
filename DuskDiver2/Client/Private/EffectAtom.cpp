#include "stdafx.h"
#include "..\Public\EffectAtom.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CEffectAtom::CEffectAtom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffectAtom::CEffectAtom(const CEffectAtom & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectAtom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectAtom::Initialize(void * pArg)
{
	_float4 vStartLook[11] = {
		{ 3.7f,-4.65f,8.03f,0.f },
		{ -6.2f,1.95f,7.6f,0.f },
		{ 0.f,-7.29f,6.84f,0.f },
		{ 7.4f,-3.65f,5.64f,0.f },
		{ 9.45f,2.92f,-1.35f,0.f },
		{ -3.4f,0.49f,9.39f,0.f },
		{ -0.35f,-9.75f,-2.17f,0.f },
		{ 6.87f,0.f,-7.26f,0.f },
		{ 0.27f,-8.03f,5.95f,0.f },
		{ -7.35f,-6.59f,-1.41f,0.f },
		{ 3.35f,-6.59f,4.21f,0.f }
	};

	m_EffectAtomDesc = *(EFFECTATOMDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_EffectAtom";

	Set_StartState(vStartLook[m_EffectAtomDesc.iAtomIndex]);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 2.f, 0.f, 1.f));
	/*m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));*/

	return S_OK;
}

void CEffectAtom::Tick(_float fTimeDelta)
{
	Update_Atom(fTimeDelta);
}

void CEffectAtom::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffectAtom::Render()
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
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ColorAlphaUp", &m_fColorAlphaUp, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ColorAlphaDown", &m_fColorAlphaDown, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 8)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffectAtom::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_GroundStar"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Tile_Distort_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

void CEffectAtom::Set_StartState(_float4 vIniLook)
{
	// 전체 과정에서의 움직임 방향을 찾아준다.
	Find_MoveLook(m_EffectAtomDesc.iAtomIndex);

	// 시작할 위치를 잡아준다
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x /*+ 1.5f * PlayerLook.x*/, NewPos.y /*+ 1.5f * PlayerLook.y*/, NewPos.z /*+ 1.5f * PlayerLook.z*/, 1.f));

	// 시작할때의 각 방향에서의 속도를 결정한다.
	m_vFriction = { 1.f, 0.4f, 30.f, 10.f };

	// 시작할때의 자전상태를 결정한다
	_float4 NewRight = { 0.f,0.f,1.f,0.f };
	_float4 NewUp = { 0.f,0.f,1.f,0.f };
	XMStoreFloat4(&NewRight, XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f,1.f,0.f,0.f), XMLoadFloat4(&vIniLook))));
	XMStoreFloat4(&NewUp, XMVector4Normalize(XMVector3Cross(XMLoadFloat4(&vIniLook), XMLoadFloat4(&NewRight))));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&NewRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&NewUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Normalize(XMLoadFloat4(&vIniLook)));
}

void CEffectAtom::Find_MoveLook(_uint Index)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 36.f * (_float)Index);

	XMStoreFloat4(&m_vMoveLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CEffectAtom::Update_Atom(_float fTimeDelta)
{
	// 위치를 변경한다
	_float4 NewPos = { 0.f,0.f,0.f,1.f };
	if (0 < m_vFriction.x)
	{
		XMStoreFloat4(&NewPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		NewPos.x += m_vMoveLook.x * m_vFriction.x;
		NewPos.z += m_vMoveLook.z * m_vFriction.x;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NewPos));
	}

	if (0 < m_vFriction.y)
	{
		XMStoreFloat4(&NewPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		NewPos.y += 1.f * m_vFriction.y;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NewPos));
	}
	// 속도를 변경한다
	m_vFriction.x -= m_vFriction.z * fTimeDelta;
	m_vFriction.y -= m_vFriction.w * fTimeDelta;

	// 속도변경이 끝나면 색상변화를 시작
	if ((0 >= m_vFriction.x) && (0 >= m_vFriction.y))
	{
		if (false == m_bReverse)
			m_fColorAlphaUp += 2.f * fTimeDelta;
		else
		{
			m_fColorAlphaDown += 1.7f * fTimeDelta;
			m_fAlpha -= 2.f * fTimeDelta;
		}
			

		if (1.f < m_fColorAlphaUp)
			m_bReverse = true;
		
		if (0.0f > m_fAlpha)
			m_bDead = true;

	}


	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
}

CEffectAtom * CEffectAtom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectAtom*		pInstance = new CEffectAtom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectAtom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectAtom::Clone(void * pArg)
{
	CEffectAtom*		pInstance = new CEffectAtom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectAtom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectAtom::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
