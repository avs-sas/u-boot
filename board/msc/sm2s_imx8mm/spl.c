// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019, 2021 NXP
 */

#include <common.h>
#include <cpu_func.h>
#include <hang.h>
#include <init.h>
#include <i2c.h>
#include <log.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/arch/ddr.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/sections.h>

#include <fsl_esdhc_imx.h>
#include <linux/delay.h>
#include <mmc.h>

#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>

#include "../common/i2c_eeprom.h"
#include "../common/boardinfo.h"
#include "ddr_timings.h"

DECLARE_GLOBAL_DATA_PTR;

struct variant_record {
	const char *rev;
	const char *feature;
	long long unsigned int dram_size;
	struct dram_timing_info *dram_timing;
};

static const struct variant_record variants[] = {
	{
		.rev		= "E0",
		.feature	= "13N4200I",
		.dram_size	= SZ_2G,
		.dram_timing	= &lpddr4_nt6an512t32avj2i_2gib_2chn_1cs_timing,
	}, {
		.rev		= "C0",
		.feature	= "13N4200I",
		.dram_size	= SZ_2G,
		.dram_timing	= &lpddr4_mt53b512m32d2np_2gib_2chn_2cs_dv1_timing,
	}, {
		NULL, NULL, 0, NULL
	},
};

#define DEFAULT_VARIANT_IDX	0

static const struct variant_record *get_variant(const char *feature,
						const char *revision)
{
	const struct variant_record *ptr;

	for (ptr = variants; ptr->feature; ptr++) {
		if (strlen(ptr->feature) != strlen(feature) ||
		    strlen(ptr->rev) != strlen(revision))
			continue;
		if (strcmp(ptr->feature, feature) ||
		    strcmp(ptr->rev, revision))
			continue;
		return ptr;
	}

	pr_warn("Warning: using default variant settings!\n");
	return &variants[DEFAULT_VARIANT_IDX];
}

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
	switch (boot_dev_spl) {
	case USB_BOOT:
		return BOOT_DEVICE_BOARD;
	case SD2_BOOT:
	case MMC2_BOOT:
		return BOOT_DEVICE_MMC2;
	case SD1_BOOT:
	case MMC1_BOOT:
		return BOOT_DEVICE_MMC1;
	case QSPI_BOOT:
		return BOOT_DEVICE_NOR;
	case NAND_BOOT:
		return BOOT_DEVICE_NAND;
	default:
		return BOOT_DEVICE_NONE;
	}
}

static void avnet_spl_dram_init(board_info_t *binfo)
{
	const struct variant_record *variant;

	variant = get_variant(bi_get_feature(binfo),
			      bi_get_revision(binfo));

	gd->ram_size = variant->dram_size;
	ddr_init(variant->dram_timing);
}

static void spl_dram_init(void)
{
	board_info_t *binfo;

	binfo = bi_init();
	if (binfo == NULL) {
		printf("Warning: failed to initialize boardinfo!\n");
	}
	else {
		bi_inc_boot_count(binfo);
		bi_print(binfo);
	}

	/* DDR initialization */
	avnet_spl_dram_init(binfo);
}

void spl_board_init(void)
{
	arch_misc_init();
}

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	debug("%s: %s\n", __func__, name);

	return 0;
}
#endif

void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

	arch_cpu_init();

	init_uart_clk(0);

	timer_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	ret = uclass_get_device_by_name(UCLASS_CLK,
					"clock-controller@30380000",
					&dev);
	if (ret < 0) {
		printf("Failed to find clock node. Check device tree\n");
		hang();
	}

	preloader_console_init();

	enable_tzc380();

	/* DDR initialization */
	spl_dram_init();

	board_init_r(NULL, 0);
}