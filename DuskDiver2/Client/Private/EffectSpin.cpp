#include "stdafx.h"
#include "..\Public\EffectSpin.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CEffectSpin::CEffectSpin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffectSpin::CEffectSpin(const CEffectSpin & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectSpin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectSpin::Initialize(void * pArg)
{
	m_EffectSpinDesc = *(EFFECTSPINDESC*)pArg;

	Set_InitializeDesc((SPINTYPE)m_EffectSpinDesc.iSpinType);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_EffectSpin";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_UpdatedSize = m_EffectSpinDesc.fSpinStartSize;
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	m_fAlpha = m_EffectSpinDesc.fSpinStartAlpha;

	switch (m_EffectSpinDesc.iPlayerType)
	{
	case PLAYERTYPE_YUMO:
		m_vSkillColor = { 1.f,0.9f,0.f };
		break;
	case PLAYERTYPE_LI:
		m_vSkillColor = { 0.f,0.87f,0.84f };
		break;
	case PLAYERTYPE_DD:
		m_vSkillColor = { 0.15f,0.18f,0.89f };
		break;
	default:
		break;
	}

	return S_OK;
}

void CEffectSpin::Tick(_float fTimeDelta)
{
	Update_Spin(fTimeDelta); // 디버깅이 아닐때 얘만 남기면 된다.
}

void CEffectSpin::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffectSpin::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	// Shader에 텍스처를 던져준다.
	if ((TYPE_RING == m_EffectSpinDesc.iSpinType) || (TYPE_BlastBall == m_EffectSpinDesc.iSpinType))
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vSpinSkillColor", &m_vSkillColor, sizeof(_float3))))
			return E_FAIL;
	}
	else if (TYPE_LENS == m_EffectSpinDesc.iSpinType)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_LensTexture", m_EffectSpinDesc.iPlayerType)))
			return E_FAIL;
	}
	else if ((TYPE_LINEBALL == m_EffectSpinDesc.iSpinType))
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_LineBallTexture")))
			return E_FAIL;
	}
	else if ((TYPE_LANDING == m_EffectSpinDesc.iSpinType) || (TYPE_KICKAURA == m_EffectSpinDesc.iSpinType))
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveV", &m_MoveUV.y, sizeof(_float))))
			return E_FAIL;
	}
	else if ((TYPE_RINGWIND2 == m_EffectSpinDesc.iSpinType) )
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveU", &m_MoveUV.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveV", &m_MoveUV.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_LensTexture", 0)))
			return E_FAIL;
	}
	else if ((TYPE_METAMORPHOSIS == m_EffectSpinDesc.iSpinType))
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveU", &m_MoveUV.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vSpinSkillColor", &m_vSkillColor, sizeof(_float3))))
			return E_FAIL;
	}

	// 바닥 아이템의 색상을 결정한다
	_float3 vItemGroundColor = { 1.f, 0.9f, 0.f };
	if (TYPE_ItemGround == m_EffectSpinDesc.iSpinType)
	{
		vItemGroundColor = { 1.f, 0.9f, 0.f }; // 보통은 노랑색
	}
	else if (TYPE_ItemGroundClue == m_EffectSpinDesc.iSpinType)
	{
		vItemGroundColor = { 0.42f, 0.f, 1.f }; // 단서는 보라색
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ItemGroundColor", &vItemGroundColor, sizeof(_float3))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_EffectSpinDesc.iPassIndex)))
			return E_FAIL;
	}
	

	return S_OK;
}

void CEffectSpin::Set_EffectPos(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
}

void CEffectSpin::Set_UpdateSize(_float fSize)
{
	m_UpdatedSize = fSize;
}

