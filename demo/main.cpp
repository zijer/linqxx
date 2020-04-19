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
    std::vector<Human> people =
        {
            Human("bob", "adolfovich", 12),
            Human("adolf", "bobovich", 15),
            Human("albert", "albertovich", 90),
            Human("mr", "polizai", 9),
            Human("pavel", "odentsov", 34),
            Human("ivan", "grechkin", 87),
            Human("john", "cena", 17),
            Human("seam", "seap", 21)};
    linqxx::from(people)
        ->where([](auto human) { return human.age > 16; })
        ->group_by([](auto human) { return human.age > 40; })
        ->for_each([](auto group) {
            std::cout << (group->key ? "old" : "young") << std::endl;
            group->for_each([](auto human) {
                std::cout
                    << "\t" << human.first_name
                    << ' ' << human.last_name
                    << '(' << human.age
                    << "y.o.)" << std::endl;
            });
        });

    return 0;
}