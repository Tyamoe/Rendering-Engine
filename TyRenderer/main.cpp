#include "Engine.h"

static TYint SignMask(TYfloat comp)
{
	return comp > TYfloat{ 0 };
}

static TYint SignMask(TYvec pos)
{
	return SignMask(pos.z) | (SignMask(pos.y) << 1) | (SignMask(pos.x) << 2);
}

int main()
{
	/*TYvec v = {};
	TYint i = -1;

	v = TYvec(-1.0f, -1.0f, -1.0f); // Back Bottom Left
	i = SignMask(v);
	TYlog << "Back Bottom Left " << i << "\n";

	v = TYvec(1.0f, 1.0f, 1.0f); // Front Top Right
	i = SignMask(v);
	TYlog << "Front Top Right " << i << "\n";

	v = TYvec(-1.0f, -1.0f, 1.0f); // Front Bottom Left
	i = SignMask(v);
	TYlog << "Front Bottom Left1 " << i << "\n";

	v = TYvec(1.0f, 1.0f, -1.0f); // Back Top Right
	i = SignMask(v);
	TYlog << "Back Top Right6 " << i << "\n";*/

	Engine* engine = new Engine();
	engine->Start();

	delete engine;
}