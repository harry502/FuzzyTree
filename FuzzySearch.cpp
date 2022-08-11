#include <iostream>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <ctime>
#include "FuzzySearch.h"

using namespace std;

#define MaxNameLenght 60
#define MaxSearchUsersNum 20
#define DefaultCharacterEncoding "UTF8"


FuzzyTreeNode::FuzzyTreeNode(vector<UserInfo> *user)
{
    UserComp cmp(user);
    this->userIds = new set<uint32_t, UserComp>(cmp);
}
FuzzyTreeNode::~FuzzyTreeNode()
{
    if (this->umap != NULL)
    {
        for (std::map<uint32_t, FuzzyTreeNode *>::iterator it = this->umap->begin(); it != this->umap->end(); it++)
        {
            delete it->second;
        }
        delete this->umap;
    }
    if(this->userIds != NULL)
    {
        delete this->userIds;
    }
}

typedef map<uint32_t, FuzzyTreeNode *> nextMap;

uint32_t FuzzyTree::UTF8ToUint32(const char *str, int offset)
{
    int i = 0;
    uint32_t ans = 0;
    if ((str[offset] & 0xc0) == 0xc0)
    {
        ans += (u_int8_t)str[offset];
    }
    else if (str[offset] & 0x80)
    {
        return 0;
    }
    else
    {
        return str[offset];
    }

    while ((str[offset] << (++i)) & 0x80)
    {
        if ((str[offset + i] & 0xc0) == 0x80 && i < 4)
        {
            ans = (ans << 8) + (u_int8_t)str[offset + i];
        }
        else
        {
            //Invalid UTF8 code
            return 0;
        }
    }
    return ans;
}

int FuzzyTree::GetUtf8Len(uint32_t value)
{
    int ans = 1;
    while (value > 0xff)
    {
        ans++;
        value /= 0x100;
    }
    return ans;
}

void FuzzyTree::AddValueList(uint32_t *values, int n, uint32_t index)
{
    FuzzyTreeNode *now;
    if (this->umap.find(values[0]) != this->umap.end())
    {
        now = this->umap[values[0]];
        now->userIds->insert(index);
    }
    else
    {
        this->umap[values[0]] = new FuzzyTreeNode(&(this->users));
        now = this->umap[values[0]];
        now->umap = NULL;
        now->userIds->insert(index);
    }
    for (int i = 1; i < n; i++)
    {
        if (now->umap == NULL)
        {
            now->umap = new nextMap();
        }
        if (now->umap->find(values[i]) != now->umap->end())
        {
            now = (*(now->umap))[values[i]];
            now->userIds->insert(index);
        }
        else
        {
            (*now->umap)[values[i]] = new FuzzyTreeNode(&(this->users));
            now = (*now->umap)[values[i]];
            now->umap = NULL;
            now->userIds->insert(index);
        }
    }
    if (n != 1)
    {
        AddValueList(values + 1, n - 1, index);
    }
}

set<uint32_t, UserComp>* FuzzyTree::findValueList(uint32_t *values, uint32_t n)
{
    FuzzyTreeNode *now;
    if (this->umap.find(values[0]) != this->umap.end())
    {
        now = this->umap[values[0]];
    }
    else
    {
        return NULL;
    }

    for (int i = 1; i < n; i++)
    {
        if (now->umap && now->umap->find(values[i]) != now->umap->end())
        {
            now = (*(now->umap))[values[i]];
            if (i == n - 1)
            {
                return now->userIds;
            }
        }
        else
        {
            return NULL;
        }
    }
    return now->userIds;
}

bool FuzzyTree::DeleteValueList(uint32_t *values, int n, uint32_t index)
{
    if (n == 0)
        return true;

    FuzzyTreeNode *now;
    if (this->umap.find(values[0]) != this->umap.end())
    {
        now = this->umap[values[0]];
        now->userIds->erase(index);
        if (now->userIds->empty())
        {
            delete now;
            this->umap.erase(values[0]);
            return DeleteValueList(values + 1, n - 1, index);
        }
    }
    else
    {
        return DeleteValueList(values + 1, n - 1, index);
    }

    for (int i = 1; i < n; i++)
    {
        if (now->umap && now->umap->find(values[i]) != now->umap->end())
        {
            // cout << 4 << ' ' << values[i] << ' ' << n << i << endl;
            FuzzyTreeNode *temp = now;
            now = (*(now->umap))[values[i]];
            now->userIds->erase(index);
            if (now->userIds->empty())
            {
                // cout << 5 << ' ' << values[i] << ' ' << n << i << endl;
                delete now;
                temp->umap->erase(values[i]);
                return DeleteValueList(values + 1, n - 1, index);
            }
        }
        else
        {
            // cout << 6 << ' ' << values[i] << ' ' << n << i << endl;
            return DeleteValueList(values + 1, n - 1, index);
        }
    }
    return DeleteValueList(values + 1, n - 1, index);
}

void FuzzyTree::addName(const char *name, int len, uint32_t index)
{
    int i = 0, n = 0;
    uint32_t values[20];
    while (i < len && name[i] != 0)
    {
        uint32_t value = UTF8ToUint32(name, i);
        values[n++] = value;
        i += GetUtf8Len(value);
    }

    return AddValueList(values, n, index);
}

int FuzzyTree::searchName(const char *name, int len, vector<UserInfo> &vecUsers)
{
    int i = 0, n = 0;
    uint32_t values[20];
    while (i < len && name[i] != 0)
    {
        uint32_t value = UTF8ToUint32(name, i);
        values[n++] = value;
        i += GetUtf8Len(value);
    }

    set<uint32_t, UserComp>* result = findValueList(values, n);

    for (set<uint32_t, UserComp>::iterator it = result->begin(); it != result->end(); it++)
    {
        vecUsers.push_back(this->users[*it]);
    }

    return vecUsers.size();
}

bool FuzzyTree::deleteName(const char *name, int len, uint32_t index)
{
    int i = 0, n = 0;
    uint32_t values[20];
    while (i < len && name[i] != 0)
    {
        uint32_t value = UTF8ToUint32(name, i);
        values[n++] = value;
        i += GetUtf8Len(value);
    }
    return DeleteValueList(values, n, index);
}

void FuzzyTree::addUser(UserInfo &user)
{
    this->users.push_back(user);
    addName(user.name.c_str(), user.name.size(), this->users.size()-1);
}

bool FuzzyTree::deleteUser(UserInfo &user)
{
    for(int i = 0; i < this->users.size(); i++)
    {
        if(user.id == this->users[i].id)
        {
            return deleteName(user.name.c_str(), user.name.size(), i);
        }
    }
    return false;
}