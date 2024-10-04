#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "NPC_T.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
END


BEGIN(Client)

class CAnimMap_Tool : public CGameObject
{
private:
	CAnimMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimMap_Tool(const CAnimMap_Tool& rhs);
	virtual ~CAnimMap_Tool() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;


public:
	void Save_Data(_int iLevelIndex);
	void Load_Data(_int iLevelIndex);
	void Load_ByInstancing(_int iLevelIndex);

	void Delete_Obj(string sImGuiID);

	_int Get_SaveID() { return m_iSaveID; };

private:
	void Create_PxObj();
	void Create_PxObj_Load(CNPC_T::NPCIDLEDESC PxDesc);



private:
	HRESULT Ready_Components();

private:
	list<class CNPC_T*> m_pNPC_T;
	char				m_sSelectedModel[MAX_PATH];
	_float				m_fSpeed = 5.f;
	_uint				m_iLevelIndex = 0;

	_int				m_iSaveID = 0;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CAnimMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END