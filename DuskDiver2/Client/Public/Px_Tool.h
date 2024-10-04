#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PxObj.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
END


BEGIN(Client)

class CPx_Tool : public CGameObject
{
private:
	CPx_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPx_Tool(const CPx_Tool& rhs);
	virtual ~CPx_Tool() = default;


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

	void Edit_SameName();
	void Delete_Obj(string sImGuiID);

private:
	void Create_PxObj();
	void Create_PxObj_Load(CPxObj::PXOBJDESC PxDesc);
	void Reset_PxObj();
	void OnPhys_PxObj();
	
	


private:
	HRESULT Ready_Components();

private:
	list<class CPxObj*> m_pPxObjs;
	char				m_sSelectedModel[MAX_PATH];
	_float				m_fSpeed = 5.f;
	_uint				m_iLevelIndex = 0;

	_int				m_iSaveID = 0;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CPx_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END