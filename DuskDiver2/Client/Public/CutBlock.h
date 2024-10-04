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

class CCutBlock : public CGameObject
{

private:
	CCutBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutBlock(const CCutBlock& rhs);
	virtual ~CCutBlock() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Start();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*			m_pVIBuffer_Cube = nullptr;
	CRigid*					m_pRigidCom = nullptr;

	_bool m_bStart = true;
	_float3 m_vDestPos;
	_float3 m_vGoLook;
	_float m_fTimeAcc = 0.f;
	_float m_fTimeDeadAcc = 0.f;
	_bool m_bCheckTime = false;

private:
	HRESULT Ready_Components();


public:
	static CCutBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END