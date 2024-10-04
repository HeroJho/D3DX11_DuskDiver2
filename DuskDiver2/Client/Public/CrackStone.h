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

class CCrackStone : public CGameObject
{
public:
	typedef struct tagCrackDesc
	{
		_float3 vPos;
		_uint iIndex;
		_float fRange;
	}CRACKDESC;


private:
	CCrackStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrackStone(const CCrackStone& rhs);
	virtual ~CCrackStone() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	//_bool					m_bLastState = false; // 이전의 렌더링 상태를 보관
	_bool					m_bRenderState = false; // 새로운 렌더링 상태를 얻어오는 용도
	_float					m_fAlpha = 0.f;

	CRACKDESC				m_Desc;

private:
	HRESULT Ready_Components();
	void Update_Turn(_float fTimeDelta);
	_bool Update_RenderState();

public:
	static CCrackStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END