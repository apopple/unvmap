# unvmap

## Introduction and Limitations

This kernel module is a simple hack to remove kernel vmap page table
entries a specific physical address. It should not be used and was
created solely for debug purposes so has several limitations:

* `vmap_area_list` is not exported as a symbol for module use,
  therefore the use must provide it.
* The `vmap_area_lock` is a static symbol. Therefore we don't bother
  with locking. In practice this usually works ok but when it doesn't
  you keep both pieces.
* It has only been tested on PPC64LE. Maybe it works on other
  platforms.

## Usage

Don't. It will break things in all sorts of fascinatingly boring and
frustrating ways.

	$ insmod unvmap.ko vmap_area=0x$(cat /proc/kallsyms | grep vmap_area_list | cut -d" " -f1)
	$ echo <phys_page_addr> > /sys/kernel/debug/unvmap
