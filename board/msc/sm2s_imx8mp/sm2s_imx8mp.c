// SPDX-License-Identifier: GPL-2.0
/*
 * Based on vendor support provided by AVNET Embedded
 *
 * Copyright (C) 2021 AVNET Embedded, MSC Technologies GmbH
 * Copyright 2021 General Electric Company
 * Copyright 2021 Collabora Ltd.
 */

#include <common.h>
#include <errno.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <linux/delay.h>
#include <handoff.h>
#include <bloblist.h>
#include "../common/i2c_eeprom.h"
#include "../common/boardinfo.h"
#include "../common/mx8m_common.h"

DECLARE_GLOBAL_DATA_PTR;

const board_info_t *binfo = NULL;

int board_phys_sdram_size(phys_size_t *size)
{
	struct spl_handoff *ho;

	ho = bloblist_find(BLOBLISTT_U_BOOT_SPL_HANDOFF, sizeof(*ho));
	if (!ho) {
		*size = PHYS_SDRAM_SIZE;
		return 0;
	}

	*size = ho->ram_size;

	return 0;
}

static void setup_fec(void)
{
	struct iomuxc_gpr_base_regs *gpr =
		(struct iomuxc_gpr_base_regs *)IOMUXC_GPR_BASE_ADDR;

	/* Enable RGMII TX clk output */
	setbits_le32(&gpr->gpr[1], BIT(22));
}

int board_phy_config(struct phy_device *phydev)
{
	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}

int board_init(void)
{
	setup_fec();

	return 0;
}

#define ENV_FDTFILE_MAX_SIZE 64

#if !defined(CONFIG_SPL_BUILD)
int board_late_init(void)
{
	char buff[ENV_FDTFILE_MAX_SIZE];
	char *fdtfile;

	if (!binfo)
		return 0;

	fdtfile = env_get("fdt_file");
	if (fdtfile)
		return 0;

	snprintf(buff, ENV_FDTFILE_MAX_SIZE, "%s-%s-%s-%s-%s.dtb",
			bi_get_company(binfo), bi_get_form_factor(binfo),
			bi_get_platform(binfo), bi_get_processor(binfo),
			bi_get_feature(binfo));
	env_set("fdt_file", buff);

#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	env_set("board_name", "SM2S");
	env_set("board_rev", "iMX8MP");
#endif

	return 0;
}
#endif /* !defined(CONFIG_SPL_BUILD) */
