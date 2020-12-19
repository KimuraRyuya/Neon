#pragma once

#include "../Library/MyLib.h"
#include "../Library/Billboard.h"

class PlayerHpUi
{
private:
	std::unique_ptr<Billboard> playerCircleUi;
	std::unique_ptr<Billboard> playerCircleUiText;
	std::unique_ptr<Billboard> playerCircleUiFrame;
	DirectX::XMFLOAT3 playerCircleUiPos;

	std::unique_ptr<Billboard> playerHpUi;
	std::unique_ptr<Billboard> buttonUI;

	LoadInf aimTexture;

	float buttonScale;
	float buttonScaleTime;
	float buttonAnim;
	DirectX::XMFLOAT3 playerHpUiPos;

	bool isFloorUIDisply;
	int playerNowHp;

	float ultGauge;
	float ang;
public:
	PlayerHpUi() {}
	~PlayerHpUi() { Finalize(); }

	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();

	void Reset();

	void SetULTGauge(float v) { ultGauge = v; }
	void SetPlayerCircleUiPos(DirectX::XMFLOAT3 pos) { playerCircleUiPos = pos; playerCircleUiPos.z *= -1; }
	void SetPlayerHpUiPosPos(DirectX::XMFLOAT3 pos) { playerHpUiPos = pos; playerHpUiPos.z *= -1; }
	void SetPlayerNowHp(int hp) { playerNowHp = hp; }
	void SetPlayerShotGauge(float n) { ang = n; }
	void SetFloorUIDisply(bool f) { isFloorUIDisply = f; }
};