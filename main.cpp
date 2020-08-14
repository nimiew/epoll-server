#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <unordered_map>

#include "util.h"
#include "item.h"
#include "item_helper.h"

#define MAX_EVENTS 131072
#define MAX_CONNECTIONS 16384
#define PORT 8080

void handle_input(int fd, std::string s, std::unordered_map<int, std::string> &fd_towrite, std::vector<Item> &item_list)
{
    std::cout << s << std::endl;
    std::vector<std::string> line_list = split_string(s);
    std::vector<std::string> request_line_list = split_string(line_list[0], ' ');
    std::string method = request_line_list[0];
    std::string url = request_line_list[1];
    if (url.size() && url[url.size() - 1] == '/')
    {
        url.pop_back();
    }
    std::string body = "";

    if (method == "GET")
    {
        if (url == "/")
        {
            body = "Hello World";
            fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
        }
        if (url == "/items")
        {
            if (item_list.empty())
            {
                body += "No items found";
            }
            else
            {
                body += "List of items:\r\n";
                for (int i = 0; i < item_list.size(); ++i)
                {
                    body += get_item_repr(item_list[i]);
                    if (i != item_list.size() - 1)
                    {
                        body += "\r\n";
                    }
                }
            }
            fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
        }
        else
        {
            if (url.substr(0, 7) == "/items/")
            {
                std::string item_name = url.substr(7);
                int item_idx = find_item_idx(item_name, item_list);
                if (item_idx != -1)
                {
                    body += "Item found:\r\n";
                    body += get_item_repr(item_list[item_idx]);
                    fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
                }
                else
                {
                    fd_towrite[fd] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0";
                }
            }
            else
            {
                fd_towrite[fd] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0";
            }
        }
    }
    else if (method == "POST")
    {
        Item item = parse_item(line_list);
        int item_idx = find_item_idx(item.name, item_list);
        if (item_idx == -1)
        {
            item_list.push_back(item);
            body = item.name + " successfully created";
            fd_towrite[fd] = "HTTP/1.1 201 Created\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
        }
        else
        {
            item_list[item_idx] = item;
            body = item.name + " successfully updated";
            fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
        }
    }
    else if (method == "DELETE")
    {
        if (url == "/items")
        {
            item_list.clear();
            fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: 0";
        }
        else
        {
            if (url.substr(0, 7) == "/items/")
            {
                std::string item_name = url.substr(7);
                int item_idx = find_item_idx(item_name, item_list);
                if (item_idx != -1)
                {
                    body += "Item deleted:\r\n";
                    body += get_item_repr(item_list[item_idx]);
                    item_list.erase(item_list.begin() + item_idx);
                    fd_towrite[fd] = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
                }
                else
                {
                    fd_towrite[fd] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0";
                }
            }
            else
            {
                fd_towrite[fd] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0";
            }
        }
    }
    else
    {
        fd_towrite[fd] = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: " + std::to_string(26) + "\r\n\r\n" + "Sorry method not supported\r\n";
    }
}

int main()
{
    struct epoll_event ev, events[MAX_EVENTS];
    int listenfd, conn_sock, nfds, epfd, fd, i, nread, n;
    socklen_t addrlen;
    struct sockaddr_in local, remote;
    char buf[BUFSIZ];

    //State
    std::unordered_map<int, std::string> fd_towrite;
    std::vector<Item> item_list;

    check(listenfd = socket(AF_INET, SOCK_STREAM, 0), "sockfd\n");
    set_non_blocking(listenfd);
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(PORT);
    check(bind(listenfd, (struct sockaddr *)&local, sizeof(local)), "bind\n");
    listen(listenfd, MAX_CONNECTIONS);

    check(epfd = epoll_create(MAX_EVENTS), "epoll_create");
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    check(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev), "epoll_ctl: listen_sock");

    while (1)
    {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        check(nfds, "epoll_pwait");

        for (i = 0; i < nfds; ++i)
        {
            fd = events[i].data.fd;

            if (fd == listenfd)
            {
                while ((conn_sock = accept(listenfd, (struct sockaddr *)&remote, &addrlen)) > 0)
                {
                    set_non_blocking(conn_sock);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = conn_sock;
                    check(epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &ev), "epoll_ctl: add");
                }
                if (conn_sock == -1 && errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
                    perror("accept");
                continue;
            }
            if (events[i].events & EPOLLIN)
            {
                n = 0;
                while ((nread = read(fd, buf + n, BUFSIZ - 1)) > 0)
                {
                    n += nread;
                }
                if (nread == 0) // client disconnect
                {
                    close(fd);
                    continue;
                }
                if (nread == -1 && errno != EAGAIN)
                {
                    perror("read error");
                }
                ev.data.fd = fd;
                ev.events = events[i].events | EPOLLOUT | EPOLLET;
                check(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev), "epoll_ctl: mod");
                handle_input(fd, std::string(buf, n), fd_towrite, item_list);
            }
            if (events[i].events & EPOLLOUT)
            {
                if (fd_towrite.find(fd) == fd_towrite.end() or fd_towrite[fd].empty())
                {
                    continue;
                }
                sprintf(buf, fd_towrite[fd].c_str());
                int nwrite, data_size = strlen(buf);
                n = data_size;
                while (n > 0)
                {
                    nwrite = write(fd, buf + data_size - n, n);
                    if (nwrite < n)
                    {
                        if (nwrite == -1 && errno != EAGAIN)
                        {
                            perror("write error");
                        }
                        break;
                    }
                    n -= nwrite;
                }
                fd_towrite[fd] = "";
            }
        }
    }
    close(epfd);
    close(listenfd);
}