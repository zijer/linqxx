#include <iostream>
#include <vector>
#include <string>
#include <linqxx/linqxx.hpp>

class Human
{
public:
    std::string first_name;
    std::string last_name;
    int age;
    Human(std::string first_name, std::string last_name, int age)
        : first_name(first_name), last_name(last_name), age(age){};
};

int main()
{
    std::vector<Human> people = {
        Human("bob", "adolfovich", 12),
        Human("a", "b", 15),
        Human("albert", "albertovich", 90),
        Human("mr", "polizai", 9),
        Human("pavel", "odentsov", 34),
        Human("ivan", "grechkin", 87),
        Human("john", "cena", 17),
        Human("seam", "seap", 21)};
    
    auto ages = linqxx::from(std::move(people))
                    ->select([](auto human) {
                        return Human(human.first_name, human.last_name, human.age + 2);
                    })
                    ->where([](auto human) { return human.age > 16; })
                    ->groupby([](auto human) { return human.age > 40; })
                    ->selectmany([](auto group) {
                        return group->select([](auto human) {
                            return human.age;
                        });
                    });

    for (auto t : *ages)
    {
        std::cout << t << std::endl;
    }

    ages->for_each([](auto age) {
        std::cout << age << std::endl;
    });

    for (auto i = 10; i < 20; i++)
    {
        if (auto it = std::find(ages->begin(), ages->end(), i))
        {
            std::cout << "found: " << *it << std::endl;
        }
        else
        {
            std::cout << "not found: " << i << std::endl;
        }
    }

    for (auto t : *ages)
    {
        std::cout << t << std::endl;
    }

    auto first = ages->first();
    std::cout << "first: " << first << std::endl;
    auto last = ages->last();
    std::cout << "last: " << last << std::endl;

    return 0;
}