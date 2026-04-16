## Implementation details
1. Separate the has_atleast_one_command, receive_one_command and parse_command functions.
   - It would probably better to start to parse the command and if the command is not finished, we could receive more data dynamically and continue parsing until we reach the end of the command
   - We could also add logic to dynamically clear/resize the buffer if there is not enough space left in the buffer
      - clear vs resize: with resize, we could use string_view but with clear, we are maintaining a fixed size buffer

## TODOs
- [ ] Event loop instead of multi threading.
- [x] Add Client class.
- [ ] Add Buffer class.
    - [ ] doubling and halving logic and compact logic and who calls who
    - [ ] limit checks before updating read_pos and write_pos
- [ ] Add CommandParser class.
    - [ ] Start parsing the command directly and receive bytes if the command is not complete, add logic for dynamic resizing of the buffer like dynamic arrays if the current buffer is not large enough for the commands.
- [ ] Command hierarchy of classes and factory pattern
- [ ] std::move for strings in get_value and set_value etc