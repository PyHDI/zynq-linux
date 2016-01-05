#insmod /drivers/udmabuf.ko udmabuf0=134217728
insmod /drivers/udmabuf.ko udmabuf0=268435456
chmod 666 /dev/udmabuf0
chmod 666 /sys/class/udmabuf/udmabuf0/sync_mode
chmod 666 /sys/class/udmabuf/udmabuf0/debug_vma
