#include "types.h"
#include "stat.h"
#include "user.h"


int 
main(void){ 
	printf(1, "hello world\n"); 
	#ifdef DEFAULT 
	printf(1, "this is how macros work\n"); 
	#endif 
	#ifdef OTHERMACRO
	printf(1, "other macro\n");
	#endif
	exit();

}
 
