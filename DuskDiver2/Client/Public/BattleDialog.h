#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBattleDialog final : public CUI
{
private:
	CBattleDialog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattleDialog(const CBattleDialog& rhs);
	virtual ~CBattleDialog() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	const _bool Get_BattleTalkOnOff();  //���ϰ��� true�̸� ��ȭ��
	const _uint& Get_BattleTalkIndex() { return m_iTalkIndex; }
	void Set_BattleTalkIndex(_uint Index);
	void Set_BattleOffTalk() { m_bRender = false; }

private:
	HRESULT Ready_Components();

	_bool Check_RenderTime(_float fTimeDelta); // ���������� �����ð��� ���� �̺�Ʈ üũ
	_bool Check_EventTalk(); // Ư���ε����� ��ũ�� �������� �̺�Ʈ�߻�(��ȭ ����, ���� ��ȯ ���)
	void Counting_DelayTime(_float fTimeDelta); // �ٷ� ���� �����ϴ� ��ȭ�� �ƴ϶�� delay�ð��� ���
	_bool ReturnZero(_float fTimeDelta); // ������ �ְ� alpha�������� ������ 0.f���� ���ư����� �Ǵ�

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bDebug = false;
	_bool					m_bRender = false; // ��ũ UI�� �߳�?
	_bool					m_bDelayOver = false;
	_float					m_fRenderAlpha = 0.f; // �ְ� 0.8f 
	_bool					m_bAlphaReverse = false;

	_uint					m_iTalkIndex = 100; //��ȭ����

	_float					m_fRenderTime = 0.f;
	_float					m_fMaxRenderTime = 2.1f;

	_float					m_fDelayTime = 0.f;
	_float					m_fMaxDelay = 0.f;

public:
	static CBattleDialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END