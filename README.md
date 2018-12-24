# A Virus Detector

Usage: [prog] [path] [signature]
The program gets 2 arguments: 
1. A path to a local folder.
2. A byte sequence as a virus signature.
At first, the program finds all the infected *ELF* files in the folder and its subfolders, and warn the user.
Next, the user asked if he would like to delete the infected files (Y/N) - and the program respons accordingly.


