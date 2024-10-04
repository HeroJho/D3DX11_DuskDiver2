#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEquipmentStatus final : public CUI
{
private:
	CEquipmentStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquipmentStatus(const CEquipmentStatus& rhs);
	virtual ~CEquipmentStatus() = default;

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

	class CEquipmentStr* m_pUINum = nullptr;
	class CEquipmentStamina* m_pStaminaNum = nullptr;
	class CEquipmentHp* m_pHpNum = nullptr;
	class CEquipmentMental* m_pMentalNum = nullptr;
	class CEquipmentMeta* m_pMetaNum = nullptr;
	class CEquipmentExplosive* m_pExplosiveNum = nullptr;
	class CEquipmentLuck* m_pLuckNum = nullptr;
	class CEquipmentLv* m_pLvNum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

public:
	static CEquipmentStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END