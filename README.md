# devzerothing

Inspired by descriptions of the way Drovorub LKM userspace part communicates with the kernel module. 

Overwriting function pointers in the file_operations structure associated with /dev/zero to divert write operations to a different function. 

(It might not actually work like this but it was a fun thing to try!)

