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

#ifndef SGT_IOCTL_H
#define SGT_IOCTL_H

struct sgt_buffer {
	uint64_t address;
	uint64_t size;
};

#define SGT_GET_MAX_SIZE	_IOW('Z', 80, struct sgt_buffer)
#define SGT_RESERVE		_IOWR('Z', 81, struct sgt_buffer)
#define SGT_UNRESERVE		_IOR('Z', 82, struct sgt_buffer)
#define SGT_UNRESERVE_ALL	_IO('Z', 83)

#define SGT_DEBUGFS_DIR		"scatter-gather"
#define SGT_DEBUGFS_FILE	"sgt"

#endif
