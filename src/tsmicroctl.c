/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef CTL
#include <getopt.h>
#endif

#include "i2c-dev.h"
#include "tsmicroctl.h"
#include "microcontroller.h"

const char copyright[] = "Copyright (c) Technologic Systems - " __DATE__ " - "
  GITCOMMIT;

enum status {
	enable = 1,
	disable,
};

#ifdef CTL
int model = 0;

int get_model()
{
	FILE *proc;
	char mdl[256];
	char *ptr;

	proc = fopen("/proc/device-tree/model", "r");
	if (!proc) {
	    perror("model");
	    return 0;
	}
	fread(mdl, 256, 1, proc);
	ptr = strstr(mdl, "TS-");
	return strtoull(ptr+3, NULL, 16);
}
#endif

#ifdef CTL

/* Enable/disable active silo charging
 * When opt_silo == 1/enable, charge.
 * When opt_silo == 2/disable, disable charging
 */
void set_silo(int opt_silo)
{
	uint8_t buf;

	buf = silab_peek8(CTRL_REG);

	if (opt_silo == enable) buf |= (CTRL_SILO_EN);
	if (opt_silo == disable) buf &= ~(CTRL_SILO_EN);

	silab_poke8(CTRL_REG, buf);
}

/* NOTE: These numbers may change when porting this application to other units!
 *
 * Calculate current charge pct based on the current total mV charge voltage
 *
 * Sustainable voltage is anything above ~3430 mV on SILO_TOT. Max SILO charge
 * averages around ~4600 mV, the uC is designed to keep the SILO charge 300 mV
 * below the 5 V rail. The pct is 0-100, where 1% starts at 3430 mV. This means
 * any non-zero charge can sustain the SBC for some period of time. The
 * difference between min and max charge is 1170 mV, multiply up and divide out
 * this difference to get a 0-100 scale.
 */
uint16_t calc_silo_pct(uint16_t silo_tot_mv)
{
	int16_t pct;

	pct = ((int32_t)((silo_tot_mv - 3430) * 10) / 117);
	if (pct > 100) pct = 100;
	if (pct < 0) pct = 0;

	return (uint16_t)pct;
}


