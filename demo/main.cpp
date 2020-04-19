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
    ages->for_each([](auto age) {
        std::cout << age << std::endl;
    });

    return 0;
}