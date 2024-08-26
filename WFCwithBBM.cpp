// Block Based Method - delete blocks when no valid possibility is met
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <random>
#include <chrono>

using namespace std;

// UNCHANGEABLE CONSTANTS

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
vector<int> offsets = {-1,0,1,0,-1};

// CHANGEABLE CONSTANTS

const int N = 100;
const int M = 100;

const int TILE_SIZE = 5;

const char EMPTY_CHAR = '3';

const int BLOCK_RADIUS = 5;

// TYPES

#define cord pair<int,int>

struct Tile{
    char disp[TILE_SIZE][TILE_SIZE];
    char sockets[TILE_SIZE*4];
    Tile(string s){
        for(int i = 0; i < TILE_SIZE; i++){
            for(int j = 0; j < TILE_SIZE; j++){
                disp[i][j] = s[i*TILE_SIZE+j];
            }
        }
        
        for(int i = 0; i < TILE_SIZE; i++) sockets[i+TILE_SIZE*0] = disp[0][i];
        for(int i = 0; i < TILE_SIZE; i++) sockets[i+TILE_SIZE*1] = disp[i][TILE_SIZE-1];
        for(int i = 0; i < TILE_SIZE; i++) sockets[i+TILE_SIZE*2] = disp[TILE_SIZE-1][TILE_SIZE-1-i];
        for(int i = 0; i < TILE_SIZE; i++) sockets[i+TILE_SIZE*3] = disp[TILE_SIZE-1-i][0];
    }
    string getSide(int i){
        string res = "";
        for(int j = 0; j < TILE_SIZE; j++) res += sockets[i*TILE_SIZE+j];
        return res;
    }
    Tile getRotated(){
        char newDisp[TILE_SIZE][TILE_SIZE];
        for(int i = 0; i < TILE_SIZE; i++){
            for(int j = 0; j < TILE_SIZE; j++){
                newDisp[i][j] = disp[j][i];
            }
        }
        for(int i = 0; i < TILE_SIZE; i++){
            for(int j = 0; j < TILE_SIZE/2; j++){
                char tmp = newDisp[i][j];
                newDisp[i][j] = newDisp[i][TILE_SIZE-j-1];
                newDisp[i][TILE_SIZE-j-1] = tmp;
            }
        }
        string s = "";
        for(int i = 0; i < TILE_SIZE; i++){
            for(int j = 0; j < TILE_SIZE; j++){
                s += newDisp[i][j];
            }
        }
        return Tile(s);
    }
};
struct qElem{
    vector<int> possibilities;
    cord at;
    bool operator<(const qElem &a) const {
        return possibilities.size() > a.possibilities.size();
    }
};

// SIMPLE FUNCTIONS

int getRandom(int from, int to){
    return uniform_int_distribution<int>(from,to)(rng);
}
int getRandomFromSet(set<int>& s){
    int ind = getRandom(0,s.size()-1);
    set<int>::iterator it = s.begin();
    for(int i = 0; i < ind; i++) it++;
    return *it;
}
bool inBounds(int i, int j){
    return i >= 0 && j >= 0 && i < N && j < M;
}
string reverse(string s){
    reverse(s.begin(),s.end());
    return s;
}
bool doSidesFit(string a, string b){
    return reverse(a) == b;
}

// GENERAL FUNCTIONS

