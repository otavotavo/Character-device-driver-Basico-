
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <stddef.h>
//#include <asm/futex.h>	 
#include <stdio.h>
#include <linux/uaccess.h>


#define SUCCESS 0
#define DEVICE_NAME "chardev"	// O device name em  /proc/devices   
#define BUF_LEN 80		// tamanho do buffer 


 //Variaveis globais sao declaradas como static, para serem globais so dentro do arquivo. 
 

static int Major;		// O major number do  device driver 
static int Device_Open = 0;	// Verificar se esta sendo usado
static char msg[BUF_LEN];	// O buffer
static char *msg_Ptr;

// Para quando o device for aberto 

static int device_open(struct inode *inode, struct file *file){
	static int counter = 0;

	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;

	return SUCCESS;
}

// * Called when a process, which already opened the dev file, attempts to
// * read from it.
 
static ssize_t device_read(struct file *filp,char *buffer,size_t length,loff_t  *offset){
	
	 // Numeros de bytes escritos no  buffer 
	 
	int bytes_read = 0;

	
	 // retornar  0 como final da mensagem 
	 
	if (*msg_Ptr == 0)
		return 0;

	 
	 // Passando data para o buffer 
	
	 while (length && *msg_Ptr) {

		/* 
		 * put_user para copiar data do kernel data segment para
		 * o user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	return bytes_read;
};
  
 // Called when a process writes to dev file: echo "hi" > /dev/hello 
 
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off){
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
};

// O struct file operations necessario.

static struct file_operations fops = {
	read : device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.owner=THIS_MODULE
};

int comecou(void){
        Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n")

	return SUCCESS;
}


 // This function is called when the module is unloaded
 
void terminou(void){

	unregister_chrdev(Major, DEVICE_NAME);

}

MODULE_LICENSE("GPL");
module_init(comecou);
module_exit(terminou);
