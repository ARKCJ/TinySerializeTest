#pragma once

#include<vector>
#include<list>
#include<map>
#include<set>
#include<stddef.h>
#include<string.h>
#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<sstream>
#include<stdexcept>


namespace test
{
namespace serialize
{

    class DataStream;

    class Serializable{
    public:
        void Serialize(DataStream& stream) const;

        bool DeSerialize(DataStream& stream);

    private:
        virtual void SerializeImp(DataStream& stream) const = 0;
        virtual bool DeSerializeImp(DataStream& stream) = 0;
    };

    class DataStream{
    friend class Serializable;
    public:
        enum DataType{
            BOOL = 0,
            CHAR,
            INT32,
            INT64,
            SIZE_T,
            FLOAT,
            DOUBLE,
            STRING,
            VECTOR,
            LIST,
            MAP,
            SET,
            CUSTOM
        };

        enum ByteOrder{
            BigEndian,
            LittleEndian
        };
    private:
        size_t m_Pos;
        ByteOrder m_byteorder;
        std::vector<char> m_Buf;

        void write(const char* data, size_t len)
        {
            Reserve(len);
            size_t size =  m_Buf.size();
            m_Buf.resize(size + len);
            memcpy(&m_Buf[m_Pos], data, len);
            m_Pos += len;
        }

        bool read(char* data, size_t len)
        {
            memcpy(data, (char*)(&m_Buf[m_Pos]), len);
            m_Pos += len;
            return true;
        }

    public:
        DataStream():m_Pos(0)
        { 
            int n = 0x12345678;
            char str[4];
            memcpy(str, &n, sizeof(int));
            if(str[0] == 0x12){
                m_byteorder = BigEndian; 
            }
            m_byteorder = LittleEndian; 
        }

        // void show() const
        // {
        //     size_t size = m_buf.size();
        //     std::cout << "data size = " << size << std::endl;
        //     //
        //     //
        // }

        void write(){}

        void write(bool val)
        {
            char type = BOOL;
            write(&type, sizeof(char));
            write((char*)&val, sizeof(char));
        }

        void write(char val)
        {
            char type = CHAR;
            write(&type, sizeof(char));
            write((char*)&val, sizeof(char));
        }

        void write(int32_t val)
        {
            // std::cout << "PUSH: " << val << std::endl;
            char type = INT32;
            write(&type, sizeof(char));
            ToSmallEndian((char*)(&val), (char*)(&val) + sizeof(int32_t));
            write((char*)(&val), sizeof(int32_t));
        }

        void write(int64_t val)
        {
            char type = INT64;
            write(&type, sizeof(char));
            ToSmallEndian((char*)(&val), (char*)(&val) + sizeof(int64_t));
            write((char*)(&val), sizeof(int64_t));
        }

        void write(size_t val)
        {
            char type = SIZE_T;
            write(&type, sizeof(char));
            ToSmallEndian((char*)(&val), (char*)(&val) + sizeof(size_t));
            write((char*)(&val), sizeof(size_t));
            // std::cout << "W: " << val << std::endl;
        }

        void write(float val)
        {
            char type = FLOAT;
            write(&type, sizeof(char));
            ToSmallEndian((char*)(&val), (char*)(&val) + sizeof(float));
            write((char*)(&val), sizeof(float));
        }

        void write(double val)
        {
            char type = DOUBLE;
            write(&type, sizeof(char));
            ToSmallEndian((char*)(&val), (char*)(&val) + sizeof(double));
            write((char*)(&val), sizeof(double));
        }

        void write(const char* val)
        {
            char type = STRING;
            write(&type, sizeof(char));
            size_t len = strlen(val);
            write(len);
            write(val, len);
        }

        void write(const std::string& val)
        {
            char type = STRING;
            write(&type, sizeof(char));
            size_t len = val.size();
            write(len);
            write(val.data(), len);
            // std::cout << "Write String: " << val << std::endl;
        }

        bool read(){ return true; }

        bool read(bool& value)
        {
            if(m_Buf[m_Pos] != BOOL){
                return false;
            }
            value = (bool)m_Buf[++m_Pos];
            ++m_Pos;
            return true;
        }

