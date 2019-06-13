#include <time.h>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

#define TRACE 0
#define SAFE_SHELTER 5555
#define INIT_POS 7777

typedef unsigned int uint;
class Token {
 public:
  uint _PosInPath;
  uint _CurDist;
  bool _TokenSafe;
  bool _IsHome;
  Token() : _PosInPath(0), _CurDist(0), _TokenSafe(false), _IsHome(false) {}
};

class Player {
 private:
  uint _PlayerID;
  uint _WinCriteria;
  uint _TokenInitialPosition;

 public:
  vector<Token> _Token;
  Player(uint mPlayerID, uint mWinCriteria, uint mTokenInitialPosition)
      : _PlayerID(mPlayerID), _WinCriteria(mWinCriteria), _TokenInitialPosition(mTokenInitialPosition) {}
  uint GetPlayerID();
  uint GetTokenInitialPosition();
  uint GetWinCriteria();
  void ReduceWinCriteria();
  void AddTokens(Token mToken);
  void GetCurrentPositionAll(int mTotalDist);
  bool ValidTokenMove(uint mTokenToMove, uint mMoveCount, uint mTotalMoves, uint mGetLastSafeCount);
};

uint Player::GetTokenInitialPosition() { return _TokenInitialPosition; }

uint Player::GetPlayerID() { return _PlayerID; }

uint Player::GetWinCriteria() { return _WinCriteria; }

void Player::ReduceWinCriteria() { _WinCriteria = _WinCriteria - 1; }

void Player::AddTokens(Token mToken) {
  mToken._PosInPath = GetTokenInitialPosition();
  _Token.push_back(mToken);
}

void Player::GetCurrentPositionAll(int mTotalDist) {
  cout << "All Token current position:\n";

  for (uint tok = 0; tok < _Token.size(); tok++) {
    if (_Token[tok]._IsHome) {
      cout << "Token Number: " << (tok + 1) << " Home!" << endl;
    } else {
      cout << "Token Number: " << (tok + 1) << " Distance To Cover: " << (mTotalDist - _Token[tok]._CurDist - 1);
      cout << " Token Safe: " << _Token[tok]._TokenSafe << endl;
    }
  }
}

bool Player::ValidTokenMove(uint mTokenToMove, uint mMoveCount, uint mTotalMoves, uint mGetLastSafeCount) {
  if (_Token[mTokenToMove]._IsHome) {
    return false;
  }

  if ((_Token[mTokenToMove]._CurDist + mMoveCount) <= (mTotalMoves + mGetLastSafeCount - 1 )) {
    return true;
  }
  return false;
}

class Board {
 private:
  uint        _TotalMoves;
  uint        _LastSafeCount;
  uint        _NoOfToken;
  uint        _NoOfPlayers;
  vector<int> _NoKillZone;

 public:
  vector<Player> _Players;
  vector<uint>   _Path;

  Board(uint total_moves, uint last_safe_count, uint noOfToken, uint noOfPlayers, vector<int> noKillZone);
  uint GetTotalMoves();
  uint GetLastSafeCount();
  uint GetNoOfToken();
  uint GetNoOfPlayers();
  bool MoveToken(uint tokenToMove, uint moveCount, Player &player);
  void AddPlayers(Player player);
  uint RollDice();
  void PrintBoard();
};

Board::Board(uint mTotalMoves, uint mLastSafeCount, uint mNbrOfToken, uint mNbrOfPlayers, vector<int> mNoKillZone)
    : _TotalMoves(mTotalMoves),
      _LastSafeCount(mLastSafeCount),
      _NoOfToken(mNbrOfToken),
      _NoOfPlayers(mNbrOfPlayers),
      _NoKillZone(mNoKillZone) {
  _Path.assign(mTotalMoves, INIT_POS);
  for (auto mpos = _NoKillZone.begin(); mpos != _NoKillZone.end(); ++mpos) {
    _Path[*mpos] = SAFE_SHELTER;
  }
}

uint Board::GetTotalMoves() { return _TotalMoves; }

uint Board::GetLastSafeCount() { return _LastSafeCount; }

uint Board::GetNoOfToken() { return _NoOfToken; }

uint Board::GetNoOfPlayers() { return _NoOfPlayers; }

void Board::AddPlayers(Player mPlayer) { _Players.push_back(mPlayer); }

uint Board::RollDice() {
  srand(time(0));
  return (rand() % 6 + 1);
}

