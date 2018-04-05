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
#include <functional>

template <typename ValueType, class InputIterator>
class IgnoreIterator : public std::iterator<
                            std::input_iterator_tag,
                            ValueType,
                            long,
                            ValueType*,
                            ValueType>
{
    InputIterator current;
    InputIterator finish;
    bool (*predicate)(const ValueType&);
public:
    IgnoreIterator(InputIterator begin, InputIterator end, bool (*pred)(const ValueType&)) :
        current(std::find_if(begin, end, pred)), finish(end), predicate(pred){}
    ValueType operator *() const { return *current; }
    IgnoreIterator& operator ++() {
        while ((++current != finish) && !predicate(*current)){}
        return *this;
    }
    IgnoreIterator operator ++(int) {
        IgnoreIterator tmp(*this);
        ++tmp;
        return *this;
    }
    bool operator ==(const InputIterator &s) const { return (*this).current == s; }
    bool operator !=(const InputIterator &s) const { return !(*this == s); }
};

template <typename ValueType,
         typename InputIterator,
         typename const_InputIterator>
class Iterable
{
public:
    virtual InputIterator begin() = 0;
    virtual InputIterator end() = 0;
    virtual const_InputIterator begin() const = 0;
    virtual const_InputIterator end() const = 0;
    virtual IgnoreIterator<ValueType, InputIterator> filter(bool (*)(const ValueType&)) = 0;
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
    IgnoreIterator<std::pair<Time, std::string>, JournalIterator>
    filter(bool (*p)(const std::pair<Time, std::string> &))
    {
        return IgnoreIterator<std::pair<Time, std::string>, JournalIterator>((*this).begin(), (*this).end(), p);
    }
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
                            PSIterator>
{
    unsigned num_in_sequence;
    unsigned N;
public:
    PSIterator(unsigned num_in_s = 1, unsigned n = 1 ) {
        num_in_sequence = num_in_s;
        N = n;
    }
    unsigned operator *() const
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
    PSIterator operator ++(int) {
        PSIterator tmp(*this);
        ++tmp;
        return *this;
    }
    bool operator ==(const PSIterator &s) const { return (*this).num_in_sequence == s.num_in_sequence; }
    bool operator !=(const PSIterator &s) const { return !(*this == s); }
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
    IgnoreIterator<unsigned, PSIterator>
    filter(bool (*p)(const unsigned &))
    {
        return IgnoreIterator<unsigned, PSIterator>((*this).begin(), (*this).end(), p);
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

#define ERROR(x,y)\
    if (x fail()) {\
        x close();\
        throw std::runtime_error(y);\
    }
class FileIterator : public std::iterator<
                            std::input_iterator_tag,
                            word,
                            long,
                            FileIterator*,
                            FileIterator&>
{
    word file_word;
    std::string file_name;
    std::ifstream *file;
    bool is_it_eof;
public:
    FileIterator(const std::string &name, bool eof_indicator = false) {
        is_it_eof = eof_indicator;
        file_name = name;
        if (!is_it_eof) {
            file = new std::ifstream(file_name, std::ios::in);
            ERROR(file->, "CAN'T OPEN FILE")
            *file >> file_word;
            ERROR(file->, "CAN'T READ WORD FROM FILE")
        }
    }
    FileIterator(const FileIterator &fi) : file_word(fi.file_word), file_name(fi.file_name),
        is_it_eof(fi.is_it_eof)
    {
        file = fi.file;
        //file.move(fi.file);
        //file = static_cast<std::ifstream &&>(fi.file);
        //file.swap(fi.file);
        //std::swap(file, fi.file);
    }

    word operator *() const { return file_word; }
    FileIterator& operator ++() {
        *file >> file_word;
        if (file->eof()) {
            file->close();
            is_it_eof = true;
        }
        return *this;
    }
    FileIterator operator ++(int) {
        FileIterator tmp(*this);
        ++tmp;
        return *this;
    }
    bool operator ==(const FileIterator &f) const  {
        return is_it_eof == f.is_it_eof;
    }
    bool operator !=(const FileIterator &f) const { return !(*this == f); }
    ~FileIterator(){ delete file;}
};
#undef ERROR

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
    IgnoreIterator<word, FileIterator>
    filter(bool (*p)(const word &))
    {
        return IgnoreIterator<word, FileIterator>((*this).begin(), (*this).end(), p);
    }
};

template<class Itrbl>
void print_iterable(const Itrbl& seq) {
    for (const auto &k: seq){
        std::cout << k << std::endl;
    }
}

bool after_2(const std::pair<Time, std::string> &record)
{
    return (std::get<0>(std::get<0>(record)) >= 2) && (std::get<0>(std::get<0>(record)) >= 0) &&
           (std::get<0>(std::get<0>(record)) > 0);
}

bool div_by_3(const unsigned &num_in_seq){ return num_in_seq % 3 == 0;}
bool amnt_of_words_larger_4(const std::string &elem){ return elem.size() > 4; }

int main(){
//Empty example for Journal iterator
    Journal empt;
    std::cout << "\x1b[32m" << "Empty example for Journal iterator" << "\x1b[0m" << std::endl;
    std::cout << empt;

//Non-empty example for Journal iterator
    Journal j ({{{3, 2, 3}, "Timmy"},
                {{1, 3, 4}, "John"}});
    std::cout << "\x1b[32m" << "Non-empty example for Journal iterator" << "\x1b[0m" << std::endl;
    std::cout << j;

//Empty (i == 1) and Non-empty example for PairSequence s(i) iterator, i == 2...9
    std::cout << "\x1b[32m" <<
        "Empty (i == 1) and Non-empty example for PairSequence iterator, i == 2...9" << "\x1b[0m" << std::endl;
    for (int i = 1; i < 10; i++) {
        PairSequence s(i);
        std::cout << s;
    }

//Non-empty Example for File iterator (or empty, if there is no file)
    const std::string test_file = "moretest";
    FileSequence h(test_file);
    std::cout << "\x1b[32m" << "Empty example for File iterator" << "\x1b[0m" << std::endl;
    try {
        for(const auto &hh : h) {}
    } catch (std::runtime_error &re) {
        std::cout << "\x1b[31m" << re.what() << " " << "\"" << test_file << "\"" << "\x1b[0m" << std::endl;
    }
    const std::string f_name = "testfile";
    FileSequence f(f_name);
    std::cout << "\x1b[32m" << "Non-empty example for File iterator" << "\x1b[0m" << std::endl;
    try {
        for (const auto &g : f) {
            std::cout << g << ' ';
        }
        std::cout << std::endl;
//Count_if for FileIterator
        std::cout << "\x1b[32m" << "Count_if for File iterator" << "\x1b[0m" << std::endl;
        int larger_then_4 = std::count_if(f.begin(), f.end(),
                                            [](word elem){ return elem.size() > 4; });
        std::cout << "Amount of words larger then 4 in file " << f_name << " is " << larger_then_4<< std::endl;
//Printing with f() file
        std::cout << "\x1b[32m" << "Printing with f() for File iterator" << "\x1b[0m" << std::endl;
        print_iterable(f);
//For with filter() for File
        std::cout << "\x1b[32m" << "For with filter() for File "<< "\x1b[0m" << std::endl;
        for (auto &&p = f.filter(amnt_of_words_larger_4); p != f.end(); ++p){
            std::cout << *p << std::endl;
        }
    } catch (std::runtime_error &re) {
        std::cout << "\x1b[31m" << re.what() << " " << "\"" << f_name << "\"" << "\x1b[0m" << std::endl;
    }
//Count_if for JournalIterator
    std::cout << "\x1b[32m" << "Count_if for Journal iterator" << "\x1b[0m" << std::endl;
    int registered_after2 = std::count_if(j.begin(), j.end(),
                                          [](std::pair<Time, std::string> record){
                                                return (std::get<0>(std::get<0>(record)) >= 2) &&
                                                       (std::get<0>(std::get<0>(record)) >= 0) &&
                                                       (std::get<0>(std::get<0>(record)) > 0);
                                            });
    std::cout << "Amount of person, who register after 2 hour is " << registered_after2 << std::endl;
//Find_if for PairSequenceIterator
    std::cout << "\x1b[32m" << "Find_if for PairSequence iterator" << "\x1b[0m" << std::endl;
    PSIterator divisible_by_3;
    int k = 10;
    PairSequence s(k);
    divisible_by_3 = std::find_if(s.begin(), s.end(), [](unsigned num_in_seq){ return num_in_seq % 3 == 0;});
    if ((*divisible_by_3) % 3 != 0) {
        std::cout << "There is no elements that divided by 3 in PairSequence with N=" << k << std::endl;
    } else {
    std::cout << "First element that divide by 3 in PairSequence with N=" << k << " is " << *divisible_by_3 << std::endl;
    }
//Printing with f()
    std::cout << "\x1b[32m" << "Printing with f() for Journal ..." << "\x1b[0m" << std::endl;
    print_iterable(j);
    std::cout << "\x1b[32m" << "and PairSequence iterators" << "\x1b[0m" << std::endl;
    print_iterable(s);
//For with filter() for Journal
    std::cout << "\x1b[32m" << "For with filter() for Journal" << "\x1b[0m" << std::endl;
    for (auto &&p = j.filter(after_2); p != j.end(); ++p){
        std::cout << *p << std::endl;
    }
    for (auto &&p = empt.filter(after_2); p != empt.end(); ++p){
        std::cout << *p << std::endl;
    }
//For with filter() for PairSequence
    std::cout << "\x1b[32m" << "For with filter() for PairSequense with N="<< k << "\x1b[0m" << std::endl;
    for (auto &&p = s.filter(div_by_3); p != s.end(); ++p){
        std::cout << *p << std::endl;
    }
}
