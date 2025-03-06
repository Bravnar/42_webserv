![](https://github.com/ayogun/42-project-badges/blob/main/badges/webservm.png?raw=true)

![42 Badge](https://img.shields.io/badge/42-Project-blue)  
![](https://img.shields.io/github/languages/code-size/Bravnar/42_webserv?color=5BCFFF)

![](https://img.shields.io/badge/Linux-0a97f5?style=for-the-badge&logo=linux&logoColor=white)
![](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=apple&logoColor=white)

# 🌐 42_webserv 🌐
Ahoy, coding navigators! Welcome to **42_webserv**, our own lightweight web server inspired by Nginx. This project dives deep into HTTP protocols, socket programming, and multi-threading to create a functional server from scratch! 🚀

## What’s Webserv? 🤔
42_webserv is a custom-built HTTP server, handling requests and responses efficiently using C++. It supports multiple clients, CGI execution, and configuration-based behavior. 🌍📡

## Cloning the Repository 🛠
Ready to build your own web server? Clone the repository with:
```bash
git clone https://github.com/Bravnar/42_webserv.git
```
## Compiling & Running Webserv 🚀
Use `Makefile` to compile the project:
```bash
make
```

To clean up object files:

```bash
make clean
```
To remove binaries and objects:

```bash
make fclean
```
To recompile:

```bash
make re
```
Run the server with:

```bash
./webserv [option: script file]
```

## Features ✨
1. **Multi-client handling** - Supports multiple simultaneous connections. ⚡
2. **HTTP request parsing** - Handles GET, POST, and DELETE requests. 🌐
3. **CGI execution** - Runs scripts dynamically via CGI (e.g., PHP, Python). 🖥
4. **Error handling** - Custom error pages for different HTTP status codes. 🚨
5. **Configurable server** - Uses .conf files for flexible settings. 🛠

## What We Learned 🧠
1. **Networking in C++** - Mastered socket programming and handling multiple connections. 🌍
2. **HTTP Protocol** - Implemented request parsing, headers, and response building. 📩
3. **Concurrency** - Managed clients efficiently using multiplexing techniques like poll(). ⚙️

## Testing Webserv 🧪
Once running, test the server using curl:

```bash
curl -X GET http://localhost:8080/
```

Or open a browser and navigate to:
```arduino
http://localhost:8080/
```