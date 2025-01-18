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


