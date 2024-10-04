#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CClue1 final : public CUI
{
private:
	CClue1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClue1(const CClue1& rhs);
	virtual ~CClue1() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Set_Clue1(_bool b) { m_bRender = b; }

private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_float m_fTime = 0.3f;
	_float m_fDeadTime = 0.f;

	_bool m_bReverse = false; // alpha값변경을 반전 시킬것인가
public:
	static CClue1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END