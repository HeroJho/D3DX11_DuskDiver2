#include "stdafx.h"
#include "..\Public\Block.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "MiniGame_Block.h"
#include "DumiBlock.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Player_Manager.h"
#include "BlockCuma.h"

CBlock::CBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CBlock::CBlock(const CBlock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlock::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *(BLOCKDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	Make_ImGui();
	m_sTag = "Tag_Cube";



	_vector vPos = XMVectorSetW(XMLoadFloat3(&m_Desc.vPos), 1.f);
	
	if (m_Desc.bDir)
	{
		vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + m_Desc.fMaxDis);
		vPos = XMVectorSetZ(vPos, XMVectorGetZ(XMLoadFloat3(&m_Desc.vPos)));
	}

	else
	{
		vPos = XMVectorSetZ(vPos, XMVectorGetZ(vPos) + m_Desc.fMaxDis);
		vPos = XMVectorSetX(vPos, XMVectorGetX(XMLoadFloat3(&m_Desc.vPos)));
	}

	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&m_Desc.vScale), 0.f));



	return S_OK;
}



_bool CBlock::Compute_Slies(CBlock* pCurBlock, _bool bBome)
{
	// 깔린 블럭의 함수이다!!!!!


	CTransform* pTran = (CTransform*)pCurBlock->Get_ComponentPtr(TEXT("Com_Transform"));

	// 나와 상대의 밑면4점 위치를 구한다.
	_vector vMePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMeScale = m_pTransformCom->Get_Scale();
	_vector vOtherPos = pTran->Get_State(CTransform::STATE_POSITION);
	_float3 vOtherScale = pTran->Get_Scale();






	_vector vMe_MxPz = vMePos + XMVectorSet(-vMeScale.x * 0.5f, 0.f, vMeScale.z * 0.5f, 1.f);
	_vector vMe_PxPz = vMePos + XMVectorSet(vMeScale.x * 0.5f, 0.f, vMeScale.z * 0.5f, 1.f);
	_vector vMe_PxMz = vMePos + XMVectorSet(vMeScale.x * 0.5f, 0.f, -vMeScale.z * 0.5f, 1.f);
	_vector vMe_MxMz = vMePos + XMVectorSet(-vMeScale.x * 0.5f, 0.f, -vMeScale.z * 0.5f, 1.f);

	_vector vOther_MxPz = vOtherPos + XMVectorSet(-vOtherScale.x * 0.5f, 0.f, vOtherScale.z * 0.5f, 1.f);
	_vector vOther_PxPz = vOtherPos + XMVectorSet(vOtherScale.x * 0.5f, 0.f, vOtherScale.z * 0.5f, 1.f);
	_vector vOther_PxMz = vOtherPos + XMVectorSet(vOtherScale.x * 0.5f, 0.f, -vOtherScale.z * 0.5f, 1.f);
	_vector vOther_MxMz = vOtherPos + XMVectorSet(-vOtherScale.x * 0.5f, 0.f, -vOtherScale.z * 0.5f, 1.f);


	_float fXScale = 0.f;
	_float fZScale = 0.f;
	_bool bMeXFirst = false;
	_bool bMeZFirst = false;
	// x축 겹침을 확인한다. 
	if (!m_Desc.bDir)
	{
		if (XMVectorGetX(vMe_PxPz) > XMVectorGetX(vOther_MxPz) && XMVectorGetX(vMe_MxPz) < XMVectorGetX(vOther_PxPz))
		{
			// 겹쳤다면 겹친 크기도 구한다.
			if (XMVectorGetX(vMe_PxPz) <= XMVectorGetX(vOther_PxPz))
			{
				fXScale = XMVectorGetX(vMe_PxPz) - XMVectorGetX(vOther_MxMz);
				bMeXFirst = true;
			}
			else
				fXScale = XMVectorGetX(vOther_PxPz) - XMVectorGetX(vMe_MxMz);

			// other의 크기를 차이 Scale로 바꾼다.
			if (!bBome)
			{
				((COBB*)*((*pCurBlock->Get_Colliders()).begin()))->Set_SizeOBB(_float3(fXScale, vOtherScale.y, vOtherScale.z));
				pTran->Set_Scale(XMVectorSet(fXScale, vOtherScale.y, vOtherScale.z, 1.f));
			}
		}


	}
	else
	{	
		// z축 겹침을 확인한다. 겹쳤다면 겹친 크기도 구한다.
		if (XMVectorGetZ(vMe_PxPz) > XMVectorGetZ(vOther_PxMz) && XMVectorGetZ(vMe_PxMz) < XMVectorGetZ(vOther_PxPz))
		{
			// 겹쳤다면 겹친 크기도 구한다.
			if (XMVectorGetZ(vMe_MxPz) <= XMVectorGetZ(vOther_PxPz))
			{
				fZScale = XMVectorGetZ(vMe_PxPz) - XMVectorGetZ(vOther_MxMz);
				bMeZFirst = true;
			}
			else
				fZScale = XMVectorGetZ(vOther_PxPz) - XMVectorGetZ(vMe_MxMz);

			// other의 크기를 차이 Scale로 바꾼다.
			if (!bBome)
			{
				((COBB*)*((*pCurBlock->Get_Colliders()).begin()))->Set_SizeOBB(_float3(vOtherScale.x, vOtherScale.y, fZScale));
				pTran->Set_Scale(XMVectorSet(vOtherScale.x, vOtherScale.y, fZScale, 1.f));
			}

		}


	}




	// 겹치는 부분이 하나도 없다 -> 탈락 다시
	if (0.f == fXScale && 0.f == fZScale && !bBome)
	{
		CRM->Get_Cam()->End_Target();
		GI->PlaySoundW(L"minigame3_fail.wav", SD_MAP, 1.f);

		return false;
	}




	_vector vEditPos;
	_vector vDumiPos;
	_vector vDumiScale;
	if (!m_Desc.bDir)
	{
		if (bMeXFirst)
		{
			// other + -> me +
			vEditPos = XMVectorSetX(vMePos, XMVectorGetX(vMePos) + vMeScale.x * 0.5f - fXScale * 0.5f);
			vEditPos = XMVectorSetY(vEditPos, XMVectorGetY(vOtherPos));

			vDumiPos = XMVectorSetX(vMePos, XMVectorGetX(vMePos) + vMeScale.x * 0.5f + (vOtherScale.x - fXScale) * 0.5f);
		}
		else
		{
			vEditPos = XMVectorSetX(vMePos, XMVectorGetX(vMePos) - vMeScale.x * 0.5f + fXScale * 0.5f);
			vEditPos = XMVectorSetY(vEditPos, XMVectorGetY(vOtherPos));

			vDumiPos = XMVectorSetX(vMePos, XMVectorGetX(vMePos) - vMeScale.x * 0.5f - (vOtherScale.x - fXScale) * 0.5f);
		}
		vDumiScale = XMLoadFloat3(&vOtherScale);
		vDumiScale = XMVectorSetX(vDumiScale, vOtherScale.x - fXScale);
	}
	else
	{
		if (bMeZFirst)
		{
			// other + -> me +
			vEditPos = XMVectorSetZ(vMePos, XMVectorGetZ(vMePos) + vMeScale.z * 0.5f - fZScale * 0.5f);
			vEditPos = XMVectorSetY(vEditPos, XMVectorGetY(vOtherPos));

			vDumiPos = XMVectorSetZ(vMePos, XMVectorGetZ(vMePos) + vMeScale.z * 0.5f + (vOtherScale.z - fZScale) * 0.5f);
		}
		else
		{
			vEditPos = XMVectorSetZ(vMePos, XMVectorGetZ(vMePos) - vMeScale.z * 0.5f + fZScale * 0.5f);
			vEditPos = XMVectorSetY(vEditPos, XMVectorGetY(vOtherPos));

			vDumiPos = XMVectorSetZ(vMePos, XMVectorGetZ(vMePos) - vMeScale.z * 0.5f - (vOtherScale.z - fZScale) * 0.5f);
		}
		vDumiScale = XMLoadFloat3(&vOtherScale);
		vDumiScale = XMVectorSetZ(vDumiScale, vOtherScale.z - fZScale);
	}

	vDumiPos = XMVectorSetY(vDumiPos, XMVectorGetY(vOtherPos));

	if (!bBome)
		pTran->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vEditPos, 1.f));


	CDumiBlock::DUMIBLOCK DumiDesc;
	if (bBome)
	{
		// 나머지 스케일로 떨어지는 블럭을 만든다.
		//CDumiBlock::DUMIBLOCK DumiDesc;
		vOtherPos = XMVectorSetY(vOtherPos, XMVectorGetY(vMePos));
		XMStoreFloat3(&DumiDesc.vPos, vOtherPos);
		DumiDesc.vScale = vOtherScale;
		DumiDesc.vColor = pCurBlock->Get_Desc().vColor;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("DumiBlock"), TM->Get_CurLevel(), TEXT("Mini_Block"), &DumiDesc)))
			return false;
	}
	else
	{
		// 나머지 스케일로 떨어지는 블럭을 만든다.
		//CDumiBlock::DUMIBLOCK DumiDesc;
		XMStoreFloat3(&DumiDesc.vPos, vDumiPos);
		XMStoreFloat3(&DumiDesc.vScale, XMVectorSetW(vDumiScale, 1.f));
		DumiDesc.vColor = pCurBlock->Get_Desc().vColor;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("DumiBlock"), TM->Get_CurLevel(), TEXT("Mini_Block"), &DumiDesc)))
			return false;
	}




	// 블럭 설치 결과를 계산해서 두더지, 플레이어에 데미지를 입힌다.
	_float fRatio = 0.f;
	if (!m_Desc.bDir)
		fRatio = XMVectorGetX(vDumiScale) / vOtherScale.x;
	else
		fRatio = XMVectorGetZ(vDumiScale) / vOtherScale.z;

	Compute_Result(pCurBlock->Get_TurnCount(), fRatio, bBome);


	return true;
}

