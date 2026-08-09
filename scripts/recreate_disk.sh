set -e
echo "Removing old disk..."
rm disk.img
echo "Recreating 16MB disk..."
dd if=/dev/zero of=disk.img bs=512 count=32000
echo "Disk created successfully!"
