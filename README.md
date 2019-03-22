# IMAIoT - Infrastructure Monitoring Agent for IoT
Um Agente Monitor de Infraestruturas para Ambientes de IoT

# Sobre
O conceito Internet da Coisas (IoT) é caracterizado pela presença de um número muito grande de dispositivos ligados à Internet, o que inclui ambientes urbanos, industriais e de agricultura, em escala mundial. O gerenciamento e monitoramento destes dispositivos, sejam eles virtuais ou físicos, em múltiplas plataformas de hardware e software, representa um grande desafio. Há soluções disponíveis no mercado, porém, específicas para certos domínios e plataformas e, em sua maioria, proprietárias e pouco personalizáveis. Apresentamos o IMAIoT, uma ferramenta para monitoramento de infraestruturas que utiliza protocolos e arquitetura altamente escalável de IoT para disponibilizar suas métricas. A versatilidade da ferramenta permite monitorar desde máquinas reais em um datacenter até dispositivos com como nós de computação em névoa.

# Requisitos

O IMAIoT foi desenvolvido em C++ com o intúito de ser uma ferramenta multi-plataforma. Apesar deste manual utilizar a distribuição Ubuntu 16.04, qualquer distribuição Linux poderá ser utilizada fazendo os ajustes necessários no arquivo automake (para o processo de compilação) e no caminho para o pseudo file system, que pode ser realizado diretamente no arquivo de configuração (imaiot.conf). A instalação pressupõe a instalação mínima do Ubuntu 16.04 Server. Como este manual considera um sistema mínimo, o procedimento será realizado em uma máquina com 2Gbytes de RAM, 5Gbytes de HD e arquitetura de 64bits. 
* Obs. O uso de arquitetura de 64bits é necessário apenas para executar a instância do Orion, ou seja, os dispositivos monitorados pelo IMAIoT NÃO estão limitados a esta arquitetura.

Para maior clareza das explicações, todos os comandos serão executados com usuário root, utilizando o comando:

```
~> sudo su
```

Digite a senha de seu usuário que deve ter poderes de root.

# Obtendo o IMAIoT

Caso seu sistema não possua a ferramenta git, instale a mesma com o seguinte comando:

```
#> apt-get install git
```

Obtenha a versão atual do IMAIoT:

``` 
#>git clone https://github.com/heideker/IMAIoT
```

## Compilação 

Para compilar o IMAIoT são necessários os pacotes de desenvolvimento do Ubuntu. Caso os mesmos não estejam instalados, utilize o seguinte comando:

````
#> apt-get install make gcc libcurl4-openssl-dev g++
````

Entre no diretório onde os fontes foram baixados, no caso IMAIoT e execute o comando make para realizar a compilação do IMAIoT:

````
#> cd IMAIoT
#IMAIoT> make all
````


## Configurações

O arquivo imaiot.conf contém as variáveis de configuração para o agente. Estas variáveis são descritas na tabela abaixo:

