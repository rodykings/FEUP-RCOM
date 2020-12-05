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

--------EXPERIENCIA 1----------------

***PC3*** 
1. ifconfig eth0 up
2. ifconfig eth0 172.16.10.1/24
3. ifconfig eth0 

IP: 172.16.10.1

MAC: 


***PC4***

4. ifconfig eth0 up
5. ifconfig eth0 172.16.10.254/24
6. ifconfig eth0 

IP: 172.16.10.254

MAC: 

***PC3***

7. ping <'ip pc3'>	-> recebe pacotes de 64 bytes desse endereço

***PC4***

8. ping <'ip pc4'>	-> recebe pacotes de 64 bytes desse endereço


***PC3***

9. route -n
	NESTE PC: 
    	- Destination 172.16.10.0
    	- Iface eth0

10. arp -a	 	-> Associa Endereço IP a um Endereço MAC
11. arp -d ipPC4
12. arp -a [retorna nada]

--------EXPERIENCIA 2----------------

ACRESCENTAR CABO ETH0 do tux2

--> Se for cumulativa não é preciso reconfigurar 3 e 4!!!

***PC3***

1. ifconfig eth0 up
2. ifconfig eth0 172.16.10.1/24
3. ifconfig eth0 

IP: 172.16.10.1

MAC: 

***PC4***

4. ifconfig eth0 up
5. ifconfig eth0 172.16.10.254/24
6. ifconfig eth0

IP: 172.16.10.254

MAC: 


***PC2***

1. ifconfig eth0 up
2. ifconfig eth0 172.16.11.1/24
3. ifconfig eth0 

IP: 172.16.11.1

MAC: 00:21:5a:5a:7e:51


***PC3***
4. Abrir gtkterm e clicar enter para confirmar que está ligado
5. Fazer login se for preciso (com en)

---CRIAR VLAN 0---

6. configure terminal
7. vlan 10
8. end
9. show vlan id 10

---(ADICIONAR PORTAS DO TUX3 e TUX4 à vlan 10)---

10. conf t
12. interface fastEthernet 0/<'porta a que está ligado o cabo gnu13 Eth0'>
13. switchport mode access
14. switchport access vlan  10
15. end (para sair do config-if)
16. show vlan brief --> Deve aparecer uma VLAN0010 com estado active e uma porta 0/<'nr da porta adicionada'>

(REPETIR OS MESMOS PASSOS MAS PARA O TUX4 - altera só o nr da porta a que está ligado o cabo gnu14 Eth0)


---CRIAR VLAN 1---

17. conf t
18. vlan 11
19. end
20. show vlan id 11

---ADICIONAR PORTAS TUX2 à vlan 11)--- 

(semelhante às portas anteriores)

21. Log no tuxy3 com ping do tux4 e tux2 (no 2 dá network is unreachable porque está numa vlan diferente da do tux3 onde fazemos o ping)

22. Logs broadcast tux3 (3 ficheiros - pc2, pc3 e pc4)
23. Logs broadcast tux2 (3 ficheiros)


--------EXPERIENCIA 3----------------

***PC3***

1. ifconfig eth0 up
2. ifconfig eth0 172.16.10.1/24
3. ifconfig eth0

IP: 172.16.10.1

MAC: 00:21:5a:5a:7d:16


***PC4***

4. ifconfig eth0 up
5. ifconfig eth0 172.16.10.254/24
6. ifconfig eth0 

IP: 172.16.10.254

MAC: 00:21:5a:5a:7b:3f

7. ifconfig eth1 up
8. ifconfig eth1 172.16.11.253/24
9. ifconfig eth1 

IP: 172.16.11.253

MAC: 00:40:f4:6f:b6:a5

***PC2***

10. ifconfig eth0 up
11. ifconfig eth0 172.16.11.1/24
12. ifconfig eth0 

IP: 172.16.11.1

MAC: 00:21:5a:5a:7e:51


