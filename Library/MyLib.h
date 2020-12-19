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
//	�v�Z
//
//*********************************************************************************************************

/// <summary>
/// �l�������Ƃɕ�����
/// </summary>
/// <param name="value"> �����������l </param>
/// <param name="frame"> ���������l���i�[����z�� </param>
/// <returns> ���� </returns>
int AnimationFrameCalc(int value/*�l*/, int frame[]/*�t���[��*/);

//*********************************************************************************************************
//
//	�T�E���h
//
//*********************************************************************************************************
void PlaySoundMem(CXAudio2* sound_name); //mode : 0..���̂�  DSBPLAY_LOOPING..���[�v
void StopSoundMem(CXAudio2* sound_name);
void SetVolume(CXAudio2* sound_name, float volume);
//*********************************************************************************************************
//
//	�u�����h
//
//*********************************************************************************************************
//void set_blend_mode(BLEND_MODE mode);

//*********************************************************************************************************
//
//	�f�o�C�X
//
//*********************************************************************************************************
void GetInputXbox();					//xbox�̏�Ԏ擾
void StartVibration(int vib_pal);		//�o�C�u���[�V�����X�^�[�g (0 ~ 60000)
void StopVibration();					//�o�C�u���[�V�����X�g�b�v

void GetInputMouse();					//�}�E�X�̏�Ԏ擾
void EnableCursor(bool enable);			//�}�E�X�J�[�\�� (true : �L��, false : ����)
//*********************************************************************************************************
//
//	�`��
//
//*********************************************************************************************************

struct LoadInf
{
	SpriteBatch* spr;
	std::vector<SpriteInf> inf;
};

bool LoadDivGraph(const wchar_t* file_name, int all_num, int w_num, int h_num, int size_x, int size_y, LoadInf* data);

void TextureRelease(LoadInf* data);
