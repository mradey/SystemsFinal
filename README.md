# Systems Final
A program that uses interprocess communication message passing created by Michael Radey, Kelly Wu, and Michael Bragalone in 
Dr. Zhang's Systems 2431 class

Compile with make

One way to run is 2 terminals on computer.  Run:

Terminal 1
./serv 9898

Terminal 2
./cli 127.0.0.1 9898

Compile the kernel module with make

load the module with sudo insmod lkm_msg.ko path="/proc/pid/fd/0" where pid is the pid of the client printed to the screen. 

After running the command if it does not exit automatically, hit CTRL+C and enter the command DMESG to view the kernel 
space printing of the message. 

Unload the module with sudo rmmod lkm_msg.ko


