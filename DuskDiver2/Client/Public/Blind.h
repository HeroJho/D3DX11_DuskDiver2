#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBlind final : public CUI
{
private:
	CBlind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlind(const CBlind& rhs);
	virtual ~CBlind() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Move_Blind(_bool b); //������ �ϳ��� ���·� ����� true : ������ false : ������
	const _bool& Get_MoveOver() { return m_bMoveOver; } // �������� �����°�?


private:
	HRESULT Ready_Components();

	void Update_Blind();
	void Close_Blind();
	void Open_Blinde();

	_bool Check_HideTime(_float fTimeDelta); // �ð��� ��� ������ �� �ð��� �ƴ��� ����

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_bool m_bMoving = false; // �����ϼ� �ִ°�?
	_bool m_bClose = false;
	_bool m_bChanged = false; // ������ġ�� �Ű����°�
	_bool m_bMoveOver = false; // �ݱ��� �����°�?

	_float m_fHideTime = 0.f; // ������ ���ʵ� ���� ����

	_uint m_iTextureIndex = 0;

	_float m_fTime = 0.f;
public:
	static CBlind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END