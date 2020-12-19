#include "Sound.h"

void Sound::Load()
{
	titleBgm = std::make_unique<CXAudio2>(L"Data/Sound/BGM/titleBGM.wav", CXAudio2::Type::loop);
	titleBgmVolume = 0.0f;
	gameBgm = std::make_unique<CXAudio2>(L"Data/Sound/BGM/gameBGM.wav", CXAudio2::Type::loop);
	gameBgmVolume = 0.0f;

	closeDissolve = std::make_unique<CXAudio2>(L"Data/Sound/SE/closeDissolve.wav", CXAudio2::Type::normal);
	openDissolve = std::make_unique<CXAudio2>(L"Data/Sound/SE/openDissolve.wav", CXAudio2::Type::normal);
	playerSpawn = std::make_unique<CXAudio2>(L"Data/Sound/SE/playerSpawn.wav", CXAudio2::Type::normal);
	menuSelect = std::make_unique<CXAudio2>(L"Data/Sound/SE/select.wav", CXAudio2::Type::normal);
	decision = std::make_unique<CXAudio2>(L"Data/Sound/SE/decision.wav", CXAudio2::Type::normal);
	enemyDestory = std::make_unique<CXAudio2>(L"Data/Sound/SE/enemyDestory.wav", CXAudio2::Type::normal);
	enemyAllDestory = std::make_unique<CXAudio2>(L"Data/Sound/SE/enemyAllDown.wav", CXAudio2::Type::normal);
	floorFill = std::make_unique<CXAudio2>(L"Data/Sound/SE/floorFill.wav", CXAudio2::Type::normal);
	playerShot = std::make_unique<CXAudio2>(L"Data/Sound/SE/playershot.wav", CXAudio2::Type::loop);
	buffPlayerShot = std::make_unique<CXAudio2>(L"Data/Sound/SE/buffPlayerShot.wav", CXAudio2::Type::loop);
}

void Sound::Update()
{
	titleBgm->Update();
	gameBgm->Update();

	closeDissolve->Update();
	openDissolve->Update();
	playerSpawn->Update();
	menuSelect->Update();
	decision->Update();
	enemyDestory->Update();
	enemyAllDestory->Update();
	floorFill->Update();
	playerShot->Update();
	buffPlayerShot->Update();
}

void Sound::StopSe()
{
	closeDissolve->StopWAV();
	openDissolve->StopWAV();
	playerSpawn->StopWAV();
	menuSelect->StopWAV();
	decision->StopWAV();
	enemyDestory->StopWAV();
	enemyAllDestory->StopWAV();
	floorFill->StopWAV();
	playerShot->StopWAV();
	buffPlayerShot->StopWAV();

}

void Sound::Release()
{
	titleBgm->Release();
	gameBgm->Release();

	closeDissolve->Release();
	openDissolve->Release();
	playerSpawn->Release();
	enemyAllDestory->Release();
	menuSelect->Release();
	decision->Release();
	floorFill->Release();
	enemyDestory->Release();
	playerShot->Release();
	buffPlayerShot->Release();
}

