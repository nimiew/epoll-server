# Basic C++ Epoll Server

## Performance

- Non-blocking I/O by setting file descriptor to be non-blocking
- Use epoll for I/O multiplexing

## Features

- GET/POST/DELETE requests
- Use raw text as message body
- Client can create/update "Item", delete "Item"(s) and view "Item"(s)

## Requirements

- g++
- Python3 (to act as client)

## Get Started

    # Compile
    make  
    # To start server
    ./main
    # For sending requests
    chmod +x send.sh 

## Request Examples

To get all items
```
GET /items HTTP/1.1
```
To get a specific item based on name
```
GET /items/{name} HTTP/1.1
```
To update item (will create if not found)
```
POST /items HTTP/1.1

name: {name}
price: {price}
```
To delete all items
```
DELETE /items HTTP/1.1
```
To delete an item
```
DELETE /items/{name} HTTP/1.1
```

## Sending Requests (use uppercase!)
### GET
Get all items
```
./send.sh GET
```
Get item with specified name
```
./send.sh GET {name}
```
### POST
Update item with specified name and if not found, create item
```
./send.sh POST {name} {price}
```
### DELETE
Delete all items
```
./send.sh DELETE
```
Delete item with specified name
```
./send.sh DELETE {name}
```