void print_info()
{
	int i;
	uint8_t buf_8[REG_LEN], build_str[80];
	uint32_t wdt_ms;
	uint16_t *buf_16 = (uint16_t *)buf_8;

	/* Human and bash readable names for each analog channel that tsmicroctl
	 * reports on. If any of the names are left blank, they are skipped in
	 * printing; otherwise the named channel will be printed, followed by
	 * its value. Not every channel is used on every SBC.
	 */
	const char *an_names[(ADC_LEN/2)] = {
	  "MV5", "MV_SILO_CHARGE", "MV3P3", "MVIN", "INIT_TEMP", "", "",
	  "MV_SILO", "MV_SILO_TOT", "", "CUR_TEMP"};

	/* XXX: Check return val! */
	silab_peekstream8(BUILD_STR_START, build_str, BUILD_STR_LEN);
	silab_peekstream8(REG_START, buf_8, REG_LEN);

	/* Convert 8bit vals to 16bit
	 *
	 * NOTES:
	 * All curent uC implementations put the MSB of the 16-bit value
	 * in the lower I2C register. This may not be true on all
	 * implementations.
	 *
	 * This also assumes that ADC_START to ADC_LEN is between REG_START and
	 * REG_LEN, inclusive.
	 *
	 * Use caution when porting.
	 */
	for (i = ADC_START; i < ((ADC_LEN/2) + ADC_START); i++) {
		buf_16[i] =
		  ((uint16_t)(buf_8[(i << 1)] << 8) | buf_8[((i << 1)|1)]);
	}

	printf("REVISION=0x%x\n", silab_peek8(REV_REG));
	printf("BUILD_STR=\'%s\'\n", build_str);

	/* Print analog values, names are set above */
	for (i = ADC_START; i < ((ADC_LEN/2) + ADC_START); i++) {
		if (*an_names[i]) {
			printf("%s=%d\n", an_names[i], buf_16[i]);
		}
	}

	printf("USB_CONN_CONNECTED=%d\n", !!(buf_8[CTRL_REG] & CTRL_USB_CONN));

	/* The statement ordering is important! Need to check in order
	 * of: discharging, charged, charging, disabled. Disabled and dischar.
	 * are not mutually exclusive, nor are charged and charging. We only
	 * care about about them in this order listed above.
	 */
	printf("SILO=");
	if	((buf_8[CTRL_REG]) & CTRL_PWR_FAIL) printf("discharging\n");
	else if	((buf_8[CTRL_REG]) & CTRL_SILO_CHARGED) printf("charged\n");
	else if	((buf_8[CTRL_REG]) & CTRL_SILO_CHARGING) printf("charging\n");
	else printf("disabled\n");

	printf("SILO_CHARGE_PCT=%d\n", 
	  calc_silo_pct(buf_16[(ADC_SILO_TOT_V_16/2)]));

	/* The default state of the TS-SILO caps at power on. When enabled, the
	 * caps are by default charged at the default charge current.
	 */
	printf("SILO_DEF=");
	if (buf_8[FLAG_REG] & FLAG_DEF_SILO_EN) printf("enabled\n");
	else printf("disabled\n");

	/* The active charge current of the caps can be modified after boot,
	 * it is represented in mA.
	 */
	printf("SILO_CHARGE_CUR=%d\n",
	  ((uint16_t)(buf_8[SILO_CHRG_CUR_16] << 8) |
	  buf_8[(SILO_CHRG_CUR_16 + 1)]));

	/* The default charge current of the caps at boot, can be modified to
	 * change future boots. Represented in mA.
	 */
	printf("SILO_DEF_CHARGE_CUR=%d\n",
	  ((uint16_t)(buf_8[SILO_DEF_CHRG_CUR_16] << 8) |
	  buf_8[(SILO_DEF_CHRG_CUR_16 + 1)]));

	printf("WDT_STATUS=%s\n", (buf_8[CTRL_REG] & CTRL_WDT_EN) ?
	  "armed" : "disabled");

	/* WDT has another set of regs associated with it. Clobber existing
	 * buf_8 values, no longer needed. Timer value is stored in uC with
	 * LSB at the lowest register address.
	 *
	 * NOTE:
	 * Most instances of this uC layout should be using 4, 8-bit regs for
	 * a 32-bit value represented in ms. This code will readily accommodate
	 * less, but will require changes to support more than 32-bit values.
	 */
	/* XXX: Check return val! */
	silab_peekstream8(WDT_TIMEOUT, buf_8, WDT_LEN);
	wdt_ms = 0;
	for (i = 0; i < WDT_LEN; i++) {
		wdt_ms |= (uint32_t)(buf_8[i] << (8 * i));
	}
	wdt_ms *= 10;
	printf("WDT_TIMEOUT_LEN=%d\n", wdt_ms);
	printf("REBOOT_SOURCE=%s\n",
	  (silab_peek8(WDT_CTRL) & WDT_CTRL_REBOOTED) ?
	  "WDT" : "poweron");
}

static void usage(char **argv) {
	fprintf(stderr,
	  "%s\n\n"
	  "Usage: %s [OPTION] ...\n"
	  "Technologic Systems Microcontroller Access\n"
	  "\n"
	  " General Options:\n"
	  "  -i, --info               Get info about the microcontroller\n"
	  "  -h, --help               This message\n\n"

	  " TS-SILO Supercapacitor Options:\n"
	  "  -e, --tssiloon           Enable charging of TS-SILO supercaps\n"
	  "  -d, --tssilooff          Disable charging of TS-SILO supercaps\n"
	  "  -c, --tssilocur=<mA>     Set TS-SILO charge current in mA\n"
	  "  -E, --def-tssiloon       Enable auto TS-SILO charging at poweron\n"
	  "  -D, --def-tssilooff      Disable auto TS-SILO charging at poweron\n"
	  "  -C, --def-tssilocur=<mA> Set TS-SILO auto charge current in mA\n"
	  "  -w, --tssilowait=<pct>   Block until TS-SILO is <pct> charged\n"
	  "\n",
	  copyright, argv[0]
	);
}

