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

class CHolo_2 final : public CGameObject
{
private:
	CHolo_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHolo_2(const CHolo_2& rhs);
	virtual ~CHolo_2() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void Set_StartTime(_float fStartTime) { m_fStartTime = fStartTime; }

private:
	_float m_fTimeDelta = 0.f;
	class CHolo_3* m_pHolo_3 = nullptr; // Holo_1
	class CHolo_4* m_pHolo_4 = nullptr; // Holo_2
	class CHolo_5* m_pHolo_5 = nullptr; // Holo_3
	class CHolo_5_Blue* m_pHolo_5_Blue = nullptr; // Holo_3
	class CHolo_6* m_pHolo_6 = nullptr; // Holo_4
	class CHolo_7* m_pHolo_7 = nullptr; // Holo_5

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;


protected:
	HRESULT		Ready_Components();

public:
	static CHolo_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END