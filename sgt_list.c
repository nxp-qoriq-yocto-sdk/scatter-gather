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

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include "sgt_list.h"

LIST_HEAD(sgt_list);
DEFINE_SPINLOCK(sgt_list_lock);

int add_list_entry(uint64_t address, uint64_t size)
{
	struct sgt_info *sgti;

	sgti = kmalloc(sizeof(*sgti), GFP_KERNEL);
	if (!sgti)
		return -ENOMEM;

	sgti->address = address;
	sgti->size = size;

	spin_lock(&sgt_list_lock);
	list_add(&sgti->list, &sgt_list);
	spin_unlock(&sgt_list_lock);

	return 0;
}

int del_list_entry(uint64_t address)
{
	struct list_head *p, *q;
	struct sgt_info *sgti;

	spin_lock(&sgt_list_lock);
	list_for_each_safe(p, q, &sgt_list) {

		sgti = list_entry(p, struct sgt_info, list);
		if (sgti->address == address) {
			list_del(p);
			spin_unlock(&sgt_list_lock);
			kfree(sgti);
			return 0;
		}

	}
	spin_unlock(&sgt_list_lock);

	return -EINVAL;
}

struct sgt_info *find_list_entry(uint64_t address)
{
	struct sgt_info *sgti;

	spin_lock(&sgt_list_lock);
	list_for_each_entry(sgti, &sgt_list, list)
		if (sgti->address == address) {
			spin_unlock(&sgt_list_lock);
			return sgti;
		}
	spin_unlock(&sgt_list_lock);

	return NULL;
}

struct sgt_info *get_first_entry(void)
{
	struct sgt_info *sgti;

	spin_lock(&sgt_list_lock);
	sgti = list_first_entry_or_null(&sgt_list, struct sgt_info, list);
	spin_unlock(&sgt_list_lock);

	return sgti;
}
