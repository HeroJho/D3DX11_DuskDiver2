#include "stdafx.h"
#include "..\Public\DialogProfile.h"
#include "GameInstance.h"
#include "DialogTalk.h"
#include "UI_Manager.h"
#include "Player_Manager.h"

CDialogProfile::CDialogProfile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CDialogProfile::CDialogProfile(const CDialogProfile & rhs)
	:CUI(rhs)
{
}

HRESULT CDialogProfile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDialogProfile::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CDialogProfile::Tick(_float fTimeDelta)
{
	m_bRender = UM->Get_TalkOnOff();
	
	// 위치가 640.f즉 중점보다 왼쪽이면 움직인다.
	Dialog_Move(fTimeDelta);
	if (true == m_bRender)
		Update_Alpha(fTimeDelta);

	__super::Tick(fTimeDelta);

	if (true == m_bRender)
	{
		m_iTalkIndex = UM->Get_TalkIndex();
		Update_Dialog();
		Update_Emotion();
	}
	// 렌더상태가 바꿔었거나 인덱스가 바뀌면 위치를 시작위치로 세팅해준다.
	if (true == Check_ProfileChange())
	{
		m_UiInfo.fX = 640.f - 20.f;
		m_iLastTalkIndex = m_iTalkIndex;
		m_fRenderAlpha = 0.f;
	}
	
	// 다음번 Render가 true일때 반짝임을 없애준다 미리 alpha를 0으로 해서
	if (false == m_bRender)
		m_fRenderAlpha = 0.f;
}

void CDialogProfile::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDialogProfile::Render()
{
	if (true == m_bRender)
	{
		if (nullptr == m_pVIBufferCom ||
			nullptr == m_pShaderCom)
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

		m_pShaderCom->Set_RawValue("g_DialogAlpha", &m_fRenderAlpha, sizeof(_float));


		if (FAILED(m_pTextureCom[m_iProfileType]->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iEmotion)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(24)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	}

	return S_OK;
}

void CDialogProfile::Render_Debug()
{
}

HRESULT CDialogProfile::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom[TYPE_HERO1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1_MK1"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom[TYPE_HERO1_MK1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1_MK2"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureCom[TYPE_HERO1_MK2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero3"), TEXT("Com_Texture3"), (CComponent**)&m_pTextureCom[TYPE_HERO3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero4"), TEXT("Com_Texture4"), (CComponent**)&m_pTextureCom[TYPE_HERO4])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero4_MK1"), TEXT("Com_Texture5"), (CComponent**)&m_pTextureCom[TYPE_HERO4_MK1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_Tutorial"), TEXT("Com_Texture6"), (CComponent**)&m_pTextureCom[TYPE_NPCTUTORIAL])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_PartTime"), TEXT("Com_Texture7"), (CComponent**)&m_pTextureCom[TYPE_NPCPARTTIME])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_Zengbo"), TEXT("Com_Texture8"), (CComponent**)&m_pTextureCom[TYPE_NPCZENGBO])))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

_bool CDialogProfile::Check_ProfileChange()
{
	if (m_iLastTalkIndex != m_iTalkIndex)
	{
		return true;
	}

	return false;
}

void CDialogProfile::Dialog_Move(_float fTimeDelta)
{
	if (640.f > m_UiInfo.fX)
	{
		m_UiInfo.fX += 100.f * fTimeDelta;
	}
	else
		m_UiInfo.fX = 640.f;
}

void CDialogProfile::Update_Alpha(_float fTimeDelta)
{
	if (1.f > m_fRenderAlpha)
		m_fRenderAlpha += 5.f * fTimeDelta;
	else
		m_fRenderAlpha = 1.f;
}

void CDialogProfile::Update_Dialog()
{
#pragma region 메그레즈
	// NPC_Tutorial
	if ((0 == m_iTalkIndex) || (2 == m_iTalkIndex) || (7 == m_iTalkIndex) || (9 == m_iTalkIndex) || (11 == m_iTalkIndex) || (25 == m_iTalkIndex)
		|| (26 == m_iTalkIndex) || (30 == m_iTalkIndex) || (31 == m_iTalkIndex) || (60 == m_iTalkIndex) || (61 == m_iTalkIndex) || (62 == m_iTalkIndex)
		|| (65 == m_iTalkIndex) || (66 == m_iTalkIndex) || (67 == m_iTalkIndex) || (72 == m_iTalkIndex) || (73 == m_iTalkIndex) || (74 == m_iTalkIndex)
		|| (75 == m_iTalkIndex) || (76 == m_iTalkIndex) || (77 == m_iTalkIndex) || (80 == m_iTalkIndex) || (83 == m_iTalkIndex) || (84 == m_iTalkIndex)
		|| (85 == m_iTalkIndex) || (86 == m_iTalkIndex) || (89 == m_iTalkIndex) || (90 == m_iTalkIndex) || (91 == m_iTalkIndex) || (100 == m_iTalkIndex)
		|| (101 == m_iTalkIndex))
	{
		m_iProfileType = TYPE_NPCTUTORIAL;
	}
#pragma endregion 메그레즈
	
#pragma region 유모
	// 유모 1단계(임시 1단계만)
	if ((1 == m_iTalkIndex) || (3 == m_iTalkIndex) || (8 == m_iTalkIndex) || (10 == m_iTalkIndex) || (13 == m_iTalkIndex) || (14 == m_iTalkIndex) 
		|| (17 == m_iTalkIndex) || (19 == m_iTalkIndex) || (21 == m_iTalkIndex) || (27 == m_iTalkIndex) || (28 == m_iTalkIndex) || (32 == m_iTalkIndex) 
		|| (35 == m_iTalkIndex) || (37 == m_iTalkIndex) || (42 == m_iTalkIndex) || (53 == m_iTalkIndex) || (56 == m_iTalkIndex))
	{
		m_iProfileType = TYPE_HERO1;
	}
	// 유모 전투
	else if ((5 == m_iTalkIndex)  || (50 == m_iTalkIndex) || (63 == m_iTalkIndex) || (70 == m_iTalkIndex) 
		|| (71 == m_iTalkIndex) || (78 == m_iTalkIndex) || (82 == m_iTalkIndex) || (87 == m_iTalkIndex) || (93 == m_iTalkIndex)
		|| (96 == m_iTalkIndex) || (102 == m_iTalkIndex))
	{
		if (false == PM->Get_BurstMode(CPlayer::PLAYER_01))
			m_iProfileType = TYPE_HERO1_MK1;
		else
			m_iProfileType = TYPE_HERO1_MK2;
	}
#pragma endregion 유모

#pragma region 리 베이다
	// 리 베이다
	if ((4 == m_iTalkIndex) || (6 == m_iTalkIndex) || (12 == m_iTalkIndex) || (15 == m_iTalkIndex) || (16 == m_iTalkIndex) || (18 == m_iTalkIndex)
		|| (23 == m_iTalkIndex) || (29 == m_iTalkIndex) || (36 == m_iTalkIndex) || (43 == m_iTalkIndex) || (49 == m_iTalkIndex) || (51 == m_iTalkIndex) 
		|| (54 == m_iTalkIndex) || (64 == m_iTalkIndex) || (68 == m_iTalkIndex) || (69 == m_iTalkIndex) || (81 == m_iTalkIndex) || (88 == m_iTalkIndex)
		|| (94 == m_iTalkIndex) || (97 == m_iTalkIndex) || (98 == m_iTalkIndex) || (99 == m_iTalkIndex))
	{
		m_iProfileType = TYPE_HERO3;
	}
#pragma endregion 리 베이다
	
#pragma region DD
	// D.D 1단계
	if ((20 == m_iTalkIndex) || (22 == m_iTalkIndex) || (24 == m_iTalkIndex) || (44 == m_iTalkIndex) || (48 == m_iTalkIndex) || (52 == m_iTalkIndex) 
		|| (79 == m_iTalkIndex) || (95 == m_iTalkIndex))
	{
		m_iProfileType = TYPE_HERO4;
	}
#pragma endregion DD
	
#pragma region 정보요원
	// 정보요원
	if ((33 == m_iTalkIndex) || (34 == m_iTalkIndex) || (38 == m_iTalkIndex) || (39 == m_iTalkIndex) || (40 == m_iTalkIndex) || (41 == m_iTalkIndex)
		|| (45 == m_iTalkIndex) || (46 == m_iTalkIndex) || (47 == m_iTalkIndex) || (55 == m_iTalkIndex) || (57 == m_iTalkIndex) || (58 == m_iTalkIndex))
	{
		m_iProfileType = TYPE_NPCZENGBO;
	}
#pragma endregion 정보요원
	
#pragma region 기타
	// 빈 말풍선
	if ((59 == m_iTalkIndex) || (92 == m_iTalkIndex))
	{
		//m_iProfileType = TYPE_END;
	}
#pragma endregion 기타
	
}