void CBlock::Compute_Result(_uint iTurnCount, _float fRatio, _bool bBome)
{
	
	
	// fDumiSize에 비례해서 두더지한테 데미지를 입힌다.
	// 0에 가까울 수록 잘한 것, 1에 가까울 수록 못 한 것.
	
	if (bBome && 0.99f > fRatio)
	{
		// Player에게 중첩된 데미지를 입힌다.

		// 데미지
		_float3 vDir;  XMStoreFloat3(&vDir, -1.f * XMLoadFloat3((_float3*)&PM->Get_PlayerWorldMatrix().m[2]));
		vDir.y = 0.f;
		PM->Set_SceneRealHit(PM->Get_SelectPlayer(), CCreture::TYPE_HIT, 20.f, 8.f, PM->Get_PlayerPos(), vDir, 100.f);
		CRM->Get_Cam()->Shake(0.25f, 0.3f, false, 0.1f);
		GI->PlaySoundW(L"song042_ojama_rakka_dai.wav", SD_NPC, 1.f);
		PM->Player01HurtVoice();
	}
	else
	{
		if (0.1f > fRatio)
		{
			// 퍼펙트 히트! 데미지 x2
			// CRM->Get_Cam()->Fov(0.05f, 0.02f, 0.f);
			CRM->Get_Cam()->Shake(0.4f, 0.3f, false, 0.1f);
			TM->Deal_CumaHp(250.f);
			GI->PlaySoundW(L"perfect.wav", SD_NPC, 1.f);
		}
		else // 일반 데미지
		{
			// fRatio값 곱해서 적용
			
			// TM->Get_MiniGame_Block_Cuma()->Shake();
			CRM->Get_Cam()->Shake(0.3f, 0.1f, false, 0.1f);
			TM->Deal_CumaHp(120.f);
			GI->PlaySoundW(L"flag.wav", SD_NPC, 1.f);
		}
	}

	
}