HRESULT CEffectSpin::Ready_Components()
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
	switch (m_EffectSpinDesc.iSpinType)
	{
	case TYPE_RING:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Ring"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_LENS:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Lens"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lens"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case TYPE_SMOKEBALL:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_SmokeBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_RINGWIND:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_RingWind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_LINEBALL:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_LineBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Tile_Distort_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case TYPE_GROUNDWIND:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_GroundWind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_GROUNDBALL:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_GroundBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_DoubleKick:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_DoubleKick"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_BlastBall:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_BlastBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_LANDING:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Landing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Tile_Distort_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;*/
		break;
	case TYPE_KICKAURA:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_KickAura"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_RINGWIND2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_RingWind2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_Wind_RG_00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case TYPE_METAMORPHOSIS:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Metamorphosis"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_ItemGround:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_ItemGround"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_ItemGroundClue:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_ItemGround"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

void CEffectSpin::Set_InitializeDesc(SPINTYPE iSpinIndex)
{
	switch (iSpinIndex)
	{
	case TYPE_RING:
		m_EffectSpinDesc.fSpinAngleSpeed = 15.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 40.f;
		m_EffectSpinDesc.fSpinStartSize = 10.f;
		m_EffectSpinDesc.fSpinStartAlpha = 0.4f;
		m_EffectSpinDesc.iPassIndex = 1;
		break;
	case TYPE_LENS:
		m_EffectSpinDesc.fSpinAngleSpeed = -8.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 20.f;
		m_EffectSpinDesc.fSpinStartSize = 25.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 2;
		break;
	case TYPE_SMOKEBALL:
		m_EffectSpinDesc.fSpinAngleSpeed = 10.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 150.f;
		m_EffectSpinDesc.fSpinStartSize = 5.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 3;
		break;
	case TYPE_RINGWIND:
		m_EffectSpinDesc.fSpinAngleSpeed = 80.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 15.f;
		m_EffectSpinDesc.fSpinStartSize = 0.8f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 4;
		break;
	case TYPE_LINEBALL:
		m_EffectSpinDesc.fSpinAngleSpeed = 15.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 20.f;
		m_EffectSpinDesc.fSpinStartAlpha = 0.f;
		m_EffectSpinDesc.fSpinStartSize = 64.f;
		m_EffectSpinDesc.iPassIndex = 5;
		break;
	case TYPE_GROUNDWIND:
		m_EffectSpinDesc.fSpinAngleSpeed = 15.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 20.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.fSpinStartSize = 2.f;
		m_EffectSpinDesc.iPassIndex = 6;
		break;
	case TYPE_GROUNDBALL:
		m_EffectSpinDesc.fSpinAngleSpeed = 0.8f;
		m_EffectSpinDesc.fSpinSizeSpeed = 2.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.fSpinStartSize = 8.f;
		m_EffectSpinDesc.iPassIndex = 7;
		break;
	case TYPE_DoubleKick:
		m_EffectSpinDesc.fSpinAngleSpeed = 15.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 65.0f;
		m_EffectSpinDesc.fSpinStartAlpha = 0.f;
		m_EffectSpinDesc.fSpinStartSize = 5.f;
		m_EffectSpinDesc.iPassIndex = 12;
		break;
	case TYPE_BlastBall:
		m_EffectSpinDesc.fSpinAngleSpeed = 15.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 90.0f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.fSpinStartSize = 1.f;
		m_EffectSpinDesc.iPassIndex = 1;
		break;
	case TYPE_LANDING:
		m_EffectSpinDesc.fSpinAngleSpeed = 0.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 10.0f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.fSpinStartSize = 5.f;
		m_EffectSpinDesc.iPassIndex = 14;
		break;
	case TYPE_KICKAURA:
		m_EffectSpinDesc.fSpinAngleSpeed = 10.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 2.0f;
		m_EffectSpinDesc.fSpinStartAlpha = 0.3f;
		m_EffectSpinDesc.fSpinStartSize = 40.f;
		m_EffectSpinDesc.iPassIndex = 19;
		break;
	case TYPE_RINGWIND2:
		m_EffectSpinDesc.fSpinAngleSpeed = 14.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 15.f;
		m_EffectSpinDesc.fSpinStartSize = 0.8f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 4;
		break;
	case TYPE_METAMORPHOSIS:
		m_EffectSpinDesc.fSpinAngleSpeed = 0.f;
		m_EffectSpinDesc.fSpinSizeSpeed = 0.f;
		m_EffectSpinDesc.fSpinStartSize = 0.2f;
		m_EffectSpinDesc.fSpinStartAlpha = 0.15f;
		m_EffectSpinDesc.iPassIndex = 20;
		break;
	case TYPE_ItemGround:
		m_EffectSpinDesc.fSpinAngleSpeed = 1.5f;
		m_EffectSpinDesc.fSpinSizeSpeed = 0.f;
		m_EffectSpinDesc.fSpinStartSize = 1.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 21;
		break;
	case TYPE_ItemGroundClue:
		m_EffectSpinDesc.fSpinAngleSpeed = 1.5f;
		m_EffectSpinDesc.fSpinSizeSpeed = 0.f;
		m_EffectSpinDesc.fSpinStartSize = 1.f;
		m_EffectSpinDesc.fSpinStartAlpha = 1.f;
		m_EffectSpinDesc.iPassIndex = 21;
		break;
	default:
		break;
	}
}

void CEffectSpin::Update_Spin(_float fTimeDelta)
{
	switch (m_EffectSpinDesc.iSpinType)
	{
	case TYPE_RING:
		Update_TypeRing(fTimeDelta);
		break;
	case TYPE_LENS:
		Update_TypeLens(fTimeDelta);
		break;
	case TYPE_SMOKEBALL:
		Update_TypeSmokeBall(fTimeDelta);
		break;
	case TYPE_RINGWIND:
		Update_TypeRingWind(fTimeDelta);
		break;
	case TYPE_LINEBALL:
		Update_TypeLineBall(fTimeDelta);
		break;
	case TYPE_GROUNDWIND:
		Update_TypeGroundWind(fTimeDelta);
		break;
	case TYPE_GROUNDBALL:
		Update_TypeGroundBall(fTimeDelta);
		break;
	case TYPE_DoubleKick:
		Update_TypeDoubleKick(fTimeDelta);
		break;
	case TYPE_BlastBall:
		Update_TypeBlastBall(fTimeDelta);
		break;
	case TYPE_LANDING:
		Update_TypeLanding(fTimeDelta);
		break;
	case TYPE_KICKAURA:
		Update_TypeKickAura(fTimeDelta);
		break;
	case TYPE_RINGWIND2:
		Update_TypeRingWind(fTimeDelta);
		break;
	case TYPE_METAMORPHOSIS:
		Update_TypeMetamorphosis(fTimeDelta);
		break;
	case TYPE_ItemGround:
		Update_TypeItemGround(fTimeDelta);
		break;
	case TYPE_ItemGroundClue:
		Update_TypeItemGround(fTimeDelta);
		break;
	default:
		break;
	}
}

void CEffectSpin::Update_TypeRing(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.

	if (false == m_bReverse)
		m_UpdatedSize += m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;
	else
		m_UpdatedSize -= 2.5f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	if (0.05f * m_EffectSpinDesc.fSpinStartSize > m_UpdatedSize)
	{
		m_bDead = true;
		/*m_UpdatedSize = m_EffectSpinDesc.fSpinStartSize;
		m_bRender = false;
		m_bReverse = false;
		m_bAlphaReverse = false;
		m_fAlpha = m_EffectSpinDesc.fSpinStartAlpha;*/
	}
	else if (2.5f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		m_bReverse = true;
	}

	// alpha를 수정한다.
	if (false == m_bAlphaReverse)
		m_fAlpha += 1.5f * fTimeDelta;
	else
	{
		m_fAlpha -= 4.f * fTimeDelta; //4.f -> 거의 마지막에 사라짐
		if (0.f > m_fAlpha)
			m_fAlpha = 0.f;
	}

	if (1.f < m_fAlpha)
	{
		m_bAlphaReverse = true;
	}

	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize * 0.3f, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y + 1.f, NewPos.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeLens(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.

	m_UpdatedSize -= 2.5f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	if (0.05f * m_EffectSpinDesc.fSpinStartSize > m_UpdatedSize)
	{
		m_bDead = true;

	}
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y + 0.1f, NewPos.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeSmokeBall(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	m_UpdatedSize += 1.7f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	if (15.f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		m_bDead = true;
		m_UpdatedSize = m_EffectSpinDesc.fSpinStartSize;
		m_bRender = false;

		//CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
		//EffectSpinDesc.iSpinType = 3;
		//GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc); // 플레이어 스킬 RingWind
	}
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + PlayerLook.x, NewPos.y + PlayerLook.y + 0.1f, NewPos.z + PlayerLook.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeRingWind(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	if (false == m_bFindMat)
	{
		m_bFindMat = true;
		m_pTransformCom->Set_WorldMatrix(PlayerMat);
	}
		
	if (false == m_bReverse)
		m_UpdatedSize += 0.6f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	m_MoveUV.x += 1.2f * fTimeDelta;
	m_MoveUV.y += 5.f * fTimeDelta;


	if (10.f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		if (0.f > m_fAlpha)
		{
			m_bDead = true;
		}

		m_bReverse = true;
		// alpha를 수정한다.
		m_fAlpha -= 2.f * fTimeDelta;

	}

	
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + 1.1f * PlayerLook.x, NewPos.y + 1.1f * PlayerLook.y + 1.f, NewPos.z + 1.1f * PlayerLook.z, 1.f));
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize * 0.2f, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMLoadFloat4(&PlayerLook), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeLineBall(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	if (false == m_bFindMat)
	{
		m_bFindMat = true;
		m_pTransformCom->Set_WorldMatrix(PlayerMat);
	}

	m_fSpinTime += 6.f * fTimeDelta;

	if (3.5f < m_fSpinTime)
	{
		if (0.5f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
			m_UpdatedSize -= 6.f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;
		m_fAlpha += 1.7f * fTimeDelta;
	}
	

	if (1.1f < m_fAlpha)
	{
		m_bDead = true;
	}

	
	
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + 1.1f * PlayerLook.x, NewPos.y + 1.1f * PlayerLook.y + 1.f, NewPos.z + 1.1f * PlayerLook.z, 1.f));
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMLoadFloat4(&PlayerLook), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeGroundWind(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	if (false == m_bFindMat)
	{
		m_bFindMat = true;
		m_pTransformCom->Set_WorldMatrix(PlayerMat);
	}

	if (5.f * m_EffectSpinDesc.fSpinStartSize > m_UpdatedSize)
	{
		m_UpdatedSize += 3.f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;
	}
	else
	{
		m_fAlpha -= 6.f * fTimeDelta;

		if (0.f > m_fAlpha)
		{
			m_UpdatedSize = m_EffectSpinDesc.fSpinStartSize;
			m_fAlpha = m_EffectSpinDesc.fSpinStartAlpha;
			m_bRender = false;
			m_bFindMat = false;
			m_bDead = true;
		}
	}

	// 위치를 잡아준다
	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + 1.5f * PlayerLook.x, NewPos.y + 1.5f * PlayerLook.y, NewPos.z + 1.5f * PlayerLook.z, 1.f));*/
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y, NewPos.z, 1.f));
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeGroundBall(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	if (false == m_bFindMat)
	{
		m_bFindMat = true;
		m_pTransformCom->Set_WorldMatrix(PlayerMat);
	}

	if (0.7f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		m_UpdatedSize -= 3.5f * m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;
	}
	else
	{
		m_fAlpha -= 2.f * fTimeDelta;

		if (0.f > m_fAlpha)
		{
			m_UpdatedSize = m_EffectSpinDesc.fSpinStartSize;
			m_fAlpha = m_EffectSpinDesc.fSpinStartAlpha;
			m_bRender = false;
			m_bFindMat = false;
			m_bDead = true;
		}
	}

	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + 1.5f * PlayerLook.x, NewPos.y + 1.5f * PlayerLook.y, NewPos.z + 1.5f * PlayerLook.z, 1.f));
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeDoubleKick(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.

	m_UpdatedSize += m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	m_fAlpha += 2.5f * fTimeDelta;

	if (6.f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		m_bDead = true;
		
	}

	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize * 1.f, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y + 1.f, NewPos.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeBlastBall(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	m_UpdatedSize += m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	if (60.f * m_EffectSpinDesc.fSpinStartSize < m_UpdatedSize)
	{
		m_bDead = true;

		

		/*EFFECTSPINDESC EffectSpinDesc;
		EffectSpinDesc.iSpinType = (_uint)TYPE_LANDING;
		GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc);*/
	}

	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x + 1.f * PlayerLook.x, NewPos.y + 1.f * PlayerLook.y - 0.1f, NewPos.z + 1.f * PlayerLook.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeLanding(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.

	m_UpdatedSize += m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	m_MoveUV.y -= 0.9f * fTimeDelta;

	if (-0.7f > m_MoveUV.y)
	{
		m_bDead = true;
	}

	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y - 0.f, NewPos.z, 1.f));
	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeKickAura(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.

	m_fAlpha -= 0.2f * fTimeDelta;

	m_UpdatedSize += m_EffectSpinDesc.fSpinSizeSpeed * fTimeDelta;

	if (0.f > m_fAlpha)
		m_bDead = true;

	m_MoveUV.x += 1.f * fTimeDelta;

	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));
	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y + 0.2f, NewPos.z, 1.f));
	// 회전 시킨다
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

