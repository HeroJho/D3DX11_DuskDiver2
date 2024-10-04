#pragma once

#include "Client_Defines.h"
#include "Parts.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer03RGun : public CParts
{
private:
	CPlayer03RGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03RGun(const CPlayer03RGun& rhs);
	virtual ~CPlayer03RGun() = default;


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

	//_float PosX = 1.f;
	//_float PosY = 1.f;
	//_float PosZ = 0.f;
	//_float AX = 0.f;
	//_float AY = 0.f;
	//_float AZ = 0.f;

protected:
	HRESULT Ready_Components();

public:
	static CPlayer03RGun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END