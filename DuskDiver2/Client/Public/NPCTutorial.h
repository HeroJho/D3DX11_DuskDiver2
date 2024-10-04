#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CNPCTutorial : public CCreture
{
private:
	enum TALK_STATE {TYPE_START, TYPE_EMERGENCY, TYPE_END};
private:
	CNPCTutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCTutorial(const CNPCTutorial& rhs);
	virtual ~CNPCTutorial() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	//void Set_NPCTalking(_bool b) { m_bTalking = b; }
	//const _bool& Get_NPCTalking() { return m_bTalking; }

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_uint					m_iCurState = 0; // ���� ��ȭ����(�� ���¿� ���� ���ɾ����� �ε��� ��ȭ ������ ���Ѵ�)
	_bool					m_bTalking = false; // ��ȭ���ΰ�? �ٽ� ��ȭ�� ���ϰ� ���� �뵵.
	_bool					m_bEmergency = false; // ��� ��ġ �ҽ��� ����°�?
private:
	HRESULT Ready_Components();
	void Check_TalkState(); // � ��ȭ�� �Ұ��ΰ� ���� �Ǵ�
	void Check_Talking(); // ��ȭ���ΰ� �Ǵ�

public:
	static CNPCTutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END