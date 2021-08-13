# Client Server Model 
A small client server model representing a shopping store, where multiple clients can access and query the server. 

## Requirements
Considering the centralized database server; you are provided by the sample dataset of the shopping store which will be on the server. Client can access that server and ask the query about the different invoice number and their respective values available in the given dataset. Server should maintain a log of access time of clients.

You need to create a connectionless client server model in which client will establish a connection with server. Client will ask about the specific invoice number and its attribute those client want to know. Server will send back that respective information to the client. The client can ask more than one query in a single connection.
If a client wants to delete some record, then one authentication message should be sent to all other clients, if they allow it then he/she can delete the record otherwise you can  print NOT ALLOWED. After record deletion a message about update of file should be sent to every client.

Client can do following tasks.
- Reading
- Delete some record (after permission from other clients).
- Add new record (Provide update message to all other clients).
- Client can request log of server access.
