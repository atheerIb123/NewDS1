// 
// 234218 Data Structures 1.
// Semester: 2023A (winter).
// Wet Exercise #1.
// 
// Recommended TAB size to view this file: 8.
// 
// The following main file is necessary to link and run your code.
// This file is READ ONLY: even if you submit something else, the compiler ..
// .. WILL use our file.
// 

#include "worldcup23a1.h"

int main()
{
    world_cup_t* obj = new world_cup_t();
    StatusType res = obj->add_team(1, 10000);
    //REQUIRE(res == StatusType::SUCCESS);
    res = obj->add_team(2, 20000);
    //REQUIRE(res == StatusType::SUCCESS);
    res = obj->add_team(3, 30000);
    //REQUIRE(res == StatusType::SUCCESS);
    res = obj->add_team(4, 40000);
    //REQUIRE(res == StatusType::SUCCESS);
    res = obj->add_player(1001, 1, 10, 0, 0, false);
    //REQUIRE(res == StatusType::SUCCESS);
    res = obj->add_player(1002, 1, 10, 0, 0, false);
    if(res == StatusType::SUCCESS)
    {
        cout << "Allah" << endl;
    }   
}