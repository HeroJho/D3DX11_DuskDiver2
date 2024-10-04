#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CRealHolo_3 final : public CGameObject
{
private:
	CRealHolo_3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRealHolo_3(const CRealHolo_3& rhs);
	virtual ~CRealHolo_3() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	_float m_fTimeDelta = 0.f;
	class CHolo_4* m_pHolo_4_1 = nullptr; // Holo_2
	class CHolo_4* m_pHolo_4_2 = nullptr; // Holo_2
	class CHolo_4* m_pHolo_4_3 = nullptr; // Holo_2
	class CHolo_6* m_pHolo_6 = nullptr; // Holo_4
	class CHolo_7* m_pHolo_7 = nullptr; // Holo_5
	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

protected:
	HRESULT		Ready_Components();

public:
	static CRealHolo_3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END