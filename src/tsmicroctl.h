#ifndef __TSMICROCTL_H__
#define __TSMICROCTL_H__

#define	I2C_ADR			0x54
#define	I2C_BUS			"/dev/i2c-0"

#define	REG_START		0
#define	REG_LEN			28

#define	ADC_START		0
#define	ADC_LEN			22
#define	ADC_WIDTH		16

#define	ADC_5V_16		0
#define	ADC_SILO_CHRG_V_16	2
#define	ADC_3V3_16		4
#define	ADC_VIN_16		6
/* 16-bit regs 8 - 12 unused on TS-7100 */
#define	ADC_SILO_V_16		14
#define	ADC_SILO_TOT_V_16	16
/* 16-bit reg 18 unused on TS-7100 */
#define	ADC_TEMP_16		20

#define	CTRL_REG		22
#define	CTRL_PWR_FAIL		(1 << 0) // External power has failed
#define	CTRL_SILO_EN		(1 << 1) // Cap charging enabled
#define	CTRL_SILO_CHARGED	(1 << 2) // Caps at min charge thresh.
#define	CTRL_SILO_CHARGING	(1 << 3) // Caps actively charging
#define	CTRL_USB_CONN		(1 << 4) // USB currently connected
#define	CTRL_WDT_EN		(1 << 6) // WDT armed and counting

#define	FLAG_REG		23
#define	FLAG_DEF_SILO_EN	(1 << 0) // When set, auto-charge SILO at pwron

#define	SILO_DEF_CHRG_CUR_16	24
#define	SILO_CHRG_CUR_16	26

#define	WDT_TIMEOUT		1024
#define	WDT_LEN			4
#define	WDT_CTRL		1028
#define	WDT_CTRL_FEED		(1 << 0) // Feed for TIMEOUT length
#define	WDT_CTRL_REBOOTED	(1 << 7) // Set if last uC reboot caused by WDT

#define	REV_REG			0xFFFF

#endif
