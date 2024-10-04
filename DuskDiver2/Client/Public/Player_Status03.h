#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayer_Status03 final : public CUI
{
private:
	CPlayer_Status03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Status03(const CPlayer_Status03& rhs);
	virtual ~CPlayer_Status03() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;
	_bool m_bRemeber = false;
	_float m_fTimeAcc = 0.f;
	_float m_fShakeTime = 0.2f;
	_float m_fOriginfX = 0.f;
	_float m_fOriginfY = 0.f;
	class CPS03_Number* m_pUINum = nullptr;

public:
	static CPlayer_Status03* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END