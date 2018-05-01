# LinkMe Android Application and Server
======

Saad Khan

saadk408@gmail.com

=====

A neat UI is developed as an Android application using XML. The values needed are IP address and port number of server, username, password and filename. First of all, using the values from the form of IP Address and port number, a socket is established in the client, if there is an error doing so, a connection error is generated. Then, an authorization request is sent to the running server, the client sends the server the username and password. The server checks to see if the client has the correct credentials. If the client receives a pass message, it sends the filename to the server. The server then opens the file, determines the size of it, reads it and puts everything in a buffer. This buffer is sent to the client. The client reads this buffer, byte to byte and generates a string from it, after checking if the file was found. Then, the contents of the file are displayed within the app as a hyperlink with the ability to either copy it or click on it to go to webpage.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Installing

A step by step series of examples that tell you have to get a development env running


In order to run this program, the user must first set up the server on machine through Terminal/Command Prompt. Once that step is
complete, the user can use the LinkMe Android App to retrieve the txt file from the server. The following steps detail how
to run this program.

## Running

### Running the Server:

Server code can be compiled using the Makefile. To run it, simply do:
./server-tcp 5000 user pass 0

where 5000 is the desired port number, user is the username, and the pass is the password. The last element is
0 to run the server once, or 1 to run the server in a loop.



### Running the Client:

The client must be uploaded onto an Android device using the Android SDK, since the application is not on Google Play.
Once the application is running on the device, follow these steps:

1) Enter IP address of Server. 
  - 10.0.2.2 for local emulator, lookup Server IP address for Android device
    - Server IP address can be found hardcoded in Server code

2) Enter Port Number

3) Enter Username

4) Enter Password

5) Enter Filename containing link. 

6) Press Submit

If connection and transfer successful, the message should appear at the bottom. The link should be clickable and will take you to your webpage.
