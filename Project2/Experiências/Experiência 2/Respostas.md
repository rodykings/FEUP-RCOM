**1. How to configure vlany0?**

- Ligação ao switch
  » Porta série /dev/ttyS0 em tuxy3, gtkterm

- Aceder ao switch:
  
  » enable
  
  » password: 8nortel

- Criação das vlans (gtkterm):
  » configure terminal
  
  » vlan 10 (vlan0)
  
  » end
  
  » show vlan id 10

- Adicionar as portas do tuxy3 e do tuxy4 à respetiva vlan:
  
  » configure terminal
  
  » interface fastethernet 0/port (sendo port o número da porta no switch à qual o computador em questão está ligado)
  
  » switchport mode access
  
  » switchport access vlan 10
  
  » end
  
  » show vlan brief (é mostrada uma VLAN0010 com estado 'active' e uma porta 0/port, sendo port o número da porta previamente associada)

- Remover a vlan no final do procedimento:
  
  » configure terminal
  
  » no vlan 10
  
  » end
  
  » show vlan brief

**2. How many broadcast domains are there? How can you conclude it from the logs?**

Existem 2 domínios de broadcast já que ao fazer broadcast apenas abrange as portas pertencentes a essa virtual lan, tal como foi visto na experiência e nos respetivos logs: no tuxy3, quando é feito o ping tuxy2, é apresentado um erro "network is unreachable" pois este está num domínio diferente do tuxy3. Já o mesmo não acontece quando, também no tuxy3, é feito ping tuxy4 pois ambos pertencem ao mesmo domínio de broadcast.
