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

class CTrash : public CGameObject
{
public:
	typedef struct tagTrashDesc {
		_float4 vPosition;
	}TRASHDESC;
private:
	CTrash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrash(const CTrash& rhs);
	virtual ~CTrash() = default;

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

	TRASHDESC				m_TrashDesc;

private:
	HRESULT Ready_Components();


public:
	static CTrash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END