string getBorderNeededAtPoint(int x, int y, vector<Tile>& tiles, vector<vector<int>>& res){
    string s = "";
    for(int i = 0; i < 4; i++){
        int nx = x+offsets[i];
        int ny = y+offsets[i+1];

        if(!inBounds(nx,ny) || res[nx][ny] == -1){
            for(int j = 0; j < TILE_SIZE; j++) s += EMPTY_CHAR;
        }else{
            s += reverse(tiles[res[nx][ny]].getSide((i+2)%4));
        }
    }
    return s;
}
bool doesTileFitBorderRequirement(Tile t, string req){
    for(int i = 0; i < TILE_SIZE*4; i++){
        if(req[i] == EMPTY_CHAR || req[i] == t.sockets[i]) continue;
        return false;
    }
    return true;
}
vector<int> getPossibilities(vector<Tile>& tiles, string borderReq){
    vector<int> res;
    for(int j = 0; j < tiles.size(); j++){
        if(doesTileFitBorderRequirement(tiles[j],borderReq)) res.push_back(j);
    }
    return res;
}
qElem getNextStep(int nx, int ny, vector<Tile>& tiles, vector<vector<int>>& res){
    string borderReq = getBorderNeededAtPoint(nx,ny,tiles,res);

    qElem next;
    next.at = {nx,ny};
    next.possibilities = getPossibilities(tiles,borderReq);

    return next;
}
vector<vector<int>> WFC(vector<Tile>& tiles){
    vector<vector<int>> res(N, vector<int>(M,-1));

    priority_queue<qElem> pq;
    
    qElem cur;
    cur.at = { getRandom(0,N-1), getRandom(0,M-1) };
    for(int i = 0; i < tiles.size(); i++) cur.possibilities.push_back(i);
    pq.push(cur);

    while(!pq.empty()){
        cur = pq.top();
        pq.pop();

        int x = cur.at.first;
        int y = cur.at.second;

        if(res[x][y] != -1) continue;
        
        if(cur.possibilities.empty()){
            int minX = max(x-BLOCK_RADIUS,0);
            int maxX = min(x+BLOCK_RADIUS,N-1);
            int minY = max(y-BLOCK_RADIUS,0);
            int maxY = min(y+BLOCK_RADIUS,M-1);
            for(int nx = minX; nx <= maxX; nx++){
                for(int ny = minY; ny <= maxY; ny++){
                    res[nx][ny] = -1;
                }
            }
            for(int nx = minX; nx <= maxX; nx++){
                for(int ny = minY; ny <= maxY; ny++){
                    if(nx == minX || nx == maxX || ny == minY || ny == maxY){
                        pq.push(getNextStep(nx,ny,tiles,res));
                    }
                }
            }
        }else{
            int tileType = cur.possibilities[getRandom(0,cur.possibilities.size()-1)];
            string borderReq = getBorderNeededAtPoint(x,y,tiles,res);
            if(!doesTileFitBorderRequirement(tiles[tileType],borderReq)) continue;;
            res[x][y] = tileType;

            for(int i = 0; i < 4; i++){
                int nx = x+offsets[i];
                int ny = y+offsets[i+1];

                if(inBounds(nx,ny) && res[nx][ny] == -1){
                    pq.push(getNextStep(nx,ny,tiles,res));
                }
            }
        }
    }

    return res;
}

void displayGenerated(vector<vector<int>>& generated, vector<Tile>& tiles){
    vector<vector<char>> display(TILE_SIZE*N, vector<char>(TILE_SIZE*M, ' '));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){

            for(int a = 0; a < TILE_SIZE; a++){
                for(int b = 0; b < TILE_SIZE; b++){

                    display[i*TILE_SIZE+a][j*TILE_SIZE+b] = tiles[generated[i][j]].disp[a][b]; 

                }
            }

        }
    }

    for(int i = 0; i < display.size(); i++){
        for(int j = 0; j < display[0].size(); j++){
            cout << display[i][j];
        }
        cout << endl;
    }
}
void addRotatedTiles(Tile t, int am, vector<Tile>& tiles){
    for(int i = 0; i < am; i++){
        tiles.push_back(t);
        t = t.getRotated();
    }
}
int main(){

    vector<Tile> tiles;

    // hand written tiles | size 3
    // addRotatedTiles(Tile(" # ###   "),4,tiles);
    // tiles.push_back(Tile("         "));
    // tiles.push_back(Tile(" # ### # "));
    // addRotatedTiles(Tile("   ###   "),2,tiles);
    // addRotatedTiles(Tile(" # ##    "),4,tiles);

    // hand written tiles | size 2;
    // tiles.push_back(Tile("    "));
    // addRotatedTiles(Tile("#   "),4,tiles);
    // addRotatedTiles(Tile("##  "),4,tiles);
    // addRotatedTiles(Tile(" ## "),2,tiles);
    // addRotatedTiles(Tile("### "),4,tiles);
    // tiles.push_back(Tile("####"));

    // circuit | size 3
    tiles.push_back(Tile("                         "));
    tiles.push_back(Tile("#########################"));
    addRotatedTiles(Tile("      ...  ...+ ...      "),4,tiles);
    addRotatedTiles(Tile("          .....          "),2,tiles);
    addRotatedTiles(Tile("#    #..  #...+#..  #    "),4,tiles);
    addRotatedTiles(Tile("#                        "),4,tiles);
    addRotatedTiles(Tile("          +++++          "),2,tiles);
    addRotatedTiles(Tile("  .    .  ++.++  .    .  "),2,tiles);
    addRotatedTiles(Tile("  .   ...  ...  ...   +  "),4,tiles);
    addRotatedTiles(Tile("  +    +  +++++          "),4,tiles);
    addRotatedTiles(Tile("  +     + +   + +     +  "),2,tiles);
    addRotatedTiles(Tile("  +     +     +          "),4,tiles);
    addRotatedTiles(Tile("      ... +...+ ...      "),2,tiles);

    vector<vector<int>> generated = WFC(tiles);

    cout << "ended" << endl;

    displayGenerated(generated,tiles);

    return 0;
}