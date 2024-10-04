#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)


class CDGCuma final : public CCreture
{
private:
	CDGCuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDGCuma(const CDGCuma& rhs);
	virtual ~CDGCuma() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;





private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();




private:
	_float m_fTimeAcc = 0.f;

	_bool	m_bMoveBlind = false;

private:
	class CDGCuma_1* m_pCuma_1 = nullptr;
	class CDGCuma_2* m_pCuma_2 = nullptr;
	class CDGCuma_3* m_pCuma_3 = nullptr;



public:
	static CDGCuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END