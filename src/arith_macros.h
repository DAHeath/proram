#ifndef ARITH_H__
#define ARITH_H__


#define EXTEND_OUT_OF_PLACE_ADD(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator+(const rhs& o) const { \
    lhs out = *this; \
    out += o; \
    return out; \
  }


#define EXTEND_OUT_OF_PLACE_SUB(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator-(const rhs& o) const { \
    lhs out = *this; \
    out -= o; \
    return out; \
  }


#define EXTEND_OUT_OF_PLACE_MUL(lhs, rhs) \
  [[nodiscard]] constexpr lhs operator*(const rhs& o) const { \
    lhs out = *this; \
    out *= o; \
    return out; \
  }


#define EXTEND_GROUP(type) \
  EXTEND_OUT_OF_PLACE_ADD(type, type) \
  EXTEND_OUT_OF_PLACE_SUB(type, type)


#define EXTEND_RING(type) \
  EXTEND_GROUP(type) \
  EXTEND_OUT_OF_PLACE_MUL(type, type)


#define EXTEND_VECTOR_SPACE(vectors, scalars) \
  EXTEND_GROUP(vectors) \
  EXTEND_OUT_OF_PLACE_MUL(vectors, scalars)


#endif
