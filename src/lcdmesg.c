#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <gpiod.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "gpiod-helper.h"
#include "helpers.h"
#include "fpga.h"

#define CONSUMER "lcdmesg"

uint16_t lcd_bias_value;

struct hd44780 {
	struct gpiod_line_request *data;
	struct gpiod_line_request *en;
	struct gpiod_line_request *rs;
	struct gpiod_line_request *wr;
};

static void set_8bit_array(enum gpiod_line_value *val, uint8_t data)
{
	const enum gpiod_line_value values[2] = {
				GPIOD_LINE_VALUE_INACTIVE,
				GPIOD_LINE_VALUE_ACTIVE};

	int i;

	for (i = 0; i < 8; i++) {
		val[i] = values[!!(data & (1 << i))];
	}
}

static void nsleep(long int nsec)
{
	struct timespec target;

	target.tv_sec = 0;
	target.tv_nsec = nsec;

	while (nanosleep(&target, &target) == -1 && errno == EINTR)
		;
}

static void gpiod_line_request_set_helper(struct gpiod_line_request *line,
					  int offs,
					  uint8_t val)
{
	enum gpiod_line_value new_val = (!!val ?
					 GPIOD_LINE_VALUE_ACTIVE :
					 GPIOD_LINE_VALUE_INACTIVE);

	gpiod_line_request_set_value(line, offs, new_val);
}

/* https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 * Sheet 58 */
static void lcd_write(struct hd44780 *lcd, uint8_t rs, uint8_t data)
{
	enum gpiod_line_value val[8];
	set_8bit_array(val, data);

	gpiod_line_request_set_helper(lcd->rs, 21, rs);
	gpiod_line_request_set_helper(lcd->wr, 19, 0);
	gpiod_line_request_set_values(lcd->data, val);
	nsleep(60); /* tAS */
	gpiod_line_request_set_helper(lcd->en, 20, 1);
	nsleep(230); /* PWEH */
	gpiod_line_request_set_helper(lcd->en, 20, 0);
	nsleep(210); /* tH/tAH + tcycE */

	usleep(37);
}

/* Set a contrast (duty cycle) from 0 (off) to 15 (max).
 * This may need to change depending on the LCD used or the altitude */
static void lcd_contrast(uint8_t duty)
{
	fpoke32(0x1c, duty & 0xf);
}

static void lcd_writechars(struct hd44780 *lcd, char *dat)
{
	while(*dat)
		lcd_write(lcd, 1, *dat++);
}

static void lcd_returnhome(struct hd44780 *lcd)
{
	/* Since we cannot poll busy, the write function waits the typical 37us
	 * execution time max.  Clear home must wait 1.52ms, but all other
	 * commands are 37us.
	 */
	lcd_write(lcd, 0, 0x2);
	usleep(1520);
}

static void lcd_init(struct hd44780 *lcd)
{
	int model;
	unsigned int datapins[8] = {10, 9, 12, 11, 16, 15, 18, 17};
	enum gpiod_line_value value_init[8] = {GPIOD_LINE_VALUE_ACTIVE};

	model = get_model();
	if(model != 0x7250){
		fprintf(stderr, "Unsupported model 0x%X\n", model);
		exit(1);
	}

	lcd->data = request_output_lines("/dev/gpiochip2",
					 datapins,
					 value_init,
					 8,
					 CONSUMER);
	lcd->en = request_output_line("/dev/gpiochip2",
				      20,
				      GPIOD_LINE_VALUE_ACTIVE,
				      CONSUMER);
	lcd->rs = request_output_line("/dev/gpiochip2",
				      21,
				      GPIOD_LINE_VALUE_ACTIVE,
				      CONSUMER);
	lcd->wr = request_output_line("/dev/gpiochip2",
				      19,
				      GPIOD_LINE_VALUE_ACTIVE,
				      CONSUMER);

	if (lcd->data == NULL ||
	    lcd->en == NULL ||
	    lcd->rs == NULL ||
	    lcd->wr == NULL) {
		fprintf(stderr, "Unable to open GPIO lines\n");
		exit(1);
	}

	fpga_init(0x50004000);

	/* Recover from any potential state to 8-bit mode, and set:
	 * Function Set
	 * DL = 1 (8-bits)
	 * N = 1 (2 line)
	 * F = 0 (5x8 dots)
	 */
	lcd_write(lcd, 0, 0x38);
	lcd_write(lcd, 0, 0x38);
	lcd_write(lcd, 0, 0x38);

	/*
	 * Entry Mode Set
	 * I/D = 1 (increment)
	 * S = 0 (display shift off)
	 */
	lcd_write(lcd, 0, 0x6);

	/*
	 * Display on/off Control
	 * D = 1 (display on)
	 * C = 0 (cursor off)
	 * B = 0 (cursor blink off)
	 */
	lcd_write(lcd, 0, 0xc);

	/*
	 * Clear Display
	 */
	lcd_write(lcd, 0, 0x1);

	/*
	 * Return home
	 */
	lcd_returnhome(lcd);


	lcd_contrast(lcd_bias_value);
}

int main(int argc, char **argv)
{
	struct hd44780 lcd;
	int i = 0;
	char *contrast = getenv("LCD_CONTRAST");

	/* Contrast can be 0-15.  Default to 12 if not specified. */
	if(contrast) {
		lcd_bias_value = atoi(contrast);
		if(lcd_bias_value > 0xf ) {
			fprintf(stderr, "LCD Contrast must be 0-15");
		}
	} else {
		lcd_bias_value = 12;
	}

	lcd_init(&lcd);
	if (argc == 2) {
		lcd_writechars(&lcd, argv[1]);
		return 0;
	}
	if (argc > 2) {
		lcd_writechars(&lcd, argv[1]);
		/* set DDRAM addr to second row */
		lcd_write(&lcd, 0, 0xa8);
		lcd_writechars(&lcd, argv[2]);
		return 0;
	}
	while(!feof(stdin)) {
		char buf[512];
		if (i) {
			/* XXX: this seek addr may be different for different
			 * LCD sizes!  -JO */
			lcd_write(&lcd, 0, 0xa8);
		} else {
			lcd_returnhome(&lcd);
		}
		i = i ^ 0x1;
		if (fgets(&buf[0], sizeof(buf), stdin) != NULL) {
			unsigned int len;
			buf[0x27] = 0;
			len = strlen(buf);
			if (buf[len - 1] == '\n') buf[len - 1] = 0;
				lcd_writechars(&lcd, buf);
		}
	}
	return 0;
}
