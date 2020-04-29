#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_operation.h>
#include <render.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/* digitpic <bmp_file> */
int main(int argc, char **argv)
{
	DebugInit();
	InitDebugChanel();

	DisplayInit();
	SelectAndInitDefaultDispDev("fb");

	AllocVideoMem(5);

	InputInit();
	AllInputDevicesInit();

	PagesInit();

	Page("main")->Run();
		
	return 0;
}

