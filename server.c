int server_fd;
struct sockaddr_in server_addr;

//create server socket
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
    ?
}

//config socket
server_addr.sin_family = AF_INET; //use IPv4
server_addr.sin_addr.s_addr = INADDR_ANY; //accepts connections from anywhere
server_addr.sin_port = htons(PORT);
//htons makes sure that numbers are stored in memory in network byte order

//bind socket to port
if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}


//start listening for connections
if (listen(server_fd, 10) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
}

while (1) {
    //client info data struct - contains IP address and port number of client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int *client_fd = malloc(sizeof(int)); //memory leak over here

    //accept client connection
    if ((*client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
        perror("accept failed");
        continue;
    }

    //create new thread to handle client request
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
    pthread_detach(thread_id); //makes it so we don't need to join with this thread whne it terminates

}

















void *handle_client(void *arg) {
    int client_fd = *((int *) arg);
    char *buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));

    //receive request data from client and store into buffere
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        //check if request is GET
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        //reg comp sets the regex
        //^ is the start of the string
        //[] matches any single character insidee the brackets
        //GET /index.html HTTP/1.1 would match index.html (^ excludes any character inside of the brackets, which is just a space)
        //* matches 0 or more occurences of the preceding pattern
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0) {
            //extract filename from request and decode URL
            buffer[matches[1].rm_eo] = '\0'; //rm_eo is where the ending part of the regex should have a null terminator
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);

            //get file extension
            char file_ext[32];
            strcpy(file_ext, get_file_extension(file_name));

            //build HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            build_http_response(file_name, file_ext, response, &response_len);

            //send HTTP response to client
            send(client_fd, response, response_len, 0);

            free(response);
            free(file_name);

        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}