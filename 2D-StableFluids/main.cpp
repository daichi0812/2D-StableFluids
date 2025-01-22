//
//  main.cpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/17.
//
#include <glad/glad.h>      // OpenGLの拡張機能をロードするためのライブラリ
#include <GLFW/glfw3.h>     // ウィンドウ管理と入力処理のためのライブラリ
#include <iostream>         // 入出力ストリーム
#include <chrono>           // 時間計測
#include <stdlib.h>         // stand, rand 関数
#include <stdexcept>        // 例外処理
#include <ctime>            // 時間関連の関数

#include <math.h>
#include "shader.hpp"       // シェーダー管理
#include "simulation.hpp"   // シミュレーション管理

#define PI 3.141592653

// フレームレートと更新間隔の設定
int FPS = 60;
double MS_PRE_UPDATE = 1000 / FPS;

// ウィンドウの識別子とサイズ
static int win_id;
static int win_x, win_y;

// マウスの前ステップの位置と現在の位置
static double omx, omy, mx, my;

// 外力の強さ
static float force = 5.0f;

// マウスボタンの状態
static int mouse_down[2] = { 0, 0 };

// 右クリック時のカーソル位置
static double xpos = -1, ypos = -1;

// 現在選択されている色(RGB)
static float rgb[3] = { 1.0f, 0.0f, 0.0f };

// キー入力イベントのコールバック関数
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    // 'Q' キーが押されたら赤色に切り替え
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        rgb[0] = 1.0f;
        rgb[1] = 0.0f;
        rgb[2] = 0.0f;
        std::cout << "Switching to red" << std::endl;
    }
    // 'W'キーが押されたら緑色に切り替え
    else if (key == GLFW_KEY_W && action == GLFW_PRESS){
        rgb[0] = 0.0f;
        rgb[1] = 1.0f;
        rgb[2] = 0.0f;
        std::cout << "Switching to green" << std::endl;
    }
    // 'E' キーが押されたら青色に切り替え
    else if (key == GLFW_KEY_E && action == GLFW_PRESS){
        rgb[0] = 0.0f;
        rgb[1] = 0.0f;
        rgb[2] = 1.0f;
        std::cout << "Switching to blue" << std::endl;
    }
}

// マウス移動イベントのコールバック関数
void mouse_move_callback(GLFWwindow *window, double x, double y){
    mx = x; // 現在のマウスのx位置を更新
    my = y; // 現在のマウスのy位置を更新
    std::cout << "cursor moved: " << x << " | " << y << std::endl;
}

// マウスボタンイベントのコールバック関数
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods){
    // 右クリックが押された場合
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        // カーソルの位置を取得
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Right Cursor Pressed at (" << xpos << " : " << ypos << ")" << std::endl;
    }
    // 右クリックが離された場合
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
        // カーソルの位置を取得
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Right Cursor Released at (" << xpos << " : " << ypos << ")" << std::endl;
        xpos = -1; // 位置をリセット
        ypos = -1;
    }
    // 左クリックが押された場合
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        // カーソルの位置を取得
        glfwGetCursorPos(window, &mx, &my);

        // クリック時の元の位置を記録
        omx = mx;
        omy = my;

        mouse_down[0] = 1; // 左ボタンが押されている状態に設定

        std::cout << "Left Cursor Pressed at (" << mx << " : " << my << ")" << std::endl;
    }
    // 左クリックが離された場合
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        // カーソルの位置を取得
        glfwGetCursorPos(window, &mx, &my);
        std::cout << "Left Cursor Released at (" << mx << " : " << my << ")" << std::endl;
        mouse_down[0] = 0; // 左ボタンが押されていない状態に設定
    }
}

// フレームバッファのサイズ変更時のコールバック関数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// 入力処理を行う関数の宣言
void processInput(GLFWwindow* window);

