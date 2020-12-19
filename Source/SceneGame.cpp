#include "../Library/FrameWork.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "CollisionJudge.h"
#include "ParticleSystem.h"
#include "Sound.h"
#include "../Library/Camera.h"
#include "EditParameter.h"

void SceneGame::Initialize()
{

	editParameter = std::make_shared<Edit::EditPrameter>();
	editParameter->Init();


	title = std::make_unique<Title>();
	title->Initialize();

	game = std::make_unique<Game>();
	game->SetEditParameter(editParameter);
	game->Initialize();

	menu = std::make_unique<Menu>();
	menu->Initialize();

	tutorial = std::make_unique<Tutorial>();
	tutorial->SetEditParameter(editParameter);
	tutorial->Initialize();

	ranking = std::make_unique<Ranking>();
	ranking->Initialize();

	pause = std::make_unique<Pause>();
	pause->Initialize();

	// ランキングの情報をもらう
	menu->ReceiveScoreValue(ranking->bestScore, ranking->bestEndlessScore);

	ParticleSystem::GetInstance()->Initialize();

	Sound::GetInstance()->Load();

	isFinish = false;
	canNextSceneState = false;
	cameraMoveTime = 0.0f;

	Camera::GetInstance().SetTarget(titleCameraTarget);
	Camera::GetInstance().SetPos(titleCameraPos);
	Camera::GetInstance().SetCameraMode(Camera::CameraMode::GAME);


	Sound::GetInstance()->titleBgm->PlayWAV();
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);
	
	// カーソルの表示/非表示
	EnableCursor(false);
	
	sceneState = SceneState::TITLE;
}


void SceneGame::Update(float elapsedTime)
{
	GetInputXbox();
	GetInputMouse();


	// 現在のステートで処理を変化
	switch (sceneState)
	{
	case SceneState::TITLE:
		sceneState = title->Update(elapsedTime);

		// 戻り値がmenuならdissolveで表示させる
		if (sceneState == SceneState::MENU)
		{
			menu->SetOpenDissolveSound(false);
			cameraMoveTime = 0.0f;
		}
		break;
	case SceneState::MENU:
		sceneState = menu->Update(elapsedTime);

		// どのモードでプレイするか保存
		if (menu->GetNowMode() == 1)
		{
			gameMode = SceneState::GAME;
		}
		else if (menu->GetNowMode() == 2)
		{
			gameMode = SceneState::ENDLESS;
		}
		break;
	case SceneState::TUTORIAL:
		sceneState = tutorial->Update(game->getBgAddress(), elapsedTime);
		
		// チュートリアル後は必ずノーマルモードに移行
		gameMode = SceneState::GAME;
		break;
	case SceneState::COUNT:

		sceneState = game->CountDownUpdate(gameMode, elapsedTime);
		Camera::GetInstance().SetTarget(gameCameraTarget);
		Camera::GetInstance().SetPos(gameCameraPos);

		break;
	case SceneState::GAME:

		if (!pause->GetNowPaused() && !pause->GetMenuTransition() && !pause->GetTitleTiransition() && !pause->GetRestartTiransition())
		{
			sceneState = game->Update(elapsedTime);

			// ポーズ画面に移行
			if (PAD->xb.MENU_BUTTON && !pause->GetPauseTrg())
			{
				pause->SetPauseTrg(true);
				pause->SetNowPaused(true);
			}
			else if (!PAD->xb.MENU_BUTTON && pause->GetPauseTrg())
			{
				pause->SetPauseTrg(false);
			}
		}
		// メニューに遷移
		else if (pause->GetMenuTransition())
		{
			// カメラを動かしている
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// サウンドのフェード
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::MENU;
				// ゲームの情報を初期化
				SomeReset();
			}
		}
		// タイトルに移行
		else if (pause->GetTitleTiransition())
		{
			// カメラを動かしている
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// サウンドのフェード
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::TITLE;
				// すべて初期化
				Reset();
			}
		}
		//ゲームをリスタート
		else if (pause->GetRestartTiransition())
		{
			// カメラの移動
			Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, cameraMoveTime, false);
			cameraMoveTime += elapsedTime;
			// サウンドのフェード
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				sceneState = SceneState::COUNT;
				// ゲームの情報を初期化
				SomeReset();
			}
		}

		pause->Update(elapsedTime);

		break;
	case SceneState::ENDLESS:

		if (!pause->GetNowPaused() && !pause->GetMenuTransition() && !pause->GetTitleTiransition() && !pause->GetRestartTiransition())
		{
			sceneState = game->EndlessUpdate(elapsedTime);
			
			// ポーズ画面に移行
			if (PAD->xb.MENU_BUTTON && !pause->GetPauseTrg())
			{
				Sound::GetInstance()->StopSe();
				pause->SetPauseTrg(true);
				pause->SetNowPaused(true);
			}
			else if (!PAD->xb.MENU_BUTTON && pause->GetPauseTrg())
			{
				pause->SetPauseTrg(false);
			}
		}
		// メニューに遷移
		else if (pause->GetMenuTransition())
		{
			// カメラを動かしている
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// サウンドのフェード
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::MENU;
				// ゲームの情報を初期化
				SomeReset();
			}
		}
		// タイトルに移行
		else if (pause->GetTitleTiransition())
		{
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				// カメラを動かしている
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				// サウンドのフェード
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::TITLE;
				// すべて初期化
				Reset();
			}
		}
		//ゲームをリスタート
		else if (pause->GetRestartTiransition())
		{
			// カメラの移動
			Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, cameraMoveTime, false);
			cameraMoveTime += elapsedTime;
			// サウンドのフェード
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				sceneState = SceneState::COUNT;
				// ゲームの情報を初期化
				SomeReset();
			}
		}

		pause->Update(elapsedTime);

		break;
	case SceneState::TIMEOVER:	case SceneState::DEAD:
		sceneState = SceneState::RESULT;
		// seをすべて止める
		Sound::GetInstance()->StopSe();
		break;
	case SceneState::RESULT:
		sceneState = game->ResultUpdate(elapsedTime);
		
		// 次のシーンに移行
		if (game->getNextSceneFlg())
		{
			// カメラの移動
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// サウンドの遷移
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			
			if (cameraMoveTime > 2.0f)
			{
				// 微調整
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				// ランキングのソート
				ranking->Sort(game->getScoreAddress()->GetNowScore(), gameMode);
				 // ランキングの情報を渡す
				menu->ReceiveScoreValue(ranking->bestScore, ranking->bestEndlessScore);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::TITLE;
				Reset();
			}
		}
		break;
	}
	ParticleSystem::GetInstance()->Update(elapsedTime);

	ImGui::Begin("EXE");
	if (ImGui::Button("reload"))
	{
		editParameter->Update();
	}

	if (ImGui::Button("Edit"))
	{
		wchar_t buf[256];
		GetCurrentDirectory(256, buf);
		SetCurrentDirectory(L"EXE");
		ShellExecute(NULL, _T("open"), _T("Tool.exe"), NULL, NULL, SW_SHOWNORMAL);
		SetCurrentDirectory(buf);
	}
	ImGui::End();

}

