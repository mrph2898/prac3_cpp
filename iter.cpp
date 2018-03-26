#include <iostream>
#include <forward_list>
#include <vector>
#include <iterator>
#include <tuple>
#include <string>
#include <ostream>
#include <fstream>
#include <ios>
#include <cstdio>
#include <stdexcept>
#include <algorithm>

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

std::ostream& operator <<(std::ostream &out, const Journal &j)
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
class PSIterator : public std::iterator<
                            std::input_iterator_tag,
                            unsigned,
                            long,
                            PSIterator*,
                            unsigned>
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
    PSIterator& operator ++() {
        num_in_sequence++;
        return *this;
    }
    PSIterator& operator ++(int) {
        ++(*this);
        return (*this);
    }
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
    for (const auto &k : s) {
        out << k << " ";
    }
    out << std::endl;
    return out;
}

typedef std::string word;

#define BIGWORD(x)\
            if (x.fail()) {\
                x.close();\
                throw std::runtime_error("BIG WORD IN FILE");\
            }
#define BIGFILE(x,y) \
            if (((x = y.tellg())) == -1L) {\
                y.close();\
                throw std::runtime_error("BIG FILE");\
            }
#define CANTOPEN(x)\
            if (x.fail()) {\
                x.close();\
                throw std::runtime_error("CAN'T OPEN FILE");\
            }

class FileIterator : public std::iterator<
                            std::input_iterator_tag,
                            word,
                            long,
                            FileIterator*,
                            word>
{
    word file_word;
    std::string file_name;
    std::ifstream file;
    long position;
    long eof_position;
    bool is_it_eof;
public:
    FileIterator(const std::string &name, bool eof_indicator = false) {
        is_it_eof = eof_indicator;
        file_name = name;
        if (!is_it_eof) {
            file.open(file_name, std::ios::ate);
            CANTOPEN(file)
            BIGFILE(eof_position, file)
            file.close();
            file.open(file_name, std::ios::in);
            CANTOPEN(file)
            file >> file_word;
            BIGWORD(file)
            BIGFILE(position, file)
        }
    }
    FileIterator(const FileIterator &fi) : file_word(fi.file_word), file_name(fi.file_name),
        position(fi.position), eof_position(fi.eof_position), is_it_eof(fi.is_it_eof)
    {
        if (!is_it_eof) {
            file.open(file_name);
            CANTOPEN(file)
            file.seekg(position, file.beg);
        }
    }
    word operator *() { return file_word; }
    FileIterator& operator ++() {
        file >> file_word;
        if (file.eof()) {
            file.close();
            is_it_eof = true;
            position = eof_position;
        } else BIGWORD(file)
          else BIGFILE(position, file)
        return *this;
    }
    FileIterator& operator ++(int) {
        ++(*this);
        return *this;
    }
    bool operator ==(const FileIterator &f) {
        return is_it_eof == f.is_it_eof;
    }
    bool operator !=(const FileIterator &f) { return !(*this == f); }
};
#undef CANTOPEN
#undef BIGFILE
#undef BIGWORD

typedef FileIterator const_FileIterator;

class FileSequence : Iterable<word, FileIterator, const_FileIterator>
{
    std::string file_name;
public:
    FileSequence(const std::string &name) { file_name = name; }
    FileIterator begin() { return FileIterator(file_name); }
    FileIterator end() { return FileIterator(file_name, 1); }
    const_FileIterator begin() const { return FileIterator(file_name); }
    const_FileIterator end() const { return FileIterator(file_name, 1); }
};

template<class Itrbl>
void print_iterable(const Itrbl& seq) {
    for (const auto &k: seq){
        std::cout << k << std::endl;
    }
}

int main(){
    //Emty example for Journal iterator
    Journal empt;
    std::cout << empt;
    //Non-empty example for Journal iterator
    Journal j ({{{3, 2, 3}, "Timmy"},
                {{1, 3, 4}, "John"}});
    std::cout << j;
    //Empty (i == 1) and Non-empty example for PairSequence iterator
    for (int i = 1; i < 10; i++) {
        PairSequence s(i);
        std::cout << s;
    }
    //Non-empty Example for File iterator (or empty, if there is no file)
    const std::string test_file = "moretest";
    FileSequence h(test_file);
    try {
        for(const auto &hh : h) {}
    } catch (std::runtime_error &re) {
        std::cout << "\x1b[31m" << re.what() << " " << "\"" << test_file << "\"" << "\x1b[0m" << std::endl;
    }
    const std::string f_name = "testfile";
    FileSequence f(f_name);
    try {
        for (const auto &g : f) {
            std::cout << g << ' ';
        }
        std::cout << std::endl;
    //Count_if for FileIterator
        int larger_then_4 = std::count_if(f.begin(), f.end(),
                                            [](std::string elem){ return elem.size() > 4; });
        std::cout << "Amount of words larger then 4 in file " << f_name << " is " << larger_then_4<< std::endl;
        //Printing with f()
        print_iterable(f);
    } catch (std::runtime_error &re) {
        std::cout << "\x1b[31m" << re.what() << " " << "\"" << f_name << "\"" << "\x1b[0m" << std::endl;
    }
    //Count_if for Journal
    int registered_after2 = std::count_if(j.begin(), j.end(),
                                          [](std::pair<Time, std::string> record){
                                                return (std::get<0>(std::get<0>(record)) >= 2) &&
                                                       (std::get<0>(std::get<0>(record)) >= 0) &&
                                                       (std::get<0>(std::get<0>(record)) > 0);
                                            });
    std::cout << "Amount of person, who register after 2 hour is " << registered_after2 << std::endl;
    //Find_if for PairSequenceIterator
    PSIterator divisible_by_3;
    PairSequence s(10);
    divisible_by_3 = std::find_if(s.begin(), s.end(), [](unsigned num_in_seq){ return num_in_seq % 3 == 0;});
    std::cout << "First element that divide by 3 in sequence with N=10 is " << *divisible_by_3 << std::endl;
    //Printing with f()
    print_iterable(j);
    print_iterable(s);
}
