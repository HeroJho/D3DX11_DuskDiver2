#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CIllustLeviada final : public CUI
{
private:
	CIllustLeviada(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIllustLeviada(const CIllustLeviada& rhs);
	virtual ~CIllustLeviada() = default;

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


	class CIllustLeviadaHp* m_pUINum = nullptr;
	class CIllustLeviadaLv* m_pLvNum = nullptr;
	class CIllustLeviadaSp* m_pSpNum = nullptr;
	class CIllustLeviadaAmor* m_pAmorNum = nullptr;
	class CIllustLeviadaAttack* m_pAttackNum = nullptr;

	_bool m_bDebug = false;

	// 처음 날라가는 거리
	_float m_fSlide = 0.f;

public:
	static CIllustLeviada* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END