#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CMetalTrashbinTop : public CGameObject
{
public:
	typedef struct tagMetalTrashbinTopDesc {
		_float4 vPosition;
	}METALTRASHBINTOPDESC;
private:
	CMetalTrashbinTop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMetalTrashbinTop(const CMetalTrashbinTop& rhs);
	virtual ~CMetalTrashbinTop() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Add_Force(_float3 vForce);


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRigid*					m_pRigidCom = nullptr;
	
	METALTRASHBINTOPDESC	m_MetalTrashbinTopDesc;


private:
	HRESULT Ready_Components();
	void InitState();

public:
	static CMetalTrashbinTop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END