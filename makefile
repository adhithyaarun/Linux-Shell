TARGETS = shell

.PHONY: clean

all: $(TARGETS)

%: %.c
	gcc $< -o $@ $<

shell: cd.o clock.o echo.o envcmd.o fg_bg.o jobs.o kjob.o ls.o overkill.o pinfo.o pwd.o quit.o remindme.o shell.o
	gcc -o shell cd.o clock.o echo.o envcmd.o fg_bg.o jobs.o kjob.o ls.o overkill.o pinfo.o pwd.o quit.o remindme.o shell.o

clean:
	rm -f $(TARGETS) *.o
