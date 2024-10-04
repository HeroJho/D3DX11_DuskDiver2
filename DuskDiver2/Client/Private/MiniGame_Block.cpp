#include "stdafx.h"
#include "..\Public\MiniGame_Block.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "Block.h"
#include "Layer.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "BlockCuma.h"

CMiniGame_Block::CMiniGame_Block(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMiniGame_Block::CMiniGame_Block(const CMiniGame_Block & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniGame_Block::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Block::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	Make_ImGui();
	m_sTag = "Tag_BlockMgr";
	m_fY = 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-138.7f, 31.276f, 96.696f, 1.f));
	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(3.f, 0.f, 0.f, 1.f));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fY * m_iBlockCount);
	XMStoreFloat3(&m_vInitPos, vPos);
	m_vScale = _float3(8.f, m_fY, 8.f);
	
	//_float3(0.f, 0.25f * m_iBlockCount, 0.f);



	CBlock::BLOCKDESC Desc;
	Desc.vPos = m_vInitPos;
	Desc.vScale = m_vScale;
	Desc.fMaxDis = 0.f;
	Desc.fSpeed = m_fSpeed;
	Desc.bDir = m_bBlockDir;
	Desc.vColor = _float4(m_fR, m_fG, m_fB, 1.f);

	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("Block"), TM->Get_CurLevel(), TEXT("Mini_Block"), &Desc, &pObj);
	m_pPreBlock = m_pCurBlock;
	m_pCurBlock = (CBlock*)pObj;
	++m_iBlockCount;
	m_pCurBlock->Stop();


	m_fR += 0.05f;
	if (1.f < m_fR)
		m_fR = 1.f;
	m_fG = 1.f - m_fR;

	m_bBlockDir = !m_bBlockDir;

	m_vInitPos.y += m_fY;
	m_fBlockHight += m_fY;



	m_fSpeed = 3.f;
	m_fMaxDis = 8.f;

	MakeBlock(m_fSpeed, m_fMaxDis);

	GI->PlaySoundW(L"minigame_common_start.wav", SD_MAP, 1.f);



	return S_OK;
}


void CMiniGame_Block::Tick(_float fTimeDelta)
{

	if (CK_DOWN(DIK_INSERT))
	{
		m_pCurBlock->Stop();
		if (m_pPreBlock)
		{
			CTransform* pCurTran = (CTransform*)m_pCurBlock->Get_ComponentPtr(TEXT("Com_Transform"));
			if (!m_pPreBlock->Compute_Slies(m_pCurBlock, m_bBome))
			{
				// 呕遏
				Set_Fail();
				// Set_Reset();
				return;
			}

			if (m_bBome)
			{
				m_vInitPos = m_vPreInitPos;
				m_vInitPos.y = m_vPreInitPos.y + m_fY;
			}
			else
			{
				m_vPreInitPos = m_vInitPos;
				XMStoreFloat3(&m_vInitPos, pCurTran->Get_State(CTransform::STATE_POSITION));
			}

			m_vScale = pCurTran->Get_Scale();

		}

		m_bPreBome = m_bBome;




		// 货肺款 广
		if (1 == GI->Get_Random(1, 5))
			m_bBome = true;
		else
			m_bBome = false;

		if (1 == GI->Get_Random(1, 6) && 4.f < m_fBlockHight)
			TM->Get_MiniGame_Block_Cuma()->Set_State(CBlockCuma::STATE_HIDE);


		if(!m_bPreBome)
			m_bBlockDir = !m_bBlockDir;


		if (m_bBome)
		{
			if (!m_bPreBome)
			{
				m_vInitPos.y += m_fY;
				m_fBlockHight += m_fY;
			}

			MakeBlock(m_fSpeed, m_fMaxDis, m_bPreBome);
		}
		else
		{
			if (1.f <= m_fR)
			{
				m_fR = 1.f;
				m_fB -= 0.1f;
				if (0.f >= m_fB)
				{
					m_fB = 0.f;
					m_fG += 0.1f;
					if (1.f <= m_fG)
					{
						m_fR = 0.f;
						m_fG = 0.f;
						m_fB = 1.f;
					}
				}
			}
			else
			{
				m_fR += 0.1f;
				m_fG = 1.f - m_fR;
			}



			Check_Level();

			if (!m_bPreBome)
			{
				m_vInitPos.y += m_fY;
				m_fBlockHight += m_fY;
			}

			if (1.f < m_fUpSpeed)
			{
				MakeBlock(m_fUpSpeed, m_fMaxDis, m_bPreBome);
				m_fUpSpeed = 0.f;
			}
			else
			{
				m_fSpeed += 0.3f;
				MakeBlock(m_fSpeed, m_fMaxDis, m_bPreBome);
			}


			// 场
			if (33 <= m_iBlockCount)
			{
				if (m_pCurBlock)
				{
					m_pCurBlock->Set_Dead();
					m_pCurBlock = nullptr;
				}
				Set_Dead();
				TM->Get_MiniGame_Block_Cuma()->Set_End();
				//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.4f);
				//GI->StopSound(SD_BGM);
				//GI->PlayBGM(L"Arcade.wav", 0.2f);
				GI->PlaySoundW(L"END1.wav", SD_MAP, 1.f);
				return;
			}

		}
	

		CRM->Get_Cam()->Hold_Pos(_float3(-128.4f, 47.375f + m_fBlockHight, 80.1f), PM->Get_Transform(), nullptr, m_vInitPos, 0.7f);

	}
	UM->Set_BlockNum(m_iBlockCount);
	UM->Set_BlockRGB(m_fR, m_fG, m_fB);
	UM->Set_BlockPos(34.375f + m_fBlockHight * 0.7f);
}					 

