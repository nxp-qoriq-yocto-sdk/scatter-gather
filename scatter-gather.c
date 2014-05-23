#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include "sgt_ioctl.h"
#include "sgt_list.h"

#define ENTRIES_PER_TABLE	(PAGE_SIZE / sizeof(unsigned long))

#define LAST_ENTRY		1
#define NORMAL_ENTRY		2
#define LINK_ENTRY		3

#define ADDR_SHIFT		8

static struct dentry *sgt_debugfs_dir;

static long sgt_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations sgt_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = sgt_ioctl,
};

MODULE_DESCRIPTION("Scatter-gather logic for multiple tables");
MODULE_AUTHOR("Cosmin Paraschiv <cosmin.paraschiv@freescale.com>");
MODULE_LICENSE("GPL");

/**
 * calculate_pages - calculates the number of pages needed
 * @size: storage size
 *
 * Return: The number of pages needed to cover the requested storage.
 */
static unsigned int calculate_pages(uint64_t size)
{
	return 1 + ((size - 1) / PAGE_SIZE);
}

/**
 * calculate_tables - calculates the number of tables needed
 * @pages: number of pages
 *
 * Return: The number of tables needed to index the pages.
 */
static unsigned int calculate_tables(unsigned int pages)
{
	unsigned int tables;

	tables = 1;
	if (pages > ENTRIES_PER_TABLE) {
		pages -= ENTRIES_PER_TABLE;
		tables += 1 + ((pages - 1) / (ENTRIES_PER_TABLE - 1));
	}

	return tables;
}

/**
 * calculate_entry - calculates the value for a table entry
 * @addr_virt: virtual address of the page
 * @type: entry type
 *
 * Return: The physical address of the indexed page, encoded with an entry type.
 */
static phys_addr_t calculate_entry(unsigned long addr_virt, unsigned int type)
{
	phys_addr_t addr_phys;

	addr_phys = virt_to_phys((void *) addr_virt);
	addr_phys >>= ADDR_SHIFT;
	addr_phys |= type;

	return addr_phys;
}

/**
 * reserve_entries - reserves memory for a table
 * @table_addr_virt: virtual starting address of the table
 * @pages: number of pages indexed by the table
 *
 * Walks the table, allocates pages and indexes them. The table walk stops when
 * all of the pages have been allocated. It always initializes an entry with 0,
 * as a hack to the case when one of the allocations has failed. This helps
 * unreserve_entries() to know when to stop the table walk.
 *
 * Return: 0 or -ENOMEM, if any of the allocations fail.
 */
static int reserve_entries(unsigned long table_addr_virt, unsigned int pages)
{
	unsigned int i, offset;
	uint32_t *entry;
	unsigned long page_addr_virt;

	while (pages)
		for (i = 0; i < ENTRIES_PER_TABLE; i++) {
			offset = i * sizeof(uint32_t);
			entry = (uint32_t *) (table_addr_virt + offset);
			*entry = 0;

			if (pages == 1) {
				page_addr_virt = __get_free_page(GFP_KERNEL);
				if (!page_addr_virt)
					return -ENOMEM;

				*entry = calculate_entry(page_addr_virt,
							LAST_ENTRY);
				pages--;

				break;
			}

			if (i < ENTRIES_PER_TABLE - 1) {
				page_addr_virt = __get_free_page(GFP_KERNEL);
				if (!page_addr_virt)
					return -ENOMEM;

				*entry = calculate_entry(page_addr_virt,
							NORMAL_ENTRY);
				pages--;
			} else {
				table_addr_virt += PAGE_SIZE;
				*entry = calculate_entry(table_addr_virt,
							LINK_ENTRY);
			}
		}

	return 0;
}

/**
 * unreserve_entries - unreserves memory for a table
 * @table_addr_virt: virtual starting address of the table
 * @pages: number of pages indexed by the table
 *
 * Walks the table and unreserves all indexed pages. The table walk stops when
 * all of the pages have been freed or when an entry is 0, which is a hack to
 * treat the case when one of the allocations from reserve() has failed and not
 * all pages have been indexed.
 */
static void unreserve_entries(unsigned long table_addr_virt, unsigned int pages)
{
	unsigned int i, offset;
	uint32_t *entry;
	phys_addr_t page_addr_phys;
	void *page_addr_virt;

	while (pages)
		for (i = 0; i < ENTRIES_PER_TABLE; i++) {
			offset = i * sizeof(uint32_t);
			entry = (uint32_t *) (table_addr_virt + offset);
			page_addr_phys = *entry;

			if (!page_addr_phys) {
				pages = 0;
				break;
			}

			if ((page_addr_phys & LINK_ENTRY) == LINK_ENTRY) {
				table_addr_virt += PAGE_SIZE;
			} else {
				page_addr_phys &= ~LINK_ENTRY;
				page_addr_phys <<= ADDR_SHIFT;
				page_addr_virt = phys_to_virt(page_addr_phys);
				free_page((unsigned long) page_addr_virt);

				pages--;
				if (!pages)
					break;
			}
		}
}

