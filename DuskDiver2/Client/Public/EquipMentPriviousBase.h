#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEquipMentPriviousBase final : public CUI
{
	// 중앙을 기준으로 현재 방향을 나타냄
	enum Direction
	{
		DIR_RIGHT,DIR_LEFT,DIR_END
	};
private:
	CEquipMentPriviousBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquipMentPriviousBase(const CEquipMentPriviousBase& rhs);
	virtual ~CEquipMentPriviousBase() = default;

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

	_bool m_bSelect = false;
	_bool m_bRender = false;

	_uint m_iSelectNum = 1;

	_bool m_bLevelUp = false;
	Direction m_eDir = DIR_END;
public:
	static CEquipMentPriviousBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END