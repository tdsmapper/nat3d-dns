# Introduction #

## Sample NSUpdate Query ##
```
nsupdate
> server xyz.com
> zone nattt
> update add 5.nattt. 1 TYPE65324 \# 10 xx xx xx xx 26 94 0a 00 00 05     
> show
> send
```

Where `5.nattt` is the domain name.

`\# 10 xx xx xx xx 26 94 0a 00 00 05     `

`10` represents size of following data. `xx xx xx xx` is external IP. `26 94` is the port number 9876. `0a 00 00 05` is the address within the NAT.

## Sample DNS Record ##

b.nattt. IN TYPE65324 \# 10 xx xx xx xx 00 67 0a 00 00 c6

An sample DNS zone file is found [here](http://code.google.com/p/nattt/wiki/SampleZoneFile)