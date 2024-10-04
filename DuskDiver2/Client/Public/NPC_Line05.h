#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCreture;

class CNPC_Line05 : public CGameObject
{
private:
	CNPC_Line05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Line05(const CNPC_Line05& rhs);
	virtual ~CNPC_Line05() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

protected:
	HRESULT Ready_Components();
	HRESULT Billboard();

private:
	_float4 m_vMonsterPos;
	_bool m_bRender = false;
	_bool m_bOn = false;

	_float m_fSizeX, m_fSizeY = 0.f;
	_float	m_fY, m_fZ = 0.f;
	_float m_fOnAcc = 0.f;
	_float4 m_fPlayerPos = { 0.f, 0.f, 0.f, 0.f };

public:
	static CNPC_Line05* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END