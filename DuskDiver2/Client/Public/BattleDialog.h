#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBattleDialog final : public CUI
{
private:
	CBattleDialog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattleDialog(const CBattleDialog& rhs);
	virtual ~CBattleDialog() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	const _bool Get_BattleTalkOnOff();  //리턴값이 true이면 대화중
	const _uint& Get_BattleTalkIndex() { return m_iTalkIndex; }
	void Set_BattleTalkIndex(_uint Index);
	void Set_BattleOffTalk() { m_bRender = false; }

private:
	HRESULT Ready_Components();

	_bool Check_RenderTime(_float fTimeDelta); // 렌더시작후 일정시간뒤 다음 이벤트 체크
	_bool Check_EventTalk(); // 특정인덱스의 토크는 끝냈을때 이벤트발생(대화 종료, 레벨 전환 등등)
	void Counting_DelayTime(_float fTimeDelta); // 바로 렌더 시작하는 대화가 아니라면 delay시간을 잰다
	_bool ReturnZero(_float fTimeDelta); // 렌더가 최고 alpha도달이후 꺼질때 0.f까지 돌아갔는지 판단

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bDebug = false;
	_bool					m_bRender = false; // 토크 UI가 뜨냐?
	_bool					m_bDelayOver = false;
	_float					m_fRenderAlpha = 0.f; // 최고 0.8f 
	_bool					m_bAlphaReverse = false;

	_uint					m_iTalkIndex = 100; //대화내용

	_float					m_fRenderTime = 0.f;
	_float					m_fMaxRenderTime = 2.1f;

	_float					m_fDelayTime = 0.f;
	_float					m_fMaxDelay = 0.f;

public:
	static CBattleDialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END