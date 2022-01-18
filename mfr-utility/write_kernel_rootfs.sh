#!/bin/bash
#* ============================================================================
# * If not stated otherwise in this file or this component's Licenses.txt file the
#* following copyright and licenses apply:
#*
#* Copyright 2020 RDK Management
#
#* Licensed under the Apache License, Version 2.0 (the "License");
#* you may not use this file except in compliance with the License.
#* You may obtain a copy of the License at
#*
#* http://www.apache.org/licenses/LICENSE-2.0
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*/

##############################################################
# Command line arg 1 is the kernel rootfs package file path
# Two memory banks bank0 and bank1 are created..by default bank0 is active
# once new image tar file in downloaded to /tmp, its untar to new directory
# meta information is extracted and is loop mounted to passive bank
# storage partition is created in active bank for backup 
##############################################################

storage_block_name=`fdisk /dev/mmcblk0 -l | tail -2 | cut -d' ' -f1 | tail -1`

echo " create ext3 partition"
mkfs.ext3 -F $storage_block_name 
mkfs.ext3 -F /dev/mmcblk0p3

##create new directory and mount storage block..
mkdir -p /imageblk

mount /dev/mmcblk0p3 /imageblk

#-----------------------------------------------------------------------------------------
#create new directory and untar the pkg file..
#Validate hmac before proceeding further
#-----------------------------------------------------------------------------------------
echo " create a directory to save untarred data"
mkdir -p /imageblk/new_image

##untar pkg.tar.gz for proceeding 
cd /imageblk/new_image

echo " untar the image started..it may take some time"
##give error if untar failed
if ! tar -xzvf "$1"
then
echo " untar the image failed.."
   exit 15
fi
echo " untar the image completed.."

##Find path for rootfs 
sdimg_path=`ls *rootfs.rpi-sdimg`


##Create file with sector/partition details
fdisk -u -l $sdimg_path > /imageblk/sector.txt


#--------------------------------------------------------------------------------------------------
# Create backup/extract directories inside storage area directory 
#--------------------------------------------------------------------------------------------------
#bank 0 active bank1 passive

mkdir -p /imageblk/passive_rootfs

mkdir -p /imageblk/bank0_linux

mkdir -p /imageblk/data_bkup_linux_bank0
mkdir -p /imageblk/data_bkup_linux_bank1

mkdir -p /imageblk/bank0_rootfs


#--------------------------------------------------------------------------------------------------
# Extract meta information of Downloaded image i
# Linux_offset is for kernal and rootfs_offset for other
#--------------------------------------------------------------------------------------------------

echo "Extract Kernal meta information of Downloaded image..."
linux_sector=`tail -2 /imageblk/sector.txt | tr -s ' ' | cut -d'*' -f2 | cut -d' ' -f2 | head -n1`
linux_offset=$((linux_sector*512))

echo " Extract rootfs meta information of Downloaded image..."
rootfs_sector=`tail -2 /imageblk/sector.txt | tr -s ' ' | cut -d'*' -f2 | cut -d' ' -f2 | tail -1`
rootfs_offset=$((rootfs_sector*512))

mkdir -p /imageblk/linux_data
mkdir -p /imageblk/rootfs_data

mkdir -p /imageblk/linux_backup_data/
mkdir -p /imageblk/rootfs_backup_data


#--------------------------------------------------------------------------------------------------
# Loop mount kernel and bootload data of Downloaded image to storage area
#--------------------------------------------------------------------------------------------------