        bool read(char& value)
        {
            if(m_Buf[m_Pos] != CHAR){
                return false;
            }
            value = (char)m_Buf[++m_Pos];
            ++m_Pos;
            return true;
        }

        bool read(int32_t& value)
        {
            if(m_Buf[m_Pos] != INT32){
                return false;
            }
            ++m_Pos;
            read((char*)(&value), sizeof(int32_t));
            ToBigEndian((char*)(&value), (char*)(&value) + sizeof(int32_t));
            return true;
        }

        bool read(int64_t& value)
        {
            if(m_Buf[m_Pos] != INT64){
                return false;
            }
            ++m_Pos;
            read((char*)(&value), sizeof(int64_t));
            ToBigEndian((char*)(&value), (char*)(&value) + sizeof(int64_t));
            return true;
        }

        bool read(size_t& value)
        {
            if(m_Buf[m_Pos] != SIZE_T){
                return false;
            }
            ++m_Pos;
            read((char*)(&value), sizeof(size_t));
            ToBigEndian((char*)(&value), (char*)(&value) + sizeof(size_t));
            // std::cout << "R: " << value << std::endl;
            return true;
        }

        bool read(float& value)
        {
            if(m_Buf[m_Pos] != FLOAT){
                return false;
            }
            ++m_Pos;
            read((char*)(&value), sizeof(float));
            ToBigEndian((char*)(&value), (char*)(&value) + sizeof(float));
            return true;
        }

        bool read(double& value)
        {
            if(m_Buf[m_Pos] != DOUBLE){
                return false;
            }
            ++m_Pos;
            read((char*)(&value), sizeof(double));
            ToBigEndian((char*)(&value), (char*)(&value) + sizeof(double));
            return true;
        }

        bool read(std::string& value)
        {
            if(m_Buf[m_Pos] != STRING){
                return false;
            }
            ++m_Pos;
            size_t len;
            read(len);
            value.assign((char*)(&m_Buf[m_Pos]), len);
            m_Pos += len;
            // std::cout << "Read String: " << value << std::endl;
            return true;
        }

        void clear()
        { 
            m_Buf.clear(); 
            m_Pos = 0;
        }

        size_t size(){ return m_Buf.size(); }

        void reset() { m_Pos = 0; }

        const char* data() const { return m_Buf.data(); }

        void save(const std::string& fileName)
        {
            std::ofstream fout(fileName);
            try{
                fout.write(data(), size());
                fout.flush();
            }catch(...){
                fout.close();
                throw;
            }
            fout.close();
        }

        void load(const std::string & fileName)
        {
            std::ifstream fin(fileName);
            std::stringstream ss;
            ss << fin.rdbuf();
            const std::string & str = ss.str();
            clear();
            Reserve(str.size());
            write(str.data(), str.size());
        }

        template<typename T, typename Alloc>
        void write(const std::vector<T, Alloc>& val)
        {
            char type = VECTOR;
            write(&type, sizeof(char));
            size_t len = val.size();
            write(len);
            for(auto& v : val){
                write(v);
            }
        }

        template<typename T, typename Alloc>
        void write(const std::list<T, Alloc>& val)
        {
            char type = LIST;
            write(&type, sizeof(char));
            size_t len = val.size();
            write(len);
            for(auto& v : val){
                write(v);
            }
        }

        template<typename K, typename V, typename Compare, typename Alloc>
        void write(const std::map<K, V, Compare, Alloc>& val)
        {
            char type = MAP;
            write(&type, sizeof(char));
            size_t len = val.size();
            write(len);
            for(auto& v : val){
                write(v.first);
                write(v.second);
            }
        }

        template<typename K, typename Compare, typename Alloc>
        void write(const std::set<K, Compare, Alloc>& val)
        {
            char type = SET;
            write(&type, sizeof(char));
            size_t len = val.size();
            write(len);
            for(auto& v : val){
                write(v);
            }
        }

        template<typename T, typename Alloc>
        bool read(std::vector<T, Alloc>& val)
        {
            if((char)m_Buf[m_Pos] != VECTOR){
                return false;
            }
            size_t original = m_Pos;
            ++m_Pos;
            val.clear();
            size_t len;
            read(len);
            for(int i = 0; i < len; ++i){
                T v;
                if(read(v) == false){
                    m_Pos = original;
                    return false;
                }
                val.push_back(v);
            }
            return true;
        }

