GTKTERM:

-------REBOOT DO SWITCH--------------

1. abrir gtkterm (passos até ao 9 aqui)
2. enter para confirmar
3. en
4. password: 8nortel
5. configure terminal
6. no vlan 2-4094
7. exit
8. copy flash:gnu1-clean startup-config
9. reload
10. fazer updateimage nos pcs 2,3 e 4 (no terminal)


---------RESET DO ROUTER (com a porta do router em vez da do switch)-------------
1. enable
2. copy flash:gnu1-clean startup-config
3. reload

-------CONFIGURAR ROUTER--------------
1. en
2. conf t
3. interface fastEthernet 0/0
4. ip address 172.16.11.254 255.255.255.0 
5. no shutdown
(6. ip nat inside)
7. exit
8. show interface fastEthernet 0/0

9. interface fastEthernet 0/1
10. ip address 172.16.1.19 255.255.255.0
11. no shutdown
(12. ip nat outside)
13. exit

(

> ip nat pool ovrld 172.16.1.19 172.16.1.19 prefix 24
> ip nat inside source list 1 pool ovrld overload
> access-list 1 permit 172.16.10.0 0.0.0.7
> access-list 1 permit 172.16.11.0 0.0.0.7
> ip route 0.0.0.0 0.0.0.0 172.16.1.254
> ip route 172.16.10.0 255.255.255.0 172.16.11.253
> end

)


-------------CRIAR VLAN 0----------

1. configure terminal
2. vlan 10
3. end
4. show vlan id 10

---ADICIONAR PORTAS DO TUX3 à vlan 0---

1. conf t
2. interface fastEthernet 0/<'porta a que está ligado o cabo gnu13 Eth0'>
3. switchport mode access
4. switchport access vlan  10
5. end (para sair do config-if)
6. show vlan brief 



---ADICIONAR PORTAS DO TUX3 à vlan 0---

1. conf t
2. interface fastEthernet 0/<'porta a que está ligado o cabo gnu14 Eth0'>
3. switchport mode access
4. switchport access vlan  10
5. end (para sair do config-if)
6. show vlan brief 



------------CRIAR VLAN 1----------

1. conf t
2. vlan 11
3. end
4. show vlan id 11


---ADICIONAR PORTAS DO TUX4 à vlan 1---

1. conf t
2. interface fastEthernet 0/<'porta a que está ligado o cabo gnu14 Eth1'>
3. switchport mode access
4. switchport access vlan  11
5. end (para sair do config-if)
6. show vlan brief 


---ADICIONAR PORTAS DO TUX2 à vlan 1---

1. conf t
2. interface fastEthernet 0/<'porta a que está ligado o cabo gnu12 Eth0'>
3. switchport mode access
4. switchport access vlan  11
5. end (para sair do config-if)
6. show vlan brief 



---ADICIONAR ROUTER à vlan 1---

1. conf t
2. interface fastEthernet 0/<'porta a que está ligado o cabo rc FE/0'>
3. switchport mode access
4. switchport access vlan  11
5. end (para sair do config-if)
6. show vlan brief --> Deve aparecer uma VLAN0010 com estado active e uma porta 0/<'nr da porta adicionada'>