#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PxObj.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CLight;
END


BEGIN(Client)

class CPointLight_Tool : public CGameObject
{
private:
	CPointLight_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointLight_Tool(const CPointLight_Tool& rhs);
	virtual ~CPointLight_Tool() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

public:
	void Create_Point();
	void Save_Data(_int iLevelIndex);
	void Load_Data(_int iLevelIndex);

	void SameLight();

private:
	HRESULT Ready_Components();


private:
	_int	m_iSelectedIndex = 0;
	CLight* m_pSelectedLight = nullptr;
	_float m_fSpeed = 5.f;
	_bool	m_bShow = false;

	_int	m_iLevelIndex = 0;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CPointLight_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END