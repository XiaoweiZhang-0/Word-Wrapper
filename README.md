# WordWrapper
This is a part of System Programming Class
###### Name                          NetID
---
Xiaowei Zhang            xz561

Feijiakang Lu              fl294

###### Test strategy
---
For word wrapping
We checked various length of buffer for holding readin chars (from 1 to 200) and various width of text to be wrapped(1 to 80). Also corner case for blank lines and required error condition when a word is longer than the width.

For checking file type
We tested the server configuration, standard input, normal file input at width 10 and 100, and directory input.The conclusion is that all of them are generated.We have not yet found a file type that is neither a regular file nor a directory.We also test whether the end of the program returns correctly if the width is exceeded or not.
