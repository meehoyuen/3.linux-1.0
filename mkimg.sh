#!/bin/bash
echo "n
p
1


w
" | sudo fdisk c.img

sudo losetup /dev/loop5 c.img
if [ $? -ne 0 ];then
	echo "losetup failed" >&2
	exit -1
fi
sudo kpartx -av /dev/loop5
sudo mkfs.ext2 /dev/mapper/loop5p1
sudo mount /dev/mapper/loop5p1 /mnt
sudo mkdir -p /mnt/sbin
sudo cp res/* /mnt/sbin
sync
sudo umount /mnt
sudo kpartx -dv /dev/loop5
sudo losetup -d /dev/loop5
