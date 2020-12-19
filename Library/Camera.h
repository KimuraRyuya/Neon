#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
enum class SceneShaderType
{
	VS,
	PS,
	CS
};

class Camera
{
public:
	enum CameraMode
	{
		GAME,
		EDIT
	};

private:
	// 初期化時の原点座標
	const DirectX::XMFLOAT3	originPos = DirectX::XMFLOAT3(0.0f, 110.0f, 0.0f);
	const DirectX::XMFLOAT3	originTarget = DirectX::XMFLOAT3(0.2f, 1.0f, 0.9f);
	const DirectX::XMFLOAT3	originUp = DirectX::XMFLOAT3(0.06f, 0.99f, 0.01f);
	const DirectX::XMFLOAT3	originRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT3	standard_pos;
	DirectX::XMFLOAT3	pos;
	DirectX::XMFLOAT3	tmpPos;
	DirectX::XMFLOAT3	tmpTarget;
	DirectX::XMFLOAT3	target;
	DirectX::XMFLOAT3	up;
	DirectX::XMFLOAT3	right;

	DirectX::XMFLOAT4 lightDirection;
	float elapsedTimeMag;
	DirectX::XMMATRIX	projection;
	DirectX::XMMATRIX	view;
	DirectX::XMMATRIX view_projection;

	DirectX::XMFLOAT2 oldCursor;
	DirectX::XMFLOAT2 newCursor;

	float rotateX;
	float rotateY;

	float distance;

	// View行列作成
	void CreateView();

	float hei;

	// 画面shake
	float shakeValue;
	bool isShake;
	float shakeTime;
	float shakeTotalTime;
	CameraMode cameraMode;
public:


	void Init();
	void Update();
	void UpdateModeGame();
	void UpdateEdit();

	// カメラモード変更 (GAME or EDIT)
	void SetCameraMode(CameraMode mode) { cameraMode = mode; }

public:
	void SetRotateX(float x) { rotateX = x; }
	void SetRotateY(float y) { rotateY = y; }
	void SetTarget(DirectX::XMFLOAT3 _p) { target = _p; }
	void SetTempPos(DirectX::XMFLOAT3 p) { tmpPos = p; }
	void SetTempTarget(DirectX::XMFLOAT3 p) { tmpTarget = p; }
	void	SetPos(DirectX::XMFLOAT3 p) { pos = p; }
	void SetElapsedTimeMag(float mag) { elapsedTimeMag = mag; }

	DirectX::XMFLOAT3 GetTempPos() { return tmpPos; }
	DirectX::XMFLOAT3 GetTempTarget() { return tmpTarget; }
	DirectX::XMMATRIX	SetOrthographicMatrix(float w, float h, float znear, float zfar);
	DirectX::XMMATRIX	SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar);
	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetViewProjectionMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix() { return	projection;}
	DirectX::XMFLOAT3	GetPos() { return pos; }
	DirectX::XMFLOAT3	GetTarget() { return target; }
	DirectX::XMFLOAT4	GetLightDirection() { return lightDirection; }
	float GetElapsedTimeMag() { return elapsedTimeMag; }
	
	// SceneConstants用のマップ関数
	void MapConstantBuffer();
	// SceneConstants用のシェーダーバインド関数
	void BindConstantBuffer(SceneShaderType type);
	Microsoft::WRL::ComPtr<ID3D11Buffer> cb_scene;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cb_time;

	// カメラシェイクOn
	void OnCameraShake(float t, float v);
	// カメラシェイクOff
	void OffCameraShake();
	/// <summary>
	/// カメラを指定の位置の動かせる
	/// </summary>
	/// <param name="oriPos"> 移動前の座標 </param>
	/// <param name="afterPos"> 移動後の座標 </param>
	/// <param name="oriTarget"> 移動前の注視点 </param>
	/// <param name="afterTarget"> 移動後の注視点 </param>
	/// <param name="t"> 経過時間 </param>
	/// <param name="isRotate"> 回転するかどうか </param>
	void CameraMove(DirectX::XMFLOAT3 oriPos, DirectX::XMFLOAT3 afterPos, DirectX::XMFLOAT3 oriTarget, DirectX::XMFLOAT3 afterTarget, float t, bool isRotate = true);

	static Camera& GetInstance()
	{
		static Camera instance;
		return instance;
	}

private:
	struct scene_constants
	{
		DirectX::XMFLOAT4X4 rowView;
		DirectX::XMFLOAT4X4 rowProjection;
		DirectX::XMFLOAT4X4 rowViewProjection;

		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 viewProjection;

		DirectX::XMFLOAT4 lightDirection;

		DirectX::XMFLOAT4	cameraPos;
		DirectX::XMFLOAT4	cameraTarget;
		DirectX::XMFLOAT4	cameraUp;
		DirectX::XMFLOAT4	cameraRight;
		DirectX::XMFLOAT4 elapsedTime;
	};

	// 定数バッファ
	void create_constnts_buffer();
	void CameraShake();

};
