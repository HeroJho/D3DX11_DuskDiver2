#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CShader_Tool : public CGameObject
{
private:
	CShader_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader_Tool(const CShader_Tool& rhs);
	virtual ~CShader_Tool() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

protected:
	HRESULT Ready_Components();

private:
	CTransform*			m_pTransformCom = nullptr;

private:
	_float4 m_vViewPos = {0.f, 5.f , 0.f , 1.f};
	_float4 m_vViewLookPos = { 0.f , 0.f , 0.f , 1.f };

public:
	static CShader_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END




// 디퍼드 설명
// - 뎁스 z값에 따라서 외각선 여부
// - 노말 w값에 따라서 툰 그림자 여부