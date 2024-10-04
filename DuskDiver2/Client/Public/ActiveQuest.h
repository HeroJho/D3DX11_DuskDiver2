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
	_bool Get_RenderState() { return m_bRender; } //미니맵에 퀘스트 위치를 띄울지 상태를 얻어옴

private:
	HRESULT Ready_Components();
	void InitDist(); // 자식을 생성하여 담는다

	void Check_PlayerPos();
	void Setting_Distance(_float x, _float y, _float fTimeDelta);

	
private:
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;

	vector<class CQuestDist*>	m_QuestDistsVec; // 퀘스트의 위치를 알려줄 객체 3개 십의 자리수, 일의 자리수, m 

	_bool						m_bDebug = false;
	_bool						m_bRender = false;
	_bool						m_bBehind = false;
	_bool						m_bHideDistance = false;// 거리표시를 숨기는가
	_float4x4					m_QuestPoint;
	_float3						m_Pos;

	_uint						m_iIconIndex = 0;
	_float						m_fDistance = 0.f; // 플레이어위치에서 퀘스트위치까지의 거리
public:
	static CActiveQuest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END