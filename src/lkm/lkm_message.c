// includes for kernel modules
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("IPC Kernel Module");
MODULE_VERSION("0.1");

static int client_pid = 0; 

// can now do insmod lkm_message.ko pid=X to pass command line argument of the pid for the client (or server)
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "The pid number of the client for the messaging");

// the output from the client can be spied on at /proc/pid/fd/1 where pid is the argument passed above
// the 1 stands for the stdout where the messages print to 
// need to use the open syscall to read in from that proc file location 

static int __init lkm_message_init(void) {
	return 0;
}

static void __exit lkm_message_exit(void) {
	
}


module_init(lkm_message_init);
module_exit(lkm_message_exit);
