//
// Created by Sumzeek on 4/19/2024.
//

#include "type_mat3x3.h"

namespace igm
{
// mat<3, 3, T> function
template<typename T>
void mat<3, 3, T>::setToIdentity() {
    value[0][0] = 1.0f;
    value[0][1] = 0.0f;
    value[0][2] = 0.0f;
    value[1][0] = 0.0f;
    value[1][1] = 1.0f;
    value[1][2] = 0.0f;
    value[2][0] = 0.0f;
    value[2][1] = 0.0f;
    value[2][2] = 1.0f;
}

template<typename T>
T mat<3, 3, T>::determinant() const {
    T a1, a2, a3, b1, b2, b3, c1, c2, c3;

    a1 = this->value[0][0];
    a2 = this->value[0][1];
    a3 = this->value[0][2];

    b1 = this->value[1][0];
    b2 = this->value[1][1];
    b3 = this->value[1][2];

    c1 = this->value[2][0];
    c2 = this->value[2][1];
    c3 = this->value[2][2];

    return a1 * determinant2x2(b2, b3, c2, c3) -
           b1 * determinant2x2(a2, a3, c2, c3) +
           c1 * determinant2x2(a2, a3, b2, b3);
}

template<typename T>
mat<3, 3, T> mat<3, 3, T>::adjoint() const {
    mat<3, 3, T> adj;
    T a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    a1 = this->value[0][0];
    a2 = this->value[0][1];
    a3 = this->value[0][2];

    b1 = this->value[1][0];
    b2 = this->value[1][1];
    b3 = this->value[1][2];

    c1 = this->value[2][0];
    c2 = this->value[2][1];
    c3 = this->value[2][2];

    adj[0][0] = determinant2x2(b2, b3, c2, c3);
    adj[0][1] = -determinant2x2(a2, a3, c2, c3);
    adj[0][2] = determinant2x2(a2, a3, b2, b3);

    adj[1][0] = -determinant2x2(b1, b3, c1, c3);
    adj[1][1] = determinant2x2(a1, a3, c1, c3);
    adj[1][2] = -determinant2x2(a1, a3, b1, b3);

    adj[2][0] = determinant2x2(b1, b2, c1, c2);
    adj[2][1] = -determinant2x2(a1, a2, c1, c2);
    adj[2][2] = determinant2x2(a1, a2, b1, b2);

    return adj;
}

template<typename T>
mat<3, 3, T> mat<3, 3, T>::invert() const {
    T det = determinant();
    if (det == 0) {
        std::cout << "igm Error: mat<3, 3, T>-matrix is not invertible"
                  << std::endl;
        throw std::runtime_error(
                "igm Error: mat<3, 3, T>-matrix is not invertible");
    }

    mat<3, 3, T> adj = adjoint();

    mat<3, 3, T> inv;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) inv[i][j] = adj[i][j] / det;
    return inv;
}

template<typename T>
mat<3, 3, T> mat<3, 3, T>::transpose() const {
    mat<3, 3, T> trans;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) { trans.value[i][j] = value[j][i]; }
    }
    return trans;
}

// global function
template<typename T>
T determinant(const mat<3, 3, T>& m) {
    return m.determinant();
}

template<typename T>
mat<3, 3, T> adjoint(mat<3, 3, T>& m) {
    mat<3, 3, T> adj = m.adjoint();
    m = adj;
    return adj;
}

template<typename T>
mat<3, 3, T> invert(mat<3, 3, T>& m) {
    mat<3, 3, T> inv = m.invert();
    m = inv;
    return inv;
}

template<typename T>
mat<3, 3, T> transpose(mat<3, 3, T>& m) {
    mat<3, 3, T> trans = m.transpose();
    m = trans;
    return trans;
}
} // namespace igm