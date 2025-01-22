//
//  shader.hpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/17.
//
// Shaderクラスの定義を行う。 定義を.hにすることで二重定義を防ぐ

// 「#ifndef」: 定義されていなければコンパイルする
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Shader クラスの定義
class Shader{
public:
    // シェーダープログラムのID
    unsigned int ID;
    
    /**
     * @brief コンストラクタ
     *  指定された頂点シェーダーとフラグメントシェーダーのパスからシェーダープログラムを作成する
     *
     * @param vertexPath 頂点シェーダーのファイルパス
     * @param fragmentPath フラグメントシェーダーのファイルパス
     */
    Shader(const char* vertexPath, const char* fragmentPath);
    
    /**
     * @brief シェーダープログラムを使用可能にします。
     *
     * 現在のシェーダープログラムを有効化し、OpenGL によって使用されるようにします。
    */
    void use();
    
    /**
     * @brief ブール型のUniform変数を設定する
     *
     * @param name Uniform変数の名前
     * @param value 設定するブール値
     */
    void setBool(const std::string &name, bool value) const;
    
    /**
     * @brief 整数型のUniform変数を設定する
     *
     * @param name Uniform変数の名前
     * @param value 設定する整数値
     */
    void setInt(const std::string &name, int value) const;
    
    /**
     * @brief 浮動小数点型のUniform変数を設定する
     *
     * @param name Uniform変数の名前
     * @param value 設定する浮動小数点数値
     */
    void setFloat(const std::string &name, float value) const;
    
private:
    /**
     * @brief シェーダーのコンパイルやプログラムのリンク時に発生したエラーをチェックする
     *
     * @param shader    シェーダーまたはプログラムのID
     * @param type  エラーの種類（"VERTEX", "FRAGMENT", "PROGRAM" など)
     */
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif
