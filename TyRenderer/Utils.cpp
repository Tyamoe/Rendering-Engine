#include "Utils.h"

TYint Hash(TYstring str)
{
	TYint seed = 131;//31  131 1313 13131131313 etc//
	TYint hash = 0;

	for (TYint i = 0; i < str.length(); i++)
	{
		hash = (hash * seed) + (str[i]);
	}

	return hash & (0x7FFFFFFF);
}

TYint Hash(TYcstring str, TYuint len)
{
	TYint seed = 131;//31  131 1313 13131131313 etc//
	TYint hash = 0;

	for (TYuint i = 0; i < len; i++)
	{
		hash = (hash * seed) + (str[i]);
	}

	return hash & (0x7FFFFFFF);
}

TYfloat lerp_t(TYfloat a, TYfloat b, TYfloat t, TYfloat dt)
{
	return (a + ((b - a) * (1.0f - pow(t, dt))));
}

TYvec TyVec3(TYstring hex)
{
	if (hex.size() != 6)
	{
		if (hex.size() == 8) hex = hex.substr(2, 6);
		else hex = hex.substr(hex.size() - 6, 6);
	}

	TYstring hex_r = hex.substr(0, 2);
	TYstring hex_g = hex.substr(2, 2);
	TYstring hex_b = hex.substr(4, 2);

	TYint r = HexToInt(hex_r);
	TYint g = HexToInt(hex_g);
	TYint b = HexToInt(hex_b);

	return TYvec(r / 255.0f, g / 255.0f, b / 255.0f);
}

TYint HexToInt(TYstring hex)
{
	TYint p = 0;
	TYint result = 0;

	std::transform(hex.begin(), hex.end(), hex.begin(), [](TYchar c) { return std::tolower(c); });

	for (TYint i = (TYint)hex.size() - 1; i >= 0; i--)
	{
		TYint v = 0;
		if (hex[i] < 'a')
		{
			v = std::stoi(hex.substr(i, 1));
		}
		else
		{
			switch (hex[i])
			{
			case 'a':
				v = 10;
				break;
			case 'b':
				v = 11;
				break;
			case 'c':
				v = 12;
				break;
			case 'd':
				v = 13;
				break;
			case 'e':
				v = 14;
				break;
			case 'f':
				v = 15;
				break;
			default:
				v = 0;
				break;
			}
		}

		v = v * (TYint)std::powf(16.0f, TYfloat(p));
		p++;

		result += v;
	}
	return result;
}