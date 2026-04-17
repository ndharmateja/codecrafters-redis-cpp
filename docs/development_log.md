## Implementation details
1. Separate the has_atleast_one_command, receive_one_command and parse_command functions.
   - It would probably better to start to parse the command and if the command is not finished, we could receive more data dynamically and continue parsing until we reach the end of the command
   - We could also add logic to dynamically clear/resize the buffer if there is not enough space left in the buffer
      - clear vs resize: with resize, we could use string_view but with clear, we are maintaining a fixed size buffer
2. Threads with mutex in the KeyValueStore class itself
3. Singleton pattern for the KeyValueStore class

## TODOs
- [ ] Event loop instead of multi threading as threads won't scale to 10k concurrent clients.
- [x] Add Client class.
- [ ] Add Buffer class.
    - [ ] doubling and halving logic and compact logic and who calls who
    - [ ] limit checks before updating read_pos and write_pos and curr_pos
- [ ] Add CommandParser class.
    - [ ] Start parsing the command directly and receive bytes if the command is not complete, add logic for dynamic resizing of the buffer like dynamic arrays if the current buffer is not large enough for the commands.
- [ ] Command hierarchy of classes and factory pattern
- [ ] std::move for strings in get_value and set_value etc
- [ ] use Rabin-Karp/KMP/Boyer-Moore algo for find in buf
- [x] add a curr_pos variable in the buffer class itself so that we can compact and resize while even parsing a command
- [ ] error flow when client connection closes or error. bool or throw. custom errors.

## Changelog
- 15 Apr 2026: add single threaded client
- 15 Apr 2026: add support for PING command
- 15 Apr 2026: run each client connection in a different thread to support concurrent clients
- 15 Apr 2026: implement a robust command parser to parse arrays of bulk strings
- 16 Apr 2026: engineer a custom Buffer class with dynamic resizing, capacity management and to handle raw byte streams
- 16 Apr 2026: encapsulate the client code into a Client class
- 16 Apr 2026: implement the KeyValueStore using the Meyer's Singleton pattern with an mutex inside it
- 16 Apr 2026: handle case insensitivity of the commands
- 16 Apr 2026: add support for ECHO, SET and GET commands