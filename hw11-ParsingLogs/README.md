# Multithreading parser of web-server logs
Program takes two arguments: path to the directory with web-server log files; and number of threads.
Logs are given in standard (Combined) log format.

Program parses all logs and prints an aggregated statistics:

- Total number of logs;
- Total numbers of bytes returned;
- Top-10 the most weighty URL's;
- Top-10 the most commonly used 'referers';

# Arguments:
Program requires exactly two input arguments in the following order:

1. Input directory path;
2. Number of threads to parallel the program;

# Dependencies:
The program uses Homework-3 MyHashMap library, so you should also build this.

# Implementation:
Each thread is actively trying to get the mutex over any file.

If thread gets the mutex then:

1. It reads K (by default 1000) lines from the file.
2. Unlocks the mutex. After this step any other thread can lock the same file and read next lines in it.
3. Thread starts to process the lines: parsing and analyzing.
4. When everything is done thread tries to get the mutex over any file again, while there is at least one not finished file.
5. If all files are finished thread returns the aggregated statistics about logs he has read.

When all threads are finished, all results are combined.
