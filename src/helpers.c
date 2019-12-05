#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int get_model(void)
{
	static int ret;

	if (!ret) {
		FILE *proc;
		char model[256];

		proc = fopen("/sys/firmware/devicetree/base/model", "r");
		if (!proc) {
			perror("model");
			exit(1);
		}
		fread(model, 256, 1, proc);

		if (strstr(model, "TS-7100"))
			ret = 0x7100;
		else if (strstr(model, "TS-7250-V3"))
			ret = 0x7250;
		else if (strstr(model, "TS-7120"))
			ret = 0x7120;
		fclose(proc);
	}

	return ret;
}
