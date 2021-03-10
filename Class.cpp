#include <iostream>
#include <fstream>
#define Left true
#define Right false
#include <string>
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::ios;
using namespace std;


class carriage {
friend class stack;
public:
    int ID=0;
    char direction='0';
 };

class stack {
    //friend carriage :: pop(stack*);
    friend class cariage;
    //int count=0;
    


public:
    carriage current;
    stack* next=NULL;
    stack* push (const carriage v){
        stack* s = new stack;
        //new int ;
        s->current = v;
        s->next = this;
        return s;
    };
};




carriage pop (stack*& s) {
    stack* t = s->next;
    carriage wagon {s->current};
    delete s;
    s = t;
    return wagon;
};




int main(int argc, char const *argv[]){
    cout << "enter the number of wagons" <<endl;
    int i;
    char ch;
    int id;
    cin >> i;
    carriage wagon;
    stack* left; 
    stack*right;
    fstream input;
    bool b = false;
    if (argc > 1) {
        
        input.open(argv[1], ios::in);
        b = true;
    }
    for (i; i != 0; i--) {
        if (!b) {
            cin >> id;
            cin >> ch;
        } else {
            input >> id;
            input >> ch;
        }
        wagon.direction = ch;
        wagon.ID = id;
        if (ch=='l') left = (*left).push(wagon);
        else right = (*right).push(wagon);//мутная история
    }
    cout << "on left side" <<endl;
    #pragma omp parallel while default(shared)
    while ((left != 0)&&(left->current.direction !='0')) {
        wagon = pop(left);
        cout << wagon.ID;
        cout << "\n";
    }
    cout << "on right side" << endl;
    while ((right != NULL)&&(right->current.direction !='0')) {
        wagon = pop(right);
        cout << wagon.ID;
        cout << "\n";
    }
}
