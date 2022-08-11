#include <iostream>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

#define MaxNameLenght 60
#define MaxSearchUsersNum 20
#define DefaultCharacterEncoding "UTF8"

struct UserInfo
{
    // 这里假设用户的信息只有id和name
    string id;
    string name;
};
class UserComp
{
public:
    UserComp(vector<UserInfo> *users)
    {
        this->users = users;
    }
    bool operator()(const uint32_t &a, const uint32_t &b) const
    {
        return (*this->users)[a].id > (*this->users)[b].id; //从大到小排序
    }

private:
    vector<UserInfo> *users;
};

class FuzzyTreeNode
{
public:
    FuzzyTreeNode(vector<UserInfo> *user);
    ~FuzzyTreeNode();
    map<uint32_t, FuzzyTreeNode *> *umap;
    set<uint32_t, UserComp> *userIds;
};


class FuzzyTree
{
public:
    int searchName(const char *name, int len, vector<UserInfo> &vecUsers);
    void addUser(UserInfo &user);
    bool deleteUser(UserInfo &user);

private:
    map<uint32_t, FuzzyTreeNode *> umap;
    vector<UserInfo> users; //为了节约内存，这里用vector，并且在数里面也不存string，改成存index，这样能保证在32位

    static uint32_t UTF8ToUint32(const char *str, int offset);
    static int GetUtf8Len(uint32_t value);
    void AddValueList(uint32_t *values, int n, uint32_t index);
    set<uint32_t, UserComp> *findValueList(uint32_t *values, uint32_t n);
    bool DeleteValueList(uint32_t *values, int n, uint32_t index);
    bool deleteName(const char *name, int len, uint32_t index);
    void addName(const char *name, int len, uint32_t index);
};