// includes for kernel modules
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

// external information about the module
// GPL is the generic open license
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael, Kelly, Michael");
MODULE_DESCRIPTION("IPC Kernel Module");
// version is unimportant to the module itself
MODULE_VERSION("1.0");

// pass in a path /proc/<pid>/fd/0 where <pid> is the pid printed out by the client
// this proc directory is where the output of the client is saved
static char *path = ""; 

// can now do insmod lkm_msg.ko path=X to pass command line argument
// module_param creates the ability to pass cla, path is the name, charp stands for char pointer, and the macros as the permissions
module_param(path, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(path, "The path of the proc file for the output of the client for the messaging");

// function to replace sys_open
/* modified from stack overflow: https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module */
struct file *file_open(const char *path, int flags, int permissions) 
{
    // temporary pointer to be returned
    struct file *filp = NULL;
    // fs is way to access fs segment register	
    mm_segment_t fs;
    // error code
    int error = 0;

    // get the x86 processor's FS segment register 
    fs = get_fs();
    set_fs(get_ds());

    // actually open the file itself with the given permissions
    filp = filp_open(path, flags, permissions);
    // set the x86 processor's FS segment register
    set_fs(fs);
    // make sure there is no error with the file pointer returned
    if (IS_ERR(filp)) {
        error = PTR_ERR(filp);
        return NULL;
    }
    // once the file pointer is created, return it
    return filp;
}
// function to replace sys_read
int file_read(struct file *file, unsigned long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t fs;
    // return value of if the file is read properly	
    int ret;

    // set the x86 processor's FS segment register
    fs = get_fs();
    set_fs(get_ds());
    
    // actually does the reading of the file with the vfs functionality since sys_read doesnt work in kernel
    ret = vfs_read(file, data, size, &offset);
    // set the x86 processor's FS segment register
    set_fs(fs);
    return ret;
} 
// function to replace sys_close
void file_close(struct file *file) 
{
	// close the file pointer and set the value to null
	filp_close(file, NULL);
}
// the function that runs upon startup of the module
static int __init lkm_message_init(void)
{
	// print to the kernel space that the module loaded and the path given	
	printk(KERN_INFO "Kernel module has been loaded\n");
	// no path has been passed in, we are unable to read the messages 
	if(strcmp(path,"") == 0)
	{
		printk(KERN_INFO "No file path passed in to open path to client");
		return 0; 
	}
	// print the proper path now that it exists	
	printk(KERN_INFO "Path of client: %s\n", path);
	
	// pointer to the file to be read
	struct file *f;
	// open the file with read only permission and no extra flags
	f = file_open(path, O_RDONLY, 0);

	// make sure the file actually opened
	if(f!= NULL)
	{
		// create buffer 		
		char buf[100];
		// read the messages from the file
		file_read(f, 0, buf, 100);
		// print out the buffer to the kernel
		printk(KERN_INFO "Client: %s\n", buf);
		exit(0);
	}
	else
	{
		// file didnt open, let the user know that the path was bad
		printk(KERN_INFO "Unable to open path to client");
	}
	//close the file
	file_close(f);
  	return 0;
}
// function that runs upon unloading of the module
static void __exit lkm_message_exit(void) {
	// let the user know that the module has finished unloading
	printk(KERN_INFO "Module unloaded\n");
}
// maps the above functions to the loading and unloading of the module itself
module_init(lkm_message_init);
module_exit(lkm_message_exit);
