# sets the vlan0 ip
ifconfig eth0 down
ifconfig eth0 up
ifconfig eth0 172.16.10.254/24
ifconfig eth0

# sets the vlan1 ip
ifconfig eth1 down
ifconfig eth1 up
ifconfig eth1 172.16.11.253/24
ifconfig eth1

# sets the rc router as the default route
route add default gw 172.16.11.254

# configure router
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts