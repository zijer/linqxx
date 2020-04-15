#include <vector>
#include <linqxx/linqxx.hpp>
#include <iostream>
int main()
{
    std::vector<int> vec;
    for(int i = 0; i < 1000; i++)vec.push_back(i);
    auto seq = linqxx::from(vec)
        ->where([](auto i) -> auto{return i % 3 == 0;})
        ->select([](auto i) -> int{return i / 3;});
    seq->for_each([](auto i)->void{
        std::cout << i << std::endl;
    });
    std::cout << seq->to_vector().size() << std::endl;
    return 0;
}