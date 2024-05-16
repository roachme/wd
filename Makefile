SRCS   := $(wildcard src/*.c)
OBJS   := $(patsubst src/%.c, obj/%.o, $(SRCS))
CC     := gcc
CFLAGS := -I inc -Wall
WDPATH := ~/.local/bin
PRJ    := _wd

all: $(PRJ)


init:
	test -e $(WDPATH) || mkdir -p $(WDPATH)
	test -e obj       || mkdir obj


$(PRJ): $(OBJS)
	$(CC) $^ -o $@

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(PRJ) obj

re: clean all


install:
	cp $(PRJ) wd.sh $(WDPATH)
	echo "PATH=~/.local/bin:$$"PATH "&& export PATH" >> ~/.bashrc
	echo "source $(WDPATH)/wd.sh" >> ~/.bashrc

uninstall:
	# delete wd function from .bashrc too
	rm -rf $(WDPATH)/$(PRJ) $(WDPATH)/wd.sh
