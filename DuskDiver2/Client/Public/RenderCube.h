#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_ColorCube;
END

BEGIN(Client)

class CRenderCube final : public CGameObject
{
public:
	typedef struct tagColorCubeDesc
	{
		_float4 vColor;
		_float3 vPos;
		_float3 vScale;
	}COLORCUBEDESC;

private:
	CRenderCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderCube(const CRenderCube& rhs);
	virtual ~CRenderCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(_float3 vPos);
	_float3 Get_Pos();

	void Set_LinkIndex(_int iIndex) { m_iLinkIndex = iIndex; }
	_int Get_LinkIndex() { return m_iLinkIndex; }

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_ColorCube*	m_pVIBufferCom = nullptr;

private:
	_float4			m_vRGB;
	_int m_iLinkIndex = 0;
	_float m_fPreCubeSize = 0.f;

private:
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static CRenderCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END