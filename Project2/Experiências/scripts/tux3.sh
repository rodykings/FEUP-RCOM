# sets the IP
ifconfig eth0 down
ifconfig eth0 up
ifconfig eth0 172.16.10.1/24
ifconfig eth0

route add -net 172.16.11.0/24 gw 172.16.10.254

# adds the route to vlan1 through tux4
# route add -net 172.16.31.0/24 gw 172.16.30.254 # (unecessary bc tux4 will be added after as default route)

# sets the tux4 as the default route
route add default gw 172.16.10.254

# adds the DNS
echo -e 'search netlab.fe.up.pt\nnameserver 172.16.1.1' > /etc/resolv.conf