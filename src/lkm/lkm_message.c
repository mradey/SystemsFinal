// includes for kernel modules
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


MODULE_LICENSE("");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("IPC Kernel Module");
MODULE_VERSION("0.1");

static int __init lkm_message_init(void) {
	return 0;
}

static void __exit lkm_message_exit(void) {
	
}


module_init(lkm_message_init);
module_exit(lkm_message_exit);
