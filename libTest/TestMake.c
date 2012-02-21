#include "ArgsParser.h"

arg args[] =
{
	{"-l", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-o", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{0}
};

int main(int argc, char **argv)
{
	return 0;
}
