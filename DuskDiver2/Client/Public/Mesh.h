#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CMesh final : public CGameObject
{
public:
	typedef struct tagMeshInfo
	{
		_tchar* sTag;
		_float3 fScale = { 1.f,1.f,1.f };
		_float4 fPos = { 0.f,0.f,0.f,1.f };
		_float3 fAngle = { 0.f,0.f,0.f };

	}MESHINFO;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	

public:
	void Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3);
	_float3 Get_Angles() { return m_fAngles; }
	_float3 Get_Pos();
	void Set_Pos(_float4 Pos);
	void Set_Scale(_float3 fScale) { m_pTransformCom->Set_Scale(XMLoadFloat3(&fScale)); }
	void Set_ScaleX(_float3 fScale) { m_pTransformCom->Set_ScaleX(XMLoadFloat3(&fScale)); }
	void Set_ScaleY(_float3 fScale) { m_pTransformCom->Set_ScaleY(XMLoadFloat3(&fScale)); }
	void Set_ScaleZ(_float3 fScale) { m_pTransformCom->Set_ScaleZ(XMLoadFloat3(&fScale)); }
	_float3 Get_Scale();
	_float4x4 Get_Matrix();
	_bool& Get_bTurn() { return m_bTurn; }
	_bool& Get_bTurnStop() { return m_bTurnStop; }
	_bool& Get_bTurnDir() { return m_bTurnDir; }
	_float& Get_TurnSpeed() { return m_fTurnSpeed; }
	_float& Get_fUVSpeed() { return m_fUVSpeed; }
	_float& Get_fShaderUVIndexX() { return m_fShaderUVIndexX; }
	_float& Get_fShaderUVIndexY() { return m_fShaderUVIndexY; }
	_float& Get_fMaxUVIndexX() { return m_fMaxUVIndexX; }
	_float& Get_fMaxUVIndexY() { return m_fMaxUVIndexY; }
	_float2& Get_fUVFixSpeed() { return m_vUVFixSpeed; }
	_bool& Get_UVFix() { return m_bUVFix; }
	void Reset_UVFixAcc() { m_vUVFixAcc = _float2{ 0.f,0.f }; }
	_bool& Get_FixPosScale() { return m_bFixPosScale; }
	_float3& Get_PosFixSpeed() { return m_vPosFixSpeed; }
	_float3& Get_ScaleFixSpeed() { return m_vScaleFixSpeed; }
	int& Get_iShaderPass() { return m_iShaderPass; }
	_float4& Get_Color() { return m_vColor; }
	_float4& Get_Color2() { return m_vColor2; }
	_float& Get_AlphaSpeed() { return m_fAlphaSpeed; }
	void Reset_Alpha() { m_fAlphaAcc = 0.f; }
	void Reset_Scale() { m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale)); }
	_float3& Get_OriginScale() { return m_vOriginScale; }
	_float4& Get_OriginPos() { return m_vOriginPos; }
	_float2& Get_OriginRatio() { return m_vOriginRatio; }
	void Set_OriginScale(_float3 vOriginScale) { m_vOriginScale = vOriginScale; }
	void Set_OriginRatio(_float2 vOriginRatio) { m_vOriginRatio = vOriginRatio; }
	_float2& Get_Ratio() { return m_vRatio; }
	_bool& Get_FixColor() { return m_bFixColor; }
	_float& Get_BlurPower() { return m_fBlurPower; }
	_float2& Get_RatioSpeed() { return m_vRatioSpeed; }
	void Reset_Ratio() { m_vRatio = m_vOriginRatio; }
	void Reset_Pos() { m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vOriginPos)); }
	const _tchar* Get_Tag() { return sTag.c_str(); }
	void PreViewPos();
	_float3 Get_Right();
	_float3 Get_Up();
	_float3 Get_Look();
	_float3 Get_Position();

private:
	_float3 m_fAngles;
	wstring sTag;
	_float m_fShaderUVAcc = 0.f;
	_float m_fUVSpeed = 0.1f;
	_float m_fShaderUVIndexX = 0;
	_float m_fShaderUVIndexY = 0;
	_float m_fMaxUVIndexX = 0;
	_float m_fMaxUVIndexY = 0;
	_float m_fTurnSpeed = 0.f;
	_bool m_bTurnStop = false;
	_bool m_bTurn = false;
	_bool m_bTurnDir = false;
	_bool m_bUVFix = false;
	_bool m_bFixPosScale = false;
	_bool m_bFixColor = false;
	_float m_fBlurPower = 0.f;
	int m_iShaderPass = 0;
	_float2 m_vUVFixSpeed = { 0.f,0.f };
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float3 m_vPosFixSpeed = { 0.f,0.f,0.f };
	_float3 m_vScaleFixSpeed = { 0.f,0.f,0.f };
	_float4 m_vColor = { 0.f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	_float m_fAlphaSpeed = 0.f;
	_float m_fAlphaAcc = 0.f;
	_float4 m_vOriginPos = { 0.f,0.f,0.f,1.f };
	_float3 m_vOriginScale = { 1.f,1.f,1.f };
	_float2 m_vOriginRatio = { 1.f,1.f };
	_float2 m_vRatio = { 1.f,1.f };
	_float2 m_vRatioSpeed = { 0.f,0.f };

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	MESHINFO*				m_MeshInfo;

private:
	HRESULT Ready_Components();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END