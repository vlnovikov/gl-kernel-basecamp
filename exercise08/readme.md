# Linux kernel and Buildroot compilation and running in QEMU
## Linux kernel compilation
Required version of linux kernel must be cloned and checked out from github.
```
make O=<OUTPUT_DIR> defconfig
cd <OUTPUT_DIR>
make menuconfig
make -j<CORE_COUNT>
```
## Buildroot compilation
There are several configurations for buildroot, list of them in `configs` directory.
For QEMU there is a default `qemu_x86_64_defconfig` or `qemu_x86_defconfig` depending on architecture.
```
make O=<OUTPUT_DIR> <CONFIG>
cd <OUTPUT_DIR>
```
Buildroot has a very wide spectrum of settings. Some notable settings:
* It could use precompiled kernel or compile it as a part of image generation process.
* `libc` by default `uClibc-ng`, which is differs from default `glibc` in most distros.
* `BusyBox` as default init system, `systemd` requires `glibc`.
* `useradd` or `usermod` commands are not available out of box, so users must be configured
in advance with `System configuration/Path ot the users tables`.
* Since QEMU uses specific network configuration `dhcpcd` package should be installed in order to
enable network.
* `OpenSSH` or `dropbear` packages required for ssh access.

```
make menuconfig
make -j<CORE_COUNT>
```

## Running compiled rootfs
I faced some issues with running QEMU with graphics in a separate window, so had to add `-nographic`
option. Everything else is quite standard.

## Current configuration
* `.config` contains current configuration with compilation of linux kernel and some additional packages.
* `run.sh` script for QEMU image run, it forwards port 60022 to default SSH(22) port of virtual machine.

## Links
[Linux kernel](https://github.com/torvalds/linux)

[buildroot](https://github.com/buildroot/buildroot)

[QEMU configuration in Arch Linux](https://wiki.archlinux.org/title/QEMU)

[QEMU network configuration](https://wiki.qemu.org/Documentation/Networking)