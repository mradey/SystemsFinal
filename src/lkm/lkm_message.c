// includes for kernel modules
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
// for stack overflow included code below
#include <linux/fs.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>

MODULE_LICENSE("");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("IPC Kernel Module");
MODULE_VERSION("0.1");


static char *pid = ""; 

// can now do insmod lkm_message.ko pid=X to pass command line argument
module_param(pid, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "The pid number of the client for the messaging");

// the output from the client can be spied on at /proc/client_pid/fd/1
// the 1 stands for the stdout where the messages print to 
// need to use the open syscall to read in from that proc file location 


/* source of code from stackoverflow https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module/1184346#1184346 */
struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}  

static int __init lkm_message_init(void) {
	char path[50];
	char data[100];
	struct file *f;

	char* path_start = "/proc/";
	char* path_end = "/fd/1/";
	strcat(path, path_start);
	strcat(path, pid);  
	strcat(path, path_end);
	
	// parameters may need adjusted here
	f = file_open(path, 0, 0);
	
	file_read(f, 0, data, 100);
	
	file_close(f);
	return 0;
}

static void __exit lkm_message_exit(void) {
	
}


module_init(lkm_message_init);
module_exit(lkm_message_exit);
