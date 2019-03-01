/**********************************************************
*                                                         *
*  IMAIoT - Infrastructure Monitoring Agent for IoT       *
*                                                         *
*  Author: Alexandre Heideker                             *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  Ver. 0.2                                               *
***********************************************************/

typedef struct {
    std::string NodeName;
    std::string NodeUUID;
    std::string KindOfNode;
    bool debugMode;
    bool CPUSingleStat;
    bool LogMode;
    std::string LogType;
    std::string LogFileName;
    bool MySQLMode;
    std::string MySQLHost;
    std::string MySQLUser;
    std::string MySQLPasswd;
    bool ServerMode;
    u_int16_t ServerPort;
    bool OrionMode;
    std::string OrionHost;
    u_int16_t OrionPort; 
    u_int16_t SampplingTime;
    u_int16_t OrionPublisherTime;
    u_int16_t LogIntervall;
    bool DockerStat;
    bool CPUStat;
    std::string CPUPathStat;
    std::string CPUPathArch;
    bool NetworkStat;
    std::string NetworkPathStat;
    bool DiskStat;
    std::vector <std::string> ProcessNames;
    std::vector <std::string> DockerNames;
} IMAIoTVar;

typedef struct {
    int pid;
    std::string Name;
    float cpu;
    long DataMem;
    long VirtMem;
} processData;

typedef struct {
    long UDPtxQueue;
    long UDPrxQueue;
    long TCPtxQueue;
    long TCPrxQueue;
    unsigned int TCPMaxWindowSize; //only TCP
} networkData;

typedef struct {
    std::string Name;
    long RxBytes;
    long RxPackets;
    long RxErrors;
    long TxBytes;
    long TxPackets;
    long TxErrors;
} networkAdapter;


void dumpVar(IMAIoTVar& IMvar){
    cout << "NodeName:\t" << IMvar.NodeName << endl;
    cout << "NodeUUID:\t" << IMvar.NodeUUID << endl;
    cout << "KindOfNode:\t" << IMvar.KindOfNode << endl;
    cout << "debugMode:\t" << IMvar.debugMode << endl;
    cout << "CPUSingleStat:\t" << IMvar.CPUSingleStat << endl;
    cout << "LogMode:\t" << IMvar.LogMode << endl;
    cout << "LogType:\t" << IMvar.LogType << endl;
    cout << "LogFileName:\t" << IMvar.LogFileName << endl;
    cout << "LogIntervall:\t" << IMvar.LogIntervall << endl;
    cout << "MySQLMode:\t" << IMvar.MySQLMode << endl;
    cout << "MySQLHost:\t" << IMvar.MySQLHost << endl;
    cout << "MySQLUser:\t" << IMvar.MySQLUser << endl;
    cout << "MySQLPasswd:\t" << IMvar.MySQLPasswd << endl;
    cout << "ServerMode:\t" << IMvar.ServerMode << endl;
    cout << "ServerPort:\t" << IMvar.ServerPort << endl;
    cout << "OrionMode:\t" << IMvar.OrionMode << endl;
    cout << "OrionHost:\t" << IMvar.OrionHost << endl;
    cout << "OrionPort:\t" << IMvar.OrionPort << endl;
    cout << "SampplingTime:\t" << IMvar.SampplingTime << endl;
    cout << "OrionPublisherTime:\t" << IMvar.OrionPublisherTime << endl;
    cout << "DockerStat:\t" << IMvar.DockerStat << endl;
    cout << "CPUStat:\t" << IMvar.CPUStat << endl;
    cout << "CPUPathStat:\t" << IMvar.CPUPathStat << endl;
    cout << "CPUPathArch:\t" << IMvar.CPUPathArch << endl;
    cout << "NetworkStat:\t" << IMvar.NetworkStat << endl;
    cout << "NetworkPathStat:\t" << IMvar.NetworkPathStat << endl;
    cout << "DiskStat:\t" << IMvar.DiskStat << endl;
    cout << "ProcessNames:\t";
    for (auto i: IMvar.ProcessNames)
        std::cout << i << ' ';
    cout << endl;
    cout << "DockerNames:\t";
    for (auto i: IMvar.DockerNames)
        std::cout << i << ' ';
    cout << endl;
}


