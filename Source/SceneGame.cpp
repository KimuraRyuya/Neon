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

	// �����L���O�̏������炤
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
	
	// �J�[�\���̕\��/��\��
	EnableCursor(false);
	
	sceneState = SceneState::TITLE;
}


void SceneGame::Update(float elapsedTime)
{
	GetInputXbox();
	GetInputMouse();


	// ���݂̃X�e�[�g�ŏ�����ω�
	switch (sceneState)
	{
	case SceneState::TITLE:
		sceneState = title->Update(elapsedTime);

		// �߂�l��menu�Ȃ�dissolve�ŕ\��������
		if (sceneState == SceneState::MENU)
		{
			menu->SetOpenDissolveSound(false);
			cameraMoveTime = 0.0f;
		}
		break;
	case SceneState::MENU:
		sceneState = menu->Update(elapsedTime);

		// �ǂ̃��[�h�Ńv���C���邩�ۑ�
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
		
		// �`���[�g���A����͕K���m�[�}�����[�h�Ɉڍs
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

			// �|�[�Y��ʂɈڍs
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
		// ���j���[�ɑJ��
		else if (pause->GetMenuTransition())
		{
			// �J�����𓮂����Ă���
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̃t�F�[�h
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::MENU;
				// �Q�[���̏���������
				SomeReset();
			}
		}
		// �^�C�g���Ɉڍs
		else if (pause->GetTitleTiransition())
		{
			// �J�����𓮂����Ă���
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̃t�F�[�h
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::TITLE;
				// ���ׂď�����
				Reset();
			}
		}
		//�Q�[�������X�^�[�g
		else if (pause->GetRestartTiransition())
		{
			// �J�����̈ړ�
			Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, cameraMoveTime, false);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̃t�F�[�h
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				sceneState = SceneState::COUNT;
				// �Q�[���̏���������
				SomeReset();
			}
		}

		pause->Update(elapsedTime);

		break;
	case SceneState::ENDLESS:

		if (!pause->GetNowPaused() && !pause->GetMenuTransition() && !pause->GetTitleTiransition() && !pause->GetRestartTiransition())
		{
			sceneState = game->EndlessUpdate(elapsedTime);
			
			// �|�[�Y��ʂɈڍs
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
		// ���j���[�ɑJ��
		else if (pause->GetMenuTransition())
		{
			// �J�����𓮂����Ă���
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̃t�F�[�h
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::MENU;
				// �Q�[���̏���������
				SomeReset();
			}
		}
		// �^�C�g���Ɉڍs
		else if (pause->GetTitleTiransition())
		{
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				// �J�����𓮂����Ă���
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				// �T�E���h�̃t�F�[�h
				Sound::GetInstance()->gameBgm->StopWAV();
				sceneState = SceneState::TITLE;
				// ���ׂď�����
				Reset();
			}
		}
		//�Q�[�������X�^�[�g
		else if (pause->GetRestartTiransition())
		{
			// �J�����̈ړ�
			Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, cameraMoveTime, false);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̃t�F�[�h
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(Camera::GetInstance().GetTempPos(), gameCameraPos, Camera::GetInstance().GetTempTarget(), gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				sceneState = SceneState::COUNT;
				// �Q�[���̏���������
				SomeReset();
			}
		}

		pause->Update(elapsedTime);

		break;
	case SceneState::TIMEOVER:	case SceneState::DEAD:
		sceneState = SceneState::RESULT;
		// se�����ׂĎ~�߂�
		Sound::GetInstance()->StopSe();
		break;
	case SceneState::RESULT:
		sceneState = game->ResultUpdate(elapsedTime);
		
		// ���̃V�[���Ɉڍs
		if (game->getNextSceneFlg())
		{
			// �J�����̈ړ�
			Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			// �T�E���h�̑J��
			Sound::GetInstance()->gameBgmVolume -= 0.01 * elapsedTime;
			if (Sound::GetInstance()->gameBgmVolume < 0.0f) Sound::GetInstance()->gameBgmVolume = 0.0f;
			Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
			
			if (cameraMoveTime > 2.0f)
			{
				// ������
				Camera::GetInstance().CameraMove(gameCameraPos, titleCameraPos, gameCameraTarget, titleCameraTarget, 2.0f);
				// �����L���O�̃\�[�g
				ranking->Sort(game->getScoreAddress()->GetNowScore(), gameMode);
				 // �����L���O�̏���n��
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
	// �J�[�\���̕\��/��\��
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
	// �J�[�\���̕\��/��\��
	EnableCursor(false);
}