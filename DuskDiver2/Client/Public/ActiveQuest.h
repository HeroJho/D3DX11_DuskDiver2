#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CActiveQuest final : public CUI
{
private:
	CActiveQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActiveQuest(const CActiveQuest& rhs);
	virtual ~CActiveQuest() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Set_QuestPoint(_float3 vPos, _uint iQuestIconIndex);
	_float3 Get_QuestPoint() { return m_Pos; };
	void Set_QuestPointOff() { m_bRender = false; }
	_bool Get_RenderState() { return m_bRender; } //�̴ϸʿ� ����Ʈ ��ġ�� ����� ���¸� ����

private:
	HRESULT Ready_Components();
	void InitDist(); // �ڽ��� �����Ͽ� ��´�

	void Check_PlayerPos();
	void Setting_Distance(_float x, _float y, _float fTimeDelta);

	
private:
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;

	vector<class CQuestDist*>	m_QuestDistsVec; // ����Ʈ�� ��ġ�� �˷��� ��ü 3�� ���� �ڸ���, ���� �ڸ���, m 

	_bool						m_bDebug = false;
	_bool						m_bRender = false;
	_bool						m_bBehind = false;
	_bool						m_bHideDistance = false;// �Ÿ�ǥ�ø� ����°�
	_float4x4					m_QuestPoint;
	_float3						m_Pos;

	_uint						m_iIconIndex = 0;
	_float						m_fDistance = 0.f; // �÷��̾���ġ���� ����Ʈ��ġ������ �Ÿ�
public:
	static CActiveQuest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END