bool readSetup(IMAIoTVar& IMvar){
    ifstream File;
    string line;
    File.open ("imaiot.conf");
    if (File.is_open()) {
        string token;
        string value;
        while (! File.eof()){
            getline(File, line);
            if (line[0] != '#') {
                token = trim(line.substr(0, line.find("=")));
                value = trim(line.substr(line.find("=")+1, line.length()-1));
                if (token == "debugMode") {
                    if (value == "1")
                        IMvar.debugMode = true;
                    else
                        IMvar.debugMode = false; 
                }
                if (token == "CPUSingleStat") {
                    if (value == "1")
                        IMvar.CPUSingleStat = true;
                    else
                        IMvar.CPUSingleStat = false; 
                }
                if (token == "LogMode") {
                    if (value == "1")
                        IMvar.LogMode = true;
                    else
                        IMvar.LogMode = false; 
                }
                if (token == "LogType") {
                    IMvar.LogType = UCase(value);
                }
                if (token == "LogFileName") {
                    IMvar.LogFileName = value;
                }
                if (token == "NodeName") {
                    IMvar.NodeName = value;
                }
                if (token == "NodeUUID") {
                    IMvar.NodeUUID = value;
                }
                if (token == "KindOfNode") {
                    IMvar.KindOfNode = value;
                }
               if (token == "MySQLMode") {
                    if (value == "1")
                        IMvar.MySQLMode = true;
                    else
                        IMvar.MySQLMode = false; 
                }
                if (token == "MySQLHost") {
                    IMvar.MySQLHost = value;
                }
                if (token == "MySQLUser") {
                    IMvar.MySQLUser = value;
                }
                if (token == "MySQLPasswd") {
                    IMvar.MySQLPasswd = value;
                }
                if (token == "ServerMode") {
                    if (value == "1")
                        IMvar.ServerMode = true;
                    else
                        IMvar.ServerMode = false; 
                }
                if (token == "ServerPort") {
                    IMvar.ServerPort = stoi(value);
                }
                if (token == "OrionMode") {
                    if (value == "1")
                        IMvar.OrionMode = true;
                    else
                        IMvar.OrionMode = false; 
                }
                if (token == "OrionHost") {
                    IMvar.OrionHost = value;
                }
                if (token == "OrionPort") {
                    IMvar.OrionPort = stoi(value);
                }
                if (token == "SampplingTime") {
                    IMvar.SampplingTime = stoi(value);
                }
                if (token == "LogIntervall") {
                    IMvar.LogIntervall = stoi(value);
                }
                if (token == "OrionPublisherTime") {
                    IMvar.OrionPublisherTime = stoi(value);
                }
                if (token == "DockerStat") {
                    if (value == "1")
                        IMvar.DockerStat = true;
                    else
                        IMvar.DockerStat = false; 
                }
                if (token == "CPUStat") {
                    if (value == "1")
                        IMvar.CPUStat = true;
                    else
                        IMvar.CPUStat = false; 
                }
                if (token == "CPUPathStat") {
                    IMvar.CPUPathStat = value;
                }
                if (token == "CPUPathArch") {
                    IMvar.CPUPathArch = value;
                }       
                if (token == "NetworkStat") {
                    if (value == "1")
                        IMvar.NetworkStat = true;
                    else
                        IMvar.NetworkStat = false; 
                }
                if (token == "NetworkPathStat") {
                    IMvar.NetworkPathStat = value;
                }
                if (token == "ProcessNames") {
                    IMvar.ProcessNames = splitString(trim(value), ' ');
                }
                if (token == "DockerNames") {
                    IMvar.DockerNames = splitString(trim(value), ' ');
                }
 
                if (token == "DiskStat") {
                    if (value == "1")
                        IMvar.DiskStat = true;
                    else
                        IMvar.DiskStat = false; 
                }
            }
        }
        File.close();
        if (IMvar.debugMode) dumpVar(IMvar);
        return 1;
    } else {
        cout << "Error reading monifog.conf\n" << endl;
        return 0;
    }
}
