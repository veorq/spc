.PHONY: test format clean

test: test.c
	$(CC) $^ -o $@ -Wall -O3 -std=c99
	./$@

format:
	clang-format -style=webkit -i test.c spc.h

clean:    
	rm -f test
