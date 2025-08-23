/**
 * Config file to provide definitions for esh project
 * which originally where defined from it's makefile
 * add line
 * #include "shell_user_config.h"
 * in shell/shell/shell_config.h file
 */
#ifndef __H_SHELL_USER_CONFIG__
#define __H_SHELL_USER_CONFIG__

#define ECHO_INIT_VALUE (0x1)

#define __PROMPT__            #
#define __BUILD_USER__        ABD
#define __BUILD_HOST__        DESKTOP-KQPPS1L
#define __SHELL_VERSION__     e1c08de
#define __USER_REPO_VERSION__ branch:memfault

#ifdef SHELL_LITE
#define SHELL_PRINTF_LITE
#define SHELL_NO_PRINTF_LL
#define SHELL_NO_UTILS
#define SHELL_NO_HISTORY
#define SHELL_NO_TAB_COMPLETE
#define SHELL_NO_BIT_UTILS
#endif


#endif