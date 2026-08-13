//
// Created by Sumzeek on 4/19/2024.
//

#include "type_mat2x2.h"

namespace igm
{
// mat<2, 2, T> function
template<typename T>
void mat<2, 2, T>::setToIdentity() {
    value[0][0] = 1.0f;
    value[0][1] = 0.0f;
    value[1][0] = 0.0f;
    value[1][1] = 1.0f;
}

template<typename T>
T mat<2, 2, T>::determinant() const {
    T a1, a2, b1, b2;

    a1 = this->value[0][0];
    a2 = this->value[0][1];

    b1 = this->value[1][0];
    b2 = this->value[1][1];

    return determinant2x2(a1, a2, b1, b2);
}

template<typename T>
mat<2, 2, T> mat<2, 2, T>::adjoint() const {
    mat<2, 2, T> adj;
    T a1, a2, b1, b2;

    a1 = this->value[0][0];
    a2 = this->value[0][1];

    b1 = this->value[1][0];
    b2 = this->value[1][1];

    adj[0][0] = b2;
    adj[0][1] = -a2;

    adj[1][0] = -b1;
    adj[1][1] = a1;

    return adj;
}

template<typename T>
mat<2, 2, T> mat<2, 2, T>::invert() const {
    T det = determinant();
    if (det == 0) {
        std::cout << "igm Error: mat<2, 2, T>-matrix is not invertible"
                  << std::endl;
        throw std::runtime_error(
                "igm Error: mat<2, 2, T>-matrix is not invertible");
    }

    mat<2, 2, T> adj = adjoint();

    mat<2, 2, T> inv;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j) inv[i][j] = adj[i][j] / det;
    return inv;
}

template<typename T>
mat<2, 2, T> mat<2, 2, T>::transpose() const {
    mat<2, 2, T> trans;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) { trans.value[i][j] = value[j][i]; }
    }
    return trans;
}

// global function
template<typename T>
T determinant(const mat<2, 2, T>& m) {
    return m.determinant();
}

template<typename T>
mat<2, 2, T> adjoint(mat<2, 2, T>& m) {
    mat<2, 2, T> adj = m.adjoint();
    m = adj;
    return adj;
}

template<typename T>
mat<2, 2, T> invert(mat<2, 2, T>& m) {
    mat<2, 2, T> inv = m.invert();
    m = inv;
    return inv;
}

template<typename T>
mat<2, 2, T> transpose(mat<2, 2, T>& m) {
    mat<2, 2, T> trans = m.transpose();
    m = trans;
    return trans;
}
} // namespace igm