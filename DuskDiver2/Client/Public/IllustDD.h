#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CIllustDD final : public CUI
{
private:
	CIllustDD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIllustDD(const CIllustDD& rhs);
	virtual ~CIllustDD() = default;

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


	class CIllustDDHp* m_pUINum = nullptr;
	class CIllustDDLv* m_pLvNum = nullptr;
	class CIllustDDSp* m_pSpNum = nullptr;
	class CIllustDDAttack* m_pAttackNum = nullptr;
	class CIllustDDArmor* m_pArmorNum = nullptr;

	_bool m_bDebug = false;

	// 처음 날라가는 거리
	_float m_fSlide = 0.f;

public:
	static CIllustDD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END