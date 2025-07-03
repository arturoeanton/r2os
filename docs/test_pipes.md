# Testing Pipe Functionality

## Commands to test:

1. `touch test1.txt` - Create a test file
2. `copycon test1.txt` - Add some content (type some lines, end with Ctrl+Z)
3. `ls` - List files to verify test1.txt exists
4. `cat test1.txt` - Display content to verify
5. `ls | wc` - Test pipe: count files
6. `cat test1.txt | grep word` - Test pipe: search in file content
7. `echo "hello world" | grep hello` - Test pipe: search in echo output
8. `ls | head 3` - Test pipe: show first 3 files

## Expected behavior:
- The pipe should capture output from the first command
- Process it through the second command
- Display the final result

## Pipe Implementation Notes:
- Supports: ls, cat, echo as input commands
- Supports: grep, wc, head, tail as output commands
- Uses simple string buffering for educational purposes
- Shows debug message: "Ejecutando pipe: 'cmd1' | 'cmd2'"