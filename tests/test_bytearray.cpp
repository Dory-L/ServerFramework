#include "../dory/dory.h"
#include "../dory/bytearray.h"

dory::Logger::ptr g_logger = DORY_LOG_ROOT();

void test() {
#define XX(type, len, write_fun, read_fun, base_len) { \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    dory::ByteArray::ptr ba(new dory::ByteArray(base_len)); \
    for (auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        DORY_ASSERT(v == vec[i]); \
    } \
    DORY_ASSERT(ba->getReadSize() == 0); \
    DORY_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type ") len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
}

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) { \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    dory::ByteArray::ptr ba(new dory::ByteArray(base_len)); \
    for (auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        DORY_ASSERT(v == vec[i]); \
    } \
    DORY_ASSERT(ba->getReadSize() == 0); \
    DORY_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type ") len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    ba->setPosition(0); \
    DORY_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    dory::ByteArray::ptr ba2(new dory::ByteArray(base_len * 2)); \
    DORY_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    ba2->setPosition(0); \
    DORY_ASSERT(ba->toString() == ba2->toString()); \
    DORY_ASSERT(ba->getPosition() == 0); \
    DORY_ASSERT(ba2->getPosition() == 0); \
}

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);

#undef XX
}

void test_01() {
    std::vector<int32_t> vec;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(rand());
    }
    dory::ByteArray::ptr ba(new dory::ByteArray(1));
    for (auto& i : vec) {
        ba->writeInt32(i);
    }
    ba->setPosition(0);
    for (size_t i = 0; i < vec.size(); ++i) {
        int32_t v = ba->readInt32();
        DORY_LOG_INFO(g_logger) << i << " - " << v << " - " << vec[i];
        DORY_ASSERT(v == vec[i]);
    }
    DORY_ASSERT(ba->getReadSize() == 0);
    ba->setPosition(0);
    DORY_ASSERT(ba->writeToFile("/tmp/int32_t.dat"));
    dory::ByteArray::ptr ba2(new dory::ByteArray(1 * 2));
    DORY_ASSERT(ba2->readFromFile("/tmp/int32_t.dat"));
    ba2->setPosition(0);
    DORY_ASSERT(ba->toString() == ba2->toString());
    DORY_ASSERT(ba->getPosition() == 0);
    DORY_ASSERT(ba2->getPosition() == 0);
}

int main(int argc, char const *argv[])
{
    test();
    // test_01();
    return 0;
}
