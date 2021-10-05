qemu-system-x86_64 -enable-kvm -nographic -kernel images/bzImage -drive format=raw,file=images/rootfs.ext2 \
                   -append "root=/dev/sda rw console=ttyS0" \
		   -netdev user,id=vmnic,hostfwd=tcp::60022-:22 -device virtio-net,netdev=vmnic
