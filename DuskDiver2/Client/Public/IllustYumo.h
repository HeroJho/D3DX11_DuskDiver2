#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CIllustYumo final : public CUI
{
private:
	CIllustYumo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIllustYumo(const CIllustYumo& rhs);
	virtual ~CIllustYumo() = default;

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


	class CIllustYumoHp* m_pUINum = nullptr;
	class CIllustYumoSp* m_pSpNum = nullptr;
	class CIllustYumoLv* m_pLvNum = nullptr;
	class CIllustYumoAttack* m_pAttackNum = nullptr;
	class CIllustYumoAmor* m_pAmorNum = nullptr;



	_bool m_bDebug = false;

	// 처음 날라가는 거리
	_float m_fSlide = 0.f;

public:
	static CIllustYumo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END