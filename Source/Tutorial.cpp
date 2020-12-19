#include "Tutorial.h"
#include "SceneManager.h"
#include "ParticleSystem.h"
#include "CollisionJudge.h"
#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include <imgui.h>
#include "EditParameter.h"

void Tutorial::Initialize()
{

	characterSystem = std::make_unique<CharacterSystem>();
	characterSystem->SetEditParameter(editParameter);
	characterSystem->Initialize();

	combo = std::make_unique<Combo>();
	combo->Initialize();

	trackingScore = std::make_unique<TrackingScore>();
	trackingScore->Initialize();

	playerHpUi = std::make_unique<PlayerHpUi>();
	playerHpUi->Initialize();

	PlayerShotSystem::GetInstance()->Initialize();

	bloomEffect = std::make_unique<Bloom>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight);
	framebuffers[0] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[1] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[2] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[3] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	tonemap_effect = std::make_unique<ToneMap>(pFrameWork.GetDevice());

	textTex = std::make_unique<Billboard>(L"Data/Texture/Tutorial.png");
	textTex->DissolveTextureLoad(L"Data/Texture/menunoise.png");
	textTex->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	buttonTex = std::make_unique<Billboard>(L"Data/Texture/Button.png");
	buttonTex->DissolveTextureLoad(L"Data/Texture/menunoise.png");
	buttonTex->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	textPos = { 20.0f, 0.5f, -6.0f };
	displyNum = 0.0f;
	buttonTrg = false;
	buttonAnimFrame = 0.0f;
	isCameraMove = false;
	cameraMoveTime = 0.0f;
	state = State::Move;

	characterSystem->SetEditParameter(editParameter);
}

void Tutorial::Reset()
{
	characterSystem->Reset();

	combo->Reset();

	trackingScore->Reset();

	playerHpUi->Reset();

	PlayerShotSystem::GetInstance()->Reset();

	textPos = { 20.0f, 0.5f, -6.0f };
	displyNum = 0.0f;
	buttonTrg = false;
	buttonAnimFrame = 0.0f;
	isCameraMove = false;
	cameraMoveTime = 0.0f;
	state = State::Move;
}

