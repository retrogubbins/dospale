
#ifndef __EMUSWITCH_H__
#define __EMUSWITCH_H__


//#define USE_RAZE
#define USE_KOGEL


#ifdef USE_RAZE
        #include "RAZE.h"
#endif

#ifdef USE_KOGEL
        #include "./KOGEL/Z80.h"
        #include "./KOGEL/KOGELMEM.h"
        #include "./KOGEL/KOGELIO.h"
#endif


#endif
