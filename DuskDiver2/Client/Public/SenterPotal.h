#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)


class CSenterPotal final : public CCreture
{
private:
	CSenterPotal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSenterPotal(const CSenterPotal& rhs);
	virtual ~CSenterPotal() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;





private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();



private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	static CSenterPotal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END