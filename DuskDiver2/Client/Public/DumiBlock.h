#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
class CRigid;
END

BEGIN(Client)

class CDumiBlock : public CGameObject
{
public:
	typedef struct tagDumiBlock
	{
		_float3 vPos;
		_float3 vScale;
		_float4 vColor;
	}DUMIBLOCK;

private:
	CDumiBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDumiBlock(const CDumiBlock& rhs);
	virtual ~CDumiBlock() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	DUMIBLOCK m_Desc;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*			m_pVIBuffer_Cube = nullptr;
	CRigid*					m_pRigidCom = nullptr;

private:
	HRESULT Ready_Components();


public:
	static CDumiBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END