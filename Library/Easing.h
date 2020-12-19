#ifndef INCLUDED_EASING
#define INCLUDED_EASING
#include <DirectXMath.h>
//******************************************************************************
//
//		Easing.h
//
//      Easing Function 早見表
//      URL https://easings.net/ja
//
//      ※下記サイトのコードを参考にしています（double型を全てfloat型に変更済み）
//      URL https://qiita.com/hart_edsf/items/962ac03281b871dcc0df
//
//******************************************************************************

static float(*const ToRadian)(float) = DirectX::XMConvertToRadians;  // 角度をラジアンに
static float(*const ToDegree)(float) = DirectX::XMConvertToDegrees;  // ラジアンを角度に

//イージング関数
namespace Easing
{
	float InQuad(float t, float totaltime, float max, float min);
	float OutQuad(float t, float totaltime, float max, float min);
	float InOutQuad(float t, float totaltime, float max, float min);
	float InCubic(float t, float totaltime, float max, float min);
	float OutCubic(float t, float totaltime, float max, float min);
	float InOutCubic(float t, float totaltime, float max, float min);
	float InQuart(float t, float totaltime, float max, float min);
	float OutQuart(float t, float totaltime, float max, float min);
	float InOutQuart(float t, float totaltime, float max, float min);
	float InQuint(float t, float totaltime, float max, float min);
	float OutQuint(float t, float totaltime, float max, float min);
	float InOutQuint(float t, float totaltime, float max, float min);
	float InSine(float t, float totaltime, float max, float min);
	float OutSine(float t, float totaltime, float max, float min);
	float InOutSine(float t, float totaltime, float max, float min);
	float InExp(float t, float totaltime, float max, float min);
	float OutExp(float t, float totaltime, float max, float min);
	float InOutExp(float t, float totaltime, float max, float min);
	float InCirc(float t, float totaltime, float max, float min);
	float OutCirc(float t, float totaltime, float max, float min);
	float InOutCirc(float t, float totaltime, float max, float min);
	float InBack(float t, float totaltime, float max, float min, float s);
	float OutBack(float t, float totaltime, float max, float min, float s);
	float OutBounce(float t, float totaltime, float max, float min);
	float InBounce(float t, float totaltime, float max, float min);
	float InOutBounce(float t, float totaltime, float max, float min);
	float Linear(float t, float totaltime, float max, float min);
}

#endif // !INCLUDED_EASING