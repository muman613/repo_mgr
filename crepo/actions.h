#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "crepo.h"

#define HANDLER_PROTO(x)                                                        \
bool (x)(const crepo_parms&)


HANDLER_PROTO(checkout_handler);
HANDLER_PROTO(diff_handler);
HANDLER_PROTO(stat_handler);
HANDLER_PROTO(help_handler);
HANDLER_PROTO(branch_handler);
HANDLER_PROTO(version_handler);
HANDLER_PROTO(tag_handler);
HANDLER_PROTO(sync_handler);

#endif // __ACTIONS_H__

