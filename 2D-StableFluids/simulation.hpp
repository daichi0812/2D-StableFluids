//
//  simulation.hpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/18.
//

// #pragma once → ヘッダーファイルが一度だけインクルードされる
#pragma once
#include <iostream>
#include <vector>

// インデックス計算用マクロ
// グリッドの座標（i, j)を1D配列(一次元配列)のインデックスに変換
#define IX(i, j) ((i) + (N + 2) * (j))

class Simulation {
private:
    int size;   // グリッドのサイズ
    std::vector<float> x;   // x方向の速度
    std::vector<float> y;   // y方向の速度
    std::vector<float> x_prev   // 前ステップのx方向の速度
    std::vector<float> y_prev   // 前ステップのy方向の速度
    std::vector<float> dens;    // 密度（色の濃さ）
    std::vector<float> dens_prev;   // 前ステップの密度（色の濃さ）
    
    std::vector<float> r;   // 赤色成分
    std::vector<float> r_prev;  // 前ステップの赤色成分
    std::vector<float> g;   // 緑色成分
    std::vector<float> g_prev;  // 前ステップの緑色の成分
    std::vector<float> b;   // 青色成分
    std::vector<float> b_prev;  // 前ステップの青色成分
    
    float viscosity = 0.0f; // 流体の粘土
    float diffusion = 0.001f;   // 拡散率
    
public:
    // コンストラクタ
    Simulation(int size);   // シミュレーションの初期化
    // デストラクタ
    ~Simulation();  // リソースの解放
    
    //  ソース項の加算
    void add_source(int N, std::vector<float>& x, std::vector<float>& s, float dt);
    
    /**
     * 外力項の加算
     * X, Y: クリックした座標、N: グリッドサイズ、u, v: マウスの動く方向に力の大きさ
     */
    void add_force(int X, int Y, int N, float u, float v);
    
    // 拡散処理
    void diffuse(int N, std::vector<float>& x, std::vector<float>& x0, float diff, float dt);
    
    // 移流処理
    void advect(int N, int b, std::vector<float>& d, std::vector<float>& d0, std::vector<float>&u, std::vector<float>& v, float dt);
    
    // 境界条件の設定
    void set_bnd(int N, int b, std::vector<float>& x);
    
    // 更新処理
    
    // 密度(色の濃さ）の更新
    void dens_step(int N, std::vector<float>& x, std::vector<float>& x0, std::vector<float>& u, std::vector<float>& v, float diff, float dt);
    
    // 速度の更新
    void vel_step(int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& u0, std::vector<float>& v0, visc, float dt);
    
    // 投影処理
    void project(int N, std::vector<float>& u, std::vector<float>& v, std::vector<float> p, std::vector<float>& div);
    
    // シミュレーションの全体的な更新
    void update(int N, float dt);
    
    // シミュレーションのリセット
    void reset(int N);
    
    // 密度（色）データの取得
    std::vector<float> getDensity(int N);
    
    // スタンプ（色の追加）
    void stamp(int X, int Y, int W, int H, int N, float R, float G, float B);
    
    // シンク（色の除去）
    void sink(int X, int Y, int W, int H, int N);
}
