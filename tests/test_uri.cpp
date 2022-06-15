#include "../dory/uri.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    dory::Uri::ptr uri = dory::Uri::Create("http://www.baidu.com/test/uri?id=100&name=dory#frg");
    std::cout << uri->toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr;
    return 0;
}
