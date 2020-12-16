# sets the IP
ifconfig eth0 down
ifconfig eth0 up
ifconfig eth0 172.16.11.1/24
ifconfig eth0 

# adds the route to vlan0 through tux4
route add -net 172.16.10.0/24 gw 172.16.11.253

# defines RC router as the default route
route add default gw 172.16.11.254