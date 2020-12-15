**1. What are the ARP packets and what are they used for?**

O Protocolo de Resolução de Endereços (ARP) é um padrão de telecomunicação responsável por mapear um endereço de rede (como um endereço IPv4) de uma máquina num endereço físico (como um endereço Ethernet ou endereço MAC) de outra máquina na rede local.
Assim, quando uma máquina tenta enviar um pacote a outra dentro da mesma rede local, enviará um pacote ARP, por broadcast, para todas as máquinas ligadas a essa mesma rede e, simultaneamente, "questionando" qual das máquinas tem um endereço MAC correspondente ao endereço IP do destinatário. Por sua vez, o destinatário envia outro pacote ARP que indica ao computador emissor qual o seu endereço MAC. Consequentemente, a transferência de pacotes passa a poder ser efetuada.

**2. What are the MAC and IP addresses of ARP packets and why?**

Quando o tuxy3 tenta enviar um pacote ao tuxy4, como a entrada da tabela ARP referente ao tuxy4 foi apagada, o tuxy3 não sabe qual é o endereço MAC associado ao endereço IP do tuxy4 (172.16.10.254). Deste modo, irá enviar um pacote ARP para toda a rede local, sendo que este pacote contém o seu endereço IP (172.16.10.1) e o seu endereço MAC (??). O endereço MAC do destinatário, sendo desconhecido, assume o valor de 00:00:00:00:00:00.

De seguida, o tuxy4 irá enviar um pacote ARP para o tuxy3, com o endereço MAC dele (??), e o seu endereço IP (172.16.10.254).

Portanto, pode-se concluir que cada pacote ARP contém campos para os endereços MAC e IP da máquina que envia, e para os endereços MAC e IP da máquina que recebe.

**3. What packets does the ping command generate?**

O comando ping permite testar a conectividade, ao nível do endereço IP, entre equipamentos.
Este gera primeiro pacotes ARP para saber qual o endereço MAC do destinatário e, posteriormente, pacotes ICMP (Internet Control Message Protocol).

**4. What are the MAC and IP addresses of the ping packets?**

Quando é efetuado um ping do tuxy3 para o tuxy4, os endereços dos pacotes enviados são os seguintes:

- Pacote request (de tuxy3 para tuxy4):

  - IP address da source: 172.16.10.1 (tuxy3)

  - MAC address da source: ?? (tuxy3)

  - IP address do destinatário: 172.16.10.254 (tuxy4)

  - MAC address do destinatário: ?? (tuxy4)

- Pacote reply (de tuxy4 para tuxy3):

  - IP address da source: 172.16.10.254 (tuxy4)

  - MAC address da source: ?? (tuxy4)

  - IP address do destinatário: 172.16.10.1 (tuxy3)

  - MAC address do destinatário: ?? (tuxy3)

**5. How to determine if a receiving Ethernet frame is ARP, IP, ICMP?**

Conseguimos determinar o tipo de frame recetora através do Ethernet header da mesma.

Se o seu valor for 0x0800, significa que a trama é do tipo IP. Se for 0x0806, a trama é do tipo ARP.

Caso a trama seja do tipo IP, podemos analisar o seu IP header. Se este tiver valor igual a 1, então o tipo de protocolo é ICMP.

**6. How to determine the length of a receiving frame?**

O comprimento de uma frame recetora pode ser visualizado no software Wireshark.

**7. What is the loopback interface and why is it important?**

O loopback pode ser um canal de comunicação com apenas um ponto final de comunicação. Assim, qualquer mensagem transmitida por meio desse canal é imediatamente recebida pelo mesmo canal.

A interface loopback é uma interface virtual da rede que permite ao computador receber respostas de si próprio, para testar a correta configuração da carta de rede.
