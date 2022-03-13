## Steps for using FTAP

 - Every command will be a single string
 - Any input will be asked explicitly after we enter the command
 - First use START command to make sure connection is established
 - Use USERN command to enter the user name. PASSWD command will be prompted as soon as you enter USERN
 - Similarly ListDir used to list all the files in the Directory
 - StoreFile used to send the file in our client side to the server side
 - GetFile can be used to download any file that got listed using ListDir command


 - FTAPServer should have logincred.txt file with all the login credentials
 - we should keep a filesystem.txt file that will contain all the file info in our server directory

 - In addition to the commands in the question I have implemented Del command to remove file from filesystem of the server


## Directory Structure

.
├── FTAPClient
│   ├── FTAPClient.c ------------------ Client program
│   └── sample.mkv -------------------- video file to illustrate StoreFile (sending file from client to server)
├── FTAPServer
│   ├── filesystem.txt ---------------- Server filesystem that maintains all the file names in server (like inode table in Operating system)
│   ├── FTAPServer.c ------------------ Server program
│   ├── logincred.txt ----------------- Comma separated username,password
│   └── sample.png -------------------- image file to illustarte GetFile (downloading file from server). This file is already entered to the filesystem.txt
├── OUTPUT
│   └── output.png -------------------- output image that illustartes code execution
└── README.md

