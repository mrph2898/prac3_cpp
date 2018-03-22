#include <iostream>
#include <forward_list>
#include <vector>
#include <iterator>
#include <tuple>
#include <string>
#include <ostream>
#include <fstream>
#include <ios>

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
    Journal(const std::forward_list<std::pair<Time, std::string>> &b) : book(b) {}
};

std::ostream& operator <<(std::ostream& out, const Journal &j)
{
        out << "Time  " << "Name" << std::endl;
        for (auto &k : j) {
            out << std::get<0>(std::get<0>(k)) << ":"
                << std::get<1>(std::get<0>(k)) << ":"
                << std::get<2>(std::get<0>(k)) << "  " << k.second << std::endl;
        }
        return out;
}

#define odd(x) ((x % 2) == 1)
class PSIterator
{
    unsigned num_in_sequence;
    unsigned N;
public:
    PSIterator(unsigned num_in_s = 1, unsigned n = 1 ) {
        num_in_sequence = num_in_s;
        N = n;
    }
    unsigned operator *()
    {
        if (odd(num_in_sequence)) {
            return 1 + (num_in_sequence / 2);
        } else {
            return N - (num_in_sequence / 2) + 1;
        }
    }
    void operator ++() { num_in_sequence++; }
    void operator ++(int) { (*this)++; }
    bool operator ==(const PSIterator &s) { return (*this).num_in_sequence == s.num_in_sequence; }
    bool operator !=(const PSIterator &s) { return !(*this == s); }
};

class PairSequence : public Iterable<unsigned, PSIterator, const PSIterator>
{
    unsigned  N;
public:
    PairSequence(unsigned n = 1) { N = n; }
    PSIterator begin() { return PSIterator(1, N); }
    PSIterator end() {
        if (odd(N)){
            return PSIterator(N , N);
        } else {
            return PSIterator(N + 1, N);
        }
    }
    const PSIterator begin() const { return PSIterator(1, N); }
    const PSIterator end() const {
        if (odd(N)){
            return PSIterator(N , N);
        } else {
            return PSIterator(N + 1, N);
        }
    }
};
#undef odd

std::ostream& operator <<(std::ostream& out, const PairSequence &s)
{
    for (auto &&k : s) {
        out << k << " ";
    }
    out << std::endl;
    return out;
}

typedef std::string word;

class FileIterator
{
    word file_word;
    std::string file_name;
public:
    void set_string(std::ifstream &file)
    {
        if (file.eof()) {
            file.close();
        } else {
            std::getline(file, file_word, ' ');
        }
    }
    explicit FileIterator(std::string name, std::ios_base::openmode mode = std::ios_base::in) {
        file_name = name;
        std::ifstream file(file_name);
        if (!file.is_open()){
            file.open(file_name, mode);
        }
        set_string(file);
    }
    word operator *() { return file_word; }
    void operator ++() {
        std::ifstream file(file_name);
        set_string(file);
    }
    void operator ++(int) { (*this)++; }
    bool operator ==(const FileIterator &f) { return (*this).file_word == f.file_word; }
    bool operator !=(const FileIterator &f) { return !(*this == f); }
};

class FileSequence : Iterable<word, FileIterator, const FileIterator>
{
    word file_word;
    std::string file_name;
public:
    FileSequence(const std::string &name) { file_name = name; }
    FileIterator begin() { return FileIterator(file_name, std::ios::in); }
    FileIterator end() { return FileIterator(file_name, std::ios::ate); }
    const FileIterator begin() const { return FileIterator(file_name, std::ios::in); }
    const FileIterator end() const { return FileIterator(file_name, std::ios::ate); }
};

int main(){
    Journal j ({{std::make_tuple(1, 2, 3), "Timmy"},
                {std::make_tuple(2, 3, 4), "John"}});
    for (int i = 1; i < 10; i++) {
        PairSequence s(i);
        std::cout << s;
    }
    std::cout << j;
    FileSequence f("testfile");
    for (auto &&g : f) {
        std::cout << g << std::endl;
    }
}
