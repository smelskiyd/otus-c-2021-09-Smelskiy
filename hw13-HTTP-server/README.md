# HTTP Server which sends files from the specific directory

# Server

Server shares files from specified directory via HTTP responses to GET requests.

If file doesn't exist server will return 404 status code.

If file needs a read permission server will return 403 status code.

## Input arguments

Program requires exactly two input arguments in the following order:
 
1. Working directory path;
2. Listening 'address:port';

Example of command line:
```c
/my/directory/path/ 127.0.0.1:1029
```

## Notes

Server uses kqueue interface, so currently it can be used only on BSD (FreeBSD / OpenBSD) and Darwin (Mac OS X / iOS) kernels.

# Client

Client sends GET HTTP requests to the server to receive any files.

When received, all files are saved in output directory path. 
If it's not specified output directory with PID-name is created in current working directory.

## Input arguments

Program requires at least 3 input argument in the following order:

1. Listening 'address:port';
2. -f option to receive a single file or -l option to receive a list of files;
3. Single file name (starting with '/') or file path (if -l specified the corresponding file must have a list of files names);
4. Output directory path (optional);

Example of command line:
```c
127.0.0.1:1029 -l /my/directory/path/all_files_list.txt ./output_directory_path
```
or
```c
127.0.0.1:1029 -f /file_name ./output_directory_path
```