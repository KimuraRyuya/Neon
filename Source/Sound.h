#pragma once

#include "../Library/Audio.h"

class Sound
{
private:
public:
	std::unique_ptr<CXAudio2> titleBgm;
	float titleBgmVolume;
	std::unique_ptr<CXAudio2> gameBgm;
	float gameBgmVolume;
	std::unique_ptr<CXAudio2> closeDissolve;
	std::unique_ptr<CXAudio2> openDissolve;
	std::unique_ptr<CXAudio2> playerSpawn;
	std::unique_ptr<CXAudio2> menuSelect;
	std::unique_ptr<CXAudio2> decision;
	std::unique_ptr<CXAudio2> enemyDestory;
	std::unique_ptr<CXAudio2> enemyAllDestory;
	std::unique_ptr<CXAudio2> floorFill;
	std::unique_ptr<CXAudio2> playerShot;
	std::unique_ptr<CXAudio2> buffPlayerShot;

public:
	Sound() {}
	~Sound() {
		Release();
	}

	void Update();
	void Load();
	void Release();

	// ‚·‚×‚Ä‚Ìse‚ð’âŽ~
	void StopSe();

	static Sound* GetInstance()
	{
		static Sound instance;
		return &instance;
	}

};