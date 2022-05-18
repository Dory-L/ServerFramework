#include "../dory/address.h"
#include "../dory/log.h"

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test() {
    std::vector<dory::Address::ptr> addrs;

    bool v = dory::Address::Lookup(addrs, "www.baidu.com");
    if (!v) {
        DORY_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }
    for (size_t i = 0; i < addrs.size(); ++i) {
        DORY_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString() << " family=" << addrs[i]->getFamily();
    }
}

void test_iface() {
    std::multimap<std::string, std::pair<dory::Address::ptr, uint32_t> > results;
    bool v = dory::Address::GetInterfaceAddress(results);
    if (!v) {
        DORY_LOG_ERROR(g_logger) << "GetinterfaceAddress fail";
        return;
    }
    for (auto& i : results) {
        DORY_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - "
            << i.second.second;
    }
}

void test_ipv4() {
    // auto addr = dory::IPAddress::Create("www.baidu.com");
    auto addr = dory::IPAddress::Create("127.0.0.8");
    if (addr) {
        DORY_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char const *argv[])
{
    // test();
    // test_iface();
    test_ipv4();
    return 0;
}
