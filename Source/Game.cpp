#include "Game.h"

#include "../Library/Framework.h"
#include "../Library/Camera.h"
#include "SceneManager.h"
#include "CollisionJudge.h"
#include "ParticleSystem.h"
#include "Sound.h"
#include "EditParameter.h"

void Game::Initialize()
{
	characterSystem = std::make_unique<CharacterSystem>();
	characterSystem->SetEditParameter(editParameter);
	characterSystem->Initialize();

	bg = std::make_shared<BG>();
	bg->Initialize();

	combo = std::make_unique<Combo>();
	combo->Initialize();

	trackingScore = std::make_unique<TrackingScore>();
	trackingScore->Initialize();

	score = std::make_unique<Score>();
	score->Initialize();

	gameTimer = std::make_unique<GameTimer>();
	gameTimer->Initialize();

	countDown = std::make_unique<CountDown>();
	countDown->Initialize();

	playerHpUi = std::make_unique<PlayerHpUi>();
	playerHpUi->Initialize();

	metaAI = std::make_unique<MetaAI>();
	metaAI->Initialize(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), bg);

	PlayerShotSystem::GetInstance()->Initialize();

	bloomEffect = std::make_unique<Bloom>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight);
	framebuffers[0] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[1] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	tonemap_effect = std::make_unique<ToneMap>(pFrameWork.GetDevice());

	finishFlg = false;
	nextSceneStateFlg = false;


}

void Game::Reset()
{
	characterSystem->Reset();
	bg->Reset();
	PlayerShotSystem::GetInstance()->Reset();
	combo->Reset();
	score->Reset();
	gameTimer->Reset();
	trackingScore->Reset();
	playerHpUi->Reset();
	countDown->Reset();
	nextSceneStateFlg = false;
	finishFlg = false;
}

SceneState Game::Update(float elapsedTime)
{
	// �^�C�����O�ɂȂ�����
	if (!characterSystem->GetPlayerAddress()->GetNewSpawn() && gameTimer->GetGameTimer() < 0)
	{
		// �^�C���I�[�o�[�ŃQ�[���I��
		finishFlg = true;
		bg->stageLuminans = DirectX::XMFLOAT4(20.0f, 20.0f, 25.0f, 1.0f);
		Camera::GetInstance().OffCameraShake();
		return SceneState::TIMEOVER;
	}
	// �v���C���[HP��0�ɂȂ�����
	else if (!characterSystem->GetPlayerAddress()->GetNewSpawn() && characterSystem->GetPlayerAddress()->GetHp() <= 0)
	{
		// �v���C���[DEAD�ŃQ�[���I��
		finishFlg = true;
		bg->stageLuminans = DirectX::XMFLOAT4(20.0f, 20.0f, 25.0f, 1.0f);
		Camera::GetInstance().OffCameraShake();
		return SceneState::DEAD;
	}

	GetInputXbox();
	// BG�̓h�������̃X�R�A�����̂܂�ADDScore�ɓ����
	score->AddScore(bg->Update(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), elapsedTime));
	combo->Update(elapsedTime);
	// �v���C���[�̈ʒu���炻�����o�t�G���A���ǂ������f
	characterSystem->GetPlayerAddress()->SetReinforce(bg->CheckPlayerFillPos(characterSystem->GetPlayerAddress()->GetPos()));
	// EnemyManager��BG��Luminans��n��(�X�|�[���̎��Ɏg����������)
	characterSystem->GetEnemyManagerAddress()->SetBgLuminans(&(bg->luminans[0][0]));
	characterSystem->Update(combo->GetLevel(), elapsedTime);

	// �A���e�B���b�g���g������
	if (characterSystem->GetPlayerAddress()->GetEnemyAllKillFlg())
	{
		bg->UltimateEffectAndCalcFill(characterSystem->GetPlayerAddress());
		// �G�l�~�[��|���̂ƃX�R�A�𑝂₷
		characterSystem->GetEnemyManagerAddress()->AllDown();
		float mag =  bg->GetUltimateFillDestroy() / 400.0f;
		score->AddScore(static_cast<int>(1000000.0f * mag));
		characterSystem->GetPlayerAddress()->SetEnemyAllKillFlg(false);
	}

	// �X�R�A�{�[���̃^�[�Q�b�g���v���C���[�ɂ���
	trackingScore->SetTrackingPos(characterSystem->GetPlayerAddress()->GetPos());
	trackingScore->Update(elapsedTime);
	PlayerShotSystem::GetInstance()->Update(elapsedTime);
	score->Update(finishFlg, elapsedTime);

	playerHpUi->Update(elapsedTime);

	// player�t�߂�UI�̈ʒu��Q�[�W����n��
	playerHpUi->SetPlayerCircleUiPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerHpUiPosPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerNowHp(characterSystem->GetPlayerAddress()->GetHp());
	playerHpUi->SetPlayerShotGauge(abs(characterSystem->GetPlayerAddress()->GetDushGauge() - 360.0f));
	playerHpUi->SetFloorUIDisply(characterSystem->GetPlayerAddress()->GetFillChipDestory());
	playerHpUi->SetULTGauge(bg->GetNowFillNum());
	combo->SetComboTextPos(characterSystem->GetPlayerAddress()->GetPos());
	combo->SetComboNumTextPos(characterSystem->GetPlayerAddress()->GetPos());

	gameTimer->Update(elapsedTime);

	// �v���C���[��HP��1�܂��́A�c�莞�Ԃ�10�b�ȉ��̎��X�e�[�W��_�ł�����
	if (characterSystem->GetPlayerAddress()->GetHp() < 2 || gameTimer->GetGameTimer() < 10)
	{
		bg->Warning();
	}

	// �����蔻��
	CollisionJudge::PlayerVSStage(characterSystem->GetPlayerAddress(), bg);
	CollisionJudge::PlayerVSEnemy(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), combo.get());
	CollisionJudge::PlayerVSTrakingScore(characterSystem->GetPlayerAddress(), trackingScore.get());
	CollisionJudge::PlayerDestroyEffectVSEnemy(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress());
	CollisionJudge::PlayerShotVSStage(PlayerShotSystem::GetInstance(), bg, characterSystem.get());
	CollisionJudge::PlayerShotVSEnemy(PlayerShotSystem::GetInstance(), characterSystem.get(), combo.get(), score.get(), bg, trackingScore.get(),characterSystem->GetPlayerAddress()->GetReinforce());

	metaAI->Update(elapsedTime);

	return SceneState::GAME;

}

