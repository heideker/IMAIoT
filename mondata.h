/**********************************************************
*                                                         *
*  IMAIoT - Infrastructure Monitoring Agent for IoT       *
*                                                         *
*  Author: Alexandre Heideker                             *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  statistic data structure class                         *
***********************************************************/

using namespace std;

class MonData{
    private:
        
    public:
        IMAIoTVar IMvar;
        unsigned int Timestamp; //timestamp
        long MemorySize;
        long MemoryAvailable;
        long cpuOldStats[10];
        long cpuStats[10];
        int cpuLevel;
        std::string arch;
        networkData netData;
        std::vector<networkAdapter> NetAdapters;
        std::vector<processData> Processes;
        std::vector<processData> Dockers;
        std::string getJsonStorage();
        std::string getJsonNetworkStats();
        std::string getJsonNetworkAdapters();
        std::string getJsonProcess();
        std::string getTxtStorage();
        std::string getTxtNetworkStats();
        std::string getTxtNetworkAdapters();
        std::string getTxtProcess();
        void Refresh();
};

void MonData::Refresh(){
    //Refresh system data
    this->Timestamp = std::time(0);
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    this->MemorySize = memInfo.totalram;
    this->MemoryAvailable = memInfo.freeram;
    std::ifstream File;
    std::string line;
    File.open (this->IMvar.CPUPathStat);
    if (File.is_open()) {
        std::string cpuId;
        File >> cpuId >> this->cpuStats[0]
                >> this->cpuStats[1]
                >> this->cpuStats[2]
                >> this->cpuStats[3]
                >> this->cpuStats[4]
                >> this->cpuStats[5]
                >> this->cpuStats[6]
                >> this->cpuStats[7]
                >> this->cpuStats[8]
                >> this->cpuStats[9];
        long cpuSumTime, cpuSumTimeOld, cpuDelta, cpuUsed, cpuIdle;
        cpuIdle = this->cpuStats[3] - this->cpuOldStats[3];
        for (int i=0; i<10; i++) {
            cpuSumTime += this->cpuStats[i];
            cpuSumTimeOld += this->cpuOldStats[i];
            this->cpuOldStats[i] = this->cpuStats[i];
        }
        cpuDelta = cpuSumTime - cpuSumTimeOld;
        cpuUsed = cpuDelta - cpuIdle;
        this->cpuLevel = (int) 100 *  cpuUsed / cpuDelta; 
        if (this->IMvar.debugMode) cout << cpuId << "  " << this->cpuLevel << endl;
        File.close();
    }
    //reading network stats
    networkData nd;
    nd.TCPtxQueue = 0;
    nd.TCPrxQueue = 0;
    nd.TCPMaxWindowSize = 0;
    std::string trash;
    File.open (this->IMvar.NetworkPathStat + "/tcp");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            unsigned long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.TCPtxQueue += v;
            ss >> v;
            nd.TCPrxQueue += v;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            if (v > nd.TCPMaxWindowSize) nd.TCPMaxWindowSize = v;
        }
        File.close();
    }
    File.open (this->IMvar.NetworkPathStat + "/tcp6");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.TCPtxQueue += v;
            ss >> v;
            nd.TCPrxQueue += v;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            if (v > nd.TCPMaxWindowSize) nd.TCPMaxWindowSize = v;
        }
        File.close();
    }
    File.open (this->IMvar.NetworkPathStat + "/udp");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.UDPtxQueue += v;
            ss >> v;
            nd.UDPrxQueue += v;
        }
        File.close();
    }
    File.open (this->IMvar.NetworkPathStat + "/udp6");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.UDPtxQueue += v;
            ss >> v;
            nd.UDPrxQueue += v;
        }
        File.close();
    }
    this->netData = nd;
    //reading network adapters stats...
    this->NetAdapters.clear();
    File.open (this->IMvar.NetworkPathStat + "/dev");
    if (File.is_open()) {
        //header
        getline(File, line);
        getline(File, line);
        networkAdapter na;
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            ss >> na.Name >> na.RxBytes >> na.RxPackets >> na.RxErrors;
            ss >> trash >> trash >> trash >> trash >> trash;
            ss >> na.TxBytes >> na.TxPackets >> na.TxErrors;
            this->NetAdapters.push_back (na);
        }
        File.close();
    }
    this->netData = nd;
    //reading architeture info...
    File.open (this->IMvar.CPUPathArch);
    if (File.is_open()) {
        while (! File.eof()){
            getline(File, line);
            if (line.substr(0, 10)=="model name"){
                this->arch = trim(ReplaceForbidden(line.substr(line.find(":")+2, line.length()-1)));
                break;
            }
        }
        File.close();
    }
    //reading processes stats...using ps shell command
    std::string stringOut;
    processData ps;
    this->Processes.clear();
    for (auto p: this->IMvar.ProcessNames) {
        stringOut = run("ps  --no-headers -C " + p + " -o pid,%cpu,rss,vsz");
        std::stringstream ss(stringOut);
        while (ss >> ps.pid >> ps.cpu >> ps.DataMem >> ps.VirtMem){
            ps.Name = ReplaceForbidden(p);
            if (this->IMvar.debugMode) cout << ps.Name << "\t" << ps.cpu << "\t" << ps.DataMem << endl;
            this->Processes.push_back (ps);
        }
    }
    if (this->IMvar.DockerStat) {
        this->Dockers.clear();
        stringOut = run("docker stats --all --no-stream --format \"{{.Name}};{{.CPUPerc}};{{.MemUsage}}\"");
        if (stringOut.size()>0) {
            replaceAll(stringOut, " / ", ";");
            replaceAll(stringOut, "MiB", "E03");
            replaceAll(stringOut, "GiB", "E06");
            replaceAll(stringOut, "B", "");
            replaceAll(stringOut, "%", "");
            replaceAll(stringOut, ";", " ");
            std::stringstream ss(stringOut);
            int n;
            ps.pid=0;
            ps.VirtMem=0;
            while (ss >> ps.Name >> ps.cpu >> ps.DataMem >> n){
                if (this->IMvar.debugMode) cout << ps.Name << "\t" << ps.cpu << "\t" << ps.DataMem << endl;
                if (this->IMvar.ProcessNames.size()>0) {
                    if (this->IMvar.ProcessNames[0] != "*") {
                        this->Dockers.push_back(ps);
                    } else {
                        for (auto d: this->IMvar.DockerNames) {
                            if (d == ps.Name)
                                this->Dockers.push_back (ps);
                        }  
                    }
                }
            }
        }   
    }
}
std::string MonData::getJsonStorage(){
    //Not implemented
    return "[]";
}
std::string MonData::getTxtStorage(){
    return "Storage";
}
std::string MonData::getJsonNetworkStats(){
    std::ostringstream s;
    s << "{\"TCPrxQueue\":" << this->netData.TCPrxQueue 
        << ", \"TCPtxQueue\":" << this->netData.TCPtxQueue 
        << ", \"TCPMaxWindowSize\":" << this->netData.TCPMaxWindowSize 
        << ", \"UDPrxQueue\":" << this->netData.TCPtxQueue 
        << ", \"UDPtxQueue\":" << this->netData.TCPtxQueue << "}";
    return s.str();
}
std::string MonData::getJsonNetworkAdapters(){
    std::ostringstream s;
    s << "[";
    bool cy = false;
    for (auto p: this->NetAdapters) {
        if (cy) s << ",";
        cy = true;
        s << "{\"name\":\"" << p.Name << "\", \"rxBytes\":" << p.RxBytes 
            << ", \"rxPackets\":" << p.RxPackets
            << ", \"rxErrors\":" << p.RxErrors
            << ", \"txBytes\":" << p.TxBytes 
            << ", \"txPackets\":" << p.TxPackets
            << ", \"txErrors\":" << p.TxErrors << "}";
    }
    s << "]";
    return s.str();
}
std::string MonData::getTxtNetworkStats(){
    return "Network";
}
std::string MonData::getTxtNetworkAdapters(){
    return "Network";
}
std::string MonData::getJsonProcess(){
    std::ostringstream s;
    s << "[";
    bool cy = false;
    for (auto p: this->Processes) {
        if (cy) s << ",";
        cy = true;
        s << "{\"type\":\"system\", \"pid\":" << p.pid << ", \"name\": \"" << p.Name
            << "\", \"memory\": " << p.DataMem << ", \"cpu\":" << p.cpu << "}";
    }
    for (auto p: this->Dockers) {
        if (cy) s << ",";
        cy = true;
        s << "{\"type\":\"docker\", \"pid\":" << p.pid << ", \"name\": \"" << p.Name
            << "\", \"memory\": " << p.DataMem << ", \"cpu\":" << p.cpu << "}";
    }
    s << "]";
    return s.str();
}
std::string MonData::getTxtProcess(){
    return "Process";
}




