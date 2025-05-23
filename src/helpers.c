#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <errno.h>

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

int chosen_read_u32(const char *name, uint32_t *value)
{
	char path[256];
	FILE *fp;
	uint32_t be_val;
	size_t n;

	snprintf(path, sizeof(path), "/sys/firmware/devicetree/base/chosen/%s", name);

	fp = fopen(path, "rb");
	if (!fp)
		return -1;

	n = fread(&be_val, 1, sizeof(be_val), fp);
	fclose(fp);

	if (n != sizeof(be_val))
		return -1;

	*value = be32toh(be_val);
	return 0;
}
