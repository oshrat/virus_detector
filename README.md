# A Virus Detector

Usage: [prog] [path] [signature]

The program gets 2 arguments: 
1. A path to a local folder.
2. A virus signature as a byte sequence.
At first, the program finds all the infected *ELF* files in the folder and its subfolders, and warn the user.
Next, the user asked if he would like to delete the infected files (Y/N) - and the program respons accordingly.

This solution is using the <ftw.h> library, for the dirent based solution contact me by private message.

Hope you'll find that useful,
Oshrat.

