#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBlind final : public CUI
{
private:
	CBlind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlind(const CBlind& rhs);
	virtual ~CBlind() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Move_Blind(_bool b); //강제로 하나의 상태로 만든다 true : 열리게 false : 닫히게
	const _bool& Get_MoveOver() { return m_bMoveOver; } // 움직임이 끝났는가?


private:
	HRESULT Ready_Components();

	void Update_Blind();
	void Close_Blind();
	void Open_Blinde();

	_bool Check_HideTime(_float fTimeDelta); // 시간을 재고 렌더를 끌 시간이 됐는지 리턴

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_bool m_bMoving = false; // 움직일수 있는가?
	_bool m_bClose = false;
	_bool m_bChanged = false; // 시작위치로 옮겨졌는가
	_bool m_bMoveOver = false; // 닫김이 끝났는가?

	_float m_fHideTime = 0.f; // 닫힌후 몇초뒤 렌더 지움

	_uint m_iTextureIndex = 0;

	_float m_fTime = 0.f;
public:
	static CBlind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END