SceneState Tutorial::Update(std::shared_ptr<BG> bg, float elapsedTime)
{

	if (isCameraMove)
	{
		trackingScore->SetTrackingPos(characterSystem->GetPlayerAddress()->GetPos());
		trackingScore->Update(elapsedTime);
		PlayerShotSystem::GetInstance()->Update(elapsedTime);
		CollisionJudge::PlayerShotVSStage(PlayerShotSystem::GetInstance(), bg, characterSystem.get());

		DirectX::XMFLOAT3 ori = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR _origin = DirectX::XMLoadFloat3(&ori);
		DirectX::XMVECTOR _cameraT = DirectX::XMLoadFloat3(&cameraTarget);
		DirectX::XMVECTOR ans;
		float t = Easing::InQuad(cameraMoveTime, 2.0f, 1.0f, 0.0f);
		ans = DirectX::XMVectorLerp(_cameraT, _origin, t);
		DirectX::XMFLOAT3 _ans = {};

		DirectX::XMStoreFloat3(&_ans, ans);

		DirectX::XMFLOAT3 _target = 
		{
			_ans.x,
			0.0f,
			_ans.z * -1.0f
		};
		Camera::GetInstance().SetTarget(_target);
		cameraMoveTime += 2.0f * elapsedTime;
		if (cameraMoveTime > 2.0f)
		{
			Camera::GetInstance().SetTarget({ 0.0f, 0.0f, 0.0f });
			Reset();
			bg->Reset();
			return SceneState::COUNT;
		}

		return SceneState::TUTORIAL;
	}

	GetInputXbox();
	bg->Update(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress(), elapsedTime);
	combo->Update(elapsedTime);
	characterSystem->GetPlayerAddress()->SetReinforce(bg->CheckPlayerFillPos(characterSystem->GetPlayerAddress()->GetPos()));
	characterSystem->TutorialUpdate(combo->GetLevel(), elapsedTime);

	if (characterSystem->GetPlayerAddress()->GetEnemyAllKillFlg())
	{
		bg->UltimateEffectAndCalcFill(characterSystem->GetPlayerAddress());
		characterSystem->GetPlayerAddress()->SetEnemyAllKillFlg(false);
	}

	trackingScore->SetTrackingPos(characterSystem->GetPlayerAddress()->GetPos());
	trackingScore->Update(elapsedTime);
	PlayerShotSystem::GetInstance()->Update(elapsedTime);
	playerHpUi->Update(elapsedTime);
	playerHpUi->SetPlayerCircleUiPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerHpUiPosPos(characterSystem->GetPlayerAddress()->GetPos());
	playerHpUi->SetPlayerNowHp(characterSystem->GetPlayerAddress()->GetHp());
	playerHpUi->SetPlayerShotGauge(abs(characterSystem->GetPlayerAddress()->GetDushGauge() - 360.0f));
	playerHpUi->SetFloorUIDisply(characterSystem->GetPlayerAddress()->GetFillChipDestory());
	playerHpUi->SetULTGauge(bg->GetNowFillNum());

	combo->SetComboTextPos(characterSystem->GetPlayerAddress()->GetPos());
	combo->SetComboNumTextPos(characterSystem->GetPlayerAddress()->GetPos());


	CollisionJudge::PlayerVSStage(characterSystem->GetPlayerAddress(), bg);
	//CollisionJudge::PlayerVSEnemy(characterSystem->getPlayerAddress(), characterSystem->getEnemyManagerAddress(), combo.get());
	CollisionJudge::PlayerVSTrakingScore(characterSystem->GetPlayerAddress(), trackingScore.get());
	CollisionJudge::PlayerDestroyEffectVSEnemy(characterSystem->GetPlayerAddress(), characterSystem->GetEnemyManagerAddress());
	CollisionJudge::PlayerShotVSStage(PlayerShotSystem::GetInstance(), bg, characterSystem.get());
	//CollisionJudge::PlayerShotVSEnemy(PlayerShotSystem::GetInstance(), characterSystem.get(), combo.get(), score.get(), bg.get(), trackingScore.get(), characterSystem->getPlayerAddress()->getReinforce());



	// Aボタンを押したらチュートリアルを一つずつスキップ
	if (PAD->xb.A_BUTTON && !buttonTrg)
	{
		buttonTrg = true;

		if (state == State::Move && static_cast<int>(displyNum) == 12)
		{
			state = State::Dash;
			displyNum = 0.0f;
			buttonAnimFrame = 0.0f;
			return SceneState::TUTORIAL;
		}
		else if (state == State::Dash && static_cast<int>(displyNum) == 14)
		{
			state = State::Shot;
			displyNum = 0.0f;
			buttonAnimFrame = 0.0f;
			return SceneState::TUTORIAL;
		}
		else if (state == State::Shot && static_cast<int>(displyNum) == 17)
		{
			state = State::Fill;
			displyNum = 0.0f;
			buttonAnimFrame = 0.0f;
			return SceneState::TUTORIAL;
		}
		else if (state == State::Fill && static_cast<int>(displyNum) == 17)
		{
			state = State::Power;
			displyNum = 0.0f;
			buttonAnimFrame = 0.0f;
			return SceneState::TUTORIAL;
		}
		else if (state == State::Power && static_cast<int>(displyNum) == 18)
		{
			isCameraMove = true;
			cameraTarget = characterSystem->GetPlayerAddress()->GetPos();
			ParticleSystem::GetInstance()->playerDestroyParticle->Burst(1000, characterSystem->GetPlayerAddress()->GetPos(), pFrameWork.GetElapsedTime(), characterSystem->GetPlayerAddress()->GetModelResource(), 0.0f, 0.0f);
			return SceneState::TUTORIAL;
			//return SceneState::TUTORIAL;
		}

		SkipText();
	}
	else if (!PAD->xb.A_BUTTON)
	{
		buttonTrg = false;
	}

	// タイプライター
	switch (state)
	{
	case State::Move:
		displyNum += 5.0f * elapsedTime;
		buttonAnimFrame += 3.0f * elapsedTime;
		if (displyNum > 12.0f)
		{
			displyNum = 12.0f;
		}
		if (buttonAnimFrame > 8.0f)
		{
			buttonAnimFrame = 0.0f;
		}
		break;
	case State::Dash:
		displyNum += 5.0f * elapsedTime;
		buttonAnimFrame += 3.0f * elapsedTime;
		if (displyNum > 14.0f)
		{
			displyNum = 14.0f;
		}
		if (buttonAnimFrame > 8.0f)
		{
			buttonAnimFrame = 0.0f;
		}
		break;
	case State::Shot:
		displyNum += 5.0f * elapsedTime;
		buttonAnimFrame -= 3.0f * elapsedTime;
		if (displyNum > 17.0f)
		{
			displyNum = 17.0f;
		}
		if (buttonAnimFrame < 0.0f)
		{
			buttonAnimFrame = 8.0f;
		}
		break;
	case State::Fill:
		displyNum += 5.0f * elapsedTime;
		buttonAnimFrame += 3.0f * elapsedTime;
		if (displyNum > 17.0f)
		{
			displyNum = 17.0f;
		}
		if (buttonAnimFrame > 8.0f)
		{
			buttonAnimFrame = 0.0f;
		}
		break;
	case State::Power:
		displyNum += 5.0f * elapsedTime;
		buttonAnimFrame += 3.0f * elapsedTime;
		if (displyNum > 18.0f)
		{
			displyNum = 18.0f;
		}
		if (buttonAnimFrame > 8.0f)
		{
			buttonAnimFrame = 0.0f;
		}
		break;
	default:
		break;
	}

#ifdef _DEBUG
	ImGui::Begin("Tutorial");
	ImGui::Image(framebuffers[0]->renderTargetShaderResourceView.Get(), ImVec2(100, 100));
	ImGui::Image(framebuffers[1]->renderTargetShaderResourceView.Get(), ImVec2(100, 100));
	ImGui::End();
#endif // _DEBUG

	return SceneState::TUTORIAL;
}

