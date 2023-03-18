#include "DataStream.h"
#include <unordered_map>

class TestA : public test::serialize::Serializable{
public:
    TestA() {}
    TestA(const std::string& name, int age)
    :m_name(name), m_age(age){}

    void show()
    {
        std::cout << "name=" << m_name << ",age=" << m_age << std::endl;
    }

    void SerializeImp(test::serialize::DataStream& stream) const override
    {
        stream << m_name << m_age;
    }

    bool DeSerializeImp(test::serialize::DataStream& stream) override
    {
        return stream.read_args(m_name, m_age);
    }

private:
    std::string m_name;
    int m_age;
};


class TestB : public test::serialize::Serializable{
public:
    TestB()
    :m_age(0), testA_Ptr(nullptr){}

    TestB(const std::string& name, int age)
    :m_name(name), m_age(age), testA_Ptr(new TestA(name, age)){}

    TestB(const TestB& other)
    :m_name(other.m_name), m_age(other.m_age), 
    testA_Ptr(new TestA(*other.testA_Ptr)){}

    TestB& operator=(const TestB& other)
    {
        if(this == &other){
            return *this;
        }
        m_name = other.m_name;
        m_age = other.m_age;
        Destory();
        testA_Ptr = new TestA(*other.testA_Ptr);
    }

    TestB(TestB&& other)
    :m_name(std::move(other.m_name)), m_age(other.m_age), 
    testA_Ptr(other.testA_Ptr){ other.testA_Ptr = nullptr; }

    TestB& operator=(TestB&& other)
    {
        if(this == &other){
            return *this;
        }
        m_name = std::move(other.m_name);
        m_age = other.m_age;
        Destory();
        testA_Ptr = other.testA_Ptr;
        other.testA_Ptr = nullptr;
    }

    void show()
    {
        std::cout << "name=" << m_name << ",age=" << m_age <<  std::endl;
        testA_Ptr -> show();
    }

    void SerializeImp(test::serialize::DataStream& stream) const override
    {
        stream << m_name << m_age;
        stream << TestA(*testA_Ptr);
    }

    bool DeSerializeImp(test::serialize::DataStream& stream) override
    {
        TestA t;
        try{
            stream >> m_name >> m_age >> t;
        }catch(std::runtime_error& e){
            e.what();
            return false;
        }
        Destory();
        testA_Ptr = new TestA(std::move(t));
        return true;
    }

    ~TestB(){ Destory(); }

private:
    std::string m_name;
    int m_age;
    TestA *testA_Ptr;

    void Destory()
    {
        delete testA_Ptr;
    }
};

int main(){
    using namespace test::serialize;
    DataStream serilizer;
    // std::vector<std::map<std::string, int>> vec1;
    // vec1.push_back({{"1 = ", 1}});
    // vec1.push_back({{"2 = ", 2}, {"3 = ", 3}});
    // serilizer << vec1;
    // std::vector<std::map<std::string, int>> vec2;
    // serilizer.reset();
    // serilizer >> vec2;
    // for(auto& v : vec2){
    //     for(auto& m : v){
    //         std::cout << m.first;
    //         std::cout << m.second <<std::endl;
    //     }
    // }
    
    
    std::vector<TestB> vec1{{"1", 1}, {"2", 2}, {"3", 3}};
    serilizer << vec1;
    serilizer.reset();
    std::vector<TestB> vec2;
    serilizer >> vec2;
    for(auto& v : vec2){
        v.show();
    }

    // TestB t1("1", 1);
    // t1.show();
    // serilizer << t1;
    // TestB t2;
    // serilizer.reset();
    // serilizer >> t2;
    // t2.show();

}