#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CDialogProfile final : public CUI
{
private:
	enum TYPE_PROFILE		{ TYPE_HERO1, TYPE_HERO1_MK1, TYPE_HERO1_MK2, TYPE_HERO3, TYPE_HERO4, TYPE_HERO4_MK1, TYPE_NPCTUTORIAL, TYPE_NPCPARTTIME, TYPE_NPCZENGBO, TYPE_END }; //컴포넌트의 갯수
	enum TYPE_HERO1			{ HERO1_NORMAL, HERO1_HAPPY, HERO1_BOTHER, HERO1_SAD, HERO1_SHOCK, HERO1_UNHAPPY, HERO1_COMA, HERO1_DOUBT, HERO1_HELPLESS, HERO1_SCARED, HERO1_SHY, HERO1_END };
	enum TYPE_HERO1_MK1		{ HERO1_MK1_NORMAL, HERO1_MK1_HAPPY, HERO1_MK1_BOTHER, HERO1_MK1_SAD, HERO1_MK1_SHOCK, HERO1_MK1_UNHAPPY, HERO1_MK1_COMA, HERO1_MK1_DOUBT, HERO1_MK1_HELPLESS, HERO1_MK1_SCARED, HERO1_MK1_SHY, HERO1_MK1_END };
	enum TYPE_HERO1_MK2		{ HERO1_MK2_NORMAL, HERO1_MK2_HAPPY, HERO1_MK2_BOTHER, HERO1_MK2_SAD, HERO1_MK2_SHOCK, HERO1_MK2_UNHAPPY, HERO1_MK2_COMA, HERO1_MK2_DOUBT, HERO1_MK2_HELPLESS, HERO1_MK2_SCARED, HERO1_MK2_SHY, HERO1_MK2_END };
	enum TYPE_HERO3			{ HERO3_NORMAL, HERO3_HAPPY, HERO3_BOTHER, HERO3_SAD, HERO3_SHOCK, HERO3_UNHAPPY, HERO3_END };
	enum TYPE_HERO4			{ HERO4_NORMAL, HERO4_HAPPY, HERO4_SHY, HERO4_HURT, HERO4_END };
	enum TYPE_HERO4_MK1		{ HERO4_MK1_NORMAL, HERO4_MK1_HAPPY, HERO4_MK1_SHY, HERO4_MK1_HURT, HERO4_MK1_END };
	enum TYPE_NPC_TUTORIAL  { NPC_TUTORIAL_NORMAL, NPC_TUTORIAL_HAPPY, NPC_TUTORIAL_BOTHER, NPC_TUTORIAL_SAD, NPC_TUTORIAL_SHOCK, NPC_TUTORIAL_UNHAPPY, NPC_TUTORIAL_LINE, NPC_TUTORIAL_END };
	enum TYPE_NPC_PARTTIME  { NPC_PARTTIME_NORMAL, NPC_PARTTIME_HAPPY, NPC_PARTTIME_BOTHER, NPC_PARTTIME_SAD, NPC_PARTTIME_SHOCK, NPC_PARTTIME_UNHAPPY, NPC_PARTTIME_END };
	enum TYPE_NPC_ZENGBO	{ NPC_ZENGBO_NORMAL, NPC_ZENGBO_END };

private:
	CDialogProfile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogProfile(const CDialogProfile& rhs);
	virtual ~CDialogProfile() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	HRESULT Ready_Components();
	_bool Check_ProfileChange();
	void Dialog_Move(_float fTimeDelta);
	void Update_Alpha(_float fTimeDelta);

	void Update_Dialog();// 누구의 대화인지 업데이트 하는 함수
	void Update_Emotion();// 표정변화를 업데이트

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr };

	_bool					m_bDebug = false;
	_bool					m_bRender = false; // 새로운 렌더 상태
	_float					m_fRenderAlpha = 0.f;

	_uint					m_iTalkIndex = 0; // 현재 대화내용
	_uint					m_iLastTalkIndex = 0; //이전 대화내용

	_uint					m_iProfileType = 0; //누구인가
	_uint					m_iEmotion = 0; // 어떤 표정인가


public:
	static CDialogProfile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END