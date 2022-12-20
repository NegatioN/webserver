#include<stdio.h>
#include<sys/socket.h>

//https://news.ycombinator.com/item?id=34037547
// relevant man pages:
// man 3 getaddrinfo
// man 2 socket
// man 7 socket
// man 7 tcp
/*

The HTTP 1.x protocol itself is pretty dead simple (if you ignore chunked requests/responses and such) and the more interesting part was actually the socket code.

Back in school, our teacher had us write a simple forking server and consult the corresponding man pages (man 2 socket, man 7 socket, man 7 tcp). It was an acceptable minimal solution to not even look at the client request, respond with a static string containing a dummy response header and HTML page, which absolutely works. Bonus points for parsing the request path and sending a file back, more extra points if the path was a directory and your server sends back an "index.html" file, further points if it instead generates an HTML directory listing on the fly.

So, for "resource" I recommend the HTTP example on Wikipedia, as well as "Beej's guide to network programming" (as have others), as well as the man pages for quick reference.
 */

int main() {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sockfd, s;

    // this is copied from man 3 getaddrinfo
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    s = getaddrinfo(NULL, 8080, &hints, &result);
    printf("%d", s);


    int sockfd = socket(AF_UNSPEC, SOCK_DGRAM, 0); // man 2 socket says there is usually only one viable protocol, so inputting 0 should be fine (?)


    //TODO see when someone has written something to the socket
    //TODO do TCP handshakes?
    //TODO write a http response into the socket?
    printf("%d\n", sockfd);
    return 0;
}
