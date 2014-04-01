#ifndef SGT_LIST_H
#define SGT_LIST_H

/**
 * struct sgt_info - information about a scatter-gather table
 * @address: physical starting address
 * @size: storage size, sent from user space
 *
 * Information about the scatter-gather tables is kept in a linked list.
 */
struct sgt_info {
	uint64_t address;
	uint64_t size;
	struct list_head list;
};

/**
 * add_list_entry - add an entry to the list
 * @address: physical starting address of the table
 * @size: storage size
 *
 * Return: 0 or -ENOMEM, if the allocation for a new entry fails.
 */
int add_list_entry(uint64_t address, uint64_t size);

/**
 * del_list_entry - delete an entry from the list
 * @address: physical starting address of the table
 *
 * Return: 0 or -EINVAL, if the address does not exist.
 */
int del_list_entry(uint64_t address);

/**
 * find_list_entry - find an entry in the list
 * @address: physical starting address of the table
 *
 * Return: An entry or NULL, if the address does not exist.
 */
struct sgt_info *find_list_entry(uint64_t address);

/**
 * get_first_entry - get the first entry from the list
 *
 * Return: An entry or NULL, if the list is empty.
 */
struct sgt_info *get_first_entry(void);

#endif
