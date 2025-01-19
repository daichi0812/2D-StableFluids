//
//  textureshader.vs
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/18.
//
#version 330 core

// 頂点データの入力
layout(location = 0) in vec3 vertexPosition_modelspace; // モデル空間での頂点位置
layout(location = 1) in vec2 texcoord;  // テクスチャ座標

// 出力データ
// out vec3 Color;
out vec2 Texcoord;  // フラグメントシェーダーに渡すテクスチャ座標

void main(){
    Texcoord = texcoord;    // 入力されたテクスチャ座標を出力変数に渡す
    gl_Position = vec4(vertexPosition_modelspace, 1.0); // 頂点の位置をクリップ空間に設定
}
