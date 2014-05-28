/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * This software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
