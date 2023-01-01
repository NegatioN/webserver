# Webserver in C

This is a mini-project made in response to [this comment on HN](https://news.ycombinator.com/item?id=34037547) stating that 
it's pretty easy to write a webserver in C, using only sockets.
I'd never done this before, so I thought I'd give it a try.

> Back in school, our teacher had us write a simple forking server and consult the corresponding man pages (man 2 socket, man 7 socket, man 7 tcp).
It was an acceptable minimal solution to not even look at the client request, respond with a static string containing a dummy response header and HTML page, which absolutely works.
Bonus points for parsing the request path and sending a file back, more extra points if the path was a directory and your server sends back an "index.html" file
further points if it instead generates an HTML directory listing on the fly.

> So, for "resource" I recommend the HTTP example on Wikipedia, as well as "Beej's guide to network programming" (as have others), as well as the man pages for quick reference.

My implementation wouldn't get full marks, but it reads a request, and responds with the contents of a file at an absolute filepath. And it was pretty fun to write.
