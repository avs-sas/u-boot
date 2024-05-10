// SPDX-License-Identifier: GPL-2.0
/*
 * Based on vendor support provided by AVNET Embedded
 *
 * Copyright (C) 2021 AVNET Embedded, MSC Technologies GmbH
 * Copyright 2021 General Electric Company
 * Copyright 2021 Collabora Ltd.
 */

#include <common.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <i2c.h>
#include <i2c_eeprom.h>
#include <env.h>
#include <errno.h>
#include <malloc.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

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

#if defined(CONFIG_TARGET_MSC_SM2S_IMX8MP_24N0600I_4G)
/* eeprom rev check */
#define BI_VER_MAJ	1
#define BI_VER_MIN	0

#define BI_COMPANY_LEN		3
#define BI_FEATURE_LEN		8
#define BI_SERIAL_LEN		11
#define BI_REVISION_LEN		2

struct bi_head {
	uint8_t magic[4];
	uint8_t version;
	uint8_t v_min;
	uint16_t body_len;
	uint16_t body_off;
	uint16_t body_chksum;
	uint32_t __reserved[2];
} __attribute__ ((__packed__));

struct bi_v1_0 {
	uint32_t __feature_bits;
	char company[BI_COMPANY_LEN + 1];
	char feature[BI_FEATURE_LEN + 1];
	char serial_number[BI_SERIAL_LEN + 1];
	char revision[BI_REVISION_LEN + 1];
	uint32_t __reserved1;
	uint16_t __reserved2;
} __attribute__ ((__packed__));

struct bi_v1_1 {
	uint32_t __feature_bits;
	char company[BI_COMPANY_LEN + 1];
	char feature[BI_FEATURE_LEN + 1];
	char serial_number[BI_SERIAL_LEN + 1];
	char revision[BI_REVISION_LEN + 1];
	uint8_t bsp_specific; /* optionally enabled via BI_HAS_BSP_SPECIFIC, content is up to the BSP */
	uint32_t __reserved1;
	uint8_t __reserved2;
} __attribute__ ((__packed__));

struct board_info {
	struct bi_head head;
	union {
		struct bi_v1_0 v1_0;
		struct bi_v1_1 v1_1;
	} body;
	uint32_t boot_count;
} __attribute__ ((__packed__));

#define BI_COMPANY_BIT		(1<<0)
#define BI_FEATURE_BIT		(1<<1)
#define BI_SERIAL_BIT		(1<<2)
#define BI_REVISION_BIT		(1<<3)
#define BI_BSP_SPECIFIC_BIT	(1<<4)

