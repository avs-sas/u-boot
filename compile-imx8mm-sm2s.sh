# export ARCH=arm64
# export DTC_FLAGS="-@"
export PATH=/opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/:$PATH
export CROSS_COMPILE=aarch64-none-linux-gnu-
export ATF_LOAD_ADDR=0x920000


# source /opt/fsl-imx-xwayland/6.6-nanbield/environment-setup-armv8a-poky-linux

#make imx8mm_evk_defconfig
make msc_sm2s_imx8mm_defconfig
make -j20
