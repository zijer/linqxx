#include <vector>
#include <linqxx/linqxx.hpp>
#include <iostream>
int main()
{
    std::vector<int> vec;
    for (int i = 0; i < 100; i++)
        vec.push_back(i);
    auto k = linqxx::grouping_enumerable<int, int>::from(3, vec);
    auto groupbytest = linqxx::groupby_enumerable<int, int>::from(linqxx::from(vec), [](auto i) -> int { return i % 3; });
    groupbytest->for_each([](auto group) -> void {
        std::cout << group->key << std::endl;
        group->for_each([](auto i) -> void {
            std::cout << "\t" << i << "%" << 3 << "=" << (i % 3) << std::endl;
        });
    });
    // auto seq = linqxx::from(vec)
    //     ->where([](auto i) -> auto{return i % 3 == 0;})
    //     ->select([](auto i) -> int{return i / 3;});
    // seq->for_each([](auto i)->void{
    //     std::cout << i << std::endl;
    // });
    // std::cout << seq->to_vector().size() << std::endl;
    return 0;
}