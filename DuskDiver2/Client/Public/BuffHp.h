#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBuffHp final : public CUI
{
private:
	CBuffHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuffHp(const CBuffHp& rhs);
	virtual ~CBuffHp() = default;

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

	class CBuffHpNum* m_pUINum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_float m_fTime = 0.f;

	_float m_fAlpha = 0.f;
	_bool m_bEat = false;
	_bool m_bEffect = false;

	_bool m_bStart = false;
public:
	static CBuffHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END