Variável |	Descrição
---------|-------------
debugMode = 0 | [boleano] Quando o valor desta variável é igual a 1, o IMAIoT apresenta o valor de todas as suas variáveis em tela além das operações realizadas e resultados obtidos para depuração do agente.
NodeName = testeIMAIT | [texto] Esta variável configura um nome amigável para o agente.
NodeUUID = urn:ngsi-ld:999 | [texto] Esta variável configura o UUID utilizado no context broker para identificar unicamente o agente.
KindOfNode = LoRaGateway | [texto] Identifica o tipo de nó monitorado para simplificar a consulta do mesmo na plataforma de IoT
SampplingTime = 5 | [inteiro] Intervalo de amostragem das métricas em segundos
LogMode = 0 | [boleano] Quando o valor da variável for igual a 1 o IMAIoT registra o log local com as métricas
LogType = txt | [txt | json] Identifica o formato do arquivo de log, se texto separado por ponto e virgula (txt) ou json
LogFileName = imaiot.log | [texto] Nome do arquivo de log. Pode conter o caminho completo até o arquivo destino
LogIntervall = 5 | [inteiro] Intervalo de registro das métricas no arquivo de log, em segundos
ServerMode = 1 | [boleano] Ativa o socket TCP para consulta ao IMAIoT quando o valor for igual a 1
ServerPort = 5999 | [inteiro] Porta utilizada para a abertura do socket TCP
OrionMode = 0 | [boleano] Ativa a publicação das métricas coletadas no context broker quando o valor for igual a 1
OrionHost = http://hostname | [texto] URL (hostname ou endereço IP) para o context broker
OrionPort = 1026 | [inteiro] Número da porta para o context broker
OrionPublisherTime = 30 | [inteiro] Intervalo entre as publicações de métricas no context broker, em segundos
DockerStat = 0 | [boleano] Ativa a coleta de estatísticas sobre containers dockers quando o valor for igual a 1
DockerNames = * | [texto] Lista de nomes de containers, separados por espaço ou o coringa (*) para coletar as estatísticas de todos os containers em execução na máquina
ProcessNames = bash apache | [texto] Lista de nomes de processos do sistema operacional separados por espaço.
CPUStat = 1 | [boleano] Coleta estatísticas de uso de CPU quando o valor for igual a 1
CPUPathStat = /proc/stat | [texto] Caminho até o pseudo arquivo de CPU (de acordo com a versão do Linux do hospedeiro)
CPUPathArch = /proc/cpuinfo | [texto] Caminho até o pseudo arquivo arquitetura (de acordo com a versão do Linux do hospedeiro)
NetworkStat = 0 | [boleano] Coleta estatísticas de rede quando o seu valor for igual a 1
NetworkPathStat = /proc/net | [texto] Caminho até o pseudo arquivo rede (de acordo com a versão do Linux do hospedeiro)

Salvo onde explicitamente apontado nenhuma modificação nestas variáveis é necessária para a execução dos exemplos aqui descritos.

# Execução e Teste

Após configurar o arquivo imaiot.conf, basta executar o programa imaiot colocando o mesmo em segundo plano:

````
#> ./imaiot &
````

Com as configurações padrão, o IMAIoT irá gerar arquivos de log em formato TXT com o nome imaiot.log. Para acompanhar o registro das métricas no arquivo de log, execute o comando:

````
#> tail -f imaiot.log
````

Para testar a comunicação via socket TCP, execute o comando telnet:

````
#> telnet localhost 5999
````

O resultado deve apresentar algo similar a:

```
Trying ::1...
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
{"id":"urn:ngsi-ld:999999", "type":"IMAIoT", "MFType":{"type":"Text", "value":"LoRaGateway"}, "Architecture":{"type":"Text", "value":"Intel R  Core TM  i5-7267U CPU @ 3.10GHz"},"MemorySize":{"type":"Integer", "value": 2111639552},"MemoryAvailable":{"type":"Integer", "value": 1955917824},"LocalTimestamp":{"type":"Integer", "value": 1552960118},"SampplingTime":{"type":"Integer", "value": 5},"CPU":{"type":"Integer", "value": 93},"Storage":[],"NetworkStats":{"TCPrxQueue":0, "TCPtxQueue":0, "TCPMaxWindowSize":10, "UDPrxQueue":0, "UDPtxQueue":0},"NetworkAdapters":[{"name":"enp0s3", "rxBytes":14097, "rxPackets":130, "rxErrors":0, "txBytes":11685, "txPackets":88, "txErrors":0},{"name":"lo", "rxBytes":13296, "rxPackets":176, "rxErrors":0, "txBytes":13296, "txPackets":176, "txErrors":0},{"name":"lo", "rxBytes":13296, "rxPackets":176, "rxErrors":0, "txBytes":13296, "txPackets":176, "txErrors":0}],"Process":[{"type":"system", "pid":1130, "name": "bash", "memory": 4464, "cpu":0},{"type":"system", "pid":1178, "name": "bash", "memory": 4388, "cpu":0},{"type":"system", "pid":1194, "name": "bash", "memory": 3536, "cpu":0.1},{"type":"system", "pid":992, "name": "sshd", "memory": 5136, "cpu":0},{"type":"system", "pid":1143, "name": "sshd", "memory": 6056, "cpu":0},{"type":"system", "pid":1177, "name": "sshd", "memory": 2964, "cpu":0}]} Connection closed by foreign host.
```

