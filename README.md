# IMAIoT - Infrastructure Monitoring Agent for IoT
Um Agente Monitor de Infraestruturas para Ambientes de IoT

# Sobre
O conceito Internet da Coisas (IoT) é caracterizado pela presença de um número muito grande de dispositivos ligados à Internet, o que inclui ambientes urbanos, industriais e de agricultura, em escala mundial. O gerenciamento e monitoramento destes dispositivos, sejam eles virtuais ou físicos, em múltiplas plataformas de hardware e software, representa um grande desafio. Há soluções disponíveis no mercado, porém, específicas para certos domínios e plataformas e, em sua maioria, proprietárias e pouco personalizáveis. Apresentamos o IMAIoT, uma ferramenta para monitoramento de infraestruturas que utiliza protocolos e arquitetura altamente escalável de IoT para disponibilizar suas métricas. A versatilidade da ferramenta permite monitorar desde máquinas reais em um datacenter até dispositivos com como nós de computação em névoa.

# Requisitos

O IMAIoT foi desenvolvido em C++ com o intúito de ser uma ferramenta multi-plataforma. Apesar deste manual utilizar a distribuição Ubuntu 16.04, qualquer distribuição Linux poderá ser utilizada fazendo os ajustes necessários no arquivo automake (para o processo de compilação) e no caminho para o pseudo file system, que pode ser realizado diretamente no arquivo de configuração (imaiot.conf). A instalação pressupõe a instalação mínima do Ubuntu 16.04 Server. Como este manual considera um sistema mínimo, o procedimento será realizado em uma máquina com 2Gbytes de RAM, 5Gbytes de HD e arquitetura i386 (32bits).
Para maior clareza das explicações, todos os comandos serão executados com usuário root, utilizando o comando:

> sudo su

Digite a senha de seu usuário que deve ter poderes de root.

# Obtendo o IMAIoT

Caso seu sistema não possua a ferramenta git, instale a mesma com o seguinte comando:

> apt-get install git

Obtenha a versão atual do IMAIoT:

> git clone https://github.com/heideker/IMAIoT

## Compilação 

Para compilar o IMAIoT são necessários os pacotes de desenvolvimento do Ubuntu. Caso os mesmos não estejam instalados, utilize o seguinte comando:

> apt-get install .....

Entre no diretório onde os fontes foram baixados, no caso IMAIoT e execute o comando make para realizar a compilação do IMAIoT:

> cd IMAIoT
> make all


## Configurações


# Execução e Teste




# Operação em modo Context Broker

## Instalando o Orion Context Broker


## Configurações


Instalação do Docker

Instalação do Docker-compose

Instalação do Orion Context Broker

## Consulta via Orion

Para realizar as consultas ao Orion, utilizaremos sua API REST, e para isso será necessário utilizar o curl. Para instalar o curl:

> #> apt-get install curl  



# Aplicação Web Exemplo

Para utilizar a aplicação web exemplo, basta abrir o arquivo ./samples/imaiotGui.html em qualquer navegador, indicar o endereço do host onde o Orion está sendo executado utililando a URL no formato http://hostOuIP:1026 e clicar no botão de conexão. Selecione um dos dispositivos monitorados que aparece no primeiro bloco e visualize as métricas publicadas pelo mesmo.

 


Create by Alexandre Heideker 
Federal University of ABC - UFABC

IMAIoT is an agent to monitoring a few operational system metrics. It's possible to monitoring system's processes or docker containers and all data may be reported in a log file (in JSON or plain text format) or in an Orion context broaker.
As a future feature, we spected include an actuator behavior, to make some interventions in SO like reboot or re-start an process, and report it's stats by remote command.

version 0.1 - First release
version 0.2 - 2019-02-27 - TCP server support and minors fixes
