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

const int N = 150;
const int M = 150;

const int TILE_SIZE = 2;

const char EMPTY_CHAR = '3';

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
};
struct qElem{
    set<int> possibilities;
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
vector<vector<int>> WFC(vector<Tile>& tiles){
    vector<vector<int>> res(N, vector<int>(M,-1));

    priority_queue<qElem> pq;
    
    qElem cur;
    cur.at = { getRandom(0,N-1), getRandom(0,M-1) };
    for(int i = 0; i < tiles.size(); i++) cur.possibilities.insert(i);
    
    pq.push(cur);
    while(!pq.empty()){
        cur = pq.top();
        pq.pop();

        int x = cur.at.first;
        int y = cur.at.second;
        set<int> possib = cur.possibilities;
        // ! NEEDS BACKTRACKING

        if(res[x][y] != -1) continue;
        int tileType = getRandomFromSet(possib);
        res[x][y] = tileType;

        for(int i = 0; i < 4; i++){
            int nx = x+offsets[i];
            int ny = y+offsets[i+1];

            if(inBounds(nx,ny) && res[nx][ny] == -1){
                string borderReq = getBorderNeededAtPoint(nx,ny,tiles,res);
                qElem next;
                next.at = {nx,ny};
                for(int j = 0; j < tiles.size(); j++){
                    if(doesTileFitBorderRequirement(tiles[j],borderReq)) next.possibilities.insert(j);
                }
                pq.push(next);
            }
        }
    }

    return res;
}

vector<Tile> getTilesFromImage(vector<string>& image, vector<Tile>& res){
    for(int i = 0; i < image.size()-TILE_SIZE+1; i++){
        for(int j = 0; j < image[0].size()-TILE_SIZE+1; j++){

            string tileImage = "";
            for(int a = 0; a < TILE_SIZE; a++){
                for(int b = 0; b < TILE_SIZE; b++){
                    tileImage += image[i+a][j+b];
                }
            }
            res.push_back(Tile(tileImage));

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

int main(){

    vector<Tile> tiles;
    // hand written tiles | size 3
    // tiles.push_back(Tile(" # ###   "));
    // tiles.push_back(Tile(" # ##  # "));
    // tiles.push_back(Tile(" #  ## # "));
    // tiles.push_back(Tile("   ### # "));
    // tiles.push_back(Tile("         "));
    // tiles.push_back(Tile(" # ### # "));
    // tiles.push_back(Tile(" #  #  # "));
    // tiles.push_back(Tile("   ###   "));
    // tiles.push_back(Tile(" # ##    "));
    // tiles.push_back(Tile(" #  ##   "));
    // tiles.push_back(Tile("   ##  # "));
    // tiles.push_back(Tile("    ## # "));

    // hand written tiles | size 2;
    tiles.push_back(Tile("    "));
    tiles.push_back(Tile("#   "));
    tiles.push_back(Tile(" #  "));
    tiles.push_back(Tile("  # "));
    tiles.push_back(Tile("   #"));
    tiles.push_back(Tile("##  "));
    tiles.push_back(Tile(" ## "));
    tiles.push_back(Tile("  ##"));
    tiles.push_back(Tile("#  #"));
    tiles.push_back(Tile("# # "));
    tiles.push_back(Tile(" # #"));
    tiles.push_back(Tile("### "));
    tiles.push_back(Tile(" ###"));
    tiles.push_back(Tile("# ##"));
    tiles.push_back(Tile("## #"));
    tiles.push_back(Tile("####"));


    // tiles from image;
    // vector<string> image = {
    //     "   # ",
    //     "#### ",
    //     " # # ",
    //     " ####",
    //     " #   "
    // };
    // getTilesFromImage(image,tiles);

    vector<vector<int>> generated = WFC(tiles);

    cout << "ended" << endl;

    displayGenerated(generated, tiles);

    return 0;
}