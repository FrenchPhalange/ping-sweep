import sys
from scapy.all import ICMP, IP, sr1
from netaddr import IPNetwork

def ping_sweep(network, netmask):
    ip_network = IPNetwork(f"{network}/{netmask}")
    live_hosts = []

    print("Starting ping sweep...")
    for scanned_hosts, host in enumerate(ip_network.iter_hosts(), 1):
        print(f"Scanning: {scanned_hosts}/{len(ip_network)}", end="\r")
        response = sr1(IP(dst=str(host))/ICMP(), timeout=1, verbose=0)
        if response is not None:
            live_host = str(host)
            live_hosts.append(live_host)
            print(f"\nHost {live_host} is online.")

    return live_hosts

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python dos_blocker.py <network> <netmask>")
        sys.exit(1)

    network = sys.argv[1]
    netmask = sys.argv[2]

    live_hosts = ping_sweep(network, netmask)
    print("\nCompleted\n")
    print(f"Live hosts: {live_hosts}")
