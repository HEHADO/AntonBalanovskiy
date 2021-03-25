#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
using namespace std;

enum Month{
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

istream& operator >> (istream& inp, Month& month){
    int i;
    cin >> i;
    switch (i)
    {
    case 1:
        month = January;
    case 2:
        month = February;
    case 3:
        month = March;
    case 4:
        month = April;
    case 5:
        month = May;
    case 6:
        month = June;
    case 7:
        month = July;
    case 8:
        month = August;
    case 9:
        month = September;
    case 10:
        month = October;
    case 11:
        month = November;
    case 12:
        month = December;
        //break;
    }
    return inp;
};

class abstract {
    protected:
    const char** yes;
    const char** no;
    private:
    int* i;
    public:
    abstract (){
        yes = new char* {"of course it is 😎\n"};
        no = new char* {"try a bit later  🙃\n"};
        i = new int {1488};
    }
    virtual ~abstract(){
        delete yes;
        delete no;
        delete i;
    }
    abstract (const abstract& a){
        yes = new auto {*a.yes};
        no =  new auto {*a.no};
        i = new auto {*a.i};
    }
    virtual const char* is_it_cool_time () = 0;
};


class Time : public abstract {
    friend ostream& operator << (ostream& inp, Time& time );
    friend istream& operator >> (istream& inp, Time& time );
    int h,m,s;
public:
    const char* is_it_cool_time(){
        if ((h == m)&&(m == s)) return *yes;
        return *no;
    }
    Time (const int a,const int b,const int c);  
};

class Date : abstract{
    const char* is_it_cool_time(){
        if (((year%100) == Day)&&(Day == m)) return *yes;
        return *no;
    }
    friend istream& operator >> (istream& inp, Date& date);
    friend ostream& operator << (ostream& inp, Date& date);
    Month m;
    int year;
    int Day;
    Time*T {nullptr};
public:
    Date (const int YEAR, const Month month, const Time& t, const int day);
    Date& operator = (Date& date) {
        Day = date.Day;
        m = date.m;
        year = date.year;
        delete T;
        T = new auto {*(date.T)};
        return *this;
    };
    Date  (const Date& d){
        m = d.m;
        year = d.year;
        T = new Time {*d.T};
        Day = d.Day;
    }
    ~Date ();
};

Time::Time (const int a=0,const int b=0,const int c=0){
    h=a;
    m=b;
    s=c;
}

Date::Date(const int YEAR, const Month month, const Time& t, const int day) {
    year = YEAR;
    m = month;
    T = new auto {t};
}
/*
Date:: Date operator = (Date date) {
    
}*/

Date::~Date(){
    delete T;
}

ostream& operator << (ostream& inp, Time& time ) {
    inp << time.h << "\t";
    inp << time.m << "\t";
    inp << time.s << "\n";
    return inp;
}

istream& operator >> (istream& inp, Time& time ) {
    cout << "enter time in format 'HH MM SS'";
      inp >> time.h;
    inp >> time.m;
    inp >> time.s;
    return inp;
}

istream& operator >> (istream& inp, Date& date) {
    cout << "enter date in format 'DD MM YYYY'";
    inp >> date.Day;
    inp >> date.m;
    inp >> date.year;
    Time time;
    inp >> time;
    date.T = new auto {time};
    return inp;
}

ostream& operator << (ostream& inp, Date& date) {
    inp << date.Day << "\t";
    inp << date.m << "\t";
    inp << date.year << "\n";
    Time time;
    cout << "time is" << endl; 
    inp << *(date.T);
    return inp;
}
