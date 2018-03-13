#include <iostream>
#include <vector>
#include <iterator>

template <typename ValueType>
class Iterable
{
public:
    class InputIterator : public std::iterator<
                              std::input_iterator_tag,
                              ValueType,
                              long,
                              const ValueType*,
                              ValueType >

    {
        ValueType value;
    public:
        explicit InputIterator(ValueType val) : value(val) {}
        ValueType operator *() { return value; } //overloaded *p
        InputIterator& operator ++() //overloaded p++
        {
            std::advance(*this, 1);
            return *this;
        }
        InputIterator operator ++(int)
        {
            InputIterator tmp = *this;
            (*this)++;
            return tmp;
        }
        bool operator ==(InputIterator another) const { return value == another.value; }
        bool operator !=(InputIterator another) const { return !( *this == another); }
    };
    virtual InputIterator begin() = 0;
    virtual InputIterator end() = 0;

};

class Record
{
    std::tuple<int, int, int> time;
    std::string message;
public:
    Record() : time(std::make_tuple(0, 0, 0)), message("") {}
    Record(std::tuple<int, int, int> t, std::string mes) : time(t), message(mes) {}
            /*std::tuple<int, int, int> t = std::make_tuple(0, 0, 0),
            std::string mes = " ");*/
    Record(const Record &rec) : time(rec.time), message(rec.message) {}
        /*time = rec.time;
        message = rec.message;
    }*/
};

class Journal : public Iterable<Record>
{
    std::vector<Record> book;
public:
    Journal() : book() {}
    Journal(std::vector<Record> rec) : book(rec.time, rec.message) {}
    Journal(Journal &jour) : book(jour.book) {}
    /*InputIterator begin()
    {*/

}

int main(){
    Journal p{ {std::make_tuple(1, 2, 3), "Timmy"},
               {std::make_tuple(12, 34, 52), "John"} };

}
