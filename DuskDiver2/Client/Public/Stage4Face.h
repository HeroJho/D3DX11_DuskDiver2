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

class CStage4Face : public CGameObject
{
private:
	enum FaceType { FACE_HAPPY, FACE_ANGRY, FACE_CONFUSED, FACE_TEARFUL, FACE_FAINT, FACE_END };
private:
	CStage4Face(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage4Face(const CStage4Face& rhs);
	virtual ~CStage4Face() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	_float m_fBlurPower = 0.15f;

	_uint m_iCurFace = 0;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom[FACE_END] = { nullptr };
	CTexture*				m_pTextureCom = nullptr;

	_float					m_fChangeTime = 0.f;

private:
	HRESULT Ready_Components();
	void Change_RandomFace(_float fTimeDelta);

public:
	static CStage4Face* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END