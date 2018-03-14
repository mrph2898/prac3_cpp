#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <string>
#include <ostream>

template <typename ValueType, typename InputIterator>
class Iterable
{
public:
    virtual InputIterator begin() = 0;
    virtual InputIterator end() = 0;
   // const virtual InputIterator const_begin() = 0;
    //const virtual InputIterator const_end() = 0;
};

//Write 1 class iterator
class Journal : public Iterable<std::pair<std::tuple<int, int, int>, std::string>, >
{
    std::vector<std::pair<std::tuple<int, int, int>, std::string>> book;
public:
    InputIterator begin() { return InputIterator(*book.begin());}
    InputIterator end() { return InputIterator(*book.end()); }
    Journal() : book(0) {}
    Journal(const std::pair<std::tuple<int, int, int>, std::string> &rec) { book.push_back(rec); }
    Journal(const std::vector<std::pair<std::tuple<int, int, int>, std::string>> &b)
    {
        for (auto &k : b) {
            book.push_back(k);
        }
    }
    Journal(const Journal &jour) { book = jour.book;}
    friend std::ostream& operator <<(std::ostream& out, const Journal &j);
};

std::ostream& operator <<(std::ostream& out, Journal &j){
        out << "  Time  " << "Name" << std::endl;
        for (auto k : j) {
            out << std::get<0>(std::get<0>(k)) << ":"
                << std::get<1>(std::get<0>(k)) << ":"
                << std::get<2>(std::get<0>(k)) << " " << k.second << std::endl;
        }
        return out;
    }

int main(){
    Journal j ({std::make_tuple(1, 2, 3), "Timmy"});/*,
                {std::make_tuple(2, 3, 4), "John"}});*/
    std::cout << j;
}