#define BI_HAS_FEATURE(BI, F) \
	((BI)->body.v1_0.__feature_bits & BI_##F##_BIT)

#define BI_GET_BODY(BI, MAJ, MIN) \
	((BI)->body.v##MAJ##_##MIN)

const char* bi_get_feature(struct board_info *bi)
{
	char *na = "N/a";

	if (BI_HAS_FEATURE(bi, FEATURE))
		return BI_GET_BODY(bi, 1, 0).feature;
	return na;
}

const char* bi_get_serial(struct board_info *bi)
{
	char *na = "N/a";

	if (BI_HAS_FEATURE(bi, SERIAL))
		return BI_GET_BODY(bi, 1, 0).serial_number;
	return na;
}

const char* bi_get_revision(struct board_info *bi)
{
	char *na = "N/a";

	if (BI_HAS_FEATURE(bi, REVISION))
		return BI_GET_BODY(bi, 1, 0).revision;
	return na;
}

const char* bi_get_ram_size(struct board_info *bi)
{
	char *ft = "N/a";

	if (BI_HAS_FEATURE(bi, FEATURE))
		ft = BI_GET_BODY(bi, 1, 0).feature;

	if (strcmp(ft, "03N0700I") == 0) return "8GB";
	if (strcmp(ft, "24N0600I") == 0) return "4GB";
	if (strcmp(ft, "14N0700I") == 0) return "2GB";
	if (strcmp(ft, "14N0740I") == 0) return "2GB";
	if (strcmp(ft, "03N0E10I") == 0) return "1GB";
	if (strcmp(ft, "14N0600E") == 0) return "2GB";
	if (strcmp(ft, "14N0E00I") == 0) return "2GB";
	if (strcmp(ft, "15N0700E") == 0) return "2GB";
	if (strcmp(ft, "24N0E10I") == 0) return "4GB";
	if (strcmp(ft, "14N0741I") == 0) return "2GB";
	if (strcmp(ft, "25N0600I") == 0) return "4GB";
	if (strcmp(ft, "28N0700I") == 0) return "4GB";
	if (strcmp(ft, "26N2700I") == 0) return "4GB";

	return ft; // Return "Unknown" if no matching revision is found
}

const char* bi_get_emmc_size(struct board_info *bi)
{
	char *ft = "N/a";

	if (BI_HAS_FEATURE(bi, FEATURE))
		ft = BI_GET_BODY(bi, 1, 0).feature;

	if (strcmp(ft, "03N0700I") == 0) return "8GB";
	if (strcmp(ft, "24N0600I") == 0) return "16GB";
	if (strcmp(ft, "14N0700I") == 0) return "16GB";
	if (strcmp(ft, "14N0740I") == 0) return "16GB";
	if (strcmp(ft, "03N0E10I") == 0) return "8GB";
	if (strcmp(ft, "14N0600E") == 0) return "16GB";
	if (strcmp(ft, "14N0E00I") == 0) return "16GB";
	if (strcmp(ft, "15N0700E") == 0) return "32GB";
	if (strcmp(ft, "24N0E10I") == 0) return "16GB";
	if (strcmp(ft, "14N0741I") == 0) return "16GB";
	if (strcmp(ft, "25N0600I") == 0) return "32GB";
	if (strcmp(ft, "28N0700I") == 0) return "256GB";
	if (strcmp(ft, "26N2700I") == 0) return "64GB";

	return ft; // Return "Unknown" if no matching revision is found
}

static void bi_set_msc_board_var(struct board_info *bi)
{
	if (bi == NULL) return;

	env_set("msc_reference", bi_get_feature(bi));
	env_set("msc_serial", bi_get_serial(bi));
	env_set("msc_revision", bi_get_revision(bi));
	env_set("msc_ram_size", bi_get_ram_size(bi));
	env_set("msc_emmc_size", bi_get_emmc_size(bi));
}

static void bi_print(struct board_info *bi)
{
	if (bi == NULL) return;

	printf("------------------------------\n");
	printf("feature .......... %s\n", bi_get_feature(bi));
	printf("serial ........... %s\n", bi_get_serial(bi));
	printf("revision (MES) ... %s\n", bi_get_revision(bi));
	printf("ram size.......... %s\n", bi_get_ram_size(bi));
	printf("eMMC size......... %s\n", bi_get_emmc_size(bi));
	printf("------------------------------\n\n");
}


int board_late_init(void)
{
	const char *path = "id-eeprom";
	struct udevice *dev;
	struct board_info *info;
	int ret, off;

	printf("\nMSC SM2S: Checking board info\n");

	off = fdt_path_offset(gd->fdt_blob, path);
	if (off < 0) {
		pr_err("%s: fdt_path_offset() failed: %d\n", __func__, off);
		return off;
	}

	ret = uclass_get_device_by_of_offset(UCLASS_I2C_EEPROM, off, &dev);
	if (ret) {
		pr_err("%s: uclass_get_device_by_of_offset() failed: %d\n",
		       __func__, ret);
		return ret;
	}

	info = malloc(sizeof(struct board_info));
	if (!info)
		return -ENOMEM;

	ret = i2c_eeprom_read(dev, 0, (uint8_t *)info,
			      sizeof(struct board_info));
	if (ret) {
		printf("%s: i2c_eeprom_read() failed: %d\n", __func__, ret);
		free(info);
		return ret;
	}

	bi_print(info);

#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	bi_set_msc_board_var(info);
	env_set("board_name", "SM2S");
	env_set("board_rev", "iMX8MP 24N0600I 4G");
#endif
	return 0;
}
#endif