int main()
{
    double lag = 0; // 更新遅延時間
    
    int size = 720; // ウィンドウサイズ（幅と高さ）
    int scale = 6;  // シミュレーションのグリッドのスケール
    
    // シミュレーションオブジェクトの生成
    Simulation *sim = new Simulation(size / scale);
    int frame = 0;  // フレームカウンタ
    
    // テクスチャデータを格納するベクター
    std::vector<float> data;
    
    // GLFWの初期化
    glfwInit();
    // OpenGLのバージョン指定（3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // コアプロファイルを使用
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // macOS向けの設定
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // ウィンドウの作成
    GLFWwindow *window = glfwCreateWindow(size, size, "2D-StableFluids", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();    // GLFWの終了
        return -1;
    }
    glfwMakeContextCurrent(window); // コンテキストを現在のウィンドウに設定
    
    // フレームバッファサイズ変更時のコールバック関数を設定
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // スティッキーモードを有効にして、キーが押された状態を保持
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // マウスボタンイベントのコールバック関数を設定
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // キー入力イベントのコールバック関数を設定
    glfwSetKeyCallback(window, key_callback);
    // マウス移動イベントのコールバック関数を設定
    glfwSetCursorPosCallback(window, mouse_move_callback);

    // GLADの初期化（OpenGL関数のポインタをロード）
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader オブジェクトの作成
    Shader myShader("/Users/daichi/Documents/DevHub/CG/2D-StableFluids/2D-StableFluids/textureshader.vs", "/Users/daichi/Documents/DevHub/CG/2D-StableFluids/2D-StableFluids/textureshader.fs");
    
    // テクスチャの生成
    unsigned int screen_tex;
    glGenTextures(1, &screen_tex); // テクスチャオブジェクトの生成
    glBindTexture(GL_TEXTURE_2D, screen_tex); // テクスチャオブジェクトをバインド
    // テクスチャのラッピングとフィルタリングのオプションを設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // スクリーンクワッド（四角形）の頂点データ
    static const float vertices[] = {
            // 位置座標         テクスチャ座標
            1.0f,  -1.0f, 0.0f,  1.0f, 1.0f,  // 右下
            1.0f,  1.0f,  0.0f,  1.0f, 0.0f,  // 右上
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  // 左下
            -1.0f, 1.0f,  0.0f,  0.0f, 0.0f,  // 左上
    };
    // スクリーンクワッドのインデックスデータ（2つの三角形を構成）
    unsigned int indices[] = {
        0, 1, 3, // 最初の三角形
        1, 2, 3  // 2番目の三角形
    };

    // 頂点バッファオブジェクト（VBO）、頂点配列オブジェクト（VAO）、エレメントバッファオブジェクト（EBO）の生成
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // インデックスバッファ

    // 頂点配列オブジェクトをバインド
    glBindVertexArray(VAO);

    // 頂点バッファオブジェクトをバインドし、頂点データをコピー
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // エレメントバッファオブジェクトをバインドし、インデックスデータをコピー
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // シェーダープログラムを使用
    myShader.use();
    // テクスチャユニットをシェーダーに設定（ユニフォーム変数 "tex" にテクスチャユニット0を割り当て）
    glUniform1i(glGetUniformLocation(myShader.ID, "tex"), 0); // 手動で設定

    // レンダリングループ
    while(!glfwWindowShouldClose(window))
    {
        // 入力処理
        processInput(window);
        // 画面の色をクリア（背景色の設定）
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 背景色を設定
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファとデプスバッファをクリア
        
        // イベントの処理
        glfwPollEvents();

        /* ------------- シミュレーションの更新 --------------*/
        sim->reset(size / scale);  // シミュレーションのリセット

        // マウス右クリックで染料を追加
        if (xpos >= 0 && ypos >= 0) {
                int N = size / scale;

                // マウス位置をグリッド座標に変換
                int i = (int)((ypos / (float)size) * N + 1);
                int j = (int)((xpos / (float)size) * N + 1);
                // シミュレーションに染料を追加
                sim->stamp(i, j, 1, 1, N, 100.0f * rgb[0], 100.0f * rgb[1], 100.0f * rgb[2]);
                // 速度ベクトルの追加（未実装）
            }

        // マウス左ボタンが押されている場合、力を追加
        if (mouse_down[0]) {
                int N = size / scale;
                // マウス位置をウィンドウサイズ内にクランプ
                if (my >= size) my = size - 1;
                if (mx >= size) mx = size - 1;
                if (my < 0) my = 0;
                if (mx < 0) mx = 0;
                // マウス位置をグリッド座標に変換
                int i = (int)((my / (float)size) * N + 1);
                int j = (int)((mx / (float)size) * N + 1);
            
                std::cout << "Mouse Position (mx, my): " << mx << ", " << my << std::endl;
                std::cout << "Grid Position (i, j): " << i << ", " << j << std::endl;
            
                // シミュレーションに力を追加
                sim->add_force(i, j, N, force * (mx - omx), force * (my - omy));
                // 前回のマウス位置を更新
                omx = mx;
                omy = my;
        }

        // シミュレーションのステップを更新
        sim->update(size / scale, 0.1f);

        // シェーダープログラムを再度使用
        myShader.use();
        // シミュレーションから密度データを取得
        data = sim->getDensity(size / scale);

        /* ------------- レンダリング --------------*/
        // 密度配列をテクスチャに変換してクワッドに適用
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size / scale, size / scale, 0, GL_RGB, GL_FLOAT, data.data());

        // 頂点属性の設定（位置）
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 頂点属性の設定（テクスチャ座標）
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // 頂点配列オブジェクトをバインド
        glBindVertexArray(VAO);

        // クワッドを描画（トライアングルストリップを使用）
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // バッファをスワップして描画を反映
        glfwSwapBuffers(window);
        // イベントの処理
        glfwPollEvents();
    }

    // リソースの解放
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // ウィンドウの破棄とGLFWの終了
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// 入力処理関数
// 全ての入力を処理し、必要に応じて反応する
void processInput(GLFWwindow *window){
    // 'Escape' キーが押されたらウィンドウを閉じる
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

// フレームブッファサイズ変更時のコールバック関数
// ウィンドウのサイズが変更された時に呼び出される
void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    // ビューポートを新しいウィンドウサイズに合わせて設定
    glViewport(0, 0, width, height);
}
