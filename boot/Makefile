# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2004-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

ifndef CONFIG_SPL_BUILD

# This option is not just y/n - it can have a numeric value
ifdef CONFIG_BOOT_RETRY_TIME
obj-y += bootretry.o
endif

obj-$(CONFIG_CMD_BOOTM) += bootm.o bootm_os.o
obj-$(CONFIG_CMD_BOOTZ) += bootm.o bootm_os.o
obj-$(CONFIG_CMD_BOOTI) += bootm.o bootm_os.o

obj-$(CONFIG_CMD_PXE) += pxe_utils.o
obj-$(CONFIG_CMD_SYSBOOT) += pxe_utils.o

endif

obj-y += image.o image-board.o
obj-$(CONFIG_ANDROID_AB) += android_ab.o
obj-$(CONFIG_ANDROID_BOOT_IMAGE) += image-android.o image-android-dt.o
obj-$(CONFIG_$(SPL_TPL_)OF_LIBFDT) += image-fdt.o
obj-$(CONFIG_$(SPL_TPL_)FIT_SIGNATURE) += fdt_region.o
obj-$(CONFIG_$(SPL_TPL_)FIT) += image-fit.o
obj-$(CONFIG_$(SPL_)MULTI_DTB_FIT) += boot_fit.o common_fit.o
obj-$(CONFIG_$(SPL_TPL_)IMAGE_SIGN_INFO) += image-sig.o
obj-$(CONFIG_$(SPL_TPL_)FIT_SIGNATURE) += image-fit-sig.o
obj-$(CONFIG_$(SPL_TPL_)FIT_CIPHER) += image-cipher.o

obj-$(CONFIG_CMD_ADTIMG) += image-android-dt.o

ifdef CONFIG_SPL_BUILD
obj-$(CONFIG_SPL_LOAD_FIT) += common_fit.o
endif