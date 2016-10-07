Compiling & Executing from Eclipse:

Change line 30 of the devkit-launcher.bat from 
"find" to "C:\Windows\System32\find"
This should be the result:
java -version 2>&1 | C:\Windows\System32\find "64-Bit" >nul:

Create the following project structure:

/project
    /include
        accelerometer.h
        gps.h
    /lib
        accelerometer.cxx
        gps.cxx
    /src
        main.cpp
    /doc
        README.md
    /bin
        main
    /utils
    makefile
    
Set the following project preferences:
    C/C++ General
        Paths and Symbols
            Includes
                Add /project/include under GNU C++
            Source Location
                Add /lib and /src
            
 Connect to Edison using root account
 Compile & Run!
 
 
 Compiling & Executing from cmd
 Create the above project structure somewhere (likely in home directory)
 Use WinSCP (or any other SFTP program) to log into user account
    Upload /project/*
 
 Use putty (or any other SSH program) to log into user account
    cd to /project
    make  //compiles project
    make clean //compiles project & removes object files afterwards
    sudo /project/bin/main  //execute project