#include "linqxx.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
// using namespace linqxx;
struct Person
{
    std::string name;
    int age;
    Person(const string &aName, int anAge) : name(aName), age(anAge)
    {
    }
};
int lol(const Person& p)
{
    return p.age;
}
int main()
{
    std::vector<Person> vec;
    for (int i = 0; i < 200; i++)
        vec.push_back(Person("lol", i));

    auto enumer = linqxx::from(vec)
        ->where([](auto a) -> bool 
        { 
            return a.age % 2 == 0; 
        })
        ->select([](auto a) -> int 
        { 
            return a.age; 
        });

    auto a = enumer->to_vector();
    auto b = enumer->to_vector();

    std::cout << a.size() << std::endl;
    std::cout << b.size() << std::endl;

    return 0;
};