SceneState Game::EndlessUpdate(float elapsedTime)
{
	// �v���C���[HP��0�ɂȂ�����
	 if (!characterSystem->GetPlayerAddress()->GetNewSpawn() && characterSystem->GetPlayerAddress()->GetHp() <= 0)
	{
		 // �v���C���[DEAD�ŃQ�[���I��
		finishFlg = true;
		bg->stageLuminans = DirectX::XMFLOAT4(20.0f, 20.0f, 25.0f, 1.0f);
		Camera::GetInstance().OffCameraShake();
		return SceneState::DEAD;
	}

	GetInputXbox();
	// BG�̓h�������̃X�R�A�����̂܂�ADDScore�ɓ����
	score->AddScore(bg->Update(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), elapsedTime));
	combo->Update(elapsedTime);
	// �v���C���[�̈ʒu���炻�����o�t�G���A���ǂ������f
	characterSystem->GetPlayerAddress()->SetReinforce(bg->CheckPlayerFillPos(characterSystem->GetPlayerAddress()->GetPos()));
	// EnemyManager��BG��Luminans��n��(�X�|�[���̎��Ɏg����������)
	characterSystem->GetEnemyManagerAddress()->SetBgLuminans(&(bg->luminans[0][0]));
	characterSystem->Update(combo->GetLevel(), elapsedTime);
	
	// �A���e�B���b�g���g������
	if (characterSystem->GetPlayerAddress()->GetEnemyAllKillFlg())
	{
		bg->UltimateEffectAndCalcFill(characterSystem->GetPlayerAddress());
		// �G�l�~�[��|���̂ƃX�R�A�𑝂₷
		characterSystem->GetEnemyManagerAddress()->AllDown();
		float mag = bg->GetUltimateFillDestroy() / 400.0f;
		score->AddScore(static_cast<int>(1000000.0f * mag));
		characterSystem->GetPlayerAddress()->SetEnemyAllKillFlg(false);
	}

	// �X�R�A�{�[���̃^�[�Q�b�g���v���C���[�ɂ���
	trackingScore->SetTrackingPos(characterSystem->GetPlayerAddress()->GetPos());
	trackingScore->Update(elapsedTime);
	PlayerShotSystem::GetInstance()->Update(elapsedTime);
	score->Update(finishFlg, elapsedTime);

	// player�t�߂�UI�̈ʒu��Q�[�W����n��
	playerHpUi->Update(elapsedTime);
	playerHpUi->SetPlayerCircleUiPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerHpUiPosPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerNowHp(characterSystem->GetPlayerAddress()->GetHp());
	playerHpUi->SetPlayerShotGauge(abs(characterSystem->GetPlayerAddress()->GetDushGauge() - 360.0f));
	playerHpUi->SetFloorUIDisply(characterSystem->GetPlayerAddress()->GetFillChipDestory());
	playerHpUi->SetULTGauge(bg->GetNowFillNum());
	combo->SetComboTextPos(characterSystem->GetPlayerAddress()->GetPos());
	combo->SetComboNumTextPos(characterSystem->GetPlayerAddress()->GetPos());

	// �v���C���[��HP��1�܂��́A�c�莞�Ԃ�10�b�ȉ��̎��X�e�[�W��_�ł�����
	if (characterSystem->GetPlayerAddress()->GetHp() < 2 || gameTimer->GetGameTimer() < 10)
	{
		bg->Warning();
	}

	// �����蔻��
	CollisionJudge::PlayerVSStage(characterSystem->GetPlayerAddress(), bg);
	CollisionJudge::PlayerVSEnemy(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), combo.get());
	CollisionJudge::PlayerVSTrakingScore(characterSystem->GetPlayerAddress(), trackingScore.get());
	CollisionJudge::PlayerDestroyEffectVSEnemy(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress());
	CollisionJudge::PlayerShotVSStage(PlayerShotSystem::GetInstance(), bg, characterSystem.get());
	CollisionJudge::PlayerShotVSEnemy(PlayerShotSystem::GetInstance(), characterSystem.get(), combo.get(), score.get(), bg, trackingScore.get(), characterSystem->GetPlayerAddress()->GetReinforce());

	metaAI->Update(elapsedTime);

	return SceneState::ENDLESS;

}

