
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

//https://news.ycombinator.com/item?id=34037547
// relevant man pages:
// man 3 getaddrinfo
// man 2 socket
// man 7 socket
// man 7 tcp
/*

The HTTP 1.x protocol itself is pretty dead simple (if you ignore chunked requests/responses and such) and the more interesting part was actually the socket code.

Back in school, our teacher had us write a simple forking server and consult the corresponding man pages (man 2 socket, man 7 socket, man 7 tcp).
It was an acceptable minimal solution to not even look at the client request, respond with a static string containing a dummy response header and HTML page, which absolutely works.
Bonus points for parsing the request path and sending a file back, more extra points if the path was a directory and your server sends back an "index.html" file
further points if it instead generates an HTML directory listing on the fly.

So, for "resource" I recommend the HTTP example on Wikipedia, as well as "Beej's guide to network programming" (as have others), as well as the man pages for quick reference.
 */
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 1024*2*2*2   // how many pending connections queue will hold


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main() {
   int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd, numbytes received
   struct addrinfo hints, *servinfo, *p;
   struct sockaddr_storage their_addr; // connector's address information
   socklen_t sin_size;
   struct sigaction sa;
   int yes=1;
   char s[INET6_ADDRSTRLEN];
   int rv;
   char buf[MAXDATASIZE];

   // START connection options
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC; // Dont care if IPv4 or IPv6
   hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM for TCP, SOCK_DGRAM for UDP
   hints.ai_flags = AI_PASSIVE; // use my IP

   sa.sa_handler = sigchld_handler; // reap all dead processes
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   // END connection options

   if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
       return 1;
   }

   // loop through all the results and bind to the first we can
   for(p = servinfo; p != NULL; p = p->ai_next) {
       if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
           perror("server: socket");
           continue;
       }

       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }

       if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
           close(sockfd);
           perror("server: bind");
           continue;
       }

       break;
   }

   freeaddrinfo(servinfo); // all done with this structure

   if (p == NULL)  {
       fprintf(stderr, "server: failed to bind\n");
       exit(1);
   }

   if (listen(sockfd, BACKLOG) == -1) {
       perror("listen");
       exit(1);
   }

   if (sigaction(SIGCHLD, &sa, NULL) == -1) {
       perror("sigaction");
       exit(1);
   }

   printf("server: waiting for connections...\n");

   while(1) {  // main accept() loop
       sin_size = sizeof their_addr;
       new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
       if (new_fd == -1) {
           perror("accept");
           continue;
       }

       inet_ntop(their_addr.ss_family,
           get_in_addr((struct sockaddr *)&their_addr),
           s, sizeof s);
       printf("server: got connection from %s\n", s);

       // Receieve data from the client
       if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
               perror("recv");
               exit(1);
           }
       buf[numbytes] = '\0'; // null terminate the recieved string
       printf("Received '%s'\n", buf);

       // Send response to client
       if (!fork()) { // this is the child process
           close(sockfd); // child doesn't need the listener
           char *msg = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-length: 45\n\n<!doctype html><html><head>HEYO</head></html>\r";
           //printf("%d\n", strlen(msg));

           if (send(new_fd, msg, strlen(msg), 0) == -1) perror("send");
           close(new_fd);
           exit(0);
       }
       close(new_fd);  // parent doesn't need this
   }

   return 0;


    //sockfd = socket(AF_UNSPEC, SOCK_DGRAM, 0); // man 2 socket says there is usually only one viable protocol, so inputting 0 should be fine (?)


    //TODO see when someone has written something to the socket
    //TODO do TCP handshakes?
    //TODO write a http response into the socket?
}
