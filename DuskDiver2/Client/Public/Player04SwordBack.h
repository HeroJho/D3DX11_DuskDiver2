#pragma once

#include "Client_Defines.h"
#include "Parts.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer04SwordBack : public CParts
{
private:
	CPlayer04SwordBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04SwordBack(const CPlayer04SwordBack& rhs);
	virtual ~CPlayer04SwordBack() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;


protected:
	HRESULT Ready_Components();

public:
	static CPlayer04SwordBack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END