#include <linux/printk.h>
#include <linux/module.h>
#include <linux/mm_types.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <asm/io.h>

MODULE_AUTHOR("Alistair Popple <alistair@popple.id.au>");
MODULE_LICENSE("GPL v2");

static struct list_head *vmap_list;

/* vmap_area_list is not exported as symbol for module use by the
 * kernel so the user gets to look it up. */
static unsigned long vmap_area;
module_param(vmap_area, long, 0400);
MODULE_PARM_DESC(vmap_area, "Address of vmap_area_list");

static void unmap_addr(u64 phys_addr)
{
	struct vmap_area *va;

	list_for_each_entry(va, vmap_list, list) {
		struct vm_struct *vm = va->vm;

		if (!vm || !vm->phys_addr)
			continue;

		if (phys_addr >= vm->phys_addr && phys_addr < vm->phys_addr + vm->size) {
			u64 ea = (u64) vm->addr + phys_addr - vm->phys_addr;

			pr_info("Unmapping virtual addr 0x%016llx to 0x%016llx -> 0x%016llx\n",
				ea, ea + PAGE_SIZE, phys_addr);
			unmap_kernel_range(ea, PAGE_SIZE);
		}
	}
}

static int set_unvmap(void *data, u64 addr)
{
	unmap_addr(addr);

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(unvmap_wo, NULL, set_unvmap, "0x%016llx\n");

static int __init unvmap_init_module(void)
{
	vmap_list = (struct list_head *) vmap_area;
	if (!vmap_list) {
		pr_alert("Address of vmap_area_list must be provided via vmap_area module parameter.\n");
		return -EINVAL;
	}

	pr_info("Init vmap unvmap. vmap_area_list = 0x%016lx\n", (unsigned long) vmap_list);

	debugfs_create_file("unvmap", 0200, NULL, NULL, &unvmap_wo);

	return 0;
}
module_init(unvmap_init_module);

static void unvmap_exit_module(void)
{
}
module_exit(unvmap_exit_module);
