#include <iostream>
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

/*typedef std::iterator<
                      std::input_iterator_tag,
                      std::pair<std::tuple<int, int, int>, std::string>,
                      long,
                      const std::pair<std::tuple<int, int, int>, std::string>*,
                      std::pair<std::tuple<int, int, int>, std::string> > InputIterator;**/
typedef std::vector<std::pair<std::tuple<int, int, int>, std::string>>::iterator JournalIterator;
typedef std::vector<std::pair<std::tuple<int, int, int>, std::string>>::const_iterator const_JournalIterator;

class Journal : public Iterable<std::pair<std::tuple<int, int, int>, std::string>,
                                JournalIterator,
                                const_JournalIterator>
{
    std::vector<std::pair<std::tuple<int, int, int>, std::string>> book;
public:
    JournalIterator begin() { return book.begin();}
    JournalIterator end() { return book.end(); }
    const_JournalIterator begin() const { return book.cbegin(); }
    const_JournalIterator end() const { return book.cend(); }
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

std::ostream& operator <<(std::ostream& out, const Journal &j){
        out << "  Time  " << "Name" << std::endl;
        for (auto &k : j) {
            out << std::get<0>(std::get<0>(k)) << ":"
                << std::get<1>(std::get<0>(k)) << ":"
                << std::get<2>(std::get<0>(k)) << " " << k.second << std::endl;
        }
        return out;
}

typedef std::vector<int>::iterator PVIterator;
typedef std::vector<int>::iterator const_PVIterator;

class PairVector : public Iterable<int, PVIterator, const_PVIterator>
{
    /////
    int N;
public:
    PVIterator begin()
}

int main(){
    Journal j ({std::make_tuple(1, 2, 3), "Timmy"});/*,
                {std::make_tuple(2, 3, 4), "John"}});*/
    std::cout << j;
}
