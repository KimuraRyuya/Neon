#pragma once

#include "../Library/Billboard.h"
#include <memory>

class Ultimate
{
private:
	std::unique_ptr<Billboard> ultimateCountDownTex;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT2 scale;
	float alpha;

	float nowScale;
	float startScale;
	float ultimateScalingTime;

	bool onceOnUltimateReset;
	bool onceOffUltimateReset;

	bool onUltimateCountDown;
	bool onUltimate;
public:

	Ultimate() {}
	~Ultimate() { }
	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	void SetOnUltimateCountDouwn(bool f) { onUltimateCountDown = f; }
	void SetOnUltimate(bool f) { onUltimate = f; }
	bool GetOnUltimateCountDouwn() { return onUltimateCountDown; }
	bool GetOnUltimate() { return onUltimate; }

};