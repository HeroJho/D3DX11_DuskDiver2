#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine	final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_UIPROJ, D3DTS_IDENTITY, D3DTS_END };
	enum DIRSTATE { DIR_RIGHT, DIR_UP, DIR_LOOK, DIR_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);

	_matrix Get_TransformMatrix(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
	}

	_matrix Get_TransformMatrixInverse(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrix[eTransformState]);
	}

	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eTransformState) const {
		return m_TransformMatrix[eTransformState];
	}
	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATE eTransformState) const {
		_float4x4		Transform;
		XMStoreFloat4x4(&Transform, XMMatrixTranspose(Get_TransformMatrix(eTransformState)));
		return Transform;
	}

	_float4x4 Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eTransformState) const {
		return m_TransformInverseMatrix[eTransformState];
	}

	_float4 Get_CamPosition() const {
		return m_vCamPosition;
	}

	_float3 Get_CamDir(DIRSTATE eState);

	void Set_InitProjTransform(_fmatrix mInitView) { XMStoreFloat4x4(&m_InitProjTransform, mInitView); };
	_matrix Get_InitProjTransform() { return XMLoadFloat4x4(&m_InitProjTransform); }

public:
	void Update();
	
private:
	_float4x4				m_TransformMatrix[D3DTS_END];
	_float4x4				m_TransformInverseMatrix[D3DTS_END];
	_float4					m_vCamPosition;
	_float4x4				m_InitProjTransform;

public:
	virtual void Free() override;
};

END