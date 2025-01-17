//
//  main.cpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/17.
//

#include "shader.h"

int main()
{
    // GLFW や GLAD の初期化（省略）

    // Shader オブジェクトの作成
    Shader myShader("path/to/vertex_shader.vs", "path/to/fragment_shader.fs");

    // シェーダープログラムを使用
    myShader.use();

    // Uniform 変数の設定
    myShader.setBool("useTexture", true);
    myShader.setInt("texture1", 0);
    myShader.setFloat("someValue", 1.0f);

    // 描画ループ（省略）

    return 0;
}
