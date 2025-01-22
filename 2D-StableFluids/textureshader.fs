//
//  textureshader.fs
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/18.
//
#version 330 core

// 出力データに関する宣言
// out vec4 color;
in vec3 Color;          // 頂点シェーダーから渡される頂点ごとの色データ
in vec2 Texcoord;       // 頂点シェーダーから渡される頂点ごとの座標データ

out vec4 FragColor;     // フラグメントシェーダーから出力される最終的なピクセルの色

uniform sampler2D tex;  // 使用するテクスチャを指定するためのユニフォーム変数

void main(){
    // テクスチャの色を取得し、アルファ値を含むベクトルと乗算して最終色を決定
    FragColor = texture(tex, Texcoord) * vec4(1.0, 1.0, 1.0, 1.0);
}