echo " Loop mount kernel of Downloaded image"
linux_sector=`tail -2 /imageblk/sector.txt | tr -s ' ' | cut -d'*' -f2 | cut -d' ' -f2 | head -n1`
mount -o loop,offset=$linux_offset $sdimg_path /imageblk/linux_data
cp -R /imageblk/linux_data/* /imageblk/linux_backup_data/
umount /imageblk/linux_data
rm -rf /imageblk/linux_data

#--------------------------------------------------------------------------------------------------
# Loop mount + Extract rootfs data of Downloaded image to storage area
#--------------------------------------------------------------------------------------------------

echo " Loop mount rootfs of Downloaded image..It may take sometime"
mount -o loop,offset=$rootfs_offset $sdimg_path /imageblk/rootfs_data
cp -R /imageblk/rootfs_data/* /imageblk/rootfs_backup_data
umount /imageblk/rootfs_data
rm -rf /imageblk/rootfs_data


#--------------------------------------------------------------------------------------------------
# Identify active bank ( either bank 0 or bank 1 ) or ( mmcblk0p2 or mmcblk0p3 )
#--------------------------------------------------------------------------------------------------

mount /dev/mmcblk0p1 /imageblk/bank0_linux

bank1_partition_name=`fdisk /dev/mmcblk0 -l | tail -2 | cut -d' ' -f1 | tail -1`

activeBank=`sed -e "s/.*root=//g" /proc/cmdline | cut -d ' ' -f1`
echo "Active bank partition is $activeBank"

if [ "$activeBank" = "$bank1_partition_name" ];
then
    echo "Modifying Bank 0 partition mmcblk0p2 Content with downloaded image ..."

    mount /dev/mmcblk0p2 /imageblk/bank0_rootfs

    passiveBank=/dev/mmcblk0p2

    rm -rf /imageblk/data_bkup_linux_bank0/*
    echo " Take backup of existing kernal information"
    cp -R /imageblk/linux_backup_data/* /imageblk/data_bkup_linux_bank0/

    #remove the existing data before copying new one

    rm -rf /imageblk/data_bkup_linux_bank1/*

    #copy the new image linux data to storage area as bank0 linux back up
    cp -R /imageblk/bank0_linux/* /imageblk/data_bkup_linux_bank1

    rm -rf /imageblk/bank0_linux/*

    #Only one linux partition for both banks. So copy the latest linux image content to bank0 linux FAT partition
    echo " copy new kernal information"
    cp -R /imageblk/linux_backup_data/* /imageblk/bank0_linux

    # change cmdline.txt for bank0 linux to partition p2 or mmcblk0p2 which has to be active bank after reboor
    echo " Modify cmdline for updating image"
    sed -i -e "s|${activeBank}|${passiveBank}|g" /imageblk/bank0_linux/cmdline.txt

    rm -rf /imageblk/bank0_rootfs/*

    #Copy the new image rootfs content to bank0 rootfs 
    echo " copy new rootfs information..It may take some time.."
    cp -R /imageblk/rootfs_backup_data/* /imageblk/bank0_rootfs

    umount /imageblk/bank0_rootfs

#--------------------------------------------------------------------------------------------------
# Upgrade passive bank mmcblk0p3 and switch it as active bank 
#--------------------------------------------------------------------------------------------------

else
echo "Modifying Bank 1 partition mmcblk0p3 Content with downloaded image ..."

#remove the existing linux data backup of bank1 from storage area

rm -rf /imageblk/data_bkup_linux_bank0/*

#copy the new image linux data to storage area as bank1 linux back up
echo " Take backup of existing kernal information"
cp -R /imageblk/bank0_linux/* /imageblk/data_bkup_linux_bank0

rm -rf /imageblk/bank0_linux/*

rm -rf /imageblk/data_bkup_linux_bank1/*

cp -R /imageblk/linux_backup_data/* /imageblk/data_bkup_linux_bank1/

#Only one linux partition for both banks. So copy the latest linux image content to bank0 linux FAT partition
echo " copy new kernal information"
cp -R /imageblk/linux_backup_data/* /imageblk/bank0_linux

##Now copy rootfs
mount $bank1_partition_name /imageblk/passive_rootfs  ##needs value here

passiveBank=$bank1_partition_name;


# change cmdline.txt for bank0 linux to partition p3 or mmcblk0p3 which has to be active bank after reboot
echo " Modify cmdline for updating image"
sed -i -e "s|${activeBank}|${passiveBank}|g" /imageblk/bank0_linux/cmdline.txt

rm -rf /imageblk/bank1_rootfs/*

#Copy the new image rootfs content to bank1 rootfs 
echo " Take backup of existing rootfs data..It may take some time"
cp -R /imageblk/rootfs_backup_data/* /imageblk/passive_rootfs

umount /imageblk/passive_rootfs

fi

#--------------------------------------------------------------------------------------------------
# Remove temp folder used for copying present inside storage area and umount memory area 
#--------------------------------------------------------------------------------------------------

echo " Remove temp folder used for copying"
umount /imageblk/bank0_linux
cd ../..
umount /imageblk
rm -rf /imageblk/rootfs*
rm -rf /imageblk/linux*
rm -rf /imageblk/bank*
rm -f /imageblk/sec*
rm -rf /imageblk/new*
rm -rf /imageblk
echo " Flashing the image is completed.."

exit 0
