#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <iterator>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#define PRT 2281

#define BUFF 1024
# define BACKLOG 5

using namespace std;

class Socket{
protected:
    int sd;
    explicit Socket(int _sd) : sd(_sd) {}
public:
    Socket() {
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0) perror("can,t find soket");
    }
    int GetSd() const { return sd;}
    void Shutdown() { if (shutdown(sd, 2) < 0) perror("can,t shutdown soket");}
    ~Socket() { close(sd);}
};

void err_cath (int num, const char* str) {
    if (num < 0) perror(str); 
}

class SocketAddr {
    struct sockaddr_in ad;
public:
    SocketAddr() {
        ad.sin_family = AF_INET;
        ad.sin_port = htons(PRT); 
        ad.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    SocketAddr(const char* ip, short port){ 
        ad.sin_family = AF_INET;
        ad.sin_port = htons(port); 
        ad.sin_addr.s_addr = inet_addr(ip);
    }
    SocketAddr(unsigned int ip, short port) {
        ad.sin_family = AF_INET;
        ad.sin_port = htons(port);
        ad.sin_addr.s_addr = htonl(ip);
    }
    struct sockaddr* GetAddr() const { return (sockaddr *)&ad;}
    int GetLen() const { return sizeof(ad);}
    
    ~SocketAddr() {}
};


class ServerSocket: public Socket {
public:
    void Bind(const SocketAddr& s) {
        err_cath(bind(sd, s.GetAddr(), s.GetLen()), "bind");
    }
    void Listen(int n){
        err_cath(listen(sd, n), "listen");
    }
    int Accept(SocketAddr& s) {
        unsigned int l = sizeof(s);
        int a = accept(sd, s.GetAddr(), (socklen_t *)&l);
        err_cath(a, "accept");
        return a;
    }

};

vector<uint8_t> Trans_to_v(int fd) {
    vector<uint8_t> v;
    char c;
    while(read(fd, &c, 1)) v.push_back(c);
    return v;
}

class ConnectedSocket: public Socket {
public:
    ConnectedSocket() : Socket() {}
    explicit ConnectedSocket(int cd) : Socket(cd) {}
    ConnectedSocket& operator<<(const string& str) {
        send(sd, str.c_str(), str.length(), 0);
        return *this;
    }
    ConnectedSocket& operator<<(const vector<uint8_t>& bytes) {
        send(sd, bytes.data(), bytes.size(), 0);
        return *this;
    }
    void operator>>(string& str) {
        char buf[BUFF];
        err_cath(recv(sd, buf, BUFF, 0), "read");
        str = buf;
    }
};

void Error(ConnectedSocket cs) {
    string str;
    switch (errno){
        case EACCES:
            str = "HTTP/1.1 403 Forbidden\n";
        case ENETRESET:
            str = "HTTP/1.1 503 Service Unavailable\n";
        default:
            str = "HTTP/1.1 404 Not Found\n";
            break;
    }
    cout << str;
    cs << str;
}

vector<string> string_to_V(string s) {
    string del = "\r\n";
    string token;
    int start = 0;
    int end;
    int l = del.length();
    vector<string> res;
    while ((end = s.find(del, start)) > 0) {
        token = s.substr(start, end - start);
        start = end + l;
        res.push_back(token);
    }
    res.push_back(s.substr(start));
    return res;
}

void Handler(ConnectedSocket cs, string path) {
    int fd;
    if (path == "/") {
        fd = open("index.html", O_RDONLY);
    } else { 
        fd = open(("."+path).c_str(), O_RDONLY);
    }
    if (fd < 0) {
        //perror("index");
        cout << "\nclient: HTTP/1.1 404 Not Found";
        cs << "HTTP/1.1 404 Not Found\0";
        fd = open("404.html", O_RDONLY);
        err_cath(fd, "fd404");
    } else {
        cout << "\nclient: HTTP/1.1 200 OK";
        cs << "HTTP/1.1 200 OK\0";
    }
    vector<uint8_t> vect = Trans_to_v(fd);
    string str;
    str += "\r\nVersion: HTTP/1.1\r\nContent-length: " + to_string(vect.size()) + "\r\n\r\n";
    cout << "l " << to_string(vect.size()) << endl;
    cs << str;
    cs << vect;
    cs.Shutdown();
    close(fd);
}

bool find_Walley(string s){
    return !(s.find('?') == -1);
}

char** get_env(string filename, string s, string request) {
    char** env = new char*[8];
    env[0] = new char[request.size()];
    env[1] = new char[22];
    env[2] = new char[24];
    env[3] = new char[25]; 
    env[4] = new char[13 + filename.size()]; 
    env[5] = new char[17]; 
    env[6] = new char[14 + s.size()]; 

    env[0] = (char *) request.c_str();
    env[1] = (char *) "SERVER_ADDR=127.0.0.1";
    env[2] = (char *)  "CONTENT_TYPE=text/plain";
    env[3] = (char *) "SERVER_PROTOCOL=HTTP/1.0";
    strcpy(env[4], "SCRIPT_NAME=");
    strcat(env[4], filename.c_str());
    env[5] = (char *) "SERVER_PORT=2281";
    strcpy(env[6], "QUERY_STRING=");
    strcat(env[6], s.c_str());
    env[7] = NULL;
    return env;
}


string Gpath(string str){
    string temp;
    int i = 0;
    while(str[i] != '?') {
        temp += str[i];
        i++;
    }
    temp[i]=0;
    return temp;
}

string Glast (string str){
    string temp;
    int i = Gpath(str).length() + 1;
    while(i != str.length()) {
        temp += str[i];
        i++;
    }
    return temp;
}

void Handler2(string path_name, ConnectedSocket s,string fr){
    int fd;
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, 1);
        close(fd);
        string path = Gpath(path_name);
        path = "sgi"+path;
        string last = Glast(path_name);
        char* argv[] = { (char*)(path).c_str(), NULL};
        char** env = get_env(path, last, fr);
        execve(path.c_str(), argv, env);
        if (errno == EACCES) {
            s << "HTTP/1.1 403 Forbidden\n";
            cout << "HTTP/1.1 403 Forbidden\n";
        } else if (errno == ENETRESET) {
            s << "HTTP/1.1 503 Service Unavailable\n";
            cout <<  "HTTP/1.1 503 Service Unavailable\n";
        } else {
            s << "HTTP/1.1 404 Not Found\n";
            cout << "HTTP/1.1 404 Not Found\n";
        }
        perror("unexecutable");
        exit(1) ;
    } else if (pid > 0) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0){
            fd = open("temp.txt", O_RDONLY);
            cout << "HTTP/1.1 200 OK\n";
            s <<  "HTTP/1.1 200 OK";
            vector<uint8_t> vect = Trans_to_v(fd);
            string str;
            str += "\r\nVersion: HTTP/1.1\r\nContent-type: text/html\r\nContent-length: ";
            str += to_string(vect.size()) + "\r\n\r\n";
            cout << "length  " << vect.size();
            s << str;
            s << vect;
            close (fd);
            s.Shutdown();
        }
    }
}


void ProcessConnection(int cd, const SocketAddr& clAddr) {
    ConnectedSocket cs(cd);
    string request;
    string path= "";
    cs >> request;
    vector<string> lines = string_to_V(request);
    cout << "serv: ", lines[0];
    string s = lines[0]; 
    int i = 4; 
    if (s[i] == ' ') i++; 
    while (s[i] != ' ') path += s[i++];
    cout << path;
    if (!find_Walley(path)){
        Handler(cs, path);
    } else {
        Handler2(path, cs, request);
    }
}


int main(){
    SocketAddr serverAddr;
    ServerSocket ss;
    ss.Bind(serverAddr);
    ss.Listen(BACKLOG);
    cout << "\nserver at adress " << PRT << "\n";
    while(true) {
        SocketAddr clAddr;
        int cd = ss.Accept(clAddr);
        ProcessConnection(cd, clAddr);
        cout <<"\n";
    }
}
