#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBeastPillar : public CCreture
{
private:
	CBeastPillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastPillar(const CBeastPillar& rhs);
	virtual ~CBeastPillar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual void Render_Debug() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT Ready_Components();
	void	Change_ColRange(_float3 vSize, _float3 vCenter);

private:
	CCreture::CREATEINFO m_CretureInfo;
	
	_float			m_fBlurPower = 0.f;
	_float			m_fDeadAcc = 0.f;
	_bool			m_bPlayerHit = true; //플레이어 한번만 충돌

	//스케일 조정
	_float			m_vScale = 1.f;
	_float			m_vColScale = 0.f;
	
public:
	static CBeastPillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END