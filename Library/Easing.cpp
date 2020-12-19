#include "easing.h"

//==============================================================================
//
//		Easing.cpp
//
//      Easing Function 早見表
//      URL https://easings.net/ja
//
//      ※下記サイトのコードを参考にしています（double型を全てfloat型に変更済み）
//      URL https://qiita.com/hart_edsf/items/962ac03281b871dcc0df
//
//==============================================================================


//イージング関数
namespace Easing
{
	float InQuad(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t + min;
	}
	float OutQuad(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return -max * t*(t - 2.0f) + min;
	}
	float InOutQuad(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2.0f < 1.0f)
			return max / 2.0f * t * t + min;
		--t;
		return -max * (t * (t - 2.0f) - 1.0f) + min;
	}
	float InCubic(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*t + min;
	}
	float OutCubic(float t, float totaltime, float max, float min)
	{
		max -= min;
		t = t / totaltime - 1.0f;
		return max * (t*t*t + 1.0f) + min;
	}
	float InOutCubic(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2.0f < 1.0f)
			return max / 2.0f * t*t*t + min;
		t -= 2.0f;
		return max / 2.0f * (t*t*t + 2.0f) + min;
	}
	float InQuart(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*t*t + min;
	}
	float OutQuart(float t, float totaltime, float max, float min)
	{
		max -= min;
		t = t / totaltime - 1.0f;
		return -max * (t*t*t*t - 1.0f) + min;
	}
	float InOutQuart(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2.0f < 1.0f)
			return max / 2.0f * t*t*t*t + min;
		t -= 2.0f;
		return -max / 2.0f * (t*t*t*t - 2.0f) + min;
	}
	float InQuint(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*t*t*t + min;
	}
	float OutQuint(float t, float totaltime, float max, float min)
	{
		max -= min;
		t = t / totaltime - 1.0f;
		return max * (t*t*t*t*t + 1.0f) + min;
	}
	float InOutQuint(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2.0f < 1.0f)
			return max / 2.0f * t*t*t*t*t + min;
		t -= 2.0f;
		return max / 2.0f * (t*t*t*t*t + 2.0f) + min;
	}
	float InSine(float t, float totaltime, float max, float min)
	{
		max -= min;
		return -max * cos(t*ToRadian(90) / totaltime) + max + min;
	}
	float OutSine(float t, float totaltime, float max, float min)
	{
		max -= min;
		return max * sin(t*ToRadian(90) / totaltime) + min;
	}
	float InOutSine(float t, float totaltime, float max, float min)
	{
		max -= min;
		return -max / 2.0f * (cos(t*DirectX::XM_PI / totaltime) - 1.0f) + min;
	}
	float InExp(float t, float totaltime, float max, float min)
	{
		max -= min;
		return t == 0.0f ? min : max * powf(2.0f, 1.0f * (t / totaltime - 1.0f)) + min;
	}
	float OutExp(float t, float totaltime, float max, float min)
	{
		max -= min;
		return t == totaltime ? max + min : max * (-powf(2.0f, -1.0f * t / totaltime) + 1.0f) + min;
	}
	float InOutExp(float t, float totaltime, float max, float min)
	{
		if (t == 0.0f)
			return min;
		if (t == totaltime)
			return max;
		max -= min;
		t /= totaltime;

		if (t / 2.0f < 1.0f)
			return max / 2.0f * powf(2.0f, 1.0f * (t - 1.0f)) + min;
		--t;
		return max / 2.0f * (-powf(2.0f, -1.0f * t) + 2.0f) + min;

	}
	float InCirc(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		return -max * (sqrtf(1.0f - (t * t)) - 1.0f) + min;
	}
	float OutCirc(float t, float totaltime, float max, float min)
	{
		max -= min;
		t = t / totaltime - 1.0f;
		return max * sqrtf(1.0f - (t * t)) + min;
	}
	float InOutCirc(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2.0f < 1.0f)
			return -max / 2.0f * (sqrtf(1.0f - (t * t)) - 1.0f) + min;
		t -= 2.0f;
		return max / 2.0f * (sqrtf(1.0f - (t * t)) + 1.0f) + min;
	}
	float InBack(float t, float totaltime, float max, float min, float s)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*((s + 1.0f)*t - s) + min;
	}
	float OutBack(float t, float totaltime, float max, float min, float s)
	{
		max -= min;
		t = t / totaltime - 1.0f;
		return max * (t*t*((s + 1.0f)*t*s) + 1.0f) + min;
	}
	float OutBounce(float t, float totaltime, float max, float min)
	{
		max -= min;
		t /= totaltime;

		if (t < 1 / 2.75f)
			return max * (7.5625f*t*t) + min;
		else if (t < 2 / 2.75f)
		{
			t -= 1.5f / 2.75f;
			return max * (7.5625f*t*t + 0.75f) + min;
		}
		else if (t< 2.5f / 2.75f)
		{
			t -= 2.25f / 2.75f;
			return max * (7.5625f*t*t + 0.9375f) + min;
		}
		else
		{
			t -= 2.625f / 2.75f;
			return max * (7.5625f*t*t + 0.984375f) + min;
		}
	}
	float InBounce(float t, float totaltime, float max, float min)
	{
		return max - OutBounce(totaltime - t, totaltime, max - min, 0.0f) + min;
	}
	float InOutBounce(float t, float totaltime, float max, float min)
	{
		if (t < totaltime / 2.0f)
			return InBounce(t * 2.0f, totaltime, max - min, max)*0.5f + min;
		else
			return OutBounce(t * 2.0f - totaltime, totaltime, max - min, 0.0f)*0.5f + min + (max - min)*0.5f;
	}
	float Linear(float t, float totaltime, float max, float min)
	{
		return (max - min)*t / totaltime + min;
	}
}
