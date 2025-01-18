//
//  shader.cpp
//  2D-StableFluids
//
//  Created by 堀田大智 on 2025/01/17.
//  Shader クラスの実装を行う
//  シェーダーの読み込み、コンパイル、リンク、および使用を簡単を行うためのクラス

#include "shader.hpp"

// Shader クラスの実装
Shader::Shader(const char* vertexPath, const char* fragmentPath){
    
    // シェーダーのソースコードをファイルから読み込む
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    // ファイルストリームが例外を投げるように設定
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try{
        // ファイルを開く
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        // ファイルの内容をストリームに読み込む
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        // ファイルを閉じる
        vShaderFile.close();
        fShaderFile.close();
        
        // ストリームを文字列に変換
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }catch(std::ifstream::failure& e){
        std::cout << "Error::Shader::File_Not_Successfully_Read: " << e.what() << std::endl;
    }
    
    // シェーダーのソースコードをCスタイルの文字列に変換
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // シェーダーのコンパイル
    unsigned int vertex, fragment;
    
    // 頂点シェーダーの作成
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "Vertex"); 
    
    // フラグメントシェーダーの作成
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "Fragment");
    
    // シェーダープログラムの作成とリンク
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "Program");
    
    // シェーダーオブジェクトの削除（リンク後は不要）
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use(){
    // シェーダープログラムを有効化
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    // Uniform変数をブール値に設定
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    // Uniform変数を整数値に設定
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    // Uniform変数を浮動小数点値に設定
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type){
    int success;
    char infoLog[1024];
    if(type != "PROGRAM"){
        // シェーダーのコンパイルエラーをチェック
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Error: Shader_Compipation_Error of type: " << type << "\n" << infoLog << "\n ---" << std::endl;
        }
    } else {
        // プログラムのリンクエラーをチェック
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Error: Program_Linking_Error of type: " << type << "\n" << infoLog << "\n ---" << std::endl;
        }
    }
}
