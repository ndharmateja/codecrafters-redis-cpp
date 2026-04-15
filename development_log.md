1. Separate the has_atleast_one_command, receive_one_command and parse_command functions.
   - It would probably better to start to parse the command and if the command is not finished, we could receive more data dynamically and continue parsing until we reach the end of the command
   - We could also add logic to dynamically clear/resize the buffer if there is not enough space left in the buffer
     - clear vs resize: with resize, we could use string_view but with clear, we are maintaining a fixed size buffer