/**********************************************************
*                                                         *
*  IMAIoT - Infrastructure Monitoring Agent for IoT       *
*                                                         *
*  Author: Alexandre Heideker                             *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  Ver. 0.2                                               *
*                                                         *
*  Version History                                        *
*  Ver. 0.1 - First release                               *
*  Ver. 0.2 - 2019-02-27 TCP server suport & minors fixes *
*                                                         *
***********************************************************/

/************************************
 *  ToDo list:
 *  - Storage stats
 *  - Improve docker stats collector
 *  - MySQL log
 *  - Subscriber in Orion to receive actuator commands
 *  - 
 *  - 
 * 
 ************************************/


#include <thread>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <mutex>
#define SEM_WAIT mt.lock();
#define SEM_POST mt.unlock();
#include "swissknife.h"
#include "imaiot.h"
#include "mondata.h"

using namespace std;

std::mutex mt;

void ServerMode(int, int, int);
void logOut(std::string, MonData);
bool createEntity();
string getRest(string, string, string);
size_t curlCallback(char *, size_t , size_t , void *);
bool ckEntity();
bool updateEntity();
void logMFstatTXTHeader();
void logMFstatTXT();
void logMFstatJSON();
void thrSampling();
void thrOrionPublisher();
void thrLog();
void thrTCPServer();
std::string getJSONstats();
std::string getTXTstats();

MonData MFstats;

int main(int argc, char *argv[]) {
    if (!readSetup(MFstats.IMvar)) return -1;
    //MFstats.setVar(MFstats.IMvar);
    if (MFstats.IMvar.OrionMode==1) {
        if (!ckEntity()) 
            if (createEntity()) return -1;
    }
    std::thread refresh (thrSampling);
    std::thread tOrion (thrOrionPublisher);
    std::thread tLog (thrLog);
    std::thread tServer (thrTCPServer);    
    tOrion.join();
    tLog.join();
    tServer.join();
    refresh.join();
    return 0;
}

void thrSampling(){
    while (true) {
        std::time_t tsIni = std::time(0);
        SEM_WAIT
        if (MFstats.IMvar.debugMode) cout << "Sampling..." << tsIni << endl;
        MFstats.Refresh();
        SEM_POST        
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = MFstats.IMvar.SampplingTime - taskTime;
        if (taskTime <= MFstats.IMvar.SampplingTime) 
            sleep(sleepTime);
    }
}

