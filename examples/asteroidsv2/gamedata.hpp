#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input {Right, Left, Down, Up, Fire};
enum class State {Playing, GameOver, Win, PlayingNoFire};

struct GameData {
    State m_state{State::Playing};
    std::bitset<5> m_input;
    int m_fires_available = 5;
};


#endif