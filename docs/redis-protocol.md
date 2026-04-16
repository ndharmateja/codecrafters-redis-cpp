# Types

## Simple strings
- Shouldn't contain '\r' or '\n'.
- Format: `+<data>\r\n`
- Eg: `"OK"` is encoded as `+OK\r\n`

## Bulk strings
- Represents a single binary string. 
- String can be of any size, but Redis limits it to 512 MB.
- Format: `$<length>\r\n<data>\r\n`
- Eg: `"hello"` is encoded as `$5\r\nhello\r\n`
- Eg: empty string `""` is encoded as `$0\r\n\r\n`
- A bulk string with a length of -1 represents a non-existent value, or `null` and is encoded as `$-1\r\n`

## Arrays
- Clients send commands to the Redis server as RESP arrays.
- Similarly, some Redis commands that return collections of elements use arrays as their replies.
- Format:
  - `*<number-of-elements>\r\n<element-1>...<element-n>`
  - Arrays can contain mixed data types. For instance, the following encoding is of a list of four integers and a bulk string.
    - Eg: `*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n$5\r\nhello\r\n` represents `[1, 2, 3, 4, "hello"]`
  - All of the aggregate RESP types support nesting. For example, a nested array of two arrays is encoded as follows.
    - Eg: `*2\r\n*3\r\n:1\r\n:2\r\n:3\r\n*2\r\n+Hello\r\n-World\r\n` represents `[[1, 2, 3], ["Hello", e"World"]]`
  - the encoding of an array consisting of the two bulk strings ["hello", "world"] is encoded as `*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n`
  - an array with a length of -1 represents a null array: `*-1\r\n`


# Commands
- Each redis command is encoded as a RESP array of bulk strings.
- Eg: `SET foo bar` is encoded as an array of three bulk strings `["SET", "foo", "bar"]` which is `*3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n` in bytes
- The server can respond using any valid RESP data type.
  - Eg: A successful SET command's response is `+OK\r\n` which is just `OK`