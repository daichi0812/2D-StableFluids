//
//  simulation.cpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/19.
//

#include "simulation.hpp"

// コンストラクタ: シミュレーションの初期化
Simulation::Simulation(int n) {
    size = (n + 2) * (n + 2);   // グリッドサイズを計算
    // ベクターを0で初期化
    x.resize(size);
    std::fill(x.begin(), x.end(), 0.0);
    
    y.resize(size);
    std::fill(y.begin(), y.end(), 0.0);
    
    x_prev.resize(size);
    std::fill(x_prev.begin(), x_prev.end(), 2.0);   // 前ステップのx速度を2.0で初期化
    
    y_prev.resize(size);
    std::fill(y_prev.begin(), y_prev.end(), -2.0);  // 前ステップの速度を2.0で初期化
    
    dens.resize(size);
    std::fill(dens.begin(), dens.end(), 0.0);   // 密度を0で初期化
    
    dens_prev.resize(size);
    std::fill(dens_prev.begin(), dens_prev.end(), 0.0); // 前ステップの密度を0で初期化
    
    r.resize(size);
    std::fill(r.begin(), r.end(), 0.0); // 赤色成分を0で初期化
    
    r_prev.resize(size);
    std::fill(r_prev.begin(), r_prev.end(), 0.0);   // 前ステップの赤色成分を0で初期化
    
    g.resize(size);
    std::fill(g.begin(), g.end(), 0.0); // 緑色成分を0で初期化
    
    g_prev.resize(size);
    std::fill(g_prev.begin(), g_prev.end(), 0.0);   // 前ステップの緑色成分を0で初期化
    
    b.resize(size);
    std::fill(b.begin(), b.end(), 0.0); // 青色成分を0で初期化
    
    b_prev.resize(size);
    std::fill(b_prev.begin(), b_prev.end(), 0.0);   // 前ステップの青色成分を0で初期化
    
}

// デストラクタ
Simulation::~Simulation(){
    
}

// ステップ1: 外力項の加算（クリックしたセルに対して外力を適用）
// X, Y: クリックした座標
// N: グリットサイズ
// u, v: マウスの動く方向に働く力（速度）
void Simulation::add_force(int X, int Y, int N, float u, float v){
    std::cout << "This is add_force function" << std::endl;
    if (X <= 0 || X > N || Y <= 0 || Y > N){
        throw std::out_of_range("Index is  out of range.");
    }
    // クリックしたセルにおいて、速度フィールドの前ステップの値に速度u, vを代入
    x_prev[IX(Y, X)] = u;
    y_prev[IX(Y, X)] = v;
}

// 全てのセルに対して、外部からの影響を時間ステップに基づいて加算する
void Simulation::add_source(int N, std::vector<float>& x, std::vector<float>& s, float dt){
    for (int i = 0; i < size; ++i){
        x[i] += dt * s[i];  // 各セルにソース項を加算
    }
}


