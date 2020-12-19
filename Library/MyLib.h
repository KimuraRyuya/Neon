#pragma once
#include "FrameWork.h"
#include <DirectXMath.h>
#include <time.h>
#include "Easing.h"
#include "InputDevice.h"
#include "Audio.h"
#include "Sprite.h"

//const int SCREEN_WIDTH = framework::SCREEN_WIDTH;
//const int SCREEN_HEIGHT = framework::SCREEN_HEIGHT;


//*********************************************************************************************************
//
//	計算
//
//*********************************************************************************************************

/// <summary>
/// 値を桁ごとに分ける
/// </summary>
/// <param name="value"> 分割したい値 </param>
/// <param name="frame"> 分割した値を格納する配列 </param>
/// <returns> 桁数 </returns>
int AnimationFrameCalc(int value/*値*/, int frame[]/*フレーム*/);

//*********************************************************************************************************
//
//	サウンド
//
//*********************************************************************************************************
void PlaySoundMem(CXAudio2* sound_name); //mode : 0..一回のみ  DSBPLAY_LOOPING..ループ
void StopSoundMem(CXAudio2* sound_name);
void SetVolume(CXAudio2* sound_name, float volume);
//*********************************************************************************************************
//
//	ブレンド
//
//*********************************************************************************************************
//void set_blend_mode(BLEND_MODE mode);

//*********************************************************************************************************
//
//	デバイス
//
//*********************************************************************************************************
void GetInputXbox();					//xboxの状態取得
void StartVibration(int vib_pal);		//バイブレーションスタート (0 ~ 60000)
void StopVibration();					//バイブレーションストップ

void GetInputMouse();					//マウスの状態取得
void EnableCursor(bool enable);			//マウスカーソル (true : 有効, false : 無効)
//*********************************************************************************************************
//
//	描画
//
//*********************************************************************************************************

struct LoadInf
{
	SpriteBatch* spr;
	std::vector<SpriteInf> inf;
};

bool LoadDivGraph(const wchar_t* file_name, int all_num, int w_num, int h_num, int size_x, int size_y, LoadInf* data);

void TextureRelease(LoadInf* data);
