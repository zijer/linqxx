#include <vector>
#include <linqxx/linqxx.hpp>
#include <iostream>
int main()
{
    std::vector<int> vec;
    for (int i = 0; i < 1000; i++)
        vec.push_back(i);

    linqxx::from(vec)
        ->where([](auto i) -> bool { return i % 5 == 0; })
        ->group_by<int>([](auto i) -> int { return i % 3; })
        ->for_each([](auto group)->void{
            std::cout << group->key << " " << group->to_vector().size() << std::endl;
        });
    return 0;
}