/**
 * get_max_size - get the amount of available memory
 *
 * Return: The number of available bytes in RAM.
 */
static uint64_t get_max_size(void)
{
	return global_page_state(NR_FREE_PAGES) * PAGE_SIZE;
}

/**
 * unreserve - unreserves memory for a table
 * @table_addr_phys: physical starting address of the table
 *
 * Return: 0 or -EINVAL, if the address does not exist.
 */
static int unreserve(phys_addr_t table_addr_phys)
{
	struct sgt_info *sgti;
	uint64_t size;
	unsigned int pages, tables, order;
	unsigned long table_addr_virt;
	int result;

	sgti = find_list_entry(table_addr_phys);
	if (!sgti)
		return -EINVAL;

	size = sgti->size;
	pages = calculate_pages(size);
	tables = calculate_tables(pages);
	order = order_base_2(tables);

	table_addr_virt = (unsigned long) phys_to_virt(table_addr_phys);
	unreserve_entries(table_addr_virt, pages);
	free_pages(table_addr_virt, order);

	result = del_list_entry(table_addr_phys);
	if (result)
		return -EINVAL;

	return 0;
}

/**
 * unreserve_all - unreserves memory for all tables
 */
static void unreserve_all(void)
{
	struct sgt_info *sgti;

	sgti = get_first_entry();
	while (sgti) {
		unreserve(sgti->address);
		sgti = get_first_entry();
	}
}

/**
 * reserve - reserves memory for a table
 * @size: storage size
 * @table_addr_phys: physical starting address of the table
 *
 * Return: 0 or -ENOMEM, if any of the allocations fail.
 */
static int reserve(uint64_t size, phys_addr_t *table_addr_phys)
{
	unsigned int pages, tables, order;
	unsigned long table_addr_virt;
	int result;

	pages = calculate_pages(size);
	tables = calculate_tables(pages);
	order = order_base_2(tables);

	table_addr_virt = __get_free_pages(GFP_KERNEL, order);
	if (!table_addr_virt)
		return -ENOMEM;

	*table_addr_phys = virt_to_phys((void *) table_addr_virt);
	result = add_list_entry(*table_addr_phys, size);
	if (result)
		goto out;

	result = reserve_entries(table_addr_virt, pages);
	if (result)
		goto out;

	return 0;

out:
	unreserve_all();
	return -ENOMEM;
}

static long sgt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct sgt_buffer sgtb;
	struct sgt_buffer *argp = (struct sgt_buffer *) arg;
	phys_addr_t table_addr_phys;
	int ret = 0;

	switch (cmd) {
	case SGT_GET_MAX_SIZE:
		sgtb.address = 0;
		sgtb.size = get_max_size();

		if (copy_to_user(argp, &sgtb, sizeof(*argp)))
			return -EFAULT;

		break;
	case SGT_RESERVE:
		if (copy_from_user(&sgtb, argp, sizeof(*argp)))
			return -EFAULT;

		ret = reserve(sgtb.size, &table_addr_phys);
		if (ret)
			break;
		sgtb.address = table_addr_phys;

		if (copy_to_user(argp, &sgtb, sizeof(*argp)))
			return -EFAULT;

		break;
	case SGT_UNRESERVE:
		if (copy_from_user(&sgtb, argp, sizeof(*argp)))
			return -EFAULT;

		table_addr_phys = sgtb.address;
		ret = unreserve(table_addr_phys);
		break;
	case SGT_UNRESERVE_ALL:
		unreserve_all();
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int __init sgt_init(void)
{
	struct dentry *sgt_debugfs_file;

	sgt_debugfs_dir = debugfs_create_dir(SGT_DEBUGFS_DIR, NULL);
	if (!sgt_debugfs_dir)
		goto out;

	sgt_debugfs_file = debugfs_create_file(SGT_DEBUGFS_FILE, S_IRUGO,
					sgt_debugfs_dir, NULL, &sgt_fops);
	if (!sgt_debugfs_file)
		goto out;

	return 0;

out:
	debugfs_remove_recursive(sgt_debugfs_dir);
	return -ENOMEM;
}

static void __exit sgt_exit(void)
{
	unreserve_all();
	debugfs_remove_recursive(sgt_debugfs_dir);
}

module_init(sgt_init);
module_exit(sgt_exit);
