#
# Copyright (C) 2021 AVNET Embedded, MSC Technologies GmbH
#
# SPDX-License-Identifier:      GPL-2.0
#

obj-y += sm2s_imx8mp.o boardinfo.o

ifdef CONFIG_XPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_mt53d512m32d2ds_2gib_2chn_2cs_timing.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_mt53d1024m32d4dt_4gib_2chn_2cs_timing.o
endif
