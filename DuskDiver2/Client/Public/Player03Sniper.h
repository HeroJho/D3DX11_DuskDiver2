#pragma once

#include "Client_Defines.h"
#include "Parts.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer03Sniper : public CParts
{
private:
	CPlayer03Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03Sniper(const CPlayer03Sniper& rhs);
	virtual ~CPlayer03Sniper() = default;


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


	/*_float PosX = 0.f;
	_float PosY = 0.f;
	_float PosZ = 0.f;
	_float AX = 0.f;
	_float AY = 0.f;
	_float AZ = 0.f;*/
public:
	static CPlayer03Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END