// ステップ２: 移流項、対流項（Advection Term)
// N: グリッドの一辺の長さ
// b: 境界条件指定パラメータ
// d: 現在の移流後の値（次のステップでの密度や速度場)
// d0: 一つ前の時間ステップでの密度や速度場
// (u, v): xy成分の速度
// dt: 時間ステップの大きさ
void Simulation::advect(int N, int b, std::vector<float>& d, std::vector<float>& d0, std::vector<float>& u, std::vector<float>& v, float dt){
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt * N;   // 時間ステップとグリッドサイズに基づくスケーリング係数
    // 全てのセルに対して処理
    for (int i = 1; i <= N; ++i){
        for(int j = 1; j <= N; ++j){
            x = i - dt0 * u[IX(i, j)];  // x方向の移流後の位置を逆に辿る
            y = j - dt0 * v[IX(i, j)];  // y方向の移流後の位置を逆に辿る
            
            // xとyの範囲をクリップしてシミュレーション領域外にでないようにする
            if (x < 0.5) x = 0.5;
            if (x > N + 0.5) x = N + 0.5;
            i0 = (int)x;    // 移流元の整数部分のインデックス
            i1 = i0 + 1;    // 隣接するインデックス（補間に使用)
            
            if (y < 0.5) y = 0.5;
            if (y > N + 0.5) y = N + 0.5;
            j0 = (int)y;    // 移流元の整数部分のインデックス
            j1 = j0 + 1;    // 隣接するインデックス（補間に使用）
            
            s1 = x - i0;    // x方向の補間比率
            s0 = 1 - s1;    // x方向の逆補間比率
            t1 = y - j0;    // y方向の補間比率
            t0 = 1 - t1;    // y方向の逆補間比率
            
            // 移流後の値を計算し、周囲4つのセルからの補間によって求める
            d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                          s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }
    set_bnd(N, b, d);   // 境界条件を設定
}

// ステップ3: 粘性項の扱い（拡散方程式）
// N: グリッドの一辺
// b: 境界条件を指定するパラメータ
// x: 計算後の値
// x0: 直前の時間ステップの値
// diff: 拡散係数（粘性係数）
// dt: 時間ステップ
void Simulation::diffuse(int N, int b, std::vector<float>& x, std::vector<float>& x0, float diff, float dt){
    int i, j, k;
    float a = dt * diff * N * N;    // 粘性係数ν, Δt, 1 /Δx^2 をまとめたもの
    for (k = 0; k < 20; ++k){   // ガウス・ザイデル法の反復回数
        // 全てのセルに対して行う
        for (i = 1; i <= N; ++i){
            for (j = 1; j <= N; ++j){
                // 拡散方程式を陰的な評価で離散化
                x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] +
                                                   x[IX(i, j -1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
            }
        }
        // 境界条件の適用
        set_bnd(N, b, x);
    }
}

// ステップ4: 投影ステップ（Projection）
// N: グリッドの一辺
// (u, v): 次の時間ステップの速度x, y成分
// p: 圧力場
// div: 速度場の発散
void Simulation::project(int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& p, std::vector<float>& div){
    int i, j, k;
    float h = 1.0f / N; // グリッドの単位長さ
    
    // 発散場を計算(中心差分法)
    for (i = 1; i <= N; ++i){
        for (j = 1; j <= N; ++j){
            div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
                                         v[IX(i, j + 1)] - v[IX(i, j - 1)]);
            p[IX(i, j)] = 0.0f; // 圧力場を初期化
        }
    }
    set_bnd(N, 0, div); // 発散場に境界条件を適用
    set_bnd(N, 0, p);   // 圧力場に境界条件を適用
    
    // ポアソン方程式をガウス・ザイデル法で反復的にとく
    for (k = 0; k < 40; ++k){
        for (i = 1; i <= N; ++i){
            for (j = 1; j <= N; ++j){
                p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
                               p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4.0f;
            }
        }
        set_bnd(N, 0, p);   // 圧力場に境界条件を適用
    }
    
    // 圧力場の勾配を引くことで速度場を非圧縮性にする
    for (i = 1; i <= N; ++i){
        for (j = 1; j <= N; ++j){
            u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
            v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
        }
    }
    set_bnd(N, 1, u);   // 速度場 u の境界条件を適用
    set_bnd(N, 2, v);   // 速度場 v の境界条件を適用
}

// 境界条件の適用
// N: グリッドの一辺
// b: 境界条件を指定するパラメータ. 0: 境界で値をそのまま内部の値に等しくさせる. 1(x軸), 2(y軸): 境界で色度成分を反転（壁での反射）
// x: 処理対象のベクター
void Simulation::set_bnd(int N, int b, std::vector<float>& x){
    for (int i = 1; i <= N; ++i){
        // 左端の境界条件
        x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
        // 右端の境界条件
        x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
        // 下端の境界条件
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        // 上端の境界条件
        x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
    }
}

// 速度の更新
void Simulation::vel_step(int N, std::vector<float> &u, std::vector<float> &v, std::vector<float> &u0, std::vector<float> &v0, float visc, float dt){
    // Step1: add_forceで更新された u0, v0 を次の時間ステップの u, v に反映
    add_source(N, u, u0, dt);
    add_source(N, v, v0, dt);
    
    // step1で得た新しい値(u, v)をstep2の(u0, v0)として扱いたい
    std::swap(u0, u);
    std::swap(v0, v);
    
    // Step2: Advect(移流処理)
    advect(N, 1, u, u0, u0, v0, dt);    // x方向
    advect(N, 2, v, v0, u0, v0, dt);    // y方向
    
    // Advectしたら一旦非圧縮にしときたい（速度場の投影）
    project(N, u, v, u0, v0);
    
    // step2で得た新しい値(u, v)をstep3の(u0, v0)として扱いたい
    std::swap(u0, u);
    std::swap(v0, v);
    
    // Step3: Diffuse(粘性の扱い)
    diffuse(N, 1, u, u0, visc, dt);
    // y方向の拡散処理
    diffuse(N, 2, v, v0, visc, dt);
    
    // Step4: Project(投影)
    project(N, u, v, u0, v0);
}

// 密度（色の濃さ）の更新
void Simulation::dens_step(int N, std::vector<float> &x, std::vector<float> &x0, std::vector<float> &u, std::vector<float> &v, float diff, float dt){
    // ソース項の加算
    add_source(N, x, x0, dt);
    std::swap(x, x0);
    // 拡散処理
    diffuse(N, 0, x, x0, diff, dt);
    std::swap(x, x0);
    // 移流処理
    advect(N, 0, x, x0, u, v, dt);
}

// 密度データの取得
std::vector<float> Simulation::getDensity(int N){
    std::vector<float> amal;    // 結果を格納するベクター
    int i, j;
    float total = 0.0f;
    for (int k = 0; k < size; ++k){
        i = k / (N + 2);
        j = k % (N + 2);
        // 境界セル以外のデータを統合
        if (i != 0 && i != N + 1 && j != 0 && j != N + 1){
            amal.push_back(std::max(r[k], 0.0f));   // 赤色成分(最低0)
            amal.push_back(g[k]);    // 緑色成分
            amal.push_back(b[k]);   // 青色成分
//            total += r[k] + g[k] + b[k];    // 合計値（デバッグ用コメント）
        }
    }
//    std::cout << "合計: " << total << std::endl;    // 合計値の出力（デバッグ用コメント）
    
    return amal;    // 結果を返す
}

// スタンプ（色の追加）
void Simulation::stamp(int X, int Y, int W, int H, int N, float R, float G, float B){
    // スタンプが範囲外の場合は例外を投げる
    if (X <= 0 || X > N || Y <= 0 || Y > N || X + W > N || Y + H > N){
        throw std::out_of_range("Index is out of range");
    }
    
    for (int i = Y; i < Y + H; ++i){
        for (int j = X; j < X + W; ++j){
            // 色成分を追加
            r_prev[IX(i, j)] += R;
            g_prev[IX(i, j)] += G;
            b_prev[IX(i, j)] += B;
        }
    }
}

// シンク（色の除去）
void Simulation::sink(int X, int Y, int W, int H, int N){
    // シンクが範囲外の場合は例外を投げる
    if (X <= 0 || X > N || Y <= 0 || Y > N || X + W > N || Y + H > N){
        throw std::out_of_range("Index is out of range");
    }
    
    for (int i = Y; i < Y + H; ++i){
        for (int j = X; j < X + W; ++j){
            // 色成分を0にセット
            r_prev[IX(i, j)] = 0.0f;
            g_prev[IX(i, j)] = 0.0f;
            b_prev[IX(i, j)] = 0.0f;
        }
    }
}

// シミュレーションの更新
void Simulation::update(int N, float dt){
    vel_step(N, x, y, x_prev, y_prev, viscosity, dt);   // 速度の更新
//    dens_step(N, dens, dens_prev, x, y, diffusion, dt); // 密度の更新
    dens_step(N, r, r_prev, x, y, diffusion, dt); // 赤色成分の更新
    dens_step(N, g, g_prev, x, y, diffusion, dt); // 緑色成分の更新
    dens_step(N, b, b_prev, x, y, diffusion, dt); // 青色成分の更新
}

// シミュレーションのリセット
void Simulation::reset(int N){
    std::fill(x_prev.begin(), x_prev.end(), 0.0f);
    std::fill(y_prev.begin(), y_prev.end(), 0.0f);
    std::fill(r_prev.begin(), r_prev.end(), 0.0f);
    std::fill(g_prev.begin(), g_prev.end(), 0.0f);
    std::fill(b_prev.begin(), b_prev.end(), 0.0f);
    
    // シンクの例
    sink(10, 10, 2, 2, N);
    sink(20, 20, 2, 2, N);
}
