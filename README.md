# CaesarCipherTCP

This project implements a TCP-based server-client application that performs Caesar Cipher encryption and decryption.
Built as a learning exercise to demonstrate socket programming and client-server communication. 
The server accepts messages from a client, processes them with a Caesar Cipher (preserving case and skipping non-letters), and returns the result. 
The client provides a simple interface to send commands and view responses.

The application supports both IPv6 and IPv4 and is cross-platform, running on Windows, Linux and macOS





## Features
- **Encrypt or decrypt** messages using a Caesar Cipher with a user-specified shift.
- **Flexible Shift**: Default shift is 3; specify a custom shift by prefixing a number (eg "5 secret").
- **Cross-Platform**: Works on Windows, Linux, and macOS with IPv4 and IPv6 support.
- **Configurable port** (defaults to 1234).



## Building
1. **Clone or Open the Project**:
    - Open in CLion or navigate to the project directory in a terminal.
2. **Configure CMake**:
    - CLion: Load `CMakeLists.txt`
    - Terminal: `mkdir build && cd build && cmake ..`
3. **Build**:
    - CLion: Click "Build" (Ctrl+F9).
    - Terminal: `cmake --build .`
    - Outputs `server` and `client` executables in `build/bin/`.

## Running
1. **Start the Server**:
   ```bash
   ./bin/server [port]
   ```
- Example: ./bin/server 5678 for port 5678.
- The Default Port is 1234

2. **Start the Client**:
   ```bash
   ./bin/client <server_ip> [port]
   ```
- IPv6: ./bin/client ::1 1234.
- IPv4: Set USE_IPV6 to false in code, then ./bin/client 127.0.0.1 1234.

3. **Interact**:
- Type a message (e.g "hello" or "5 secret").
- Server responds with encrypted text (e.g., "Encrypted with shift 3: khoor").
- Type "." to quit the client.

Example: See example_interaction.txt in the root directory for a sample session.



## Notes
- This project was created as a learning exercise to practice socket programming and develop a client-server TCP application. It serves as an educational tool to explore network communication and basic encryption.
- The Caesar Cipher shifts alphabetic characters only, preserving case and leaving non-letters unchanged.
- Messages should fit within the buffer size (1024 bytes)
- The server supports one client at a time.

## Contact
For questions, feedback, or issues, please reach out:

- **Email**: [johnnycwatt@gmail.com](mailto:johnnycwatt@gmail.com)
- **GitHub**: [Johnny's GitHub](https://github.com/johnnycwatt)

