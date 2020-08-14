#include "item_helper.h"

std::string get_item_repr(Item item)
{
    return "Name: " + item.name + "\r\nPrice: " + item.price;
};

Item parse_item(std::vector<std::string> &line_list)
{
    bool has_body = false;
    Item item;
    for (std::string line : line_list)
    {
        if (line == "\r")
        {
            has_body = true;
        }
        if (has_body)
        {
            int colon_pos = line.find(": ");
            if (colon_pos == std::string::npos)
            {
                continue;
            }
            std::string key = line.substr(0, colon_pos);
            std::string val = line.substr(colon_pos + 2);
            if (val.size() && val[val.size() - 1] == '\r')
            {
                val.pop_back();
            }
            if (key == "name")
            {
                item.name = val;
            }
            else if (key == "price")
            {
                item.price = val;
            }
        }
    }
    return item;
}

int find_item_idx(std::string item_name, std::vector<Item> &item_list)
{
    for (int i = 0; i < item_list.size(); ++i)
    {
        if (item_list[i].name == item_name)
        {
            return i;
        }
    }
    return -1;
}