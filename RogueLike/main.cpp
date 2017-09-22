#include "common.h"
#include "Engine.h"

int main(int argc, char *argv[])
{
	printf("hello");

	Engine engine;

	engine.initialize();

	while (engine.update());

	engine.cleanup();

	return 0;
}