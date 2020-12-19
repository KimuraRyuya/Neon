#define _CRT_SECURE_NO_WARNINGS
#define __STDC_WANT_LIB_EXT1__ 1
#include "Editor.h"
#include <string.h>
#include<imgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>

MainEditor::MainEditor()
{
	player = std::make_shared<Edit::Player::EditPlayerParameter>();
	GetCurrentDirectory(256, dirrectory);
	filePath = wcscat(dirrectory, path);
}

void MainEditor::Edit()
{
	ImGui::Begin("Player");

	ImGui::DragInt(u8"�e�̂΂��0�i�K", &player->shotRandomValue[0]);
	ImGui::DragInt(u8"�e�̂΂��1�i�K", &player->shotRandomValue[0]);
	ImGui::DragInt(u8"�e�̂΂��2�i�K", &player->shotRandomValue[1]);
	ImGui::DragInt(u8"�e�̂΂��3�i�K", &player->shotRandomValue[2]);
	
	ImGui::DragFloat("NormalSpeed", &player->normalSpeed);
	ImGui::DragFloat("DashSpeed", &player->dashSpeed);

	ImGui::DragFloat(u8"�ړ����̉�]��", &player->moveRotation);

	ImGui::DragFloat(u8"�_�b�V���Q�[�W�̉��Z��", &player->addDashGauge);
	ImGui::DragFloat(u8"�_�b�V���Q�[�W�̌��Y��(�_�b�V����)", &player->subDashGauge);

	if (player->normalSpeed < 0.0f) player->normalSpeed = 0.0f;
	if (player->dashSpeed < 0.0f) player->dashSpeed = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		if (player->shotRandomValue[i] < 0) player->shotRandomValue[i] = 0;
	}
	char input[1024];
	std::wstring iii = filePath;
	wcstombs(input, filePath, sizeof(wchar_t) * int(iii.size()));

	ImGui::Text("%s", input);

	if (ImGui::Button("Save"))
	{
		player->loader->Save(L"PlayerParameter.json", player.get());
	}

	ImGui::End();
}