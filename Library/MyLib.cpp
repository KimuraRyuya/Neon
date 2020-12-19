#include "MyLib.h"

int AnimationFrameCalc(int value/*値*/, int frame[]/*フレーム*/)
{
	int Dividevalue = 10;
	int tmpvalue = value;
	int digit = 0;
	while (1)
	{
		tmpvalue = tmpvalue / 10;
		digit++;
		if (tmpvalue == 0) break;
	}
	tmpvalue = value;
	for (int i = 0; i < digit - 2; i++)
	{
		Dividevalue *= 10;
	}

	if (digit - 1 <= 0)
	{
		frame[0] = tmpvalue;
	}
	else
	{
		for (int i = 0; i < digit; i++)
		{
			frame[i] = tmpvalue / Dividevalue;
			tmpvalue %= Dividevalue;
			Dividevalue /= 10;
		}
	}

	return digit;
}

bool LoadDivGraph(const wchar_t* file_name, int all_num, int w_num, int h_num, int size_x, int size_y, LoadInf* data)
{
	static int num = 0;
	//合計分割数と縦と横の分割数の合計が違うかったら即Return
	if (all_num != w_num * h_num) return false;
	SpriteInf _inf;

	DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(0, 0);
	DirectX::XMFLOAT2 dist_moved = DirectX::XMFLOAT2(0, 0);//動いたマス
	data->spr = new SpriteBatch(file_name);

	for (int i = 0; i < all_num; i++)
	{
		_inf.sx = pos.x;
		_inf.sy = pos.y;
		_inf.sw = static_cast<float>(size_x);
		_inf.sh = static_cast<float>(size_y);

		if (w_num - 1 <= dist_moved.x)
		{
			pos.x = 0;
			pos.y += size_y;
			dist_moved.x = 0;
			dist_moved.y++;
		}
		else
		{
			pos.x += size_x;
			dist_moved.x++;
		}
		data->inf.push_back(_inf);
	}
	return true;
}

void TextureRelease(LoadInf* data)
{
	data->inf.clear();
	delete data->spr;
	data->spr = NULL;
}