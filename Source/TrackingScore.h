#pragma once
#include "../Library/Billboard.h"
#include "Constant.h"

class TrackingScore
{
private:
	// スコアボールのパラメーター
	struct TrackingInf
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 velocity;
		float angle;
		bool isExist;
		void Clear()
		{
			pos = { 0.0f, 0.0f, 0.0f };
			velocity = { 0.0f, 0.0f, 0.0f };
			angle = 0.0f;
			isExist = false;
		}
	};
	std::unique_ptr<Billboard> trackingTex;
	TrackingInf trackingInf[Constant::MAX_TRACKINGSCORE];
	DirectX::XMFLOAT3 trackingPos;

public:
	TrackingScore() {}
	~TrackingScore() {}

	void Reset();

	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();

	TrackingInf GetTrakingScoreInf(int i) { return trackingInf[i]; }
	void SetTrackingPos(DirectX::XMFLOAT3 p) { trackingPos = p; }
	/// <summary>
	/// スコアボールの生成
	/// </summary>
	/// <param name="emitP"> 生成する座標 </param>
	void EmitTrackingScore(DirectX::XMFLOAT3 emitP);
	void Destory(int i);
};