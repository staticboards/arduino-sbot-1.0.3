#include <WProgram.h>

//int main(void) __attribute__((noreturn));
int main(void)
{
	_init_sbot_internal_();

	setup();
    
	while (1) {
		loop();
	}
}

