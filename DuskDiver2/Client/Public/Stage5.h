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

class CStage5 : public CGameObject
{
private:
	CStage5(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage5(const CStage5& rhs);
	virtual ~CStage5() = default;

public:
	typedef struct tagInfo
	{
		_float4 vPos;
	}POS;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


private:
	HRESULT Ready_Components();
	
	_float m_fAlpa = 0.f;

public:
	static CStage5* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END