SceneState Game::CountDownUpdate(SceneState nextScene, float elapsedTime)
{
	countDown->Update(characterSystem->GetPlayerAddress(), elapsedTime);

	// �J�E���g�_�E�����ɉ����t�F�[�h������
	Sound::GetInstance()->titleBgmVolume -= 0.1f * elapsedTime;
	if (Sound::GetInstance()->titleBgmVolume < 0.0f)
	{
		Sound::GetInstance()->titleBgmVolume = 0.0f;
		Sound::GetInstance()->titleBgm->StopWAV();
	}
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);

	Sound::GetInstance()->gameBgmVolume += 0.005 * elapsedTime;
	if (Sound::GetInstance()->gameBgmVolume > 1.0f) Sound::GetInstance()->gameBgmVolume = 1.0f;
	Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);

	if (countDown->GetNowGamePlay())
	{
		Sound::GetInstance()->titleBgm->StopWAV();
		characterSystem->GetPlayerAddress()->Reset();
		return nextScene;
	}
	return SceneState::COUNT;
}

SceneState Game::ResultUpdate(float elapsedTime)
{
	score->Update(finishFlg, elapsedTime);
	if (PAD->xb.A_BUTTON && score->GetDissolveScaler() <= 0.0f && !nextSceneStateFlg)
	{
		score->SetAddDissolveScaler(2.0f);
		nextSceneStateFlg = true;
	}

	if (nextSceneStateFlg && score->GetDissolveScaler() >= 2.0f)
	{
		return SceneState::RESULT;
	}
	return SceneState::RESULT;
}

void Game::Render()
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	characterSystem->GetPlayerAddress()->GetPlayerUltimate()->Render();
	bg->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->BloomRender();
	trackingScore->Render();
	countDown->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
	characterSystem->Render();
	PlayerShotSystem::GetInstance()->Render();
	framebuffers[0]->DeActivate(pFrameWork.GetContext());



	// generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[0]->renderTargetShaderResourceView.Get());
	// convolute bloom texture to scene framebuffer
	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 0.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());

	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[1]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);
	
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->NotBloomRender();
	if (countDown->GetNowGamePlay())playerHpUi->Render();
	if (countDown->GetNowGamePlay())combo->Render();
	gameTimer->Render();
	score->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

}

void Game::EndlessRender()
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	characterSystem->GetPlayerAddress()->GetPlayerUltimate()->Render();
	bg->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->BloomRender();
	trackingScore->Render();
	countDown->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
	characterSystem->Render();
	PlayerShotSystem::GetInstance()->Render();
	framebuffers[0]->DeActivate(pFrameWork.GetContext());


	///generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[0]->renderTargetShaderResourceView.Get());
	//convolute bloom texture to scene framebuffer
	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());
	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[1]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);


	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->NotBloomRender();
	if (countDown->GetNowGamePlay())playerHpUi->Render();
	if (countDown->GetNowGamePlay())combo->Render();
	score->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
}

void Game::ResultRender()
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	bg->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	
	ParticleSystem::GetInstance()->BloomRender();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
	framebuffers[0]->DeActivate(pFrameWork.GetContext());


	///generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[0]->renderTargetShaderResourceView.Get());
	//convolute bloom texture to scene framebuffer
	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());
	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[1]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->NotBloomRender();
	score->ScoreRender();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
}

void Game::Finalize()
{
	characterSystem->Finalize();
	bg->Finalize();
	PlayerShotSystem::GetInstance()->Finalize();
	playerHpUi->Reset();
	combo->Finalize();
	score->Finalize();
	gameTimer->Finalize();
	trackingScore->Finalize();
	countDown->Finalize();
}

void Game::SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr)
{
	editParameter = ptr;
}