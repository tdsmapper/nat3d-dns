The client deployment is like running any other application.

You can use a file to send input to the executable. The format of the file is very rudimentary and should be exactly as below:

```
192.168.1.20
255.255.255.0
127.0.0.1
80
username
password
domain
```

Where the first two lines represent the local adapter IP and Netmask. The next two lines represent the IP address and port of the listening HTTP server.

The username/password/domain are as registered on the DynDNS web interface. You can create new domains with the client.