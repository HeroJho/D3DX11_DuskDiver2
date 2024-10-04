#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CDialogTalk final : public CUI
{
private:
	CDialogTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogTalk(const CDialogTalk& rhs);
	virtual ~CDialogTalk() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	const _bool Get_TalkOnOff();  //리턴값이 true이면 대화중
	const _uint& Get_TalkIndex() { return m_iTalkIndex; }
	void Set_TalkIndex(_uint Index);
	void Set_OffTalk() { m_bRender = false; }

private:
	HRESULT Ready_Components();
	_bool Check_TalkChange(); // 대화 내용이 바뀌었는지 판단
	void Dialog_Move(_float fTimeDelta); // UI움직임
	void Update_Alpha(_float fTimeDelta);

	void Check_RenderTime(_float fTimeDelta); // 렌더 신호가 들어올때 시간을 재는 함수, 목표시간에 도달하면 m_bRender를 true로
	_bool Check_EventTalk(); // 특정인덱스의 토크는 끝냈을때 이벤트발생(대화 종료, 레벨 전환 등등)

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bDebug = false;
	_bool					m_bRender = false; // 토크 UI가 뜨냐?
	_bool					m_bStartRender = false; // 대화를 시작할것이냐? true일때 시간을 재고 그 시간을 채우면 m_bRender를 true로 하여 움직임이 있게
	_float					m_fRenderAlpha = 0.f;

	_uint					m_iTalkIndex = 100; //대화내용
	_uint					m_iLastTalkIndex = 100; // 지난 대화내용

	_float					m_fRenderTime = 0.f;
	_float					m_fMaxRenderTime = 3.f;

	_bool					m_bSound = false; //사운드용

public:
	static CDialogTalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END