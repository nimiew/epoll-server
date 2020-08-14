#pragma once

#include <string>
#include <vector>
#include "item.h"

std::string get_item_repr(Item item);
Item parse_item(std::vector<std::string> &line_list);
int find_item_idx(std::string item_name, std::vector<Item> &item_list);