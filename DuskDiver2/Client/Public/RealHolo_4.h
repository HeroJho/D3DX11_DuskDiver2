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

class CRealHolo_4 final : public CGameObject
{
private:
	CRealHolo_4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRealHolo_4(const CRealHolo_4& rhs);
	virtual ~CRealHolo_4() = default;


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
	class CHolo_3* m_pHolo_3_1 = nullptr; // Holo_1
	class CHolo_3* m_pHolo_3_2 = nullptr; // Holo_1
	class CHolo_5* m_pHolo_5 = nullptr;   // Holo_3
	class CHolo_6* m_pHolo_6 = nullptr;   // Holo_4
	class CHolo_7* m_pHolo_7 = nullptr;   // Holo_5
	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;
	_bool m_bSoundOn = 0.f;
	_float m_fSoundTime = 0.f;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

protected:
	HRESULT		Ready_Components();

public:
	static CRealHolo_4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END