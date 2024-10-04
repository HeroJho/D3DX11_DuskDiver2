#include "stdafx.h"
#include "..\Public\YMTest.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "ToolManager.h"
#include "Bear_HP_Bar.h"
#include "Crack_Manager.h"
#include "Time_Manager.h"

CYMTest::CYMTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CYMTest::CYMTest(const CYMTest & rhs)
	: CCreture(rhs)
{
}

HRESULT CYMTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYMTest::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Test";

	Make_ImGui();



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(99.83f, -31.4f, 218.273f, 1.f));
	 m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	m_eHitType = TYPE_AWAY;
	m_fDamage = 0.f;
	m_fAirPower = 5.f;
	m_fPushPower = 3.f;

	

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 50.f;
	UiInfoDesc.fSizeY = 50.f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = this;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniHockey"), TM->Get_CurLevel(), TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	return S_OK;
}

void CYMTest::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);


	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	if (true == CRKM->Get_DeleteHockey())
		Set_Dead();

	if (true == CRKM->Get_ActivateHockey())
	{
		m_fTurn += fTimeDelta * 80.f;
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fTurn));

		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		m_fHitTime += fTimeDelta;

		switch (m_iNum)
		{
		case 0:
			if (m_pTransformCom->Move(XMVectorSet(88.67f, -31.4f, 223.f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(88.67f - 81.2f);
				_float y = abs(223.f - 204.228f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 88.67f - x / z * i, -31.4f, 223.f - y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}

				m_iNum++;
			}
			break;
		case 1:
			if (m_pTransformCom->Move(XMVectorSet(81.2f, -31.4f, 204.228f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				m_iNum++;
				_float x = abs(81.2f - 110.735f);
				_float y = abs(204.228f - 214.6f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 81.2f + x / z * i, -31.4f, 204.228f + y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
			}

			break;
		case 2:
			if (m_pTransformCom->Move(XMVectorSet(110.735f, -31.4f, 214.6f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				m_iNum++;
				_float x = abs(110.735f - 120.830f);
				_float y = abs(214.6f - 226.f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 110.735f + x / z * i, -31.4f, 214.6f + y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
			}

			break;
		case 3:
			if (m_pTransformCom->Move(XMVectorSet(120.830f, -31.4f, 226.f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(120.830f - 106.f);
				_float y = abs(226.f - 233.189f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 120.830f - x / z * i, -31.4f, 226.f + y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
				m_iNum++;
			}
			break;
		case 4:
			if (m_pTransformCom->Move(XMVectorSet(106.f, -31.4f, 233.189f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(106.f - 77.f);
				_float y = abs(233.189f - 210.118f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 106.f - x / z * i, -31.4f, 233.189f - y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
				m_iNum++;
			}
			break;
		case 5:
			if (m_pTransformCom->Move(XMVectorSet(77.f, -31.4f, 210.118f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(77.f - 94.54f);
				_float y = abs(210.118f - 203.6f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 77.f + x / z * i, -31.4f, 210.118f - y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
				m_iNum++;
			}
			break;
		case 6:
			if (m_pTransformCom->Move(XMVectorSet(94.54f, -31.4f, 203.6f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(94.54f - 119.5f);
				_float y = abs(203.6f - 231.f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 94.54f + x / z * i, -31.4f, 203.6f + y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
				m_iNum++;
			}
			break;
		case 7:
			if (m_pTransformCom->Move(XMVectorSet(119.5f, -31.4f, 231.f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				_float x = abs(119.5f - 88.67f);
				_float y = abs(231.f - 223.f);
				_int z = (_int)sqrt(x * x + y * y) / 4;

				GI->PlaySoundW(L"se_Env12_Hockey_Shoot.wav", SD_MAP, 0.4f);

				for (int i = 0; i <= z; ++i)
				{
					_float4 Pos = { 119.5f - x / z * i, -31.4f, 231.f - y / z * i, 1.f };
					if ((pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_HockeyReserve"), TM->Get_CurLevel(), TEXT("Layer_UI"), &Pos)))
						return;
				}
				m_iNum++;
			}
			break;
		case 8:
			if (m_pTransformCom->Move(XMVectorSet(88.67f, -31.4f, 223.f, 1.f), 11.f, fTimeDelta, 1.f))
			{
				m_iNum = 0;
			}
			break;
		}

		//m_pTransformCom->Set_TurnSpeed(fTimeDelta);


		Safe_Release(pGameInstance);
	}

}

void CYMTest::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


	if (true == CRKM->Get_ActivateHockey())
	{
		Tick_Col(m_pTransformCom->Get_WorldMatrix());
		Choose_Col("YMTest");
		Render_Col(m_pRendererCom);

	}

}

HRESULT CYMTest::Render()
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


	_float fGama = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
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

void CYMTest::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_HockeyReserve" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("YMTest", Desc.pMyDesc->sTag) && !strcmp("HockeyReserve", Desc.pOtherDesc->sTag))
		{
			((CHockeyReserve*)Desc.pOther)->Set_Dead();
		}
	}

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("YMTest", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag) && m_fHitTime > 1.f)
		{
				((CCreture*)Desc.pOther)->On_Hit(this);
				m_fHitTime = 0.f;
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("YMTest", Desc.pMyDesc->sTag) && !strcmp("Monster_Body1", Desc.pOtherDesc->sTag) && m_fHitTime > 1.f)
		{
			//쿠마 피가 깎이도록
			TM->Get_CumaHpBar()->Deal_HakiHp(500.f);


			((CCreture*)Desc.pOther)->On_Hit(this);

			m_fHitTime = 0.f;
		}
	}
}

void CYMTest::Create_HitEffect(_float4 vWorldPos)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vWorldPos) +
		XMVector3Normalize((XMLoadFloat4(&GI->Get_CamPosition()) - XMLoadFloat4(&vWorldPos) * 1.f)));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + _vector{ GI->Get_FloatRandom(-0.5f,0.5f),
		GI->Get_FloatRandom(-0.5f,0.5f) ,  GI->Get_FloatRandom(-0.5f,0.5f) });

	CPlayer01HitEffect_2::HITEFFECT2INFO* HitEffect2 = new CPlayer01HitEffect_2::HITEFFECT2INFO;
	HitEffect2->vScale = { 3.f,3.f,3.f };
	HitEffect2->vPosition = vPos;
	HitEffect2->vRatio = { 3.f,2.f,20.f };
	HitEffect2->vColor = { 0.f,0.f,0.f,1.f };
	HitEffect2->vColor2 = { 0.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_2", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect2);

	CPlayer01HitEffect_1::HITEFFECT1INFO* HitEffect = new CPlayer01HitEffect_1::HITEFFECT1INFO;
	HitEffect->vScale = { 2.f,2.f,2.f };
	HitEffect->vPosition = vPos;
	HitEffect->vRatio = { 3.f,2.f,15.f };
	HitEffect->vColor = { 1.f,1.f,0.9f,1.f };
	HitEffect->vColor2 = { 1.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);



	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);
}

HRESULT CYMTest::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_HockeyAa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(3.7f, 4.f, 4.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "YMTest");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CYMTest * CYMTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYMTest*		pInstance = new CYMTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYMTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CYMTest::Clone(void * pArg)
{
	CYMTest*		pInstance = new CYMTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYMTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYMTest::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CYMTest::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("YMTest")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("YMTest")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("YMTest")->Set_Size(vSize);
	Get_Colliders("YMTest")->Set_Center(vCenter);



	ImGui::End();
}