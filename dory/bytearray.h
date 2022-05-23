#pragma once

#include <memory>
#include <string>
#include <stdint.h>
#include <sys/uio.h>
#include <vector>

namespace dory {

class ByteArray {
public:
    typedef std::shared_ptr<ByteArray> ptr;

    struct Node {
        Node(size_t s);
        Node();
        ~Node();

        char* ptr;
        Node* next;
        size_t size;
    };
    
    ByteArray(size_t base_size = 4096);
    ~ByteArray();

    //write
    void writeFint8(int8_t value);
    void writeFuint8(uint8_t value);
    void writeFint16(int16_t value);
    void writeFuint16(uint16_t value);
    void writeFint32(int32_t value);
    void writeFuint32(uint32_t value);
    void writeFint64(int64_t value);
    void writeFuint64(uint64_t value);

    void writeInt32(int32_t value);
    void writeUint32(uint32_t value);
    void writeInt64(int64_t value);
    void writeUint64(uint64_t value);

    void writeFloat(float value);
    void writeDouble(double value);
    //length:int16, data
    void writeStringF16(const std::string& value);
    //length:int32, data
    void writeStringF32(const std::string& value);
    //length:int64, data
    void writeStringF64(const std::string& value);
    //length:varint, data
    void writeStringVint(const std::string& value);
    //data
    void writeStringWithoutLength(const std::string& value);

    //read
    int8_t   readFint8();
    uint8_t  readFuint8();
    int16_t  readFint16();
    uint16_t readFuint16();
    int32_t  readFint32();
    uint32_t readFuint32();
    int64_t  readFint64();
    uint64_t readFuint64();

    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();

    float    readFloat();
    double   readDouble();

    //length:int16, data
    std::string readStringF16();
    //length:int32, data
    std::string readStringF32();
    //length:int64, data
    std::string readStringF64();
    //length:varint, data
    std::string readStringVint();

    //内部操作
    void clear();

    void write(const void* buf, size_t size);
    void read(void* buf, size_t size);
    void read(void* buf, size_t size, size_t position) const;
    size_t getPosition() const { return m_position; }
    void setPosition(size_t v);

    bool writeToFile(const std::string& name) const;
    bool readFromFile(const std::string& name);

    size_t getBaseSize() const { return m_baseSize; }
    size_t getReadSize() const { return m_size - m_position; }

    bool isLittleEndian() const;
    void setIsLittleEndian(bool val);

    /**
     * @brief 将ByteArray里的数据[m_position, m_size)转换成std::string
     * 
     * @return std::string 
     */
    std::string toString() const;

    /**
     * @brief 将ByteArray里的数据[m_position, m_size)转换成16进制的std::string(格式:FF FF FF)
     * 
     * @return std::string 
     */
    std::string toHexString() const;

    /**
     * @brief 获取可读取的缓存，保存成iovec数组
     * 
     * @param buffers 保存可读取的内存的iovec数组
     * @param len 读取数据的长度，如果len > getReadSize() 则len = getReadSize()
     * @param position 读取数据的位置
     * @return uint64_t 实际的长度
     */
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const ;

    /**
     * @brief 获取可读取的缓存，保存成iovec数组，从position位置开始
     * 
     * @param buffers 保存可读取的内存的iovec数组
     * @param len 读取数据的长度，如果len > getReadSize() 则len = getReadSize()
     * @param position 读取数据的位置
     * @return uint64_t 实际的长度
     */
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;

    /**
     * @brief 获取可写入的缓存，保存成iovec数组
     * 
     * @param[out] buffers 保存可写入的内存的iovec数组
     * @param[in] len 写入的长度
     * @return uint64_t 实际的长度
     * @post 如果(m_position + len) > m_capacity扩容N个节点以容纳len长度
     */
    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);
    /**
     * @brief 返回数据的长度
     * 
     * @return size_t 
     */
    size_t getSize() const { return m_size; }
private:
    /**
     * @brief 扩容ByteArray，使其可以容纳size个数据（如果原本可以容纳，则不扩容）
     */
    void addCapacity(size_t size);

    /**
     * @brief 获取当前的可写入容量
     * 
     * @return size_t 
     */
    size_t getCapacity() const { return m_capacity - m_position; }

private:
    ///内存块的大小
    size_t m_baseSize;
    ///当前的操作位置
    size_t m_position;
    ///当前的总容量
    size_t m_capacity;
    ///当前的数据大小
    size_t m_size;
    ///字节序，默认大段
    int8_t m_endian;
    ///第一个内存块指针
    Node* m_root;
    ///当前操作的内存指针
    Node* m_cur;
};

}