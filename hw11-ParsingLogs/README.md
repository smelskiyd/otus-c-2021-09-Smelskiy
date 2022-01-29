Each thread is actively trying to get the mutex over any file.

If thread gets the mutex then:

1. It reads 'K' lines from the file
2. Unlocks the mutex
3. Thread starts to process the lines: parsing and analyzing
4. When everything is done thread tries to get the mutex over any file again