void CDialogProfile::Update_Emotion()
{
#pragma region 메그레즈
	// NPC_Tutorial
	if ((0 == m_iTalkIndex) || (2 == m_iTalkIndex))
	{
		m_iEmotion = NPC_TUTORIAL_NORMAL;
	}
	else if ((7 == m_iTalkIndex) || (9 == m_iTalkIndex))
	{
		m_iEmotion = NPC_TUTORIAL_SHOCK;
	}
	else if ((11 == m_iTalkIndex))
	{
		m_iEmotion = NPC_TUTORIAL_UNHAPPY;
	}
	else if ((25 == m_iTalkIndex) || (26 == m_iTalkIndex) || (30 == m_iTalkIndex) || (31 == m_iTalkIndex) || (60 == m_iTalkIndex) || (61 == m_iTalkIndex)
		|| (62 == m_iTalkIndex) || (65 == m_iTalkIndex) || (66 == m_iTalkIndex) || (67 == m_iTalkIndex) || (72 == m_iTalkIndex) || (73 == m_iTalkIndex)
		|| (74 == m_iTalkIndex) || (75 == m_iTalkIndex) || (76 == m_iTalkIndex) || (77 == m_iTalkIndex) || (80 == m_iTalkIndex) || (83 == m_iTalkIndex)
		|| (84 == m_iTalkIndex) || (85 == m_iTalkIndex) || (86 == m_iTalkIndex) || (89 == m_iTalkIndex) || (90 == m_iTalkIndex) || (91 == m_iTalkIndex)
		|| (100 == m_iTalkIndex) || (101 == m_iTalkIndex))
	{
		m_iEmotion = NPC_TUTORIAL_LINE;
	}
#pragma endregion 메그레즈
	
#pragma region 유모
	// 유모
	if ((1 == m_iTalkIndex) || (3 == m_iTalkIndex) || (32 == m_iTalkIndex) || (35 == m_iTalkIndex) || (56 == m_iTalkIndex) || (82 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_NORMAL;
	}
	else if ((5 == m_iTalkIndex) || (53 == m_iTalkIndex) || (63 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_MK1_HAPPY;
	}
	else if ((8 == m_iTalkIndex) || (17 == m_iTalkIndex) || (19 == m_iTalkIndex) || (21 == m_iTalkIndex) || (71 == m_iTalkIndex) || (93 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_SHOCK;
	}
	else if ((13 == m_iTalkIndex) || (27 == m_iTalkIndex) || (37 == m_iTalkIndex) || (96 == m_iTalkIndex) || (102 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_UNHAPPY;
	}
	else if ((14 == m_iTalkIndex) || (42 == m_iTalkIndex) || (87 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_COMA;
	}
	else if ((50 == m_iTalkIndex) || (78 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_MK1_DOUBT;
	}
	else if ((10 == m_iTalkIndex) || (28 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_MK1_SCARED;
	}
	else if ((70 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_MK1_SHY;
	}
#pragma endregion 유모
	
#pragma region 리 베이다
	// 리 베이다
	if ((4 == m_iTalkIndex) || (36 == m_iTalkIndex) || (54 == m_iTalkIndex) || (81 == m_iTalkIndex))
	{
		m_iEmotion = HERO3_NORMAL;
	}
	else if ((6 == m_iTalkIndex))
	{
		m_iEmotion = HERO3_HAPPY;
	}
	else if ((16 == m_iTalkIndex) || (44 == m_iTalkIndex))
	{
		m_iEmotion = HERO3_BOTHER;
	}
	else if ((12 == m_iTalkIndex) || (15 == m_iTalkIndex) || (18 == m_iTalkIndex) || (23 == m_iTalkIndex) || (29 == m_iTalkIndex) || (43 == m_iTalkIndex)
		|| (49 == m_iTalkIndex) || (51 == m_iTalkIndex) || (64 == m_iTalkIndex) || (68 == m_iTalkIndex) || (69 == m_iTalkIndex) || (88 == m_iTalkIndex)
		|| (94 == m_iTalkIndex) || (97 == m_iTalkIndex) || (98 == m_iTalkIndex) || (99 == m_iTalkIndex))
	{
		m_iEmotion = HERO3_UNHAPPY;
	}
#pragma endregion 리 베이다
	
#pragma region DD
	// D.D 1단계
	if ((24 == m_iTalkIndex) || (48 == m_iTalkIndex) || (52 == m_iTalkIndex) || (79 == m_iTalkIndex) || (95 == m_iTalkIndex))
	{
		m_iEmotion = HERO4_NORMAL;
	}
	else if (22 == m_iTalkIndex)
	{
		m_iEmotion = HERO4_SHY;
	}
	else if (20 == m_iTalkIndex)
	{
		m_iEmotion = HERO4_HURT;
	}
#pragma endregion DD
	
#pragma region 정보요원
	// NPC_Zengbo
	if ((33 == m_iTalkIndex) || (34 == m_iTalkIndex) || (38 == m_iTalkIndex) || (39 == m_iTalkIndex) || (40 == m_iTalkIndex) || (41 == m_iTalkIndex)
		|| (45 == m_iTalkIndex) || (46 == m_iTalkIndex) || (47 == m_iTalkIndex) || (55 == m_iTalkIndex) || (57 == m_iTalkIndex) || (58 == m_iTalkIndex))
	{
		m_iEmotion = NPC_ZENGBO_NORMAL;
	}
#pragma endregion 정보요원

#pragma region 말풍선
	if ((59 == m_iTalkIndex) || (92 == m_iTalkIndex))
	{
		m_iEmotion = HERO1_MK1_END; //아무 얼굴도 안뜸
	}
#pragma endregion 말풍선
	
}

CDialogProfile * CDialogProfile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDialogProfile*		pInstance = new CDialogProfile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDialogProfile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDialogProfile::Clone(void * pArg)
{
	CDialogProfile*		pInstance = new CDialogProfile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDialogProfile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialogProfile::Free()
{
	__super::Free();

	for (_uint i = 0; TYPE_END > i; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	Safe_Release(m_pShaderCom);
}
