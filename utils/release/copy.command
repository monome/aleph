cd $(dirname "$0")
cp -v -R ./* /Volumes/ALEPH/
echo "ejecting..."
diskutil unmount ALEPH
