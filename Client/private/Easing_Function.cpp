#include "stdafx.h"
#include "Easing_Function.h"
#include <math.h>

namespace Client
{
	float EaseLine(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * fCurrentTime / fMaxTime;
	}

	float EaseInSine(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * (1 - cosf(fCurrentTime / fMaxTime * 3.14f) / 2.f);
	}

	float EaseOutSine(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * sinf((fCurrentTime / fMaxTime * 3.14f) / 2.f);
	}

	float EaseInOutSine(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * -(cosf(3.14f * fCurrentTime / fMaxTime) - 1.f) / 2.f;
	}

	float EaseInQuad(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * (fCurrentTime / fMaxTime) * (fCurrentTime / fMaxTime);
	}

	float EaseOutQuad(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * (1.f - (1.f - fCurrentTime / fMaxTime) * (1.f - fCurrentTime / fMaxTime));
	}

	float EaseInOutQuad(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * (fCurrentTime / fMaxTime < 0.5f ? 2.f * fCurrentTime / fMaxTime * fCurrentTime / fMaxTime : 1.f - powf(-2.f * fCurrentTime / fMaxTime + 2.f, 2.f)) / 2.f;
	}

	float EaseInCubic(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * fLerpPoint * fLerpPoint * fLerpPoint;
	}

	float EaseOutCubic(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (1.f - powf(1.f - fLerpPoint, 3.f));
	}

	float EaseInOutCubic(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (fLerpPoint < 0.5f ? 4.f * fLerpPoint * fLerpPoint * fLerpPoint : 1.f - powf(-2.f * fLerpPoint + 2.f, 3.f)) / 2.f;
	}

	float EaseInQuart(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * fLerpPoint * fLerpPoint * fLerpPoint * fLerpPoint;
	}

	float EaseOutQuart(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (1.f - powf(1.f - fLerpPoint, 4.f));
	}

	float EaseInOutQuart(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (fLerpPoint < 0.5f ? 8.f * fLerpPoint * fLerpPoint * fLerpPoint * fLerpPoint : 1.f - powf(-2.f * fLerpPoint + 2.f, 4.f)) / 2.f;
	}

	float EaseInQuint(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * fLerpPoint * fLerpPoint * fLerpPoint * fLerpPoint * fLerpPoint;
	}

	float EaseOutQuint(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (1.f - powf(1.f - fLerpPoint, 5.f));
	}

	float EaseInOutQuint(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (fLerpPoint < 0.5f ? 16.f * fLerpPoint * fLerpPoint * fLerpPoint * fLerpPoint : 1.f - powf(-2.f * fLerpPoint + 2.f, 5.f)) / 2.f;
	}

	float EaseInExpo(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (fLerpPoint == 0.f ? 0.f : powf(2.f, 10.f * fLerpPoint - 10.f));
	}

	float EaseOutExpo(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (fLerpPoint == 1.f ? 1.f : 1.f - powf(2.f, -10.f * fLerpPoint));
	}

	float EaseInOutExpo(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize *
			(fLerpPoint == 0.f ? 0.f :
			(fLerpPoint == 1.f ? 1.f :
				(fLerpPoint < 0.5f ? powf(2.f, 20.f * fLerpPoint - 10.f) / 2.f :
				(2.f - powf(2.f, -20.f * fLerpPoint + 10.f))))) / 2.f;
	}

	float EaseInCirc(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * (1.f - sqrtf(1.f - powf(fLerpPoint, 2.f)));
	}

	float EaseOutCirc(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize * sqrtf(1.f - powf(fLerpPoint - 1.f, 2.f));
	}

	float EaseInOutCirc(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;
		return fSize *
			(fLerpPoint < 0.5f ? (1.f - sqrtf(1.f - powf(2.f * fLerpPoint, 2.f))) / 2.f :
			(sqrtf(1.f - powf(-2.f * fLerpPoint + 2.f, 2.f)) + 1.f) / 2.f);
	}

	float EaseInBack(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (c3 * fLerpPoint * fLerpPoint * fLerpPoint - c1 * fLerpPoint * fLerpPoint);
	}

	float EaseOutBack(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (1.f + c3 * powf(fLerpPoint - 1.f, 3.f) + c1 * powf(fLerpPoint - 1.f, 2.f));
	}

	float EaseInOutBack(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (fLerpPoint < 0.5f ? (powf(2.f * fLerpPoint, 2.f) * ((c2 + 1.f) * 2.f * fLerpPoint - c2)) / 2.f :
			(powf(2.f * fLerpPoint - 2.f, 2.f) * ((c2 + 1.f) * (fLerpPoint * 2.f - 2.f) + c2) + 2.f) / 2.f);
	}

	float EaseInElastic(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c4 = (2.f * 3.14f) / 3.f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (fLerpPoint == 0.f ? 0.f :
			(fLerpPoint == 1.f ? 1.f :
			(-powf(2.f, 10.f * fLerpPoint - 10.f) * sinf((fLerpPoint * 10.f - 10.75f) * c4))));
	}

	float EaseOutElastic(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c4 = (2.f * 3.14f) / 3.f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (fLerpPoint == 0.f ? 0.f :
			(fLerpPoint == 1.f ? 1.f :
			(powf(2.f, -10.f * fLerpPoint) * sinf((fLerpPoint * 10.f - 0.75f) * c4) + 1.f)));
	}

	float EaseInOutElastic(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float c5 = (2.f * 3.14f) / 4.5f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (fLerpPoint == 0.f ? 0.f :
			(fLerpPoint == 1.f ? 1.f :
			(fLerpPoint < 0.5f ? -(powf(2.f, 20.f * fLerpPoint - 10.f) * sinf((20.f * fLerpPoint - 11.125f)) / 2.f) :
				(powf(2.f, -20.f * fLerpPoint + 10.f) * sinf((fLerpPoint * 20.f - 11.125f) * c5)) / 2.f + 1.f)));
	}

	float EaseOutBounce(float fSize, float fCurrentTime, float fMaxTime)
	{
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		float fLerpPoint = fCurrentTime / fMaxTime;

		if (fLerpPoint < 1.f / d1)
			return fSize * n1 * fLerpPoint * fLerpPoint;
		else if (fLerpPoint < 2.f / d1)
			return fSize * n1 * (fLerpPoint -= 1.5f / d1) * fLerpPoint + 0.75f;
		else if (fLerpPoint < 2.5f * d1)
			return n1 * (fLerpPoint -= 2.25f / d1) * fLerpPoint + 0.9375f;
		else
			return n1 * (fLerpPoint -= 2.625f / d1) * fLerpPoint + 0.984375f;
	}

	float EaseInBounce(float fSize, float fCurrentTime, float fMaxTime)
	{
		return fSize * EaseOutBounce(1.f, fCurrentTime, fMaxTime);
	}

	float EaseInOutBounce(float fSize, float fCurrentTime, float fMaxTime)
	{
		float fLerpPoint = fCurrentTime / fMaxTime;

		return fSize * (fLerpPoint < 0.5f ? (1.f - EaseOutBounce(1.f, fMaxTime - 2.f * fCurrentTime, fMaxTime)) / 2.f :
			(1.f + EaseOutBounce(1.f, 2.f * fCurrentTime - fMaxTime, fMaxTime)) / 2.f);
	}

}