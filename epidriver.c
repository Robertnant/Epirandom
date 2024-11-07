#include "epidriver.h"

#define DEVICE_NAME "epidriver"
#define DRIVER_CLASS "epirandom"

// Number of ASCII characters (extended ASCII included).
#define ASCII_COUNT 256

// Maximum buffer size (NULL character excluded).
#define BUF_LEN 80

static char msg[BUF_LEN + 1];
static char *msg_pointer;

// Variable to check if current device is open.
static int is_open = 0;

// Device file variables.
static dev_t major_num;
static struct cdev device;
static struct class *device_class;

// Variables for module parameters.
static char *alphabet = NULL;
static int alphabet_len = 0;
static int level = 0;

static struct file_operations fops = { .owner = THIS_MODULE,
                                       .read = device_read,
                                       .open = device_open,
                                       .release = device_release };

module_param(alphabet, charp, 0);
MODULE_PARM_DESC(alphabet, "Alphabet for random characters to return");

module_param(level, int, 0);
MODULE_PARM_DESC(level, "Level for which module should be run");

// Initializes and registers created device.
static int init_device(void)
{
    cdev_init(&device, &fops);

    if (cdev_add(&device, major_num, 1) == -1)
    {
        pr_err("epidriver: Failed to register device.\n");
        device_destroy(device_class, major_num);
        class_destroy(device_class);
        unregister_chrdev(major_num, DEVICE_NAME);
        return -1;
    }

    pr_info("epidriver: Successfully created module dev file. Path: /dev/%s\n",
            DEVICE_NAME);

    return 0;
}

/**
 * Function called when module is loaded.
 * A device file is automatically created.
 */
static __init int epidriver_init(void)
{
    if (alloc_chrdev_region(&major_num, 0, 1, DEVICE_NAME) < 0)
    {
        pr_info("epidriver: Failed to allocate device number\n");
        return -1;
    }

    pr_info("epidriver: Successfully allocated major %d and minor %d numbers\n",
            MAJOR(major_num), MINOR(major_num));

    // Create device class and file.
    device_class = class_create(THIS_MODULE, DRIVER_CLASS);
    if (device_class == NULL)
    {
        pr_err("epidriver: Failed to create device class\n");
        unregister_chrdev(major_num, DEVICE_NAME);
        return -1;
    }

    device_create(device_class, NULL, major_num, NULL, DEVICE_NAME);
    if (device_class == NULL)
    {
        pr_err("epidriver: Failed to create device file\n");
        class_destroy(device_class);
        unregister_chrdev(major_num, DEVICE_NAME);
        return -1;
    }

    // Initialize and register device.
    return init_device();
}

/**
 * Function called when on module unload.
 * Automatically deletes device file and class as well.
 */
static __exit void epidriver_exit(void)
{
    pr_info("epidriver: Unloading module, sadly...");

    cdev_del(&device);
    device_destroy(device_class, major_num);
    class_destroy(device_class);
    unregister_chrdev_region(major_num, 1);
}

// Fills output buffer with random bytes.
static void get_random_elements(int counter, int alphabet_len)
{
    // Get random elements from alphabet if given by user.
    if (counter < BUF_LEN)
    {
        unsigned int rand = get_random_int();

        if (level == 1)
        {
            pr_info("Running level 1\n");
            msg[counter++] = rand % 10 + '0';
        }
        else if (level == 2)
        {
            pr_info("Running level 2\n");
            msg[counter++] = rand % ASCII_COUNT;
        }
        else if (alphabet_len)
        {
            pr_info("Running level 4\n");
            msg[counter++] = alphabet[rand % alphabet_len];
        }
        else
        {
            pr_info("Running level 3\n");

            // Get random bytes like /dev/urandom.
            get_random_bytes(msg_pointer, BUF_LEN);
            return;
        }

        // Recursively set next bytes of buffer.
        get_random_elements(counter, alphabet_len);
    }
}

static int device_open(struct inode *inode, struct file *file)
{
    if (is_open)
        return -EBUSY;

    is_open = 1;

    // Increment count of processes using module.
    try_module_get(THIS_MODULE);

    // Get alphabet length.
    if (alphabet)
    {
        while (alphabet[alphabet_len])
        {
            alphabet_len++;
        }
    }

    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    is_open = 0;

    // Decrement count of processes using module.
    module_put(THIS_MODULE);

    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length,
                           loff_t *offset)
{
    // Copy data from kernel data segment to user data segment.
    size_t bytes_count = 0;
    int to_send = 0;

    while (length > 0)
    {
        // Prefill or reset result buffer with new random bytes.
        msg_pointer = msg;
        get_random_elements(0, alphabet_len);

        to_send = BUF_LEN;
        if (to_send > length)
            to_send = length;

        if (copy_to_user(buffer, msg_pointer, to_send) != 0)
            pr_err("epidriver: Error copying bytes\n");

        buffer += to_send;
        msg_pointer += to_send;

        length -= to_send;
        bytes_count += to_send;
    }

    return bytes_count;
}

module_init(epidriver_init);
module_exit(epidriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Nantchouang Nantchouang");
MODULE_DESCRIPTION("Kernel module to generate random elements");
