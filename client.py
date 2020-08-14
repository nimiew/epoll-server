import socket
import textwrap
import sys

def GET(name=None):
    if name:
        return textwrap.dedent(
            f"""
            GET /items/{name} HTTP/1.1
            """
        )
    else:
        return textwrap.dedent(
            """
            GET /items HTTP/1.1
            """
        )

def POST(name, price):
    return textwrap.dedent(
        f"""
        POST /items HTTP/1.1\r
        \r
        name: {name}\r
        price: {price}
        """
    )

def DELETE(name=None):
    if name:
        return textwrap.dedent(
            f"""
            DELETE /items/{name} HTTP/1.1
            """
        )
    else:
        return textwrap.dedent(
            """
            DELETE /items HTTP/1.1
            """
        )

if __name__ == "__main__":
    sock = socket.socket()
    sock.connect(("localhost", 8080))
    sock.setblocking(1)
    if len(sys.argv) == 1:
        sock.send(POST("abc", 123).encode())
        print(sock.recv(4096).decode())
        sock.send(POST("abcd", 1234).encode())
        print(sock.recv(4096).decode())
        sock.send(GET("abc").encode())
        print(sock.recv(4096).decode())
        sock.send(GET("abcd").encode())
        print(sock.recv(4096).decode())
        sock.send(GET().encode())
        print(sock.recv(4096).decode())
        sock.send(DELETE("abc").encode())
        print(sock.recv(4096).decode())
        sock.send(GET().encode())
        print(sock.recv(4096).decode())
        sock.send(POST("abc", 123).encode())
        print(sock.recv(4096).decode())
        sock.send(GET().encode())
        print(sock.recv(4096).decode())
        sock.send(DELETE().encode())
        print(sock.recv(4096).decode())
        sock.send(GET().encode())
        print(sock.recv(4096).decode())
    else:
        method = sys.argv[1]
        if method == "GET":
            if len(sys.argv) == 2:
                sock.send(GET().encode())
                print(sock.recv(4096).decode())
            else:
                sock.send(GET(sys.argv[2]).encode())
                print(sock.recv(4096).decode())
        elif method == "POST":
            sock.send(POST(sys.argv[2], sys.argv[3]).encode())
            print(sock.recv(4096).decode())
        else:
            if len(sys.argv) == 2:
                sock.send(DELETE().encode())
                print(sock.recv(4096).decode())
            else:
                sock.send(DELETE(sys.argv[2]).encode())
                print(sock.recv(4096).decode())
    sock.close()
