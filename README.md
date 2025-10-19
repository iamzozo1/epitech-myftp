# 🗂️ MyFTP

**MyFTP** is a lightweight, multiclient **File Transfer Protocol (FTP) server** built in **C++**.  
It follows the [RFC 959](https://datatracker.ietf.org/doc/html/rfc959) specification and supports all standard FTP commands, including authentication, file listing, transfers, and mode switching (active/passive).

This project was developed as part of a systems and networking module at **Epitech**, focusing on socket programming, concurrency management, and protocol design.

---

## 🚀 Features

- ✅ **RFC 959–compliant** FTP server  
- 👥 **Multiple clients** handled concurrently using `poll()`  
- 🔐 **User authentication** (username & password)  
- 📂 Support for file **upload (STOR)** and **download (RETR)**  
- 🌐 **Active** and **Passive** data transfer modes  
- ⚙️ Command parsing and error handling per session  
- 🧰 Clean and modular C++ codebase with Makefile or CMake support

---

## 🧠 Technical Overview

| Component | Description |
|------------|-------------|
| **Language** | C++ (C++17 standard) |
| **Networking** | TCP sockets (`socket`, `bind`, `listen`, `accept`, `poll`) |
| **Concurrency** | Event-driven multiplexing with `poll()` |
| **Protocol** | FTP (RFC 959) |
| **Architecture** | Command channel + Data channel per client |
| **Error handling** | Robust per-client logging and exception-safe design |

---

## 🧩 Project Structure

```
myftp/
│
├── src/
│   ├── Server.cpp        # Main server loop and client handling
│   ├── FtpCommands.cpp   # Core FTP command implementations (LIST, RETR, STOR, etc.)
│   ├── Auth.cpp          # Login and user management
│   ├── DataTransfer.cpp  # Passive/Active mode handling
│   ├── Utils.cpp         # Utility functions (string parsing, logging, etc.)
│   └── ...
│
├── include/
│   └── MyFTP.hpp
│
├── Makefile / CMakeLists.txt
└── README.md
```

---

## ⚡ Installation & Usage

### 1. Clone the repository
```bash
git clone https://github.com/<your-username>/myftp.git
cd myftp
```

### 2. Build the server
```bash
make
```

### 3. Run the server
```bash
./myftp <port> <path-to-user-file>
```

**Example:**
```bash
./myftp 2121 users.txt
```

---

## 🧪 Example Session

```bash
$ ftp localhost 2121
Connected to localhost.
220 Welcome to MyFTP server
Name (localhost:user): admin
331 Please specify the password.
Password:
230 Login successful.
ftp> ls
ftp> get file.txt
ftp> quit
221 Goodbye.
```

---

## 🔧 Configuration

The `users.txt` file defines authorized FTP accounts in the following format:

```
username password
john doe123
admin admin42
```

---

## 🧰 Build Commands

```bash
make            # Build the project
make clean      # Remove object files
make fclean     # Remove all binaries
make re         # Rebuild everything
```

---

## 🧑‍💻 Author

**Enzo Fokwen-Rocher**  
Software Engineering Student @ Epitech  
[LinkedIn](https://www.linkedin.com/in/enzo-fokwen/) · [GitHub](https://github.com/iamzozo1)

---

## 📜 License

This project is distributed under the **MIT License** — feel free to use, modify, and share for educational purposes.

---

## 🧩 Keywords

`C++` · `FTP` · `Sockets` · `Concurrency` · `Networking` · `RFC959` · `System Programming` · `Epitech`
