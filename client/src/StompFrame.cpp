#include "../include/StompFrame.h"
#include <vector>

StompFrame::StompFrame(string msg) {
    //maybe need to check about '\0'
    bool firstLine = true;
    size_t pos=0;
    string line;
    while((pos = msg.find('\n')) != string::npos){
        line=msg.substr(0,pos);
        if(firstLine){
            command=line;
            firstLine = false;
        }
        else if (line.empty())
            break;
        else
        {
            int delpos = line.find(":");
            string headerName = line.substr(0,delpos);
            string headerValue = line.substr(delpos+1, line.length() - delpos - 1);
            headers.insert(std::pair<string, string> (headerName,headerValue));
        }
        msg.erase(0, pos + 1);
    }
    msg.erase(0, msg.find('\n') + 1);
    body=msg; //place for problem (maybe)
}

StompFrame::StompFrame(string _command,unordered_map <string, string> _headers, string _body ):
command(_command), headers(_headers), body(_body)
{}

StompFrame::StompFrame():command(""), headers(unordered_map<string, string>()),body(""){} 

string StompFrame::getCommand(){
    return command;
}

unordered_map<string,string> StompFrame::getHeaders(){
    return headers;
}

string StompFrame::getBody(){
    return body;
}

string StompFrame::toString(){
    string output = command;
    for (auto it = headers.begin(); it != headers.end(); ++it) 
        output += "\n" + it->first + ":" + it->second;
    
    output += "\n\n" + body;

    return output;
}