void CBlock::Tick(_float fTimeDelta)
{
	Emp_Col();

	if (m_bStop)
		return;



	_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Desc.vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (m_Desc.fMaxDis < fDis)
	{
		m_bTurn = !m_bTurn;
		++m_iTurnCount;


		if (2 < m_iTurnCount)
		{
			// Player에게 중첩된 데미지를 입힌다.

			// 데미지
			_float3 vDir;  XMStoreFloat3(&vDir, -1.f * XMLoadFloat3((_float3*)&PM->Get_PlayerWorldMatrix().m[2]));
			vDir.y = 0.f;
			PM->Set_SceneRealHit(PM->Get_SelectPlayer(), CCreture::TYPE_HIT, 20.f, 8.f, PM->Get_PlayerPos(), vDir, 100.f);
			PM->Player01HurtVoice();
			// CRM->Get_Cam()->Shake(0.25f, 0.3f, false, 0.1f);
		}


	}

	if (m_Desc.bDir)
	{
		if(m_bTurn)
			m_pTransformCom->Go_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_Desc.fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_Desc.fSpeed, fTimeDelta);
	}
	else
	{
		if (m_bTurn)
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_Desc.fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, -1.f, 0.f), m_Desc.fSpeed, fTimeDelta);
	}
}

void CBlock::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Choose_Col(nullptr);

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	
	Render_Col(m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBlock::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_Desc.vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBuffer_Cube->Render();

	return S_OK;
}

HRESULT CBlock::Ready_Components()
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


	///* For.Com_Rigid */
	//CRigid::RIGIDEDESC RigidDesc;
	//RigidDesc.vStartPos = _float3(0.f, 5.f, 0.f);
	//RigidDesc.vPos = _float3(0.f, -.8f, 0.f);
	//RigidDesc.vExtense = _float3(1.5f, 1.5f, 1.5f);
	//RigidDesc.fWeight = 100.f;
	//RigidDesc.fStaticFriction = 1.f;
	//RigidDesc.fDynamicFriction = 1.f;
	//RigidDesc.fRestitution = 0.1f;
	//RigidDesc.fAngleDump = 1.f;

	//// Prototype_Component_Rigid_Sphere
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
	//	return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = m_Desc.vScale;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "OBB_Block");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CBlock * CBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlock*		pInstance = new CBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlock::Clone(void * pArg)
{
	CBlock*		pInstance = new CBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlock::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Cube);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRigidCom);
}





void CBlock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	//IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}