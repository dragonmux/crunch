#include <pthread.h>
#include "Logger.h"

const static int ok;

void *testRunner(void *_)
{
	logResult(RESULT_SUCCESS, "\n");
	return &ok;
}

int main(int argc, char **argv)
{
	return 0;
}
