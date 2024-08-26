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

const int N = 70;
const int M = 70;

const int TILE_SIZE = 3;

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

string getBorderNeededAtPoint(cord at, vector<Tile>& tiles, vector<vector<int>>& output){
    string s = "";
    for(int i = 0; i < 4; i++){
        int nx = at.first+offsets[i];
        int ny = at.second+offsets[i+1];

        if(!inBounds(nx,ny) || output[nx][ny] == -1){
            for(int j = 0; j < TILE_SIZE; j++) s += EMPTY_CHAR;
        }else{
            s += reverse(tiles[output[nx][ny]].getSide((i+2)%4));
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
set<int> getPossibilitiesAtCell(cord at, vector<Tile>& tiles, vector<vector<int>>& output){
    string borderReq = getBorderNeededAtPoint(at,tiles,output);
    set<int> res;
    for(int j = 0; j < tiles.size(); j++) if(doesTileFitBorderRequirement(tiles[j],borderReq)) res.insert(j);
    return res;
}
void debugGrid(string msg, cord at, vector<vector<int>>& output, set<cord>& border){
    cout << msg << endl;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){

            if(at == cord(i,j)){
                cout << "X ";
            }else if(border.count({i,j})){
                cout << "# ";
            }else if(output[i][j] != -1){
                cout << output[i][j] << " ";
            }else{
                cout << ". ";
            }

        }
        cout << endl;
    }
    _sleep(100);
    system("CLS");
}
bool goOver(cord at, vector<Tile>& tiles, set<cord>& border, vector<vector<int>>& output, vector<vector<set<int>>>& possibilities, int count){
    int x = at.first;
    int y = at.second;
    // no longer part of border, cuz about to be fixed
    border.erase(at);



    // run while there are tiles that are valid
    while(!possibilities[x][y].empty()){
        // fix cell to a random tile
        int tileType = getRandomFromSet(possibilities[x][y]);
        output[x][y] = tileType;
        // don't check the same possibility after the next iterations
        possibilities[x][y].erase(tileType);
        // if final step, end successfully
        if(count == N*M-1) return true;



        // update border and border possibilities
        for(int i = 0; i < 4; i++){
            int nx = x+offsets[i];
            int ny = y+offsets[i+1];
            if(inBounds(nx,ny) && output[nx][ny] == -1){
                // if the cell is adjacent and not fixed, add to border and update possibility
                possibilities[nx][ny] = getPossibilitiesAtCell({nx,ny},tiles,output);
                border.insert({nx,ny});
            }else{
                // otherwise, remove from border
                border.erase({nx,ny});
            }
        }



        // pick next move
        cord minPosCell = *(border.begin());
        for(cord cell : border){
            if(possibilities[cell.first][cell.second].size() < possibilities[minPosCell.first][minPosCell.second].size()){
                // if cell on border has fewer possibilities, then update
                minPosCell = cell;
            }
        }
        //debugGrid("Added",at,output,border);
        // run on next cell
        if(goOver(minPosCell,tiles,border,output,possibilities,count+1)) return true;
    }



    // if here, then couldn't find a suitable tile, so backtrack
    output[x][y] = -1;
    possibilities[x][y] = getPossibilitiesAtCell({x,y},tiles,output);
    border.insert(at);
    // reset border and border possibilities
    for(int i = 0; i < 4; i++){
        int nx = x+offsets[i];
        int ny = y+offsets[i+1];
        if(inBounds(nx,ny) && output[nx][ny] == -1){
            // check adjacent cells 
            int covered = 0;
            for(int j = 0; j < 4; j++){
                int nnx = nx+offsets[j];
                int nny = ny+offsets[j+1];
                if(inBounds(nnx,nny) && output[nnx][nny] != -1){
                    covered++;
                }
            }
            if(covered == 0){
                // if adj cell isn't covered by a fixed cell, remove it from border
                border.erase({nx,ny});
            }else{
                // if it is, then add it and update possibility
                possibilities[nx][ny] = getPossibilitiesAtCell({nx,ny},tiles,output);
                border.insert({nx,ny});
            }
        }
    }
    //debugGrid("Removed",at,output,border);

    return false;
}
vector<vector<int>> WFC(vector<Tile>& tiles){
    vector<vector<int>> res(N, vector<int>(M,-1));
    set<int> defPos;
    for(int i = 0; i < tiles.size(); i++) defPos.insert(i);
    vector<vector<set<int>>> possibilities(N, vector<set<int>>(M, defPos));
    set<cord> border;

    goOver({getRandom(0,N-1), getRandom(0,M-1)},tiles,border,res,possibilities,0);

    return res;
}

int main(){

    vector<Tile> tiles;
    tiles.push_back(Tile(" # ###   ")); // up
    tiles.push_back(Tile(" # ##  # ")); // left
    tiles.push_back(Tile(" #  ## # ")); // right
    tiles.push_back(Tile("   ### # ")); // down
    tiles.push_back(Tile("         ")); // blank
    // tiles.push_back(Tile(" # ### # "));
    // tiles.push_back(Tile(" #  #  # "));
    // tiles.push_back(Tile("   ###   "));
    // tiles.push_back(Tile(" # ##    "));
    // tiles.push_back(Tile(" #  ##   "));
    // tiles.push_back(Tile("   ##  # "));
    // tiles.push_back(Tile("    ## # "));
    vector<vector<int>> generated = WFC(tiles);

    cout << "ended" << endl;

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

    return 0;
}