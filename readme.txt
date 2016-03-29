CSCE 3530 Computer Networks Programming Assignment #1 ReadMe
Michael Hummer
Approach:
Both the file server and the proxy server are combined into one program. The server opens a connection with a client who then will request an html file of a web page. If the file is not found on the server, the server will open a socket on port 80 of the requested web page’s host. The server will then request the html homepage of the host. The server then caches and sends the host’s response to the client.
If the file is found on the server, the server will send the requested file to the client.
Limitations:
	 When requesting the html file from a remote server (file is not on local server), The program continues to run and must end using ctrl+c
	 Some webpages did not recognize the “GET / HTTP/1.1” request
	 The server must be re-executed for each request
	 The previous limitation requires one to wait for the port specified in the code to reopen after each request
	 the server always opens its connection on port 45455
Compilation Instructions:
	To compile the server’s source code, type the following in a UNIX console:
		gcc -o server server.c
Usage:
	To run the server type the following into the console:
		./server
	To make a request from your web browser:
		http://”server address”:45455/”requested file”
	e.g. where server is on cse01.cse.unt.edu and requesting www.google.com:
		http://129.120.151.94:45455/www.google.com