# Operação em modo Context Broker

O IMAIoT oferece a opção de registro de métricas em um context broker Orion. Para isso, destrua qualquer instância em execução do IMAIoT com o seguinte comando:

````
#> killall imaiot
````

Em seguida, efetue as configurações no arquivo imaiot.conf da seguint forma:

>OrionMode = 1

>OrionHost = http://HostNameOrIP

>OrionPort = 1026
### Instalação do Docker

Adicione os repositórios do Docker ao APT:

````
#> apt-get install apt-transport-https ca-certificates curl software-properties-common
#> curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add -
#> add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
````
Em seguida faça a atualização dos repositorios e instale o Docker:

````
#> apt-get update
#> apt-get install docker-ce
````
Para verificar se a instalação do Docker foi finalizada com sucesso, execute seu "hello world":

````
#> docker run hello-world
````

Instalação do Docker-compose

O Docker Compose é um aplicativo em python utilizado para automação de tarefas no docker. Para instalar o docker-compose primeiro instale o suporte ao python-pip:

````
#> apt-get install python-pip
````

E, finalmente, utilizando o pip, faça a instalação do docker-compose:

````
#> pip install docker-compose
````

Para lançar uma instância do Orion, execute:

````
#> docker-compose --log-level ERROR -p fiware up -d --remove-orphans
````
* Na primeira execução do comando anterior as imagens do Docker necessárias serão baixadas.

Para parar a instância do Orion, execute:

````
#> docker-compose --log-level ERROR -p fiware down -v --remove-orphans
````

## Consulta via Orion

Para realizar as consultas ao Orion, utilizaremos sua API REST. Uma simples consulta pode ser realizada com o comando abaixo, onde todas as entidades ligadas ao context broker serão listadas:
````
#> curl -X GET -H 'fiware-service: openiot' -H 'fiware-servicepath: /' --url 'http://HostNameOrIP:1026/v2/entities'
````
Outra consulta pode ser realizada em apenas uma entidade específica, em busca do valor de um atributo específico:

````
#> curl -X GET -H 'fiware-service: openiot' -H 'fiware-servicepath: /' --url 'http://HostNameOrIP:1026/v2/entities/urn:ngsi-ld:555/attrs/Architecture/value'

#> curl -X GET -H 'fiware-service: openiot' -H 'fiware-servicepath: /' --url 'http://HostNameOrIP:1026/v2/entities/urn:ngsi-ld:555/attrs/MemoryAvailable/value'
````

* Substitua o termo "HostNameOrIP" pelo endereço da máquina onde o Orion está sendo executado.

# Aplicação Web Exemplo

Para utilizar a aplicação web exemplo, basta abrir o arquivo imaiotGui.html em qualquer navegador, indicar o endereço do host onde o Orion está sendo executado utililando a URL no formato "HostNameOrIP:1026" e clicar no botão de conexão. Selecione um dos dispositivos monitorados que aparece no primeiro bloco e visualize as métricas publicadas pelo mesmo.

 *Substitua o termo "HostNameOrIP" pelo endereço da máquina onde o Orion está sendo executado.


# Agradecimentos
* Programa de Doutorado em Engenharia da Informação da UFABC
* Projeto SWAMP

O video com estas instruções está disponível em: https://youtu.be/0M_0YtABibc
