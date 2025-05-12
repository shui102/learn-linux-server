src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

ALL:a.out

$(obj):%.o:%.c
	gcc -c $< -o $@ -I ./inc

a.out:$(obj)
	gcc $^ -o $@

clean:
	-rm -rf $(obj) a.out