bool Board::MoveToken(uint mTokenToMove, uint mMoveCount, Player &mplayer) {
  vector<Token> &mToken = mplayer._Token;

  // Get Current distance traveled by the Token
  mToken[mTokenToMove]._CurDist = mToken[mTokenToMove]._CurDist + mMoveCount;

  // Safe from killing
  uint mHomeCount = GetTotalMoves() + GetLastSafeCount() - 1;
  if ((mToken[mTokenToMove]._CurDist > GetTotalMoves()) && (mToken[mTokenToMove]._CurDist < mHomeCount)) {
    mToken[mTokenToMove]._TokenSafe = true;
    _Path[mToken[mTokenToMove]._PosInPath] = INIT_POS;
    return false;
  }

  // Reached Home
  if (mToken[mTokenToMove]._CurDist == mHomeCount) {
    mToken[mTokenToMove]._IsHome = true;
    _Path[mToken[mTokenToMove]._PosInPath] = INIT_POS;
    mplayer.ReduceWinCriteria();
    if (mplayer.GetWinCriteria() == 0) {
      cout << "Player " << mplayer.GetPlayerID() + 1 << " Won The GAME!" << endl;
      return true;
    }
    return false;
  }

  uint newPosInPath = (mToken[mTokenToMove]._PosInPath + mMoveCount) % GetTotalMoves();

  // Reset Previous location Path for this token
  if (_Path[mToken[mTokenToMove]._PosInPath] != SAFE_SHELTER && _Path[mToken[mTokenToMove]._PosInPath] != INIT_POS) {
    _Path[mToken[mTokenToMove]._PosInPath] = INIT_POS;
    if (TRACE) {
      cout << "Reset Previous Position" << endl;
    }
  }

  // set new token position
  mToken[mTokenToMove]._PosInPath = newPosInPath;

  if (find(_NoKillZone.begin(), _NoKillZone.end(), newPosInPath) == _NoKillZone.end()) {
    if (TRACE) {
      cout << "No Safe Zone" << endl;
    }
    if (_Path[newPosInPath] == INIT_POS) {
      if (TRACE) {
        cout << "No Kill Scenario" << endl;
      }
      _Path[newPosInPath] = mplayer.GetPlayerID();  // No Kill
    } else {
      if (TRACE) {
        cout << "Kill Scenario" << endl;
      }
      uint del_PlayerID = _Path[newPosInPath];
      if (del_PlayerID != mplayer.GetPlayerID()) {
        _Path[newPosInPath] = mplayer.GetPlayerID();
        Player        &del_Player = _Players[del_PlayerID];
        vector<Token> &del_Token  = del_Player._Token;

        for (uint tok = 0; tok < del_Token.size(); tok++) {
          if (newPosInPath == del_Token[tok]._PosInPath) {
            cout << "Yeppieee! Player " << del_PlayerID + 1 << " Token " << tok + 1 << " Killed" << endl;
            del_Token[tok]._CurDist   = 0;
            del_Token[tok]._PosInPath = del_Player.GetTokenInitialPosition();
          }
        }
      }
    }
  } else {
    if (TRACE) {
      cout << "Token are in Safe Zone" << endl;
    }
  }

  return false;
}

bool PlayersTurn(Board &mBoard, uint mPlayerID) {
  uint moveCount = mBoard.RollDice();

  cout << "\nDice Rolled: " << moveCount << endl;

  Player &p1 = mBoard._Players[mPlayerID - 1];
  p1.GetCurrentPositionAll(mBoard.GetTotalMoves() + mBoard.GetLastSafeCount());

  vector<int> validToken;
  cout << "\n\nValid Token are: ";
  for (uint token = 0; token < p1._Token.size(); token++) {
    if (p1.ValidTokenMove(token, moveCount, mBoard.GetTotalMoves(), mBoard.GetLastSafeCount())) {
      cout << token + 1 << " ";
      validToken.push_back(token + 1);
    }
  }

  if (validToken.empty()) {
    cout << "Bad Luck!! No Valid Token to move, Wait for Next Move";
    return false;
  }
  cout << "\nPlease Choose One of them!" << endl;

  int  myToken;
  uint nbrOfchance = 4;
  while (true) {
    cout << "Input: ";
    cin >> myToken;
    nbrOfchance--;
    if (find(validToken.begin(), validToken.end(), myToken) != validToken.end()) {
      break;
    }
    cout << "Invalid selection! Try Again!" << endl;

    if ((nbrOfchance <= 0) && !validToken.empty()) {
      myToken = validToken.front();
      cout << "All Chance Exhausted, Token " << myToken << " chosen by the program!";
      break;
    }
  }

  return mBoard.MoveToken(myToken - 1, moveCount, p1);
}

void PrepareLudoGame(Board &mBoard) {
  // Prepare All Players
  uint   nbrOfToken = mBoard.GetNoOfToken();
  uint   initPos    = 0;
  uint   playerID   = 0;
  Player p1(playerID, nbrOfToken, initPos + 0);
  Player p2(++playerID, nbrOfToken, initPos + 8);
  Player p3(++playerID, nbrOfToken, initPos + 16);
  Player p4(++playerID, nbrOfToken, initPos + 24);

  // Create Assign All Tokens to respective Players
  for (uint TokenNbr = 0; TokenNbr < mBoard.GetNoOfToken(); TokenNbr++) {
    p1.AddTokens(Token());
    p2.AddTokens(Token());
    p3.AddTokens(Token());
    p4.AddTokens(Token());
  }

  // Add players to Board
  mBoard.AddPlayers(p1);
  mBoard.AddPlayers(p2);
  mBoard.AddPlayers(p3);
  mBoard.AddPlayers(p4);
}

void LetsPlayTheGame(Board &mBoard) {
  uint p1ayerID = 0;
  while (true) {
    p1ayerID = (p1ayerID % 4) + 1;
    cout << "\n\nPlayer " << p1ayerID << " Turn" << endl;
    if (PlayersTurn(mBoard, p1ayerID)) {
      cout << "Game Over!";
      break;
    }
  }
}

int main() {
  cout << "Welcome to LUDO GAME!" << endl;

  vector<int> noKillZone{0, 8, 16, 24, 6, 12, 18, 26};
  uint        mTotalMoves    = 35;
  uint        mLastSafeCount = 4;
  uint        mNbrToken      = 4;
  uint        mNbrPlayer     = 4;
  Board       mBoard(mTotalMoves, mLastSafeCount, mNbrToken, mNbrPlayer, noKillZone);

  PrepareLudoGame(mBoard);

  LetsPlayTheGame(mBoard);

  return 0;
}
