#pragma warning(disable:4996)
#pragma warning(disable:4229)
#pragma warning(disable:4477)
#pragma warning(disable:4244)
#pragma warning(disable:4018)


#include "hsserver.h"
#include "serverMain2.h"
#include <time.h>
void main() {

	HSServer hss(666);
	hss.start();
	
	//ServerMain2 ss(666);
	//ss.start();
}
