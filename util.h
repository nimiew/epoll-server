#pragma once

#include <vector>
#include <string>

void check(int exp, const char *msg);
void set_non_blocking(int sockfd);
std::vector<std::string> split_string(std::string s, char delim = '\n');
void print_vector(std::vector<std::string> v);