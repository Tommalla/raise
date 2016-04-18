CFLAGS=-m32 -g

tests := exitcode stdio syscall malloc bigmalloc stack pageprot pie

.PHONY: all
all: $(tests:%=check_%)

.PHONY: $(tests:%=check_%)
$(tests:%=check_%): check_%: prog_% raise wrapper
	-./test_$*

$(tests:%=prog_%): %: %.c

prog_pie: prog_pie.c
	$(CC) $(CFLAGS) -o $@ -fPIC -pie $^

wrapper: wrapper.c

clean:
	rm -f $(tests:%=prog_%) wrapper core.*
