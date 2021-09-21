#include <iostream>
#include <fstream>
#include <cassert>
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
    struct node{
        node* next {nullptr};
        carriage wagon;
    };
    node* top {nullptr};
    friend class cariage;
    public:
    void push (const carriage v);
    carriage pop ();
    bool is_somthing ();
};

void stack::push(const carriage v) {
    top = new node {top,v};
};

carriage stack::pop () {
    assert(top && "stack is empty");
    //auto t
    auto temp {top};
    auto temp2 {top->wagon};
    top = top->next;
    delete temp;
    return temp2;
};

bool stack::is_somthing (){
    return top;
};

istream& operator >> (istream& inp, carriage& wagon ) {
    inp >> wagon.ID;
    inp >> wagon.direction;
    return inp;
};


int main(int argc, char const *argv[]){
    cout << "enter the number of wagons" <<endl;
    int i;
    cin >> i;
    carriage wagon;
    stack left; 
    stack right;
    fstream fs;
    bool b = false;
    if (argc > 1) {
        fs.open(argv[1], ios::in);
        b = true;
    }

    std::istream& input = (argc>1)?fs:cin;

    for (; i != 0; i--) {
        input >> wagon;
        if (wagon.direction == 'l') left.push(wagon);
        else right.push(wagon);
    }
    cout << "on left side" <<endl;
    #pragma omp parallel while default(shared)
    while (left.is_somthing()) {
        wagon = left.pop();
        cout << wagon.ID << "\n";
    }
    cout << "on right side" << endl;
    while (right.is_somthing()) {
        wagon = right.pop();
        cout << wagon.ID << "\n";
    }
    //cin.eof()    
}
