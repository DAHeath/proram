#ifndef ARITH_H__
#define ARITH_H__


// Macros that define operators that are the natural choices given other
// operators.


// Define out-of-place addition in terms of in-place addition.
#define EXTEND_OUT_OF_PLACE_ADD(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator+(const rhs& o) const { \
    lhs out = *this; \
    out += o; \
    return out; \
  }


// Define out-of-place subtraction in terms of in-place subtraction.
#define EXTEND_OUT_OF_PLACE_SUB(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator-(const rhs& o) const { \
    lhs out = *this; \
    out -= o; \
    return out; \
  }


// Define out-of-place multiplication in terms of in-place multiplication.
// Note, this multiplication allows the scalar rhs to have a different type
// than the lhs.
#define EXTEND_OUT_OF_PLACE_MUL(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator*(const rhs& o) const { \
    lhs out = *this; \
    out *= o; \
    return out; \
  }


// If the type T is a group, we can generate out-of-place addition and
// subtraction for free.
#define EXTEND_GROUP(type) \
  EXTEND_OUT_OF_PLACE_ADD(type, type) \
  EXTEND_OUT_OF_PLACE_SUB(type, type)


// If the type T is a group, we can generate out-of-place addition,
// subtraction, and multiplication for free.
#define EXTEND_RING(type) \
  EXTEND_GROUP(type) \
  EXTEND_OUT_OF_PLACE_MUL(type, type)


// If the type T is a vector space, we can generate out-of-place addition,
// subtraction, and scalar multiplication for free.
#define EXTEND_VECTOR_SPACE(vectors, scalars) \
  EXTEND_GROUP(vectors) \
  EXTEND_OUT_OF_PLACE_MUL(vectors, scalars)


#endif
