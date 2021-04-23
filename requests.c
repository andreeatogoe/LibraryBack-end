#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url,char *token, char *cookies)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "GET %s HTTP/1.1", url);

    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    if (cookies != NULL)
    {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie:");
       sprintf(line + strlen(line), " %s;", cookies);
       line[strlen(line) - 1] = '\0';
       compute_message(message, line);
    }

    if (token != NULL)
    {
       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer");
       sprintf(line + strlen(line), " %s", token);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    int content_length = strlen(body_data);
    
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    if (token != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer");
       sprintf(line + strlen(line), " %s", token);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }
    compute_message(message, "");

    compute_message(message, body_data);
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url,char *token, char *cookies)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "DELETE %s HTTP/1.1", url);

    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    if (cookies != NULL)
    {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie:");
       sprintf(line + strlen(line), " %s;", cookies);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    if (token != NULL)
    {
       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer");
       sprintf(line + strlen(line), " %s", token);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}