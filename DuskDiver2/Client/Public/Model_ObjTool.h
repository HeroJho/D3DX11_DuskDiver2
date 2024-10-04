#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CModel_ObjTool : public CGameObject
{
private:
	CModel_ObjTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_ObjTool(const CModel_ObjTool& rhs);
	virtual ~CModel_ObjTool() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

protected:
	HRESULT Ready_Components();

private:
	CTransform*			m_pTransformCom = nullptr;

public:
	static CModel_ObjTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END