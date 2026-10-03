Exercise 1: Basic Fork-Based Server in C
What I did: I wrote a C server (server.c) that creates a socket, binds it to port 8080, and listens for incoming connections. When a client connects, the server calls fork(). The child process handles that one client (reads its message, echoes a reply, closes the connection), while the parent keeps listening for new clients.

Testing: I compiled with gcc server.c -o server and gcc client.c -o client. I ran ./server in one terminal and ./client in another. The client sent "Hello Server," and the server printed Received: Hello Server and replied "Hello from server," which the client displayed.

Observations: The key idea is that fork() lets the server handle many clients at once — each connection gets its own child process, so one slow client doesn't block the others. I noticed the parent process never handles the client directly; it just accepts and forks.

Exercise 2: Fork-Based Server in C++
What I did: I recreated the same architecture in C++ (server.cpp and client.cpp). The logic is identical to Exercise 1 — socket, bind, listen, accept, fork — but using C++ syntax and std::cout for output.

Testing: I compiled with g++ server.cpp -o server and g++ client.cpp -o client. Running them produced the same exchange as Exercise 1, with the server printing Received: Hello C++ Server.

Observations: This shows that socket programming is language-independent — the system calls (socket(), bind(), accept(), fork()) are the same in C and C++. The main difference is just the output and include style.

Exercise 3: Fork with Multiple Messages
What I did: I modified the C server so each child process handles multiple messages in a loop until the client sends "exit." I updated the client to prompt the user for input repeatedly and send each message.

Testing: I ran the server, then the client, and typed several messages like "hello," "hi there," then "exit." The server echoed each message back with an "Echo: " prefix and printed Client disconnected when I typed exit.

Observations: The while(1) loop and the strncmp(buffer, "exit", 4) check are the core of this exercise. The loop keeps the child alive for many messages instead of dying after one. I saw that the echo uses snprintf to build the reply with the "Echo: " prefix.

Exercise 4: Fork with Client Counter
What I did: I extended the C++ server to track the number of active clients using shared memory (shmget/shmat). Each child increments the counter on connection and decrements it on disconnection. The server prints the active count for each new connection.

Testing: I ran the server and connected two clients in separate terminals. The server printed Active clients: 1 after the first connected and Active clients: 2 after the second. When I closed one client, the count went back down.

Observations: Shared memory is what lets all the separate child processes read and update the same counter — each process normally has its own memory, so without shared memory the counter wouldn't be visible across children. I used __sync_fetch_and_add/__sync_fetch_and_sub to make the increments/decrements safe.

Exercise 5: Fork with Timeout
What I did: I modified the C server so a child process terminates if the client doesn't send a message within 10 seconds. I used alarm(10) and a SIGALRM signal handler.

Testing: I ran the server, then started the client but did not type anything. After about 10 seconds, the server printed No message in 10 seconds, closing connection. and the child exited. When I did send a message quickly, the exchange worked normally.

Observations: The alarm() call schedules a SIGALRM signal after 10 seconds, and the handler function timeout_handler runs when it fires, calling exit(0) to close that child. This is a simple way to enforce a timeout without blocking the rest of the server.

Exercise 6: Fork with Error Handling
What I did: I enhanced the C++ server to check the return values of socket(), bind(), listen(), accept(), and fork(). On any failure, I log the error with a timestamp to server_errors.log instead of printing to the console. The server keeps running even if a child errors.

Testing: I ran the server normally and confirmed the client still works. I then checked cat server_errors.log — it was empty during normal operation, which is expected because no errors occurred. I also verified the log format by looking at the log_error() function, which writes [timestamp] message.

Observations: The log_error() helper uses fopen(..., "a") to append, and ctime() to get the current timestamp. Checking each return value means the server fails gracefully and continues rather than crashing. This is more robust than the earlier versions, which assumed every call succeeded.

Exercise 7: Fork with Broadcast
What I did: I modified the C server to maintain a list of active client sockets in an array. When one client sends a message, the server forwards it to all other connected clients via a broadcast() function. I updated the client to receive and display broadcast messages in a loop.

Testing: I ran the server and opened three client terminals. When I typed a message in client 1, it appeared in clients 2 and 3 as Broadcast: <message>. The sender did not receive its own message back.

Observations: The clients[] array and client_count track all active connections. The broadcast() function loops through the list and writes to every socket except the sender. When a client disconnects, its slot is removed by swapping in the last client. This is a simple chat-room-style design.

Overall Observations
Across all seven exercises, I learned how fork() enables a server to handle multiple clients concurrently by giving each connection its own process. I also learned how to make servers more robust (error handling, timeouts) and how to share state between processes (shared memory) and between clients (broadcast). The main challenges were understanding process memory isolation and getting the socket/fork error checking right.# linux-c-lab3