void CMiniGame_Block::LateTick(_float fTimeDelta)
{

}

HRESULT CMiniGame_Block::Render()
{

	return S_OK;
}

HRESULT CMiniGame_Block::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;


	return S_OK;
}

CMiniGame_Block * CMiniGame_Block::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniGame_Block*		pInstance = new CMiniGame_Block(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniGame_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniGame_Block::Clone(void * pArg)
{
	CMiniGame_Block*		pInstance = new CMiniGame_Block(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniGame_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Block::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}





void CMiniGame_Block::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::Checkbox("IsBome", &m_bBome);


	ImGui::End();
}

void CMiniGame_Block::MakeBlock(_float fSpeed, _float fMaxDis, _bool bBome)
{
	CBlock::BLOCKDESC Desc;
	Desc.vPos = m_vInitPos;
	Desc.vScale = m_vScale;
	Desc.fMaxDis = fMaxDis;
	Desc.fSpeed = fSpeed;
	Desc.bDir = m_bBlockDir;
	Desc.vColor = _float4(m_fR, m_fG, m_fB, 1.f);

	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("Block"), TM->Get_CurLevel(), TEXT("Mini_Block"), &Desc, &pObj);

	if (bBome)
	{
		m_pCurBlock->Set_Dead();
		m_pCurBlock = (CBlock*)pObj;
	}
	else
	{
		m_pPreBlock = m_pCurBlock;
		m_pCurBlock = (CBlock*)pObj;
		++m_iBlockCount;

	}
	
}

void CMiniGame_Block::Set_Fail()
{
	// 单固瘤
	_float3 vDir;  XMStoreFloat3(&vDir, -1.f * XMLoadFloat3((_float3*)&PM->Get_PlayerWorldMatrix().m[2]));
	PM->Set_SceneRealHit(PM->Get_SelectPlayer(), CCreture::TYPE_SPIN, 20.f, 8.f, PM->Get_PlayerPos(), vDir, 100.f);


	if (m_pCurBlock)
		m_pCurBlock->Set_Dead();


	CRM->Get_Cam()->End_Target();
	TM->Get_MiniGame_Block_Cuma()->Set_Puse();


	Set_Dead();
	//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.4f);
	//GI->StopSound(SD_BGM);
	//GI->PlayBGM(L"Arcade.wav", 0.2f);
}

void CMiniGame_Block::Set_Reset()
{
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Block")))->second;
	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();
	pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Block_Cuma")))->second;
	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();

	CRM->Get_Cam()->End_Target();

	Set_Dead();
	//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.3f, true, 0.4f);
	//GI->StopSound(SD_BGM);
	//GI->PlayBGM(L"Arcade.wav", 0.2f);
}

void CMiniGame_Block::Check_Level()
{
	// 0.5f
	if (3.f <= m_fBlockHight && 3.1f >m_fBlockHight)
	{
		m_fUpSpeed = m_fSpeed * 2.f;
	}

}


