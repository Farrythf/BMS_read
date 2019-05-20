# BMS_read
This is a C++ version communication tool using Modbus TCP protocol to connect Windows and target device.  
Socket is used to establish the connection, enter the IP address before use.  
## Function
  * Repeated connection with timeout secure.
  * Read all the data from 802 address space.
  * Repeated read with timeout secure.
  * Non-blocking receiving with timeout secure.
## Instruction
Enter the IP address before use.  
The port number is 502 for default.  
Data will be read and wrote to a file call res.out in the root folder.  
