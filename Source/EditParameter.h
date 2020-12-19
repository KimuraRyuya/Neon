#pragma once
#include <windows.h>
#include <string>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

//creal
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/base_class.hpp>

	template<class T>
	class ParameterLoader
	{
	private:
		SYSTEMTIME tmpStFileTime{};
	public:
		ParameterLoader() {}
		~ParameterLoader() {}
		void Init(const wchar_t* name)
		{
			HANDLE file;
			FILETIME fileTime;
			FILETIME lpFileTime;

			file = CreateFile(
				name, GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
			);

			if (file == INVALID_HANDLE_VALUE) {
				CloseHandle(file);
				return;
			}

			GetFileTime(file, NULL, &fileTime, NULL);
			CloseHandle(file);

			tmpStFileTime.wYear = 0;
			tmpStFileTime.wMonth = 0;
			tmpStFileTime.wDay = 0;
			tmpStFileTime.wMinute = 0;
			tmpStFileTime.wSecond = 0;

			FileTimeToLocalFileTime(&fileTime, &lpFileTime);

			FileTimeToSystemTime(&lpFileTime, &tmpStFileTime);
		}
		void CheckUpdateDay(const wchar_t* name, T* variables)
		{
			HANDLE file;
			FILETIME fileTime;
			FILETIME lpFileTime;
			SYSTEMTIME stFileTime{};

			file = CreateFile(
				name, GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
			);

			if (file == INVALID_HANDLE_VALUE) {
				CloseHandle(file);
				return;
			}

			GetFileTime(file, NULL, &fileTime, NULL);
			CloseHandle(file);

			FileTimeToLocalFileTime(&fileTime, &lpFileTime);
			FileTimeToSystemTime(&lpFileTime, &stFileTime);

			if (stFileTime.wYear != tmpStFileTime.wYear ||
				stFileTime.wMonth != tmpStFileTime.wMonth ||
				stFileTime.wHour != tmpStFileTime.wHour ||
				stFileTime.wMinute != tmpStFileTime.wMinute ||
				stFileTime.wSecond != tmpStFileTime.wSecond)
			{
				Load(name, variables);
				tmpStFileTime = stFileTime;
			}
		}
		void Load(const wchar_t* name, T* variables)
		{
			// json読み込み
			std::ifstream ifs;
			ifs.open(name, std::ios::in);
			{
				cereal::JSONInputArchive i_archive(ifs);
				i_archive(*variables);
			}
			ifs.close();
		}
		void Save(const wchar_t* name, T* variables)
		{
			//cereal でjson とbinary に保存
			std::ofstream ofs;
			ofs.open(name, std::ios::out);
			if (ofs.is_open())
			{
				cereal::JSONOutputArchive o_archive(ofs);
				o_archive(*variables);
			}
			ofs.close();
		}

		template<class CT>
		void serialize(CT& archive, unsigned int version)
		{
			return;
		}
	};

namespace Edit
{
	namespace Player
	{
		class EditPlayerParameter
		{
		public:
			EditPlayerParameter() {}
			~EditPlayerParameter() {}

			std::unique_ptr<ParameterLoader<EditPlayerParameter>>  loader;

			int shotRandomValue1 = 10;
			int shotRandomValue2 = 16;
			int shotRandomValue3 = 20;
			float normalSpeed = 0.015f;
			float dashSpeed = 0.03f;
			float moveRotation = 100.0f;
			float addDashGauge = 3.0f;
			float subDashGauge = -150.0f;

			void Init(const wchar_t* name)
			{
				loader = std::make_unique<ParameterLoader<EditPlayerParameter>>();
				loader->Init(name);
				loader->Load(name, this);
			}

			void Update(const wchar_t* name)
			{
				loader->CheckUpdateDay(name, this);
			}

			template<class T>
			void serialize(T& archive, unsigned int version)
			{
				archive
				(
					cereal::make_nvp(u8"弾ばらつき1段階", static_cast<float>(shotRandomValue1)),
					cereal::make_nvp(u8"弾ばらつき2段階", static_cast<float>(shotRandomValue2)),
					cereal::make_nvp(u8"弾ばらつき3段階", static_cast<float>(shotRandomValue3)),
					cereal::make_nvp(u8"通常のスピード", static_cast<float>(normalSpeed)),
					cereal::make_nvp(u8"ダッシュのスピード", static_cast<float>(dashSpeed)),
					cereal::make_nvp(u8"移動時の回転量", static_cast<float>(moveRotation)),
					cereal::make_nvp(u8"ゲージの増える量", static_cast<float>(addDashGauge)),
					cereal::make_nvp(u8"ゲージの減る量 : ダッシュ", static_cast<float>(subDashGauge))
					);
			}

		};
	}

	class EditPrameter
	{
	public:
		std::shared_ptr<Edit::Player::EditPlayerParameter> playerEditParam;

		void Init()
		{
			playerEditParam = std::make_shared<Edit::Player::EditPlayerParameter>();
			playerEditParam->Init(L"Data/Document/PlayerParameter.json");
		}

		void Update()
		{
			playerEditParam->Update(L"Data/Document/PlayerParameter.json");
		}
	};

}

CEREAL_CLASS_VERSION(Edit::Player::EditPlayerParameter, 1)
//CEREAL_REGISTER_TYPE(ParameterLoader<Edit::Player::EditPlayerParameter>)
//CEREAL_REGISTER_TYPE(Edit::Player::EditPlayerParameter)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(ParameterLoader<Edit::Player::EditPlayerParameter>, Edit::Player::EditPlayerParameter);