void Tutorial::Render(std::shared_ptr<BG> bg)
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());
	
	framebuffers[0]->DeActivate(pFrameWork.GetContext());

	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());

	framebuffers[2]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[2]->Activate(pFrameWork.GetContext());

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	characterSystem->GetPlayerAddress()->GetPlayerUltimate()->Render();
	bg->Render();
	if (!isCameraMove)
	{

		textTex->Begin();
		textTex->Render(textPos, 0.0f, 0.0f, 1920.0f, 1080.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 17.0f * (1920.0f / 1080.0f), 17.0f }, { 30.0f, 30.0f, 30.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
		textTex->End();

		trackingScore->Render();
		pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
		characterSystem->Render();
	};
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->BloomRender();
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	PlayerShotSystem::GetInstance()->Render();

	framebuffers[2]->DeActivate(pFrameWork.GetContext());

	///generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[2]->renderTargetShaderResourceView.Get());
	//convolute bloom texture to scene framebuffer
	framebuffers[3]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[3]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[3]->DeActivate(pFrameWork.GetContext());
	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[3]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ParticleSystem::GetInstance()->NotBloomRender();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	if (!isCameraMove)
	{

		pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
		playerHpUi->Render();
		combo->Render();
		pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
		
		float _x = 0.0f, _y = 0.0f, _z = 0.0f;
		int csatButtonAnimFrame = static_cast<int>(buttonAnimFrame);
		switch (state)
		{
		case State::Move:
			textTex->Begin();
			for (int i = 0; i < static_cast<int>(displyNum); i++)
			{
				_x = textPos.x + 10.0f - (1.85f * i);
				_y = textPos.y;
				_z = textPos.z;

				textTex->Render({ _x, _y, _z }, 0.0f + 100 * i, 1080.0f, 100.0f, 100.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 1.5f, 1.5f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
			}
			textTex->End();

			_x = textPos.x + 10.0f;
			_y = textPos.y;
			_z = textPos.z + 3.0f;

			buttonTex->Begin();
			buttonTex->Render({ _x, _y, _z }, 0.0f + (512.0f * csatButtonAnimFrame), 512.0f, 512.0f, 512.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 3.0f, 3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f}, false);
			buttonTex->End();

			break;
		case State::Dash:
			textTex->Begin();
			for (int i = 0; i < static_cast<int>(displyNum); i++)
			{
				_x = textPos.x + 12.0f - (1.85f * i);
				_y = textPos.y;
				_z = textPos.z;

				textTex->Render({ _x, _y, _z }, 0.0f + 100 * i, 1080.0f + 100.0f * 1.0f, 100.0f, 100.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 1.3f, 1.3f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
			}
			textTex->End();



			buttonTex->Begin();
			_x = textPos.x + 5.0f;
			_y = textPos.y;
			_z = textPos.z + 3.0f;
			buttonTex->Render({ _x, _y, _z }, 0.0f + (512.0f * csatButtonAnimFrame), 512.0f, 512.0f, 512.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 3.0f, 3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, false);
			_x = textPos.x + 10.0f;
			buttonTex->Render({ _x, _y, _z }, 0.0f + (512.0f * (csatButtonAnimFrame % 2)), 512.0f * 2.0f, 512.0f, 512.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 3.0f, 3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, false);
			buttonTex->End();

			break;
		case State::Shot:
			textTex->Begin();
			for (int i = 0; i < static_cast<int>(displyNum); i++)
			{
				_x = textPos.x + 12.0f - (1.35f * i);
				_y = textPos.y;
				_z = textPos.z;

				textTex->Render({ _x, _y, _z }, 0.0f + 100 * i, 1080.0f + 100.0f * 2.0f, 100.0f, 100.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 1.2f, 1.2f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
			}
			textTex->End();
			_x = textPos.x - 6.0f;
			_y = textPos.y;
			_z = textPos.z + 3.0f;

			buttonTex->Begin();
			buttonTex->Render({ _x, _y, _z }, 0.0f + (512.0f * csatButtonAnimFrame), 512.0f, 512.0f, 512.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 3.0f, 3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, false);
			buttonTex->End();

			break;
		case State::Fill:
			textTex->Begin();
			for (int i = 0; i < static_cast<int>(displyNum); i++)
			{
				_x = textPos.x + 12.0f - (1.35f * i);
				_y = textPos.y;
				_z = textPos.z;

				textTex->Render({ _x, _y, _z }, 0.0f + 100 * i, 1080.0f + 100.0f * 3.0f, 100.0f, 100.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 1.2f, 1.2f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
			}
			textTex->End();
			break;
		case State::Power:
			textTex->Begin();
			for (int i = 0; i < static_cast<int>(displyNum); i++)
			{
				_x = textPos.x + 12.0f - (1.35f * i);
				_y = textPos.y;
				_z = textPos.z;

				textTex->Render({ _x, _y, _z }, 0.0f + 100 * i, 1080.0f + 100.0f * 4.0f, 100.0f, 100.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 1.2f, 1.2f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0, 1.0f }, false);
			}
			textTex->End();
			break;
		default:
			break;
		}
		_x = textPos.x - 10.0f;
		_y = textPos.y;
		_z = textPos.z + 3.0f;

		buttonTex->Begin();
		buttonTex->Render({ _x, _y, _z }, 0.0f + (512.0f * (csatButtonAnimFrame % 2)), 0.0f, 512.0f, 512.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 3.0f, 3.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, false);
		buttonTex->End();
	}

}

void Tutorial::Finalize()
{

}

void Tutorial::SkipText()
{
	if (state == State::Move)
	{
		displyNum = 12.0f;
	}
	else if (state == State::Dash)
	{
		displyNum = 14.0f;
	}
	else if (state == State::Shot)
	{
		displyNum = 17.0f;
	}
	else if (state == State::Fill)
	{
		displyNum = 17.0f;
	}
	else if (state == State::Power)
	{
		displyNum = 18.0f;
	}
}

void Tutorial::SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr)
{
	editParameter = ptr; 
}