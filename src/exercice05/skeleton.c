// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging
#include <linux/moduleparam.h>	// needed for module parameters
#include <linux/io.h>
#include <linux/ioport.h>
const unsigned int PSZ = 4096;

struct resources_util_s {
	//index mapped
	//res at idx 0 has phys_addr at index 0
	int nb_ressources;
	struct resource* resources[3];
	unsigned long physical_addr[3];
	unsigned long page_aligned_addr[3];
	unsigned long addr_offset[3]; 
	char* resources_names[3];
}resource_util_default = {
	3,
	{NULL,NULL,NULL},
	{0x01c14200,0x01c25080,0x01c30050},
	{0x01c14200 - 0x01c14200%PSZ, 0x01c25080 -0x01c25080%PSZ,0x01c30050-0x01c30050%PSZ},
	{0x01c14200%PSZ, 0x01c25080%PSZ,0x01c30050%PSZ},
	{"allwinner_h5_sid","allwinner_h5_ths","allwinner_h5_emac"}
};

typedef struct resources_util_s resource_util;
static int __init skeleton_init(void)
{
	resource_util res = resource_util_default;
	pr_info ("Linux module 05 skeleton loaded\n");
	//buffer of void pointers
	//this buffer contains the start adresses of each ressource we
	//are going to read
	//we are using char because it is the smallest value usable for 
	//pointer arithmetic
	char* reg_start_addr[3] = {0,0,0};
	//buffer containing all the chip id values
	//unsigned int because each value is coded
	//over 32-bits and strictly greater than 0
	unsigned int chip_id_values[4] = {0,0,0,0};
	//cpu_temp read from register
	//long is signed because temp can be negative
	long cpu_temp = 0;
	//mac_addr values
	unsigned int mac_addr[2] ={0,0}; 
	//reserve all the ressources for usage
	//request page aligned memory zone 
	int i =0;

	for (i = 0; i < res.nb_ressources; i++)
	{
		res.resources[i] = request_mem_region(res.page_aligned_addr[i],PSZ,res.resources_names[i]);
		reg_start_addr[i] = ioremap(res.page_aligned_addr[i],PSZ);
	}
	//every mem region was requested
	//get chip_id values
	//map addresses
	
	for(i=0;i<4;i++){
		//use IOREAD32
		//reads 32 bits 
		chip_id_values[i] = ioread32(reg_start_addr[0] + res.addr_offset[0] +i*sizeof(int));
	}
	for(i=0;i<4;i++){
		pr_info ("val %0x80 ",chip_id_values[i]);
	}

	//get cpu_temp
	cpu_temp = -1191 * (int)ioread32(reg_start_addr[1] + res.addr_offset[1])/10 + 223000;
	pr_info ("temp =%ld", cpu_temp);
	for(i = 0;i<2;i++){
		mac_addr[i] = ioread32(reg_start_addr[2] + res.addr_offset[2] + i*sizeof(int));
	}
	//display mac addr
	for (i = 0; i < res.nb_ressources; i++)
	{
		iounmap(reg_start_addr[i]);
	}

	for(i=0;i<res.nb_ressources;i++){
		release_mem_region(res.page_aligned_addr[i],PSZ);
	}
	pr_info ("end module, you can exit");

	return 0;
}

static void __exit skeleton_exit(void)
{
	pr_info ("Linux module skeleton unloaded\n");
	pr_debug ("  sizeof int: %d\n sizeof long: %d\n", sizeof(int), sizeof(long));
}	

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");