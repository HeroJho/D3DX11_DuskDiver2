#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBattleEndBase final : public CUI
{
private:
	CBattleEndBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattleEndBase(const CBattleEndBase& rhs);
	virtual ~CBattleEndBase() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	void Check_Result();

private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	class CNumResultExp* m_pUINum = nullptr;
	class CNumResultCombo* m_pUICombo = nullptr;
	class CNumResultBonus* m_pUIBonus = nullptr;
	class CNumResult* m_pUIResult = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_float m_fEndTime = 0.f;
	_float m_fTime = 0.f;

	_float m_fRandom = 0.5f;
	_float m_fRandomTime = 0.f;

	_bool Test = false;

	_float m_fAlpha = 0.f;
	_float m_fDeadTime = 0.f;

	_uint m_iNum0, m_iNum1, m_iNum2, m_iNum3;

	_int m_iBonus;

public:
	static CBattleEndBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END