        template<typename T, typename Alloc>
        bool read(std::list<T, Alloc>& val)
        {
            if((char)m_Buf[m_Pos] != LIST){
                return false;
            }
            size_t original = m_Pos;
            ++m_Pos;
            val.clear();
            size_t len;
            read(len);
            for(int i = 0; i < len; ++i){
                T v;
                if(read(v) == false){
                    m_Pos = original;
                    return false;
                }
                val.push_back(v);
            }
            return true;
        }

        template<typename K, typename V, typename Compare, typename Alloc>
        bool read(std::map<K, V, Compare, Alloc>& val)
        {
            if((char)m_Buf[m_Pos] != MAP){
                return false;
            }
            size_t original = m_Pos;
            ++m_Pos;
            val.clear();
            size_t len;
            read(len);
            for(int i = 0; i < len; ++i){
                K k;
                if(read(k) == false){
                    m_Pos = original;
                    return false;
                }
                V v;
                if(read(v) == false){
                    m_Pos = original;
                    return false;
                }
                val[k] = v;
            }
            return true;
        }

        template<typename K, typename Compare, typename Alloc>
        bool read(std::set<K, Compare, Alloc>& val)
        {
            if((char)m_Buf[m_Pos] != SET){
                return false;
            }
            size_t original = m_Pos;
            ++m_Pos;
            val.clear();
            size_t len;
            read(len);
            for(int i = 0; i < len; ++i){
                K v;
                if(read(v) == false){
                    m_Pos = original;
                    return false;
                }
                val.insert(v);
            }
            return true;
        }

        void write(const Serializable& val)
        {
            val.Serialize(*this);
        }
        
        bool read(Serializable& val)
        {
            return val.DeSerialize(*this);
        }

        template<typename T>
        DataStream& operator<<(const T& val)
        {
            write(val);
            return *this;
        }

        template<typename T>
        DataStream& operator>>(T&& val)
        {
            if(read(std::forward<T>(val)) == false){
                throw std::runtime_error("Try to read wrong type.");
            }
            return *this;
        }

        template<typename T, typename ...Args>
        void write_args(T&& head, Args&&... args)
        {
            write(std::forward<T>(head));
            write_args(std::forward<Args>(args)...);
        }

        void write_args(){}

        template<typename T, typename ...Args>
        bool read_args(T& head, Args&... args)
        {
            if(read(head) == false){
                return false;
            }
            return read_args(args...);
        }

        bool read_args(){ return true; }

    private:
        void Reserve(size_t len)
        {
            size_t cap = m_Buf.capacity();
            cap = std::max((size_t)1, cap);
            while(cap < m_Buf.size() + len){
                cap *= 2;
            }
            m_Buf.reserve(cap);
        }

        void ToSmallEndian(char* first, char* last)
        {
            if(m_byteorder == BigEndian){
                std::reverse(first, last);
            }
        }

        void ToBigEndian(char* first, char* last)
        {
            if(m_byteorder == BigEndian){
                std::reverse(first, last);
            }
        }
    };

    void Serializable::Serialize(DataStream& stream) const
    {
        char type = DataStream::CUSTOM;
        stream.write(&type, sizeof(char));
        // std::cout << "Class Name(S)" << std::string(typeid(*this).name()) << std::endl;
        stream << std::string(typeid(*this).name());
        this -> SerializeImp(stream);
    }

    bool Serializable::DeSerialize(DataStream& stream)
    {
        if((char)stream.m_Buf[stream.m_Pos] != DataStream::CUSTOM){
            return false;
        }
        size_t orginal = stream.m_Pos;
        ++stream.m_Pos;
        std::string className;
        stream >> className;
        // std::cout << "Class Name(D1)" << className << std::endl;
        // std::cout << "Class Name(D2)" << typeid(*this).name() << std::endl;
        if(typeid(*this).name() != className){
            stream.m_Pos = orginal;
            return false;
        }
        if(this -> DeSerializeImp(stream) == false){
            stream.m_Pos = orginal;
            return false;
        }
        return true;
    }
}
}