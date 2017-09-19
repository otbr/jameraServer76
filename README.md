# Custom Tibia OT server version 7.6

Server for tibia, based on version 7.6. This server is with the source code compiled for Linux, however it can be easily compiled for windows.

Server with monster, items, npcs and own missions

- Utilities Link for editing and to a virtual machine (Vitural Box) with the server cloned, installed, running:
https://mega.nz/#F!kToiUCBB!bQ5z7P_pR2w90AAMLnCjdQ!saZCBawZ
user: ubuntuserver
password: pinacles

## Instructions to open the server:

To open the server follow these steps:
- git clone https://github.com/brunoofgod/jameraServer76;
- Import database db.sql on mysql server 
- cd jameraServer76;
- vi config.lua;
- edit the lines 97 ~ 103 with your data;
- ./otserv
- follow the steps to configure the client on the link: https://github.com/brunoofgod/jameraClient76



## Instructions to open the server in the virtual machine (easy);

- Open the virtual machine in the Virtual Box software;
- User: ubuntuserver / Password: pinacles
- cd /servers/classicrpg-server
- vi config.lua;
- edit the lines 97 ~ 103 with your data;
- ./otserver
- follow the steps to configure the client on the link: https://github.com/brunoofgod/jameraClient76
