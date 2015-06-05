# Introduction #

This implements a very simple DynDNS Service over HTTP for NAT3D.


# Details #

## Description ##
[NAT3D](http://code.google.com/p/nattt) requires a DNS server with support for custom record types. Notes on how to set up BIND9 to work with NAT3D are found [here](http://code.google.com/p/nattt/wiki/ResolverSetup).

This project creates a Dynamic DNS update system to work along with NAT3D and the aforementioned BIND9 setup. This project consists of a DynDNS update client and the server setup.

## Server ##
The server is very basic, written in PHP5/MySQL. It uses MySQL to keep track of the

Unlike DynDNS.com which records only one IP address in an "A" record, NAT3D requires the external IP address of the NAT box, internal IP address within the NAT box, as well as the listening port of NAT3D. These are encoded in a TYPE65324 record, described [here](CustomRecord.md).

[Learn](ServerDeployment.md) how to deploy the server.

## Client ##
The client is written in C, works on UNIX/Linux and Windows. It is meant to update the address/domain of one machine.

It polls the local interface as well as the external IP address (via checkip.dyndns.com) for an address change. In case of the local interface, it can detect changes in interface address, or a different interface on the same network. When there is a change of address, it reports the internal and external addresses to the DynDNS Server.

[Learn](ClientDeployment.md) how to deploy the client.