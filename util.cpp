#include <fcntl.h>
#include <sstream>
#include <iostream>

#include "util.h"

void check(int exp, const char *msg)
{
    if (exp < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void set_non_blocking(int sockfd)
{
    int opts = fcntl(sockfd, F_GETFL);
    check(opts, "fcntl(F_GETFL)\n");
    opts = (opts | O_NONBLOCK);
    check(fcntl(sockfd, F_SETFL, opts), "fcntl(F_SETFL)\n");
}

std::vector<std::string> split_string(std::string s, char delim)
{
    std::istringstream orig_stream(s);
    std::vector<std::string> line_list;
    std::string cur_line;
    while (std::getline(orig_stream, cur_line, delim))
    {
        if (!cur_line.empty())
        {
            line_list.push_back(cur_line);
        }
    }
    return line_list;
}

void print_vector(std::vector<std::string> v)
{
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
}