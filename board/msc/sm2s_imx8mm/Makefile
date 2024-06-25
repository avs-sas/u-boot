#
# Copyright (C) 2019 AVNET Integrated, MSC Technologies GmbH
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += sm2s_imx8mm.o boardinfo.o

ifdef CONFIG_XPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_mt53d512m32d2ds_2gib_2chn_2cs_dv1_timing.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_mt53b512m32d2np_2gib_2chn_2cs_dv1_timing.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_nt6an512t32avj2i_2gib_2chn_1cs_timing.o
endif
