KDIR = /lib/modules/`uname -r`/build

kbuild:
	make -C $(KDIR) M=`pwd`

.PHONY: demo
demo:
	make -C demo

clean:
	make -C $(KDIR) M=`pwd` clean
	make -C demo clean
