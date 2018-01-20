#ifndef MATH_H
#define MATH_H

#pragma once

#include <math.h>

#include "vector.h"
#include "quaternion.h"
#include "color.h"

namespace Nixie
{
	class Vector2;
	class Vector3;
	class Quaternion;
	class Color;

	namespace Math
	{
		inline float InvSqrtf(float value) { return 1 / sqrtf(value); }

		inline float Clampf(float value, float min, float max)
		{
			float result = value;

			if (result < min)
				result = min;
			else if (result > max)
				result = max;

			return result;
		}

		inline int Clamp(int value, int min, int max)
		{
			int result = value;

			if (result < min)
				result = min;
			else if (result > max)
				result = max;

			return result;
		}
	}
}

#endif