# Welcome to Gramado OS - Codename 'Presence'


This repository has the source code for:

* Gramado boot loader
* Gramado kernel
* Gramado Window Server
* Applications
* Commands

## What is Gramado?

    Gramado (c) is a 64bit multithreaded operating system.

## Kernel features.

> * Paging.
> * Threads.
> * FAT16 file system.
> * ps/2 keyboard. 
> * ps/2 mouse works fine only on qemu.
> * Serial port. 
> * IDE PATA. 
> * Window Server. See: gramado/core/

## Userland features.

> * Unix-like commands running in the virtual console.
> * Some few clients connected to the window server via unix-sockets.

## Libraries.

> * unix-like library. (rtl)
> * Lib for the window server's clients. (libgws)

## Fred's compilation on Linux.
```
    Host machine: Ubuntu 18.04.5 LTS
    Linux 5.4.0-77-generic x86_64
    gcc (Ubuntu) 7.5.0 
    GNU ld (GNU Binutils for Ubuntu) 2.30
    NASM version 2.13.02
```
## How to Build?

> See the [Gramado OS build instructions](https://github.com/frednora/gramado/blob/master/docs/build.md)

## Testing

```
	$ ./run
	$ make qemu-test
```

Tested only on qemu and on my real machine.
My machine: Gigabyte/Intel Core 2 Duo/Chipset VIA.
Please, give us some feedback.

## Documentation

> See the [docs](https://github.com/frednora/gramado/tree/master/docs/).
> See the [DOCS](https://github.com/frednora/gramado/blob/master/base/GRAMADO/DOCS/).

## More information

Gramado is a Free and Open Source operating system.
The source code uses the BSD license.

# Quotes:
> Oh boy, there is no spoon!

