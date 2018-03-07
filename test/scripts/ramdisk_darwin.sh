#!/usr/bin/env sh

set -euo pipefail

# Unmount a leftover mount point in case something went really wrong.
if [ -e test/ramdisk ]; then
    diskutil eject test/ramdisk -force 2>/dev/null || true
fi

# Create the ramdisk.
DEVICE=$(hdid -nomount ram://$1)
export BLOCKSIZE=4096
newfs_hfs -v 'ramdisk' -b ${BLOCKSIZE} ${DEVICE} 2>/dev/null

# Mount the ramdisk.
rm -rf test/ramdisk
mkdir -p test/ramdisk
function remount { mount $@ -o nobrowse -t hfs ${DEVICE} test/ramdisk; }
remount

# Ensure that we're always unmounting the ramdisk to prevent them from piling up.
function finish {
  diskutil eject test/ramdisk -force
  rm -rf test/ramdisk
}
trap finish EXIT

# Remount with different options until we get a signal to terminate.
while read -r LINE; do
  case ${LINE} in
    readonly)
        remount -u -r
        ;;
    readwrite)
        remount -u -w
        ;;
    filldisk*)
        # Creates a file that fills up all sectors on the disk, but leaves room for the specified
        # amount of sectors
        rm -f test/ramdisk/fill
        REMAINING=$((${LINE#filldisk}))
        COUNT=$(df test/ramdisk | awk "!/Used/{print \$4 - ${REMAINING}}")
        dd of=test/ramdisk/fill bs=${BLOCKSIZE} count=${COUNT} </dev/zero 2>/dev/null
        ;;
    emptydisk)
        rm -f test/ramdisk/fill
  esac
  # df test/ramdisk >&2
  echo "\n\0"
done
