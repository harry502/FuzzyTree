#include <iostream>
#include <set>
#include "FuzzySearch.h"

int main()
{
    FuzzyTree* root = new FuzzyTree();
    ifstream in, in2;
    in.open("users.txt", ios::in);

    string id;
    string name;
    const string spilt = ",";

    while (in >> id >> name)
    {
        UserInfo a = {id, name};
        root->addUser(a);
    }

    string str;
    while (cin >> str)
    {
        time_t start, end;
        if (!root)
            break;
        vector<UserInfo> vecUsers;
        root -> searchName(str.c_str(), str.size(), vecUsers);
        if (vecUsers.size() != 0)
        {
            for(UserInfo user: vecUsers)
            {
                cout << "id: " << user.id << ", name: " << user.name << endl;
            }
        }
        else
        {
            cout << "no find!" << endl;
        }
    }
}