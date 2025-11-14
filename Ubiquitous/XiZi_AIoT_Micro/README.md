### XiZi_AIoT Microkernel

XiZi_AIoT is a microkernel designed to facilitate task management, memory management, IPC, and various userland sample applications.

### Building Instructions

To build the XiZi_AIoT microkernel, navigate to the directory xiuos/Ubiquitous/XiZi_AIoT and run the command `make BOARD=$(BOARD)`. By default, running `make` is equivalent to `make BOARD=imx6q-sabrelite`. For building XiZi_AIoT specifically for the imx6q-sabrelite board, you'll need the gcc-arm-none-eabi toolchain. We recommend using version "arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors 6-2017-q1-update) 6.3.1 20170215 (release)", as this version was used during development. If the build process is successful, the generated files can be found in the "build" directory. Use either XiZi-$(BOARD).elf or XiZi-$(BOARD).bin as the image to run on the board or QEMU.

### Running on QEMU

QEMU is a useful tool for emulating boards while testing or developing XiZi_AIoT. Use the following command:

```
qemu-system-arm -M sabrelite -m 1G -smp 4 -cpu cortex-a9 \
    -display none -serial null -serial stdio \
    -kernel xiuos/Ubiquitous/XiZi_AIoT/build/XiZi-imx6q-sabrelite.elf
```
Replace "xiuos/Ubiquitous/XiZi_AIoT/build/XiZi-imx6q-sabrelite.elf" with the appropriate path in your directory. We recommend using version "QEMU emulator version 7.2.0" for successful emulation of the sabrelite board.

### Makefile Usage

XiZi_AIoT utilizes a Makefile to build all its files, including .c and .S files. The compiler.mk file enables the make tool to iterate through all sub-directories defined by *SRC_DIR* and compile files defined by *SRC_FILES* using parameters defined in config.mk. Each board independently defines its config.mk file in hardkernel/arch/.../config.mk. Additionally, path_kernel.mk defines all include paths needed by XiZi_AIoT, and link.mk manages the linking process after all .c and .S files are compiled.