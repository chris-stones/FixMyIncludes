FixMyIncludes
=============
  
Tool to help in porting c/c++ software from windows to GNU/Linux and other case sensitive operating systems. 
  
Under windows, your #include statements can have back-slashes, use in-correct case, and mix up <> and "".  
This tool attempts to correct these faults for use under GNU/Linux and UNIX.
  
Run the tool, specifying your include search path, and your source root.
e.g:  
  fixmyincludes -I /usr/include \  
                -I /usr/local/include \  
                -I /usr/lib64/gcc/x86_64-pc-linux-gnu/4.7.3/include/g++-v4/ \  
                -I /usr/include/boost-1_49 \  
                path/to/your/source/  
  
This is a work in progress... It works, but may be buggy.
Make sure your work is backed up!
  
KNOWN BUGS:
  This tool cannot yet handle "../" in your includes... they are ugly anyway, dont use them!  

