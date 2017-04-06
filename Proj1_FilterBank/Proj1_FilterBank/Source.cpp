#define WIN32
#include <Fl/Fl.h>
#include "FilterBank.h"

int main(int argc, char** args)
{
	FilterBank filterBank(SIZE_WINDOW_W, SIZE_WINDOW_H, "Filter Bank");
	filterBank.show();

	Fl::run();

	return 0;
}