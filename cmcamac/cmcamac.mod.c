#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x14522340, "module_layout" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0xcff53400, "kref_put" },
	{ 0xd2037915, "dev_set_drvdata" },
	{ 0xd691cba2, "malloc_sizes" },
	{ 0xab2a6ec4, "usb_deregister_dev" },
	{ 0xced66309, "usb_deregister" },
	{ 0xea147363, "printk" },
	{ 0x3656bf5a, "lock_kernel" },
	{ 0x85f8a266, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0xdd508520, "usb_register_dev" },
	{ 0xb1f975aa, "unlock_kernel" },
	{ 0x373636b3, "usb_get_dev" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x2611da1, "usb_bulk_msg" },
	{ 0x5ac8313f, "usb_put_dev" },
	{ 0x625c9bf5, "usb_find_interface" },
	{ 0x2044fa9e, "kmem_cache_alloc_trace" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0x236c8c64, "memcpy" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x251ce55e, "usb_register_driver" },
	{ 0x9edbecae, "snprintf" },
	{ 0x3302b500, "copy_from_user" },
	{ 0xa92a43c, "dev_get_drvdata" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v1279p0064d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "135DC28F1E0A3BE8A11BFD6");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 5,
};
