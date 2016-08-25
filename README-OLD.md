zynq-linux
================================================================================

A package for constructing linux system on Xilinx Zynq

Copyright (C) 2015, Shinya Takamaeda-Yamazaki

E-mail: shinya\_at\_is.naist.jp


License
================================================================================

Apache License 2.0
(http://www.apache.org/licenses/LICENSE-2.0)


Contents
================================================================================

- config: linux kernel configuration and dts configuration files
- setting: configuration file for linux system
- lib: user-level library
- sample: sample code of lib
- compiled_file: compiled kernal image (uImage), dtb (zynq-*.dtb), and CMA memory driver (udmabuf.ko)


Requirements
================================================================================

- Vivado (2014.4)

- U-boot (v2014.4)

```
git clone git://git.xilinx.com/u-boot-xlnx.git && cd u-boot-xlnx
git checkout -b xilinx-v2014.4 xilinx-v2014.4
```

- analogdevicesinc/linux (for 2014.4)

```
git clone https://github.com/analogdevicesinc/linux.git && cd linux 
git checkout xcomm_zynq
git checkout 3590cfe88b1a2d418d59e0ed9c928eb8c094c7cf
make zynq_xcomm_adv7511_defconfig
```

- shtaxxx/udmabuf (User-level contigunous physical memory space (CMA) allocator)
     - This is a customized version of @ikwzm's useful library. Thank you very much.

```
git clone https://github.com/shtaxxx/udmabuf.git
```


How to build a linux system on Zynq
================================================================================

Bitstream synthesis on Vivado
----------------------------------------

### Making an IP-core

Create an AXI4 IP-core by HDL or high level synthesis tools.

If you want to develop a high performance IP-core with Python, please use [PyCoRAM](https://github.com/PyHDI/PyCoRAM.git), and refer to [Zynq+PyCoRAM(+Debian) Introduction (In Japanese)](http://www.slideshare.net/shtaxxx/zynqpycoram).

### Open a new project and synthesize it

Create and use an example design. Go "Open Example Project" -> "Embedded Design". Then select "ZedBoard".

After the bitstream synthesis, go "File" -> "Export Harware".

Check "including bit stream" option and create a SDK project.

Then go "File" -> "Launch SDK".


Boot loader (U-boot)
----------------------------------------

### Setup cross compilation environment

for bash / zsh

```
source /opt/xilinx/Vivado/2014.4/setting64.sh
export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
export ARCH=arm
```

for tcsh

```
source /opt/xilinx/Vivado/2014.4/setting64.csh
setenv CROSS_COMPILE arm-xilinx-linux-gnueabi-
setenv ARCH arm
```

### Build U-boot 

Download U-Boot and checkout xilinx-v2014.4.

```
git clone git://git.xilinx.com/u-boot-xlnx.git; cd u-boot-xlnx 
git checkout -b xilinx-v2014.4 xilinx-v2014.4
```

Edit zynq_common.h ('+' lines will be added and '-' lines will be deleted.)

Or, replace the file with "zynq-linux/config/zynq-common.h".

```
emacs include/configs/zynq-common.h
```

```
diff --git a/include/configs/zynq-common.h b/include/configs/zynq-common.h
index db74f14..b3f821f 100644
--- a/include/configs/zynq-common.h
+++ b/include/configs/zynq-common.h
@@ -239,11 +239,11 @@
 #define CONFIG_EXTRA_ENV_SETTINGS      \
        "ethaddr=00:0a:35:00:01:22\0"   \
        "kernel_image=uImage\0" \
-       "kernel_load_address=0x2080000\0" \
+       "kernel_load_address=0x3000000\0" \
        "ramdisk_image=uramdisk.image.gz\0"     \
        "ramdisk_load_address=0x4000000\0"      \
        "devicetree_image=devicetree.dtb\0"     \
-       "devicetree_load_address=0x2000000\0"   \
+       "devicetree_load_address=0x2A00000\0"   \
        "bitstream_image=system.bit.bin\0"      \
        "boot_image=BOOT.bin\0" \
        "loadbit_addr=0x100000\0"       \
@@ -289,8 +289,7 @@
                        "echo Copying Linux from SD to RAM... && " \
                        "fatload mmc 0 ${kernel_load_address} ${kernel_image} && " \
                        "fatload mmc 0 ${devicetree_load_address} ${devicetree_image} && " \
-                       "fatload mmc 0 ${ramdisk_load_address} ${ramdisk_image} && " \
-                       "bootm ${kernel_load_address} ${ramdisk_load_address} ${devicetree_load_address}; " \
+                       "bootm ${kernel_load_address} - ${devicetree_load_address}; " \
                "fi\0" \
        "usbboot=if usb start; then " \
                        "run uenvboot; " \
```

Build a U-boot image (u-boot). 

for ZedBoard

```
make zynq_zed_config
```

for ZC706

```
make zynq_zc70x_config
```

Then "make"

```
make
```

You have a "u-boot-xlnx/u-boot". Copy it as "u-boot.elf"

```
cp u-boot-xlnx/u-boot ~/work/u-boot.elf
```


FSBL and boot image (BOOT.BIN)
----------------------------------------

On SDK, select "File" -> "New" -> "Application Project". The project name is "fsbl", and select "Zynq FSBL" as the template.

On the "fsbl" project, click "create Boot Image".

Add the "u-boot.elf" that was generated above, and click "Create Image".

You have "zynq.sdk/fsbl/bootimage/BOOT.bin". Then rename it as "BOOT.BIN" on a directory somewhere (work).

```
cp zed.sdk/fsbl/bootimage/BOOT.bin ~/work/BOOT.BIN
```


Linux kernel (uImage)
----------------------------------------

### Downlaod kernel image from Analog device's repository

Download the linux kernel image. Revision 3590cfe88b1a2d418d59e0ed9c928eb8c094c7cf works well for ZC706 and ZedBoard.


```
git clone https://github.com/analogdevicesinc/linux.git ; cd linux 
git checkout xcomm_zynq
git checkout 3590cfe88b1a2d418d59e0ed9c928eb8c094c7cf
```

Load the default setting

```
make zynq_xcomm_adv7511_defconfig
```

### Configure

Configure the kernel. Edit each option as following.

Or, replace ".config" with "zynq-linux/config/linug.config".

```
make menuconfig
```

- UIO

Follow "Device Drivers" —> "Userspace I/O drivers". Then select <*> for three itmes there.

- CMA

Follow "Device Drivers" —> "Generic Driver Options" —> select <*> for DMA Contiguous Memory Allocator. Then set 260 (not 256) for "Size in Mega Bytes" and 8 for "Maximum PAGE_SIZE order of alignment for contiguous buffers".

- NFS

For NFS users, follow "File systems" —> select <*> for "Network file systems". Then select <*> for some related items of NFS.

### Check

Check ".config" has lines as below.

for UIO

```
CONFIG_UIO=y
CONFIG_UIO_PDRV=y
CONFIG_UIO_PDRV_GENIRQ=y
```

for CMA

```
#
# Default contiguous memory area size:
#
CONFIG_CMA_SIZE_MBYTES=260
CONFIG_CMA_SIZE_SEL_MBYTES=y
# CONFIG_CMA_SIZE_SEL_PERCENTAGE is not set
# CONFIG_CMA_SIZE_SEL_MIN is not set
# CONFIG_CMA_SIZE_SEL_MAX is not set
CONFIG_CMA_ALIGNMENT=8
```

for NFS

```
CONFIG_NETWORK_FILESYSTEMS=y
CONFIG_NFS_FS=y
CONFIG_NFS_V2=y
CONFIG_NFS_V3=y
CONFIG_NFS_V3_ACL=y
CONFIG_NFS_V4=y
CONFIG_NFS_SWAP=y
CONFIG_NFS_V4_1=y
CONFIG_NFS_V4_2=y
CONFIG_PNFS_FILE_LAYOUT=y
CONFIG_NFS_V4_1_IMPLEMENTATION_ID_DOMAIN="kernel.org"
CONFIG_NFS_V4_1_MIGRATION=y
# CONFIG_ROOT_NFS is not set
CONFIG_NFS_USE_LEGACY_DNS=y
# CONFIG_NFSD is not set
CONFIG_GRACE_PERIOD=y
CONFIG_LOCKD=y
CONFIG_LOCKD_V4=y
CONFIG_NFS_ACL_SUPPORT=y
CONFIG_NFS_COMMON=y
```

Expand PATH variable with "u-boot-xlnx/tools"

for bash or zsh

```
export PATH=/home/yourname/u-boot-xlnx/tools:$PATH
```

for tcsh or csh

```
setenv PATH /home/yourname/u-boot-xlnx/tools:$PATH
```

Build a kernel image.

```
make uImage LOADADDR=0x00008000
```

You have "uImage" at "linux/arch/arm/boot". Copy it to somewhere.

```
cp linux/arch/arm/boot/uImage ~/work/uImage
```


Device tree (devicetree.dtb)
----------------------------------------

This package includes the custom "*.dts" and "*.dtsi" files. In those, "bootargs" option is modified, and several "dmem-uio" interfaces are appended.

Copy "zynq-linux/config/*.dts" and "zynq-linux/config/*.dtsi" into "linux/arch/arm/boot/dts/"

```
cp zynq-linux/config/*.dts* ~/work/linux/arch/arm/boot/dts
```

for ZedBoard

```
make zynq-zed-empty-custom.dtb
make zynq-zed-empty-custom-cma.dtb
```

for ZC706

```
make zynq-zc706-empty-custom.dtb
make zynq-zc706-empty-custom-cma.dtb
```

zynq-*-empty-custom.dtb is a separated design that uses the first half of DRAM as linux-managed region and the second half as the accelerator reserved region. The second half region can be accessed via "/dev/uio0" from linux. Note that the cache memory of ARM processor for the second region is disabled, because it uses /dev/uio0.

zynq-*-empty-custom-cma.dtb is a more sophisticated design that uses the entire DRAM as linux-managed region, but some contigunous physical spaces (260 MB) of that region can be allocated for accelerator use by using udmabuf driver. Note that the region has contigunous physical spaces on DRAM, so that the accelerator can access easily that region without a complex address translation. Additionally, that region can be cached by the cache memory. If you use ACP port to access the DRAM from an accelerator, enabling the cache memory will improve the performance.

Copy the corresponding dtb file on "linux/arch/arm/boot/dts/" as "devicetree.dtb" on somewhere.

For example

```
cp linux/arch/arm/boot/dts/zynq-zed-empty-custom-cma.dtb ~/work/devicetree.dtb
```

If you use "zynq-*-empty-custom.dtb (for /dev/uio0)", prepare "uEnv.txt" as below.

```
touch ~/work/uEnv.txt
emacs ~/work/uEnv.txt
```

for ZedBoard

```
fdt_high=0x10000000
initrd_high=0x10000000
```

for ZC706

```
fdt_high=0x20000000
initrd_high=0x20000000
```

Debian root file system 
----------------------------------------

Debian 7.0 (wheezy) and Debian 8.0 (jessie) are tested.

### Initialization

```
cd ~/work/
sudo apt-get install qemu-user-static debootstrap binfmt-support
export targetdir=rootfs
export distro=jessie
mkdir $targetdir
sudo debootstrap --arch=armhf --foreign $distro $targetdir
sudo cp /usr/bin/qemu-arm-static $targetdir/usr/bin
sudo cp /etc/resolv.conf $targetdir/etc
```

### chroot

```
sudo chroot $targetdir
```

```
distro=jessie 
export LANG=C
/debootstrap/debootstrap --second-stage
```

```
cat <<EOT > /etc/apt/sources.list
deb http://ftp.jp.debian.org/debian $distro main contrib non-free
deb-src http://ftp.jp.debian.org/debian $distro main contrib non-free
deb http://ftp.debian.org/debian $distro-updates main contrib non-free
deb-src http://ftp.debian.org/debian $distro-updates main contrib non-free
deb http://security.debian.org/debian-security $distro/updates main contrib non-free
deb-src http://security.debian.org/debian-security $distro/updates main contrib non-free
EOT
```

```
cat << EOT > /etc/apt/apt.conf.d/71-no-recommends
APT::Install-Recommends "0";
APT::Install-Suggests "0";
EOT
```

```
apt-get update
apt-get install locales dialog 
dpkg-reconfigure locales
apt-get install openssh-server ntpdate resolvconf sudo less hwinfo ntp tcsh zsh
passwd
```

If you have multiple Zynq-linux systems, assign an appropriate hwaddress value for each system.

```
echo <<EOT >> /etc/network/interfaces
auto eth0
iface eth0 inet static
hwaddress ether 00:0a:35:00:01:??
address 192.168.0.xxx
netmask 255.255.255.0
gateway 192.168.0.yyy
dns-nameservers 192.168.0.aaa 192.168.0.bbb
dns-domain foo.bar.jp
dns-search foo.bar.jp
EOT
```

```
echo <<EOT >> /etc/resolv.conf
nameserver 192.168.0.aaa
nameserver 192.168.0.bbb
EOT
```

Create a new admin user.

```
adduser yourname
```

Enter, enter, enter, ... .
Then edit sudoers.

```
editor=vi visudo
```

Edit like this.

```
# User privilege specification
root    ALL=(ALL:ALL) ALL
yourname ALL=(ALL:ALL) ALL
```

Open "/etc/rc.local".

```
vi /etc/rc.local
```

Then add 3 lines to use NTP at boot time.

```
service ntp stop
ntpdate ntp.nict.jp
service ntp start
exit 0
```

Setup hostname.

```
echo debian-zynq > /etc/hostname
echo T0:2345:respawn:/sbin/getty -L ttyS0 115200 vt1000 >> /etc/inittab
echo 127.0.0.1       debian-zynq >> /etc/hosts
```

Install some applications as you like.

```
apt-get install build-essential
apt-get install screen bash-completion time
apt-get install python python-pip python3 python3-pip
apt-get install nis nfs-common
```

Finish.

```
exit
```

### Finalization

```
sudo rm -f $targetdir/usr/bin/qemu-arm-static
```

SD card as main storage
----------------------------------------

Setup a disk partition by using gparted or fdisk as below.

- 1st partition: empty (4MB)
- 2nd partition: BOOT (64MB, FAT32, bootable)
- 3rd partition: rootfs (entire rest, ext4)

Copy "BOOT.BIN", "devicetree.dtb", "uImage", and "uEnv.txt (If you need)" into the "BOOT" partition.

```
cp ~/work/BOOT.BIN /media/yourname/BOOT/
cp ~/work/devicetree.dtb /media/yourname/BOOT/
cp ~/work/uEnv.txt /media/yourname/BOOT/
cp ~/work/uImage /media/yourname/BOOT/
```

Copy all the contentes in "rootfs" directory you created above into "rootfs" on SD card.

```
sudo cp -a ~/work/rootfs/* /media/yourname/rootfs/
```

Boot
----------------------------------------

### Hardware settting and login

Boot the Zynq linux system with the prepared SD card.

Jumper-pin setting should be as below.

```
H  __
L _  __
```

Login the system via ssh.

```
ssh yourname@192.168.0.xxx
```

### NTP and Timezone

After you login the Debian on Zynq, please set up NTP and Timezone.

```
(on Zynq) sudo emacs /etc/ntp.conf
```

Comment out the orignal lines and insert a new line as below.

```
#server 0.debian.pool.ntp.org iburst
#server 1.debian.pool.ntp.org iburst
#server 2.debian.pool.ntp.org iburst
#server 3.debian.pool.ntp.org iburst

server ntp.nict.jp
```

Restart NTP.

```
(on Zynq) sudo service ntp restart
```

Change the time zone to Asia/Tokyo

```
(on Zynq) sudo dpkg-reconfigure tzdata
```

Select "Asia", then "Tokyo".

### Mount SD card

Mount the 1st partition of SD card on /sdcard .

```
(on Zynq) sudo mkdir /sdcard
(on Zynq) sudo vi /etc/fstab
```

Insert 1 line.

```
/dev/mmcblk0p1 /sdcard auto defaults 0 0
```

### locales

```
(on Zynq) sudo dpkg-reconfigure locales
```

select "en_US.UTF-8", "ja_JP.UTF-8", and "ja_JP.EUC-JP". Then select "en_US.UTF-8" as default.

Reboot

```
(on Zynq) sudo reboot
```


CMA device driver (udmabuf.ko)
----------------------------------------

### Setup kernel image

Copy the "linux" directory used above from the host computer to "/usr/src/" on Zynq-linux system.

```
scp -r ~/work/linux yourname@192.168.0.xxx:
```

Then create a symbolic link named "/usr/src/kernel" to it on Zynq-linux system.

```
(on Zynq) sudo mv linux /usr/src/linux
(on Zynq) sudo ln -s /usr/src/linux /usr/src/kernel
```

Go to "/usr/src/kernel" and "make modules_prepare"

```
(on Zynq) sudo make modules_prepare
```

### Download and compile

Download the CMA memory driver from GitHub, and copy it to Zynq-linux system.

```
git clone https://github.com/shtaxxx/udmabuf.git
scp -r udmabuf yourname@192.168.0.xxx:
```

```
(on Zynq) cd udmabuf
(on Zynq) make 
```

You will have "udmabuf.ko".

Or use "zynq-linux/compiled_file/udmabuf.ko".

Copy it into "/drivers/".

```
(on Zynq) sudo mkdir /drivers
(on Zynq) sudo cp udmabuf.ko /drivers
```

User-level driver library (cma.h)
----------------------------------------

Go to "zynq-linux/sample/. Then compile a sample program that uses user-level library (zynq-linux/lib/cma.h).

```
scp -r zynq-linux yourname@192.168.0.xxx
```

```
(on Zynq) cd zynq-linux/sample
(on Zynq) make
(on Zynq) ./a.out 10000 1
```

The second argument of a.out is the run mode:
- 0: standard memory space (malloc, fastest)
- 1: CMA space (cma_malloc without O_SYNC, CPU cache enabled, very fast but no consistent to accelerators)
- 2: CMA space (cma_malloc with O_SYNC, CPU cache disabled, slow)
- 3: CMA space (cma_malloc with O_SYNC, CPU cache disabled, write combined, fast)
- 4: CMA space (cma_malloc with O_SYNC, CPU cache disabled, DMA coherent, fast)

You can find other user-level libraries ("axis.h" and "umem.h").


Reference
================================================================================

Analog Device Reference Project
----------------------------------------

If you need HDMI, download the FPGA reference design from GitHub.
Refer to this web page. [ADI Reference Designs HDL User Guide](http://wiki.analog.com/resources/fpga/docs/hdl)

```
git clone https://github.com/analogdevicesinc/hdl.git
```

Yet Another Guide to Running Linaro Ubuntu Linux Desktop on Xilinx Zynq on the ZedBoard
----------------------------------------

https://fpgacpu.wordpress.com/2013/05/24/yet-another-guide-to-running-linaro-ubuntu-desktop-on-xilinx-zynq-on-the-zedboard/


Building a pure Debian armhf rootfs
----------------------------------------

https://blog.night-shade.org.uk/2013/12/building-a-pure-debian-armhf-rootfs/


ADI Reference Designs HDL User Guide
----------------------------------------

http://wiki.analog.com/resources/fpga/docs/hdl


udmabuf (User space mappable DMA Buffer) (In Japanese)
----------------------------------------

http://qiita.com/ikwzm/items/cc1bb33ff43a491440ea


Zynq+PyCoRAM(+Debian) Introduction (In Japanese)
----------------------------------------

http://www.slideshare.net/shtaxxx/zynqpycoram
