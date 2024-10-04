#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
//class CModel;
END

BEGIN(Client)

class CCrystalBody : public CCreture
{
public:
	typedef struct tagCrystalBodyDesc {
		_float4 vPosition;
		_float	vRadianAngle;
		_bool	bClue; // �ܼ����ΰ�? 
	}BODYDESC;
private:
	CCrystalBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrystalBody(const CCrystalBody& rhs);
	virtual ~CCrystalBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	//CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	BODYDESC				m_BodyDesc;

	vector<class CCrystalPart*>		m_PartALayer; // A������ �����ϴ� ���̾�
	vector<class CCrystalPart*>		m_PartBLayer; // B������ �����ϴ� ���̾�

	_bool					m_bHited = false;
	_bool					m_bItemCreated = false;
	_float					m_fDissolveAcc = 0.f;

	//==============================================for Debug & IMGUI
	_bool					m_bRender = false;
	_bool					m_bSelectA = false;
	_bool					m_bSelectB = false;
	_bool					m_bEditA = false;
	_bool					m_bEditB = false;
	_int					m_iChooseIndex = 0; // �������� ����� ��ư ���������� ���õ� �ε����� �� �ε����� �Ѱ��ش�
	_int					m_iSelectIndex = 0; // ���� Ÿ���� ���° ���� edit�Ұ��ΰ�
	_float4					m_SelectedPosition = { 0.f,0.f,0.f,1.f };
	_float					m_SelectedAngle = 0.f;
	_float					m_SelectedSize = 1.f;


private:
	HRESULT Ready_Components();
	void Initialize_Parts(_float3 WorldPos, _float WorldRadianAngle);
	void Create_Item();
	void Update_Dissolve(_float fTimeDelta);
	void Update_RenderState();

public:
	static CCrystalBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END