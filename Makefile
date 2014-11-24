all: git-wrapper

%: %.o
	$(CC) $(CFLAGS) -o $@ $<
