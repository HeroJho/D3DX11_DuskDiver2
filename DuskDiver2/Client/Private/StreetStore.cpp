#include "stdafx.h"
#include "..\Public\StreetStore.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CStreetStore::CStreetStore(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStreetStore::CStreetStore(const CStreetStore & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStreetStore::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStreetStore::Initialize(void * pArg)
{
	//{ 50.f, 0.f, 172.f, 1.f }, // 감자
	//{ 50.f,0.f,165.f,1.f }, // 소세지
	//{ 50.f,0.f,158.f,1.f }, // 와플
	//{ 55.f,0.f,156.f,1.f }, // 지파이
	//{ 55.f,0.f,174.f,1.f }, // 오징어
	//{ 55.f,0.f,165.f,1.f }  // 과일
	_float4		vPosition[6] = {
		{ 58.f,0.f,170.4f,1.f }, // 감자
		{ 58.f,0.f,166.8f,1.f }, // 소세지
		{ 58.f,0.f,163.2f,1.f }, // 와플
		{ 58.f,0.f,156.f,1.f }, // 지파이
		{ 58.f,0.f,174.f,1.f }, // 오징어
		{ 58.f,0.f,159.5f,1.f }  // 과일
	};
	_float		fRotationAngle[6] = { 90.f, 90.f , -90.f ,0.f, 180.f, 90.f };

	m_StreetStoreDesc = *(STREETSTOREDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_Props";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPosition[m_StreetStoreDesc.iStoreIndex].x, 0.f, vPosition[m_StreetStoreDesc.iStoreIndex].z, 1.f));
	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRotationAngle[m_StreetStoreDesc.iStoreIndex]));

	return S_OK;
}

void CStreetStore::Tick(_float fTimeDelta)
{
}

void CStreetStore::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStreetStore::Render()
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

	_float fGama = 0.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 0;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CStreetStore::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("StreetStore", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			switch (m_StreetStoreDesc.iStoreIndex)
			{
			case 0:
				UM->Set_TalkButtonType(3);
				UM->Set_TalkButton(true);
				break;
			case 1:
				break;
			case 2:
				UM->Set_TalkButtonType(3);
				UM->Set_TalkButton(true);
				break;
			case 3:
				UM->Set_TalkButtonType(3);
				UM->Set_TalkButton(true);
				break;
			case 4:
				break;
			case 5:
				UM->Set_TalkButtonType(3);
				UM->Set_TalkButton(true);
				break;
			}
			if (CK_UP(DIK_C))
			{
				UM->Set_StreetShop(true);
				if (false == m_bShopping)
				{
					// 상점을 오픈.
					switch (m_StreetStoreDesc.iStoreIndex)
					{
					case 0: // 감자
						GI->PlaySoundW(L"NM20_Store_01.wav", SD_UI, 0.8f);
						UM->Vendor3Select_On();
						m_bShopping = true;
						break;
					case 1: // 소세지 x
						break;
					case 2: // 와플
						GI->PlaySoundW(L"NF20_Store_01.wav", SD_UI, 0.8f);
						UM->Vendor4Select_On();
						m_bShopping = true;
						break;
					case 3: // 닭강정
						UM->Vendor2Select_On();
						m_bShopping = true;
						break;
					case 4: // 오징어 x
						break;
					case 5: // 과일
						GI->PlaySoundW(L"NM20_Store_01.wav", SD_UI, 0.8f);
						UM->Vendor1Select_On();
						m_bShopping = true;
						break;
					default:
						break;
					}
				}
				else
				{
					UM->Set_StreetShop(false);
					switch (m_StreetStoreDesc.iStoreIndex)
					{
					case 0: // 감자
						GI->PlaySoundW(L"NM20_Store_02.wav", SD_UI, 0.8f);
						UM->Vendor3Select_Off();
						m_bShopping = false;
						CRM->Get_Cam()->End_Target();
						break;
					case 1: // 소세지 x
						break;
					case 2: // 와플
						GI->PlaySoundW(L"NF20_Store_02.wav", SD_UI, 0.8f);
						UM->Vendor4Select_Off();
						m_bShopping = false;
						CRM->Get_Cam()->End_Target();
						break;
					case 3: // 닭강정
						UM->Vendor2Select_Off();
						m_bShopping = false;
						CRM->Get_Cam()->End_Target();
						break;
					case 4: // 오징어 x
						break;
					case 5: // 과일
						GI->PlaySoundW(L"NM20_Store_02.wav", SD_UI, 0.8f);
						UM->Vendor1Select_Off();
						m_bShopping = false;
						CRM->Get_Cam()->End_Target();
						break;
					default:
						break;
					}
				}
			}
		}

	}
}

HRESULT CStreetStore::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	switch (m_StreetStoreDesc.iStoreIndex)
	{
	case 0:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("StreetStore5"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "StreetStore");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}


CStreetStore * CStreetStore::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStreetStore*		pInstance = new CStreetStore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStreetStore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStreetStore::Clone(void * pArg)
{
	CStreetStore*		pInstance = new CStreetStore(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStreetStore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStreetStore::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
