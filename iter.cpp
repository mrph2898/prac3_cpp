#include <iostream>
#include <forward_list>
#include <vector>
#include <iterator>
#include <tuple>
#include <string>
#include <ostream>

template <typename ValueType, typename InputIterator, typename const_InputIterator>
class Iterable
{
public:
    virtual InputIterator begin() = 0;
    virtual InputIterator end() = 0;
    virtual const_InputIterator begin() const = 0;
    virtual const_InputIterator end() const = 0;
};

//I use forward_list, because it doesn't needed in bidirectional access
typedef std::forward_list<std::pair<std::tuple<int, int, int>, std::string>>::iterator JournalIterator;
typedef std::forward_list<std::pair<std::tuple<int, int, int>, std::string>>::const_iterator const_JournalIterator;
typedef std::tuple<int, int, int> Time;

class Journal : public Iterable<std::pair<Time, std::string>,
                                JournalIterator,
                                const_JournalIterator>
{
    std::forward_list<std::pair<Time, std::string>> book;
public:
    JournalIterator begin() { return book.begin();}
    JournalIterator end() { return book.end(); }
    const_JournalIterator begin() const { return book.begin(); } //.cbegin and .cend are equal with .begin and .end
    const_JournalIterator end() const { return book.end(); }     //in this methods
    Journal() : book(0) {}
    Journal(const std::pair<Time, std::string> &rec) { book.push_front(rec); }
    Journal(const std::forward_list<std::pair<Time, std::string>> &b)
    {
        for (auto &k : b) {
            book.push_front(k);
        }
    }
};

std::ostream& operator <<(std::ostream& out, const Journal &j){
        out << "  Time  " << "Name" << std::endl;
        for (auto &k : j) {
            out << std::get<0>(std::get<0>(k)) << ":"
                << std::get<1>(std::get<0>(k)) << ":"
                << std::get<2>(std::get<0>(k)) << " " << k.second << std::endl;
        }
        return out;
}
/*
typedef std::vector<int>::iterator PVIterator;
typedef std::vector<int>::iterator const_PVIterator;

class PairVector : public Iterable<int, PVIterator, const_PVIterator>
{
    int sequence_num;
    int N;
public:
    PVIterator begin() { return 1; }
};
*/
int main(){
    Journal j ({std::make_tuple(1, 2, 3), "Timmy"});
            //   {std::make_tuple(2, 3, 4), "John"});
    std::cout << j;
}