void SceneGame::Render()
{
	switch (sceneState)
	{
	case SceneState::TITLE:
		pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
		title->Render(game->getBgAddress());
		pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
		break;
	case SceneState::MENU:
		pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
		menu->Render(game->getBgAddress());
		pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
		break;
	case SceneState::TUTORIAL:
		tutorial->Render(game->getBgAddress());
		break;
	case SceneState::COUNT:
	case SceneState::GAME:
	case SceneState::ENDLESS:
	case SceneState::TIMEOVER:
	case SceneState::DEAD:
		if(gameMode == SceneState::GAME) game->Render();
		else if (gameMode == SceneState::ENDLESS) game->EndlessRender();
		break;
	case SceneState::RESULT:
		pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
		game->ResultRender();
		pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
		break;
	default:
		break;
	}
	pause->Render();
}

void SceneGame::Finalize()
{
	PlayerShotSystem::GetInstance()->Finalize();
	ParticleSystem::GetInstance()->Finalize();
	title->Finalize();
	menu->Finalize();
	tutorial->Finalize();
	pause->Finalize();
	game->Finalize();
}

void SceneGame::Reset()
{
	Camera::GetInstance().SetElapsedTimeMag(0);
	PlayerShotSystem::GetInstance()->Reset();
	game->Reset();
	title->Reset();
	menu->Reset();
	tutorial->Reset();
	pause->Reset();
	isFinish = false;
	canNextSceneState = false;
	cameraMoveTime = 0.0f;

	Camera::GetInstance().SetTarget(titleCameraTarget);
	Camera::GetInstance().SetPos(titleCameraPos);
	sceneState = SceneState::TITLE;

	Sound::GetInstance()->gameBgm->StopWAV();
	Sound::GetInstance()->titleBgm->PlayWAV();
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);
	// カーソルの表示/非表示
	EnableCursor(false);
}

void SceneGame::SomeReset()
{
	Camera::GetInstance().SetElapsedTimeMag(0);
	PlayerShotSystem::GetInstance()->Reset();
	game->Reset();
	title->Reset();
	menu->Reset();
	pause->Reset();
	tutorial->Reset();
	isFinish = false;
	canNextSceneState = false;
	cameraMoveTime = 0.0f;

	Camera::GetInstance().SetTarget(titleCameraTarget);
	Camera::GetInstance().SetPos(titleCameraPos);

	Sound::GetInstance()->gameBgm->StopWAV();
	Sound::GetInstance()->titleBgm->PlayWAV();
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);
	// カーソルの表示/非表示
	EnableCursor(false);
}