#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST_SERVER "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define PORT 8080

#define URL_REGISTER "/api/v1/tema/auth/register"
#define URL_LOGIN "/api/v1/tema/auth/login"
#define URL_LOGOUT "/api/v1/tema/auth/logout"
#define URL_ACCESS "/api/v1/tema/library/access"
#define URL_BOOKS "/api/v1/tema/library/books"



struct sockaddr *get_addr(char* name)
{
	int ret;
	struct addrinfo hints, *result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	ret = getaddrinfo(name, NULL, &hints, &result);
	if (ret != 0)
	{
		perror("getaddrinfo");
	}

	return result->ai_addr;
}

int main(int argc, char *argv[])
{
    int server_socket;

    // Get server IP
    struct sockaddr_in *serv_addr = (struct sockaddr_in *) get_addr(HOST_SERVER);

    char ip[100];
    memset(ip, 0, 100);
    inet_ntop(AF_INET, &(serv_addr->sin_addr), ip, 100);

    char *cookie;
    char *jwt;

    //char *request;
    //char *response;
    while (1) {
    	server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

    	char reader[15];
    	scanf("%s", reader);

    	if (strncmp(reader, "register", 8) == 0) {

    		// Registe into server
    		//afisez promptul pentru username si parola
    		printf("username=");
    		char username[50];
    		scanf("%s", username);
    		printf("password=");
    		char password[50];
    		scanf("%s", password);

    		JSON_Value *val = json_value_init_object();
    		JSON_Object *obj = json_value_get_object(val);

    		json_object_set_string(obj, "username", username);
    		json_object_set_string(obj, "password", password);

    		char json[BUFLEN];
    		strcpy(json, json_serialize_to_string(val));

    		char *msg = compute_post_request(HOST_SERVER, URL_REGISTER, "application/json", json, NULL);
    		send_to_server(server_socket, msg);
    		memset(msg, 0, BUFLEN);
    		msg = receive_from_server(server_socket);
    		char code[3];
    		memcpy(code, msg + 9, 3);

    		if (strncmp(code, "201", 3) == 0) {
    			printf("Created\n");

    		} else {
    			char *err;
    			err = strtok(msg, "\r\n");
    			while (err && err[0] != '{') {
    				err = strtok(NULL, "\r\n");
    			}

    			if (err) {
    				JSON_Value *v = json_parse_string(err);
    				printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    			}
    		}
    		
    	} else {
    		if (strncmp(reader, "login", 5) == 0) {
    			// Login into server
    			//afisez promptul pentru username si parola

    			printf("username=");
    			char username[50];
	    		scanf("%s", username);
    			printf("password=");
    			char password[50];
    			scanf("%s", password);

	    		JSON_Value *val = json_value_init_object();
    			JSON_Object *obj = json_value_get_object(val);

	    		json_object_set_string(obj, "username", username);
    			json_object_set_string(obj, "password", password);

    			char json[BUFLEN];
    			strcpy(json, json_serialize_to_string(val));

    			char *msg = compute_post_request(HOST_SERVER, URL_LOGIN, "application/json", json, NULL);
	    		send_to_server(server_socket, msg);
    			memset(msg, 0, BUFLEN);
    			msg = receive_from_server(server_socket);
    			char code[3];
	    		memcpy(code, msg + 9, 3);

	    		if (strncmp(code, "200", 3) == 0) {
    				printf("Successful login\n");
    				cookie = malloc(sizeof(char) * 500);
				    memset(cookie, 0, LINELEN);

				    char *start = strstr(msg, "Set-Cookie: ");
    				start += strlen("Set-Cookie: ");
				    char *end = strstr(start, "\r\n");
		    		strncpy(cookie, start, end - start);
    			} else {
    				char *err;
	    			err = strtok(msg, "\r\n");
    				while (err && err[0] != '{') {
    					err = strtok(NULL, "\r\n");
    				}

	    			if (err) {
    					JSON_Value *v = json_parse_string(err);
    					printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    				}
    			}
    		} else {
    			if (strncmp(reader, "enter_library", 13) == 0) {
    				//enter library
    				char *msg = compute_get_request(HOST_SERVER, URL_ACCESS, NULL, cookie);
    				send_to_server(server_socket, msg);
    				memset(msg, 0, BUFLEN);
    				msg = receive_from_server(server_socket);
    				char code[3];
	    			memcpy(code, msg + 9, 3);

	    			if (strncmp(code, "200", 3) == 0) {

	    				printf("Access to library granted!\n");
	    				//retin token ul JWT
	    				char *token = strtok(msg, "\r\n");
		    			while (token && token[0] != '{') {
    						token = strtok(NULL, "\r\n");
    					}
    					JSON_Value *v = json_parse_string(token);
    					jwt = malloc(sizeof(char) * 500);
    					strcpy(jwt, json_object_get_string(json_value_get_object(v), "token"));
    					
	    			} else {
	    				char *err;
		    			err = strtok(msg, "\r\n");
    					while (err && err[0] != '{') {
    						err = strtok(NULL, "\r\n");
    					}

		    			if (err) {
    						JSON_Value *v = json_parse_string(err);
    						printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    					}
    
	    			}

    			} else {
    				if (strncmp(reader, "get_books", 9) == 0) {
    					//get books
    					char *msg = compute_get_request(HOST_SERVER, URL_BOOKS, jwt, NULL);
    					send_to_server(server_socket, msg);
	    				memset(msg, 0, BUFLEN);
    					msg = receive_from_server(server_socket);

    					char code[3];
	    				memcpy(code, msg + 9, 3);

	    				if (strncmp(code, "200", 3) == 0) {
	    					//afisez cartile
	    					char *tok;
			    			tok = strtok(msg, "\r\n");
    						while (tok && tok[0] != '[') {
    							tok = strtok(NULL, "\r\n");
    						}

			    			if (tok) {
    							printf("%s\n", tok);
    						}

	    				} else {
	    					//afisez eroarea
	    					char *err;
			    			err = strtok(msg, "\r\n");
    						while (err && err[0] != '{') {
    							err = strtok(NULL, "\r\n");
    						}

			    			if (err) {
    							JSON_Value *v = json_parse_string(err);
    							printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    						}
    	
	    				}

    				} else {
    					if (strncmp(reader, "get_book", 8) == 0) {
    						//get book id
    						printf("id=");
 				   			char *id = malloc(sizeof(char) * 15);
	    					scanf("%s", id);
	    					char url[50];
	    					strcpy(url, "/api/v1/tema/library/books/");
	    					strcat(url, id);

	    					char *msg = compute_get_request(HOST_SERVER, url, jwt, NULL);
    						send_to_server(server_socket, msg);
	    					memset(msg, 0, BUFLEN);
    						msg = receive_from_server(server_socket);
	    					char code[3];
		    				memcpy(code, msg + 9, 3);

		    				if (strncmp(code, "200", 3) == 0) {
		    					//afisez cartea
		    					char *tok;
			    				tok = strtok(msg, "\r\n");
	    						while (tok && tok[0] != '[') {
    								tok = strtok(NULL, "\r\n");
    							}

    							if(tok) {
    								printf("%s\n", tok);
    							}

		    				} else {
		    					char *err;
			    				err = strtok(msg, "\r\n");
	    						while (err && err[0] != '{') {
    								err = strtok(NULL, "\r\n");
    							}

				    			if (err) {
    								JSON_Value *v = json_parse_string(err);
    								printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    							}
    	
		    				}

    					} else {
    						if (strncmp(reader, "add_book", 8) == 0) {
    							//add a book
    							printf("title=");
				    			char title[50];
	    						scanf("%s", title);
				    			printf("author=");
    							char author[50];
				    			scanf("%s", author);
				    			printf("genre=");
				    			char genre[50];
				    			scanf("%s", genre);
				    			printf("publisher=");
				    			char publisher[50];
				    			scanf("%s", publisher);
				    			printf("page_count=");
				    			char page_count[50];
				    			scanf("%s", page_count);

				    			JSON_Value *val = json_value_init_object();
    							JSON_Object *obj = json_value_get_object(val);

	    						json_object_set_string(obj, "title", title);
    							json_object_set_string(obj, "author", author);
    							json_object_set_string(obj, "genre", genre);
    							json_object_set_string(obj, "publisher", publisher);
    							json_object_set_number(obj, "page_count", atoi(page_count));

    							char json[BUFLEN];
    							strcpy(json, json_serialize_to_string(val));
    							printf("%s\n", jwt);
    							char *msg = compute_post_request(HOST_SERVER, URL_BOOKS, "application/json", json, jwt);
	    						printf("%s\n", msg);
	    						send_to_server(server_socket, msg);
    							memset(msg, 0, BUFLEN);
    							msg = receive_from_server(server_socket);
    							char code[3];
	    						memcpy(code, msg + 9, 3);

	    						if (strncmp(code, "200", 3) == 0) {
	    							printf("Added successfuly\n");
	    						} else {
	    							char *err;
				    				err = strtok(msg, "\r\n");
	    							while (err && err[0] != '{') {
    									err = strtok(NULL, "\r\n");
    								}

					    			if (err) {
    									JSON_Value *v = json_parse_string(err);
    									printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    								}
	    						}

    						} else {
    							if (strncmp(reader, "delete_book", 11) == 0) {
    								printf("id=");
		 				   			char *id = malloc(sizeof(char) * 15);
	    							scanf("%s", id);
	    							char url[50];
	    							strcpy(url, "/api/v1/tema/library/books/");
	    							strcat(url, id);

	    							char *msg = compute_delete_request(HOST_SERVER, url, jwt, NULL);
	    							send_to_server(server_socket, msg);
			    					memset(msg, 0, BUFLEN);
    								msg = receive_from_server(server_socket);

			    					char code[3];
				    				memcpy(code, msg + 9, 3);

				    				if (strncmp(code, "200", 3) == 0) {
				    					printf("Removed successfuly\n");
				    				} else {
				    					char *err;
			    						err = strtok(msg, "\r\n");
	    								while (err && err[0] != '{') {
    										err = strtok(NULL, "\r\n");
    									}

					    				if (err) {
    										JSON_Value *v = json_parse_string(err);
    										printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    									}
    	
		    						}

    							} else {
    								if (strncmp(reader, "logout", 6) == 0) {
    									//logout from server
    									char *msg = compute_get_request(HOST_SERVER, URL_LOGOUT, NULL, cookie);
			    						send_to_server(server_socket, msg);
	    								memset(msg, 0, BUFLEN);
    									msg = receive_from_server(server_socket);

				    					char code[3];
		    							memcpy(code, msg + 9, 3);

					    				if (strncmp(code, "200", 3) == 0) {
		    								printf("Logged out\n");
		    								free(cookie);
		    								free(jwt);
		    							} else {
		    								char *err;
						    				err = strtok(msg, "\r\n");
	    									while (err && err[0] != '{') {
    											err = strtok(NULL, "\r\n");
    										}

							    			if (err) {
    											JSON_Value *v = json_parse_string(err);
    											printf("%s\n", json_object_get_string(json_value_get_object(v), "error"));
    										}
    	
					    				}

    								} else {
    									if (strncmp(reader, "exit", 4) == 0) {
    										//exit
    										close_connection(server_socket);
    										break;
    									}
    								}
    							}
    						}
    					}
    				}
    			}
    		}
    	}
	}


    return 0;
}

