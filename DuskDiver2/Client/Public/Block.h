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

class CBlock : public CGameObject
{
public:
	typedef struct tagBlockDesc
	{
		_float3 vPos;
		_float3 vScale;
		_float4 vColor;
		_bool bDir;
		_float fSpeed;
		_float fMaxDis;
	}BLOCKDESC;

private:
	CBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlock(const CBlock& rhs);
	virtual ~CBlock() = default;

public:
	void Stop() { m_bStop = true; }
	BLOCKDESC Get_Desc() { return m_Desc; }

	_uint Get_TurnCount() { return m_iTurnCount; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


public:
	_bool Compute_Slies(CBlock* pCurBlock, _bool bBome =false);
	void Compute_Result(_uint iTurnCount, _float fRatio, _bool bBome);

private:
	BLOCKDESC m_Desc;
	_bool m_bTurn = false;
	_bool m_bStop = false;
	_uint m_iTurnCount = 0;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*			m_pVIBuffer_Cube = nullptr;
	CRigid*					m_pRigidCom = nullptr;

private:
	HRESULT Ready_Components();


public:
	static CBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END