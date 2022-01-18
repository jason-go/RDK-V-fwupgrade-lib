##*
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

##---------------------------------------------------------------------------------------
## For firmware control plugin, more than two partitions are required in SD card
## Two partitions are created by default when image is flashed,
## one for kernal and one for rootfs.
## One more partition is created to save new image's rootfs
##----------------------------------------------------------------------------------------

echo "\n validate partitions"

partition_check_name_blk0=`fdisk /dev/mmcblk0 -l | grep /dev/mmcblk0 | tail -2 | tr -s ' ' | cut -d ' ' -f1 | tail -1`
partition_check_name_blk1=`fdisk /dev/mmcblk0 -l | grep /dev/mmcblk0 | tail -2 | tr -s ' ' | cut -d ' ' -f1 | head -n 1`

if [ "$partition_check_name_blk0" = "/dev/mmcblk0p2" ] || \
   [ "$partition_check_name_blk1" = "/dev/mmcblk0p2" ]
then
    echo "creating partitioni.. Box will reboot after creating partition\n"
    if [ "$partition_check_name_block1" = "/dev/mmcblk0p2" ]
        then
            bank1_partition=`fdisk /dev/mmcblk0 -l | tail -2 | tr -s ' ' | cut -d ' ' -f3 | tail -1`
        else
            bank1_partition=`fdisk /dev/mmcblk0 -l | tail -2 | tr -s ' ' | cut -d ' ' -f3 | head -n 1`
        fi
    
    bank1_partition=`fdisk /dev/mmcblk0 -l | grep /dev/mmcblk0 | tail -2 | tr -s ' ' | cut -d ' ' -f3 | tail -1`
    bank_offset=1
    size_offset=81380
    bank1_start=$((bank1_partition+bank_offset))
    bank1_end=$((bank1_start+size_offset))
    echo -e "\nn\np\n3\n$((bank1_start))\n$((bank1_end))\np\nw" | fdisk /dev/mmcblk0
    mkfs.ext3 -F /dev/mmcblk0p3 
    
    echo "creating next partition\n"
    storage_partition=`fdisk /dev/mmcblk0 -l | grep /dev/mmcblk0 | tail -2 | tr -s ' ' | cut -d ' ' -f3 | tail -1`
    storage_offset=1
    size_offset=40986
    storage_start=$((storage_partition+storage_offset))
    storage_end=$((storage_start+size_offset))
    echo -e "\nn\np\n$((storage_start))\n$((storage_end))\np\nw" | fdisk /dev/mmcblk0
    mkfs.ext3 -F /dev/mmcblk0p4
 
    ##Board will reboot after creating partition
    echo "Board will reboot after creating partition"   
    reboot -f

else
   echo "Partitions are available"
fi
exit 0
