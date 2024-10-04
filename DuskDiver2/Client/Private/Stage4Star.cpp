#include "stdafx.h"
#include "..\Public\Stage4Star.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CStage4Star::CStage4Star(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CStage4Star::CStage4Star(const CStage4Star & rhs)
	: CCreture(rhs)
{
}

HRESULT CStage4Star::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Star::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_StarDesc = *(STARDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	Init_RotateAxis();

	return S_OK;
}

void CStage4Star::Tick(_float fTimeDelta)
{
	Update_Star(fTimeDelta);

	/*if (CK_DOWN(DIK_Z))
	{
		Speed *= 10.f;
		if (10.f < Speed)
			Speed = 1.f;
	}
	if (CK_PRESS(DIK_RSHIFT))
	{
		if (CK_PRESS(DIK_UP))
		{
			NewY += Speed;
		}
		if (CK_PRESS(DIK_DOWN))
		{
			NewY -= Speed;
		}
	}
	else
	{
		if (CK_PRESS(DIK_UP))
		{
			NewZ += Speed;
		}
		if (CK_PRESS(DIK_DOWN))
		{
			NewZ -= Speed;
		}
	}

	if (CK_PRESS(DIK_LEFT))
	{
		NewX -= Speed;
	}
	if (CK_PRESS(DIK_RIGHT))
	{
		NewX += Speed;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewX, NewY, NewZ, 1.f));*/
}

void CStage4Star::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CStage4Star::Render()
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

	m_fBlurPower = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 0.9f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

	//if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpa, sizeof(_float))))
	//	return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 20;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage4Star::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_CenterTransform"), (CComponent**)&m_pCenterTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_DecorationsB"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

void CStage4Star::Init_RotateAxis()
{
	// 공전중심의 가상의 물체의 월드행렬을 구한다.
	// 가상의 물체가 Turn함수로 회전을 하고 
	// Star본체는 이 가상의 물체의 Pos + Radius * Right로 진짜 WorldPos를 구한다. 매 틱에서

	// 가상 물체의 Up
	while (true)
	{
		m_vRtAxis.x = (1.f + m_StarDesc.iStarIndex) * (1.f + m_StarDesc.iStarIndex) * GI->Get_FloatRandom(0.f, 1000.f);
		m_vRtAxis.y = (1.f + m_StarDesc.iStarIndex) * GI->Get_FloatRandom(0.f, 1000.f);
		m_vRtAxis.z = (1.f + m_StarDesc.iStarIndex) * (1.f + m_StarDesc.iStarIndex) * (1.f + m_StarDesc.iStarIndex) * GI->Get_FloatRandom(0.f, 1000.f);

		XMStoreFloat4(&m_vRtAxis, XMVector4Normalize(XMLoadFloat4(&m_vRtAxis)));

		_float  fCos = abs(m_vRtAxis.y);

		if (0.5f <= fCos)
			break;
	}

	// 가상 물체의 Right
	_float4 vRtRight = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vRtRight, XMVector4Normalize(XMVector3Cross(XMVector4Normalize(XMVector3Cross(XMLoadFloat4(&m_vRtAxis), XMVectorSet(0.f, 1.f, 0.f, 0.f))), XMLoadFloat4(&m_vRtAxis))));
	// 가상 물체의 Look
	_float4 vRtLook = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vRtLook, XMVector4Normalize(XMVector3Cross(XMLoadFloat4(&vRtRight), XMLoadFloat4(&m_vRtAxis))));
	// 가상 물체의 Pos
	_float4 vRotateCenterPos[10] = {
		{ -102.f,50.f,2.f,1.f },
		{ 82.f,0.f,14.f,1.f },
		{ 161.f,34.f,255.f,1.f },
		{ 43.f,-7.f,265.f,1.f },
		{ -59.f,8.f,309.f,1.f },
		{ -154.f,25.f,226.f,1.f },
		{ 2.f,16.f,163.f,1.f },
		{ 0.f,0.f,0.f,1.f },
		{ 0.f,0.f,0.f,1.f },
		{ 0.f,0.f,0.f,1.f }
	};

	// 수평공전
	m_vRtAxis = _float4(0.f, 1.f, 0.f, 0.f);
	vRtRight = _float4(1.f, 0.f, 0.f, 0.f);
	vRtLook = _float4(0.f, 0.f, 1.f, 0.f);
	for (_uint i = 0; 10 > i; ++i)
	{
		vRotateCenterPos[i].y += 20.f;
	}

	m_pCenterTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&vRtRight));
	m_pCenterTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&m_vRtAxis));
	m_pCenterTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&vRtLook));
	m_pCenterTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vRotateCenterPos[m_StarDesc.iStarIndex]));

	m_StarDesc.fRadius = 70.f; //80.f
	m_fRtSpeed = pow((-1.f), (_float)m_StarDesc.iStarIndex) * 0.02f;

	m_pTransformCom->Set_Scale(XMVectorSet(5.f, 5.f, 5.f, 1.f));
}

void CStage4Star::Update_Star(_float fTimeDelta)
{
	// 본인의 라업룩을 갱신, 사이즈 갱신
	//Update_SelfState(fTimeDelta);
	

	// 부모에서 정보를 얻어와서 위치를 갱신
	Update_Center(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pCenterTransformCom->Get_State(CTransform::STATE_POSITION) + m_StarDesc.fRadius * m_pCenterTransformCom->Get_State(CTransform::STATE_RIGHT));

}

void CStage4Star::Update_Center(_float fTimeDelta)
{
	m_pCenterTransformCom->Turn(XMLoadFloat4(&m_vRtAxis), m_fRtSpeed, fTimeDelta);
}

void CStage4Star::Update_SelfState(_float fTimeDelta)
{
	_float NewX = GI->Get_FloatRandom(0.f, 1000.f);
	_float NewY = GI->Get_FloatRandom(0.f, 1000.f);
	_float NewZ = GI->Get_FloatRandom(0.f, 1000.f);

	//m_pTransformCom->LookAt(XMLoadFloat4(&PM->Get_PlayerPos()));

	// 사이즈를 변화 시킨다.
	if (false == m_bReverse)
		m_fStarSize += 1.f * fTimeDelta;
	else
		m_fStarSize -= 1.f * fTimeDelta;

	if ((5.f < m_fStarSize) || (1.f > m_fStarSize))
		m_bReverse = !m_bReverse;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fStarSize, m_fStarSize, m_fStarSize, 1.f));
}

CStage4Star * CStage4Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Star*		pInstance = new CStage4Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Star"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Star::Clone(void * pArg)
{
	CStage4Star*		pInstance = new CStage4Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Star"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage4Star::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pRendererCom);
	Safe_Release(m_pCenterTransformCom);
	Safe_Release(m_pTextureCom);
}
