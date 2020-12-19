#pragma once

#include "../Source/EditParameter.h"
#include <memory>

class MainEditor
{
private:
	std::shared_ptr<Edit::Player::EditPlayerParameter> player;
	wchar_t* filePath;
	wchar_t dirrectory[256];
	const wchar_t* path = L"\\..\\Data\\Document\\PlayerParameter.json";
public:
	MainEditor();
	~MainEditor() {}
	void Edit();
};