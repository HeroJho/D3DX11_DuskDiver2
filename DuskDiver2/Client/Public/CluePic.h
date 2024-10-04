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

class CCluePic : public CGameObject
{
private:
	CCluePic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCluePic(const CCluePic& rhs);
	virtual ~CCluePic() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Clue_Found() { m_bFound = true; }

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	_bool					m_bFound = false;
	_bool					m_bRender = true;
	_bool					m_bShowUI = false; // ´Ü¼­ UIÄÆ½ÅÀ» ¶ç¿ì´Â°¡

	_bool			m_bFind = false;
	_float			m_fTimeAcc = 0.f;

private:
	HRESULT Ready_Components();
	void Clue_Move(_float fTimeDelta);

public:
	static CCluePic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END