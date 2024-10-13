#ifndef DEBUG_H
#define DEBUG_H

// debug print functions

#ifdef DEBUG

#define PRINTX(var) printf("\033[34m%s: 0x%x\033[0m\n", #var, var);
#define PRINTD(var)  printf("\033[34m%s: 0x%d\033[0m\n", #var, var);

#else

#define PRINTX(var)
#define PRINTD(var)

#endif



#endif