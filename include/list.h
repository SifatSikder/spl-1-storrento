#ifndef LIST_H
#define LIST_H

#include "bencode.h"


be_list *list_create (be_node node);

be_list *list_add (be_list *list, be_node node);

void list_free (be_list *list);

#endif /* LIST_H */