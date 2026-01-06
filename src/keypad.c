#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

#include "gpiod-helper.h"
#include "helpers.h"

const char *key_label[16] = {
	"1", "2", "3", "UP",
	"4", "5", "6", "DOWN",
	"7", "8", "9", "2ND",
	"CLEAR", "0", "HELP", "ENTER",
};

void set_4bit_array(enum gpiod_line_value *val, uint8_t data)
{
	const enum gpiod_line_value values[2] = {GPIOD_LINE_VALUE_INACTIVE,
						 GPIOD_LINE_VALUE_ACTIVE};
	int i;

	for (i = 0; i < 4; i++) {
		val[i] = values[!!(data & (1 << i))];
	}
}

void scan_keypad(struct gpiod_line_request *dout,
		 struct gpiod_line_request *din,
		 uint8_t *keys)
{
	int key;
	enum gpiod_line_value lines[4] = {GPIOD_LINE_VALUE_INACTIVE};
	int r;
	uint8_t row, col;
	memset(keys, 0, 16);

	for (row = 0; row < 4; row++) {
		set_4bit_array(lines, ~(1 << row));
		r = gpiod_line_request_set_values(dout, lines);
		assert (!r);
		r = gpiod_line_request_get_values(din, lines);
		assert (!r);
		for (col = 0; col < 4; col++) {
			key = (row * 4) + col;
			if (!lines[col]) {
				keys[key] = 1;
			}
		}
	}
}

void debounce_keypad(uint8_t *keys, uint8_t *debounced)
{
	struct timeval exptime, now, maxtime;
	static struct timeval db[16];
	int i, ret;
	memset(debounced, 0, 16);

	/* Require minimum press of 50ms. */
	exptime.tv_sec = 0;
	exptime.tv_usec = 1000 * 50;

	ret = gettimeofday(&now, NULL);
	assert(!ret);
	for (i = 0; i < 16; i++) {
		if (keys[i]){
			if (!timerisset(&db[i])) {
				db[i] = now;
				continue;
			} else {
				timeradd(&db[i], &exptime, &maxtime);
				/* Debounce until exptime */
				if(timercmp(&maxtime, &now, >))
					continue;
				debounced[i] = 1;
			}
		}
		timerclear(&db[i]);
	}
}

int main()
{
	int i;
	unsigned int out_pins[4] = {1, 2, 3, 4};
	unsigned int in_pins[4] = {6, 7, 8, 9};
	struct gpiod_line_request *dout;
	struct gpiod_line_request *din;
	enum gpiod_line_value value_init[4] = {GPIOD_LINE_VALUE_ACTIVE};
	uint8_t keys[16] = {0};
	uint8_t debounced[16] = {0};
	uint8_t oldstate[16] = {0};

	if(get_model() != 0x7250) {
		fprintf(stderr, "This is only supported on the TS-7250-V3\n");
		return 1;
	}

	dout = request_output_lines("/dev/gpiochip5",
				    out_pins,
				    value_init,
				    4,
				    "keypad rows");

	din = request_input_lines("/dev/gpiochip5",
				   in_pins,
				   4,
				   "keypad cols");
				    
	while(1) {
		scan_keypad(dout, din, keys);
		debounce_keypad(keys, debounced);
		for (i = 0; i < 16; i++) {
			if(keys[i] && oldstate[i])
				continue;
			if(keys[i] != oldstate[i]) {
				if(debounced[i]) {
					printf("%s\n", key_label[i]);
					oldstate[i] = 1;
				}
			}
			if(!keys[i]) {
				oldstate[i] = 0;
			}
		}
		/* Poll at ~100hz */
		usleep(10000);
	}

	return 0;
}
