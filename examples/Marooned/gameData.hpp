#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include "abcg.hpp"

#include <bitset>

enum class Input { Right, Left, Down, Up, Fire };
enum class State { Playing, ExplosaoChao, ExplosaoColisao, Win };
enum class Acao  { Nulo, Morte, Resgate};

struct GameData {
  State m_state{State::Playing};
  Acao m_acao{Acao::Nulo};
  std::bitset<5> m_input;  // [fire, up, down, left, right]
  abcg::ElapsedTimer pessoaResgatada;
  abcg::ElapsedTimer pessoaMorta;
  abcg::ElapsedTimer messageDelay;
};

#endif