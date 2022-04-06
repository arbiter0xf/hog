#include <stdio.h>

extern int asm_return_33(void);

int c_return_33(void)
{
	return 33;
}

int main(void)
{
	int ret_c = c_return_33();
	int ret_asm = asm_return_33();

	printf("Return value from C: %d\n", ret_c);
	printf("Return value from ASM: %d\n", ret_asm);

	return 0;
}
