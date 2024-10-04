#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CDialogTalk final : public CUI
{
private:
	CDialogTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogTalk(const CDialogTalk& rhs);
	virtual ~CDialogTalk() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	const _bool Get_TalkOnOff();  //���ϰ��� true�̸� ��ȭ��
	const _uint& Get_TalkIndex() { return m_iTalkIndex; }
	void Set_TalkIndex(_uint Index);
	void Set_OffTalk() { m_bRender = false; }

private:
	HRESULT Ready_Components();
	_bool Check_TalkChange(); // ��ȭ ������ �ٲ������ �Ǵ�
	void Dialog_Move(_float fTimeDelta); // UI������
	void Update_Alpha(_float fTimeDelta);

	void Check_RenderTime(_float fTimeDelta); // ���� ��ȣ�� ���ö� �ð��� ��� �Լ�, ��ǥ�ð��� �����ϸ� m_bRender�� true��
	_bool Check_EventTalk(); // Ư���ε����� ��ũ�� �������� �̺�Ʈ�߻�(��ȭ ����, ���� ��ȯ ���)

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bDebug = false;
	_bool					m_bRender = false; // ��ũ UI�� �߳�?
	_bool					m_bStartRender = false; // ��ȭ�� �����Ұ��̳�? true�϶� �ð��� ��� �� �ð��� ä��� m_bRender�� true�� �Ͽ� �������� �ְ�
	_float					m_fRenderAlpha = 0.f;

	_uint					m_iTalkIndex = 100; //��ȭ����
	_uint					m_iLastTalkIndex = 100; // ���� ��ȭ����

	_float					m_fRenderTime = 0.f;
	_float					m_fMaxRenderTime = 3.f;

	_bool					m_bSound = false; //�����

public:
	static CDialogTalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END