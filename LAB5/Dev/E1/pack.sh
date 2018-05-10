#!/bin/bash

if [ $# -ne 1 ]
then
echo Usage: $0  number of k-bytes
exit 1
fi

kbytes=$1
# clean log files
rm pack1.txt 1>/dev/null 2>/dev/null
rm pack2.txt 1>/dev/null 2>/dev/null

# save these to chmod files back
var_user=laface
var_group=laface

# create and output file hd.img of the size passed by argument
# all filled by o values. Count=1 skip a sector used by the MBR
dd if=/dev/zero of=hd.img bs=512 count=1 seek=$((kbytes*1024))

# Allow to see normal file like devices
losetup /dev/loop1 hd.img

# creates a disk image of dimension kbytes, with a single partition
# Allow output is redirected to fdisk.input
# expert mode
# head=16
# sector=63
cat <<EOF > fdisk.input
x
h
16
s
63
c
EOF

echo $kbytes >> fdisk.input

# New partition
# Partion number1
# Ending with write
cat <<EOF >> fdisk.input
r
n
p
1


a
1
w
EOF

# Update hd.img to a real file with partition given by the pevious fdisk.input
fdisk hd.img < fdisk.input 1>>pack1.txt 2>>pack2.txt

fdisk -l -u /dev/loop1 1>>pack1.txt 2>>pack2.txt
# Take the number of the block
blocks=$(fdisk -u -l /dev/loop1|tail -1|tr -s " "|cut -d " " -f 5|cut -d "+" -f 1)
# Detach the disk
losetup -d /dev/loop1

# Mount again the disk leaving space for the MBR
losetup -o $((63*512)) --sizelimit $(($blocks*1024)) /dev/loop0 hd.img 1>>pack1.txt 2>>pack2.txt
mkfs.ext2 /dev/loop0

# Creating the menu list for GRUB Boot Loader
# ro=ReadOnly
cat <<EOF > menu.lst
title  minimal linux like kernel
kernel /boot/minimal_linux_like_kernel root=/dev/hda ro quiet splash
# initrd /boot/minimal_linux_like_kernel_initrd
quiet

EOF

# mount /dev/loop0 to mount_point
mkdir mount_point 1>>pack1.txt 2>>pack2.txt
mount /dev/loop0 mount_point 1>>pack1.txt 2>>pack2.txt

# cp grub files from local hard drive
mkdir -p mount_point/boot/grub
cp ../grub/stage1 ../grub/stage2 menu.lst mount_point/boot/grub 1>>pack1.txt 2>>pack2.txt

# cp kernel
cp -v src/kernel mount_point/boot/minimal_linux_like_kernel 1>>pack1.txt 2>>pack2.txt
# umount mount_point
umount mount_point 1>>pack1.txt 2>>pack2.txt
rm -r mount_point 1>>pack1.txt 2>>pack2.txt

# detach /dev/loop0
losetup -d /dev/loop0  1>>pack1.txt 2>>pack2.txt

# creates the grub.input file for grub
cat <<EOF > grub.input
device (hd0,0) hd.img
EOF
echo geometry \(hd0\) $kbytes 16 63 >> grub.input
cat <<EOF >> grub.input
root (hd0,0)
setup (hd0)
quit
EOF

 ../grub/grub.bin --device-map=/dev/null < grub.input 1>>pack1.txt 2>>pack2.txt


chown ${var_user}:${var_group} pack1.txt
chown ${var_user}:${var_group} pack2.txt
chown ${var_user}:${var_group} hd.img
chown ${var_user}:${var_group} menu.lst
