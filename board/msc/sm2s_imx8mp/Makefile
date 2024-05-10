#
# Copyright (C) 2021 AVNET Embedded, MSC Technologies GmbH
#
# SPDX-License-Identifier:      GPL-2.0
#

ifdef CONFIG_TARGET_MSC_SM2S_IMX8MP
ifdef CONFIG_SPL_BUILD
obj-y += spl.o lpddr4_timing.o
else
obj-y += sm2s_imx8mp.o
endif
endif

ifdef CONFIG_TARGET_MSC_SM2S_IMX8MP_24N0600I_4G
obj-y += sm2s_imx8mp.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_msc_sm2s_24N0600I_4G.o
endif
endif