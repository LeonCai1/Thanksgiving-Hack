//
// Created by Charlie on 11/25/2022.
//
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <map>
using namespace std;

map<string, int> read_config(const string &filename);

int parse_json(string line, map<string, int>& json){
    regex e (".*\"(.*)\": (.*?),?[\r\n]?");
    smatch sm;
    if (regex_match(line, sm, e)){
        string key = sm[1];
        string _value = sm[2];
        int value = stoi(_value);
        json[key] = value;
        return 1;
    }else{
        return -1;
    }
}

map<string, int> read_config(const string &filename) {
    string line;
    bool start_flag = false;
    ifstream config_file(filename);
    map<string, int> json;
    if (config_file.is_open())
    {
        while( getline(config_file, line))
        {
            cout << line << '\n';
            if(!start_flag && line.find("{")!=string::npos) {
                start_flag = true;
            }else if(start_flag && line.find("}")!=string::npos){
                start_flag = false;
            }else if(start_flag){
                parse_json(line, json);
            }
        }
        config_file.close();
    }
    else cout << "Unable to open config file";
    return json;
}

// test_main
//int main(int argc, const char * argv[]){
//    string filename = "./config.json";
//    map<string, int> json = read_config(filename);
//    cout << json["line"];
//}


