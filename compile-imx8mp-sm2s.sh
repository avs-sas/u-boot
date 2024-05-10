export ARCH=arm64
export DTC_FLAGS="-@"
export PATH=/opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/:$PATH
export CROSS_COMPILE=aarch64-none-linux-gnu-

make msc_sm2s_imx8mp_24N0600I_4G_defconfig
#make msc_sm2s_imx8mp_defconfig
#make imx8mp_evk_defconfig

make -j20
