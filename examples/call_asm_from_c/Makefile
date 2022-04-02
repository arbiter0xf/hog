call_asm_from_c: callme.o main.o
	gcc $^ -o $@

main.o: main.c
	gcc -c $< -o $@

callme.o: callme.asm
	nasm -f elf64 $^
