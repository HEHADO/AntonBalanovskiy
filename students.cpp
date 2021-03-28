#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#define ALL_PLACES 1024
#define PLACES 64;

using std::string;
class Student {

protected:
    string students[ALL_PLACES]{nullptr};
    string facultets[ALL_PLACES]{nullptr}; 
public:
    virtual int count() = 0;
    Student (){
        for (int i=0;i < ALL_PLACES;i++){
            students[i] = "";
            facultets[i] = "";
        }
    }
    Student (const Student& s){
        for (int i=0;i < ALL_PLACES;i++){
            students[i] = s.students[i];
            facultets[i] = s.facultets[i];
        }
    }
    void ADD (string Name, string f){
        int i {0};
        while ((students[i] != "")&&(i != ALL_PLACES));
        assert((students[i] != "") && "owerflow");
        students[i] = Name;
        facultets[i] = f; 
    }
    void del (string Nane){
        int i {0};
        while ((students[i] != "")&&(i != ALL_PLACES));
        assert((students[i] == "") && "there is no matches");
        students[i] = "" ;
        facultets[i] = "";
    }
    virtual ~Student(){
            delete students;
            delete facultets;
    }
};



class CMS : public Student{
    friend std::ostream& operator << (std::ostream &os, CMS &o);
    int quantity;
public:
    int count (){
        int t;
        for (int i=0;i < ALL_PLACES;i++){
            if (facultets[i] == "CMS") t++;
        }
        return t;
    }
    CMS& operator = (CMS& s){
        if (students != nullptr ) delete students;
        if (facultets != nullptr ) delete facultets;
        CMS s1 = s;
        *this = s1;
    }
};

std::ostream& operator << (std::ostream &os, CMS &o) {
    for (int i=0;i < ALL_PLACES;i++){
        if (o.facultets[i] == "CMS") std::cout << o.students[i]<< std::endl;
    }
}

int main(int argc, char const *argv[]){
    return 0;
}