int main(int argc, char **argv)
{
	int c;
	int opt_silo = 0, opt_def_silo = 0;
	int opt_silo_cur = 0, opt_def_silo_cur = 0;
	int opt_silo_wait = 0;

	static struct option long_options[] = {
	  { "info",		no_argument, 		0, 'i' },
	  { "help",		no_argument,		0, 'h' },
	  { "tssiloon",		no_argument,		0, 'e' },
	  { "tssilooff",	no_argument,		0, 'd' },
	  { "tssilocur",	required_argument,	0, 'c' },
	  { "def-tssiloon",	no_argument,		0, 'E' },
	  { "def-tssilooff",	no_argument,		0, 'D' },
	  { "def-tssilocur",	required_argument,	0, 'C' },
	  { "tssilowait",	required_argument,	0, 'w' },
	  { 0, 0, 0, 0 }
	};

	if(argc == 1) {
		usage(argv);
		return(1);
	}

	model = get_model();
	switch(model) {
	  case 0x7100:
		break;
	  default:
		fprintf(stderr, "Unsupported model TS-%x\n", model);
		return 1;
	}

	if (silab_init(I2C_BUS, I2C_ADR) == -1) {
		perror("Unable to open I2C bus");
		return 1;
	}

	while((c = getopt_long(argc, argv, 
	  "ihedc:EDC:w:",
	  long_options, NULL)) != -1) {
		switch (c) {
		  case 'i': /* Info */
			print_info();
			break;
		  case 'e': /* SILO charge enable */
			opt_silo = enable;
			break;
		  case 'd': /* SILO charge disable */
			opt_silo = disable;
			break;
		  case 'c': /* SILO set charge current */
			opt_silo_cur = strtoul(optarg, NULL, 0);
			break;
		  case 'E': /* SILO set default charge enable */
			opt_def_silo = enable;
			break;
		  case 'D': /* SILO set default charge disable */
			opt_def_silo = disable;
			break;
		  case 'C': /* SILO set default charge current */
			opt_def_silo_cur = strtoul(optarg, NULL, 0);
			break;
		  case 'w': /* SILO charge enable and wait till pct charged */
			opt_silo_wait = strtoul(optarg, NULL, 0);
			if (opt_silo_wait > 100) opt_silo_wait = 100;
			if (opt_silo_wait < 0) opt_silo_wait = 0;
			break;
		  case 'h':
		  default:
			usage(argv);
			return 1;
		}
	}

	/************************
	 * Active TS-SILO Settings
	 ************************/
	if(opt_silo) {
		set_silo(opt_silo);
	}

	if (opt_silo_cur) {
		silab_poke16(SILO_CHRG_CUR_16, opt_silo_cur);
	}

	/************************
	 * Default TS-SILO Settings
	 ************************/
	if (opt_def_silo) {
		uint8_t buf;

		buf = silab_peek8(FLAG_REG);

		if (opt_def_silo == enable) buf |= (FLAG_DEF_SILO_EN);
		if (opt_def_silo == disable) buf &= ~(FLAG_DEF_SILO_EN);

		silab_poke8(FLAG_REG, buf);
	}

	if (opt_def_silo_cur) {
		silab_poke16(SILO_DEF_CHRG_CUR_16, opt_def_silo_cur);
	}

	/***************************
	 * Wait until TS-SILO charge
	 ***************************/
	if (opt_silo_wait) {
		/* Enable charging if not already on */
		set_silo(enable);

		while (calc_silo_pct(silab_peek16(ADC_SILO_TOT_V_16)) <
		  opt_silo_wait) {
			usleep(100000);
		}
	}


	
	return 0;
}

#endif