void CEffectSpin::Update_TypeMetamorphosis(_float fTimeDelta)
{
	_float4 NewPos = PM->Get_PlayerPos(); // 플레이어의 위치를 얻어온다.
	_float4x4 PlayerMat = PM->Get_PlayerWorldMatrix();
	_float4 PlayerLook = { 0.f,0.f,0.f,0.f };
	PlayerLook.x = PlayerMat._31;
	PlayerLook.y = PlayerMat._32;
	PlayerLook.z = PlayerMat._33;

	_float4 NewRight = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&NewRight, XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&PlayerLook))));

	m_fSpinTime += 1.f * fTimeDelta;

	if (2.f < m_fSpinTime)
		m_fAlpha += 1.1f * fTimeDelta;

	if (1.f > m_UpdatedSize)
		m_UpdatedSize += 1.4f *fTimeDelta;

	if (1.f < m_fAlpha)
		m_bDead = true;

	m_MoveUV.x += 1.f * fTimeDelta;

	// 위치를 잡아준다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewPos.x, NewPos.y + 0.2f, NewPos.z, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&NewRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&PlayerLook));
	// 크기를 변화시킨다.
	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));

}

void CEffectSpin::Update_TypeItemGround(_float fTimeDelta)
{
	// 얘는 위치를 다른곳에서 지정해줄것이다.

	m_pTransformCom->Set_Scale(XMVectorSet(m_UpdatedSize, m_UpdatedSize, m_UpdatedSize, 1.f));

	// 회전 시킨다
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectSpinDesc.fSpinAngleSpeed, fTimeDelta);
}

CEffectSpin * CEffectSpin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectSpin*		pInstance = new CEffectSpin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectSpin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectSpin::Clone(void * pArg)
{
	CEffectSpin*		pInstance = new CEffectSpin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectSpin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectSpin::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
