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

class CBuildings : public CGameObject
{
public:
	typedef struct tagBDDesc {
		_uint	iIndex; // ���° ������ �ǹ����ΰ�
		_uint	iModelType; // LOD���� �����ΰ�
	}BDDESC;
private:
	enum MODELTYPE { MODEL_LOD0, MODEL_LOD1, MOEDL_END };
	enum BDAREA {AREA_IT, AREA_NC, AREA_AZIT, AREA_END }; // ��Ŭ������ ��������� �ǹ��̳�? �÷��̾��� ��ġ�� �Ǵ��ϱ� ���� ����
private:
	CBuildings(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuildings(const CBuildings& rhs);
	virtual ~CBuildings() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual HRESULT Render_ShadowDepth();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	//CModel*					m_pModelCom[MOEDL_END] = { nullptr };
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	BDDESC					m_BDDesc;
	_bool					m_bRender = false;

	_float					m_fBlurPow = 0.1f;

private:
	HRESULT Ready_Components();
	void Update_ModelType(); // �÷��̾��� ��ġ�� �޾ƿͼ� ��� ���������� �Ǵ��Ѵ�.


public:
	static CBuildings* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END