#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/moduleparam.h>	// needed for module parameters
#include <linux/slab.h>//needed for dynamic alloc
#include <linux/list.h>//needed for list

static char* text_elt = "defaultText";
module_param(text_elt, charp, 0664);
static int  number_of_elt = 10;
module_param(number_of_elt, int, 0);

struct elt{
    char* text;//need to be kzallocated
    int elt_index;
    struct list_head node;
};
static LIST_HEAD (my_list);

static unsigned int mystrlen(const char* str){
    unsigned int len = 0;
    while(*str!= 0){
        str++;
        len++;
    }
    return len;
}
static int __init skeleton_init(void)
{

    int i;
	pr_info ("Linux module 04 skeleton loaded\n");
	pr_debug ("  text: %s\n  number_of_elt: %d\n", text_elt, number_of_elt);
    int strlen = mystrlen(text_elt);
    for(i = 0;i<number_of_elt;i++){
        struct elt* ele;
        ele = kzalloc(sizeof(*ele),GFP_KERNEL);
        char* txt = kzalloc(sizeof(char)*strlen,GFP_KERNEL);
        txt = text_elt;
        if(ele!=NULL && txt!=NULL){
            (ele)->text =txt;
            (ele)->elt_index=i;
            list_add_tail(&ele->node,&my_list);
        }
    }
	return 0;
}

static void __exit skeleton_exit(void)
{
    //exit will process all nodes
    struct elt* ele;
    int ele_nr=0;
    list_for_each_entry(ele,&my_list,node){
        pr_info("ele [%d/%d/%d] = %s\n",ele_nr,ele->elt_index,number_of_elt,ele->text);
        ele_nr++;
    }
    //we need to free each elt
    while(!list_empty(&my_list)){
        ele=list_entry(my_list.next,struct elt,node);
        list_del(&ele->node);
        kfree(&ele->text);
        kfree(ele);
    }

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");