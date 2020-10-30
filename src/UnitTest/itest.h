#ifndef ITEST_H
#define ITEST_H
#include <map>
#include <string>
#include <iostream>
class IUnitTest
{
public:
    virtual int Test(int argc, char* argv[]) = 0;
    virtual bool IsEnabled(int argc, char* argv[]) = 0;
    virtual ~IUnitTest(){}
};


class CTestManager
{
public:
    ~CTestManager()
    {
        for (auto it : m_mapTests)
        {
            delete it.second;
        }
    }

    void AddTest(char* szName, IUnitTest* pTest)
    {
        auto it = m_mapTests.find(szName);
        if (it != m_mapTests.end())
            delete it->second;

        m_mapTests[szName] = pTest;
    }

    virtual int Test(int argc, char* argv[])
    {
        std::cout << "======== Test Begin ========";
        for (auto it : m_mapTests)
        {
            if (it.second->IsEnabled(argc, argv))
            {
                std::cout << "================";
                std::cout << "Run: %s" << it.first.c_str();
                it.second->Test(argc, argv);
            }
        }

        std::cout << "======== Test Exit ========";
        return 0;
    }

protected:
    std::map<std::string, IUnitTest*> m_mapTests;
};

static CTestManager s_testMgr;

#define DECLARE_TEST(TestClassName) static struct TEST_ADDER_##TestClassName\
{\
    TEST_ADDER_##TestClassName(char* szName, IUnitTest* pTest)\
    {\
        s_testMgr.AddTest(szName, pTest);\
    }\
}adder_##TestClassName(#TestClassName, new TestClassName);

#define RUN_TEST(argc, argv) s_testMgr.Test((argc), (argv));


#endif // ITEST_H
