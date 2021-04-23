#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(char *host, char *url, char *token, char *cookies);

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char* token);

char *compute_delete_request(char *host, char *url, char *token, char *cookies);

#endif
