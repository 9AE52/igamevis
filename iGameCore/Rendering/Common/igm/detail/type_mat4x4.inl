//
// Created by Sumzeek on 4/19/2024.
//

#include "type_mat4x4.h"

namespace igm
{
// mat<4, 4, T> function
template<typename T>
void mat<4, 4, T>::setToIdentity() {
    value[0][0] = 1.0f;
    value[0][1] = 0.0f;
    value[0][2] = 0.0f;
    value[0][3] = 0.0f;
    value[1][0] = 0.0f;
    value[1][1] = 1.0f;
    value[1][2] = 0.0f;
    value[1][3] = 0.0f;
    value[2][0] = 0.0f;
    value[2][1] = 0.0f;
    value[2][2] = 1.0f;
    value[2][3] = 0.0f;
    value[3][0] = 0.0f;
    value[3][1] = 0.0f;
    value[3][2] = 0.0f;
    value[3][3] = 1.0f;
}

template<typename T>
T mat<4, 4, T>::determinant() const {
    T a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    a1 = this->value[0][0];
    a2 = this->value[0][1];
    a3 = this->value[0][2];
    a4 = this->value[0][3];

    b1 = this->value[1][0];
    b2 = this->value[1][1];
    b3 = this->value[1][2];
    b4 = this->value[1][3];

    c1 = this->value[2][0];
    c2 = this->value[2][1];
    c3 = this->value[2][2];
    c4 = this->value[2][3];

    d1 = this->value[3][0];
    d2 = this->value[3][1];
    d3 = this->value[3][2];
    d4 = this->value[3][3];

    return a1 * determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4) -
           b1 * determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4) +
           c1 * determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4) -
           d1 * determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

template<typename T>
mat<4, 4, T> mat<4, 4, T>::adjoint() const {
    mat<4, 4, T> adj;
    T a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    a1 = this->value[0][0];
    a2 = this->value[0][1];
    a3 = this->value[0][2];
    a4 = this->value[0][3];

    b1 = this->value[1][0];
    b2 = this->value[1][1];
    b3 = this->value[1][2];
    b4 = this->value[1][3];

    c1 = this->value[2][0];
    c2 = this->value[2][1];
    c3 = this->value[2][2];
    c4 = this->value[2][3];

    d1 = this->value[3][0];
    d2 = this->value[3][1];
    d3 = this->value[3][2];
    d4 = this->value[3][3];

    adj[0][0] = determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    adj[0][1] = -determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    adj[0][2] = determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    adj[0][3] = -determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    adj[1][0] = -determinant3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    adj[1][1] = determinant3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    adj[1][2] = -determinant3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    adj[1][3] = determinant3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

    adj[2][0] = determinant3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    adj[2][1] = -determinant3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    adj[2][2] = determinant3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    adj[2][3] = -determinant3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

    adj[3][0] = -determinant3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    adj[3][1] = determinant3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    adj[3][2] = -determinant3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    adj[3][3] = determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);

    return adj;
}

template<typename T>
mat<4, 4, T> mat<4, 4, T>::invert() const {
    T det = determinant();
    if (det == 0) {
        std::cout << "igm Error: mat<4, 4, T>-matrix is not invertible"
                  << std::endl;
        throw std::runtime_error(
                "igm Error: mat<4, 4, T>-matrix is not invertible");
    }

    mat<4, 4, T> adj = adjoint();

    mat<4, 4, T> inv;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) inv[i][j] = adj[i][j] / det;
    return inv;
}

template<typename T>
mat<4, 4, T> mat<4, 4, T>::transpose() const {
    mat<4, 4, T> trans;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) { trans.value[i][j] = value[j][i]; }
    }
    return trans;
}

// global function
template<typename T>
T determinant(const mat<4, 4, T>& m) {
    return m.determinant();
}

template<typename T>
mat<4, 4, T> adjoint(mat<4, 4, T>& m) {
    mat<4, 4, T> adj = m.adjoint();
    m = adj;
    return adj;
}

template<typename T>
mat<4, 4, T> invert(mat<4, 4, T>& m) {
    mat<4, 4, T> inv = m.invert();
    m = inv;
    return inv;
}

template<typename T>
mat<4, 4, T> transpose(mat<4, 4, T>& m) {
    mat<4, 4, T> trans = m.transpose();
    m = trans;
    return trans;
}
} // namespace igm