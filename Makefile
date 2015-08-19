#
# Copyright (C) 2014 Freescale Semiconductor, Inc.
# All rights reserved.
#
# This software may be distributed under the terms of the
# GNU General Public License ("GPL") as published by the Free Software
# Foundation, either version 2 of that License or (at your option) any
# later version.
#
# THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

ccflags-y = -Wall -g

obj-m = sgt.o
sgt-objs = scatter-gather.o sgt_list.o

KERNEL_SRC ?= /lib/modules/`uname -r`/build
SRC := $(shell pwd)

.PHONY: build modules_install demo clean

all: build

build:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

demo:
	$(MAKE) -C demo

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
	$(MAKE) -C demo clean
