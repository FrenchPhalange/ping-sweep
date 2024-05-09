# 🌐 Network Ping Sweep Tool

Welcome to the Network Ping Sweep repository! This repository contains two versions of a network ping sweep tool: one written in Python and another in C. These tools are designed to help you scan a network to find live hosts by sending ICMP echo requests.

## 🌟 Features

- **Python Version**: Utilizes the Scapy library to easily create and send ICMP packets, and handle responses.
- **C Version**: Uses raw sockets to manually construct ICMP packets, offering a deeper understanding of network protocols.
- **Network Scanning**: Both tools scan an IP range within a specified network and CIDr to identify which hosts are online.