void thrTCPServer(){
    if (MFstats.IMvar.ServerMode!=1) return;
    int opt = 1;
    int client;
    struct sockaddr_in SvrAddr, CliAddr;
    socklen_t sin_len = sizeof(CliAddr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "Error: can't open socket!" << endl;
        return;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(MFstats.IMvar.ServerPort);
    if (bind(sock, (struct sockaddr *) &SvrAddr, sizeof(SvrAddr)) == -1) {
        close(sock);
        cout << "Error: can't bind socket!" << endl;
        return;
    }
    listen(sock, 5);
    while (true){
        client = accept(sock, (struct sockaddr *) &CliAddr, &sin_len);        
        if (MFstats.IMvar.debugMode) cout << "Got connection..." << endl;
        std:string response = getJSONstats();
        send(client, response.c_str(), strlen(response.c_str()), 0);
        close(client);
    }
}

void thrLog(){
    if (MFstats.IMvar.LogMode!=1) return;
    if (MFstats.IMvar.LogType!="JSON") logMFstatTXTHeader();
    while (true) {
        std::time_t tsIni = std::time(0);
        if (MFstats.IMvar.debugMode) cout << "Updating Log-file..." << endl;
        if (MFstats.IMvar.LogType=="JSON")
            logMFstatJSON();
        else 
            logMFstatTXT();
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = MFstats.IMvar.LogIntervall - taskTime;
        if (taskTime <= MFstats.IMvar.LogIntervall) 
            sleep(sleepTime);
    }
}

void thrOrionPublisher(){
    if (MFstats.IMvar.OrionMode!=1) return;
    while (true) {
        std::time_t tsIni = std::time(0);
        if (MFstats.IMvar.debugMode) cout << "Updating Orion Context-broker..." << endl;
        updateEntity();
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = MFstats.IMvar.OrionPublisherTime - taskTime;
        if (taskTime <= MFstats.IMvar.OrionPublisherTime) 
            sleep(sleepTime);
    }
}

std::string getJSONstats(){
    ostringstream json;
    SEM_WAIT
    json << "{\"id\":\"" << MFstats.IMvar.NodeUUID << "\", \"type\":\"IMAIoT\", \"MFType\":{\"type\":\"Text\", \"value\":\""
            << MFstats.IMvar.KindOfNode << "\"}, \"Archtecture\":{\"type\":\"Text\", \"value\":\"" << MFstats.arch 
            << "\"},\"MemorySize\":{\"type\":\"Integer\", \"value\": " << MFstats.MemorySize 
            << "},\"MemoryAvailable\":{\"type\":\"Integer\", \"value\": " << MFstats.MemoryAvailable
            << "},\"LocalTimestamp\":{\"type\":\"Integer\", \"value\": " << MFstats.Timestamp
            << "},\"SampplingTime\":{\"type\":\"Integer\", \"value\": " << MFstats.IMvar.SampplingTime
            << "},\"CPU\":{\"type\":\"Integer\", \"value\": " << MFstats.cpuLevel //:" << MFstats.getJsonCpu() 
            << "},\"Storage\":" << MFstats.getJsonStorage() 
            << ",\"NetworkStats\":" << MFstats.getJsonNetworkStats() 
            << ",\"NetworkAdapters\":" << MFstats.getJsonNetworkAdapters() 
            << ",\"Process\":" << MFstats.getJsonProcess() 
            << "}";
    SEM_POST
    return json.str();
}

void logMFstatJSON(){
    ofstream File;
    File.open (MFstats.IMvar.LogFileName.c_str(), std::ofstream::out | std::ofstream::app);
    File << getJSONstats() << "\n";
    File.close();
    return;
}

void logMFstatTXTHeader(){
    ofstream File;
    //Header's file:
    // NodeUUID;Timestamp;Archtecture;CPUlevel;MemorySize;MemoryFree;TCPtxQueue;TCPrxQueue; ->
    // -> UDPtxQueue;UDPrxQueue;TCPMaxWindow;[Processes/Dockers(Kind;Name;CPU;Memory)]
    File.open (MFstats.IMvar.LogFileName.c_str(), std::ofstream::out);
    File << "NodeUUID;Timestamp;Archtecture;CPUlevel;MemorySize;MemoryFree;TCPtxQueue;TCPrxQueue;" <<
            "UDPtxQueue;UDPrxQueue;TCPMaxWindow;[Processes/Dockers(Kind;Name;CPU;Memory)]" << endl;
    File.close();
    return;
}

std::string getTXTstats(){
    ostringstream txt;
    SEM_WAIT
    txt << MFstats.IMvar.NodeUUID << ";" << MFstats.Timestamp << ";" << MFstats.arch << ";" 
            << MFstats.cpuLevel << ";" << MFstats.MemorySize << ";" << MFstats.MemoryAvailable << ";" 
            << MFstats.netData.TCPtxQueue << ";" << MFstats.netData.TCPrxQueue << ";" 
            << MFstats.netData.UDPtxQueue << ";" << MFstats.netData.UDPrxQueue << ";"
            << MFstats.netData.TCPMaxWindowSize;
    //log watched processes
    for (auto p: MFstats.Processes) {
        txt << ";process;" << p.Name << ";" << p.cpu << ";" << p.DataMem;
    }    

    //log watched dockers
    for (auto p: MFstats.Dockers) {
        txt << ";docker;" << p.Name << ";" << p.cpu << ";" << p.DataMem;
    }    
    SEM_POST
    return txt.str();
}

void logMFstatTXT(){
    ofstream File;
    File.open (MFstats.IMvar.LogFileName.c_str(), std::ofstream::out | std::ofstream::app);
    File << getTXTstats() << endl;
    File.close();
    return;
}

bool updateEntity(){
    if (!ckEntity()) return false;
    ostringstream json;
    SEM_WAIT
    json << "{\"MFType\":{\"type\":\"Text\", \"value\":\""
            << MFstats.IMvar.KindOfNode << "\"}, \"Archtecture\":{\"type\":\"Text\", \"value\":\"" << MFstats.arch 
            << "\"},\"MemorySize\":{\"type\":\"Integer\", \"value\": " << MFstats.MemorySize 
            << "},\"MemoryAvailable\":{\"type\":\"Integer\", \"value\": " << MFstats.MemoryAvailable
            << "},\"LocalTimestamp\":{\"type\":\"Integer\", \"value\": " << MFstats.Timestamp
            << "},\"SampplingTime\":{\"type\":\"Integer\", \"value\": " << MFstats.IMvar.SampplingTime
            << "},\"CPU\":{\"type\":\"Integer\", \"value\": " << MFstats.cpuLevel //:" << MFstats.getJsonCpu() 
            << "},\"Storage\":" << MFstats.getJsonStorage() 
            << ",\"NetworkStats\":" << MFstats.getJsonNetworkStats() 
            << ",\"NetworkAdapters\":" << MFstats.getJsonNetworkAdapters() 
            << ",\"Process\":" << MFstats.getJsonProcess() 
            << "}";

    SEM_POST
    ostringstream url;
    string header;
    header = "Content-Type: application/json";
    url << MFstats.IMvar.OrionHost << ":" << MFstats.IMvar.OrionPort << "/v2/entities/" << MFstats.IMvar.NodeUUID << "/attrs?options=keyValues";
    if (MFstats.IMvar.debugMode) cout << "URL:\t" << url.str() << endl;
    if (MFstats.IMvar.debugMode) cout << "Header:\t" << header << endl;
    if (MFstats.IMvar.debugMode) cout << "JSON:\t" << json.str() << endl;
    string retStr = getRest(url.str(), header, json.str());
    if (retStr.find("{\"error\":",0)) 
        return false;
    return true;
}

bool createEntity(){
    if (ckEntity()) return false;
    ostringstream json;
    SEM_WAIT
    json << "{\"id\":\"" << MFstats.IMvar.NodeUUID << "\", \"type\":\"IMAIoT\", \"MFType\":{\"type\":\"Text\", \"value\":\""
            << MFstats.IMvar.KindOfNode << "\"}, \"Archtecture\":{\"type\":\"Text\", \"value\":\"" << MFstats.arch 
            << "\"},\"MemorySize\":{\"type\":\"Integer\", \"value\": " << MFstats.MemorySize 
            << "},\"MemoryAvailable\":{\"type\":\"Integer\", \"value\": " << MFstats.MemoryAvailable
            << "},\"LocalTimestamp\":{\"type\":\"Integer\", \"value\": " << MFstats.Timestamp
            << "},\"SampplingTime\":{\"type\":\"Integer\", \"value\": " << MFstats.IMvar.SampplingTime
            << "},\"CPU\":{\"type\":\"Integer\", \"value\": " << MFstats.cpuLevel //:" << MFstats.getJsonCpu() 
            << "},\"Storage\":" << MFstats.getJsonStorage() 
            << ",\"NetworkStats\":" << MFstats.getJsonNetworkStats() 
            << ",\"NetworkAdapters\":" << MFstats.getJsonNetworkAdapters() 
            << ",\"Process\":" << MFstats.getJsonProcess() 
            << "}";
    SEM_POST
    ostringstream url;
    string header;
    header = "Content-Type: application/json";
    url << MFstats.IMvar.OrionHost << ":" << MFstats.IMvar.OrionPort << "/v2/entities?options=keyValues";
    if (MFstats.IMvar.debugMode) cout << "URL:\t" << url.str() << endl;
    if (MFstats.IMvar.debugMode) cout << "Header:\t" << header << endl;
    if (MFstats.IMvar.debugMode) cout << "JSON:\t" << json.str() << endl;
    string retStr = getRest(url.str(), header, json.str());
    if (retStr.find("{\"error\":",0)) 
        return false;
    return true;
}
bool ckEntity(){
    ostringstream url;
    string header;
    header = "Content-Type: application/json";
    url << MFstats.IMvar.OrionHost << ":" << MFstats.IMvar.OrionPort << "/v2/entities/?type=IMAIoT&id=" << MFstats.IMvar.NodeUUID;
    if (MFstats.IMvar.debugMode) cout << "URL:\t" << url.str() << endl;
    if (MFstats.IMvar.debugMode) cout << "Header:\t" << header << endl;
    string retStr = getRest(url.str(), header, "");
    if (retStr != "[]") 
        return true;
    return false;
}

size_t curlCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string getRest(string url, string header, string data) {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (MFstats.IMvar.debugMode) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        if (data != "") {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, header.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
        }
        string Buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "";
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return Buffer;
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return "";
}

string getDockerProcesses(){
    string Cmd("docker stats --all --no-stream --format \";{{.Name}};{{.CPUPerc}};{{.MemUsage}}\"");
    string ret(run(Cmd));
    replaceAll(ret, "\n", ";");
    return ret;
}

void ServerMode(int MonTime, int TCPPort, int SamplingTime){
    cout << "Not Implemented!" << endl;
    return;
}






