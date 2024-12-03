#include "Level.h"

int main()
{
	Level* lv = Level::GetPtr();

	if (lv->Initialize() != 0) //Error at init
	{
		lv->DeletePtr();
		return -1;
	}

	lv->Run();

	lv->Cleanup();

	return 0;
}