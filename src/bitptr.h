#ifndef BIT_PTR_H__
#define BIT_PTR_H__


#include <vector>

struct BitPtr {
public:
  BitPtr() { }
  BitPtr(std::vector<bool>& data) : data(&data), offset(0) { }
  BitPtr(std::vector<bool>* data, std::size_t offset)
    : data(data), offset(offset) { }

  auto operator[](std::size_t ix) { return (*data)[ix + offset]; }
  auto operator[](std::size_t ix) const { return (*data)[ix + offset]; }

  BitPtr operator+(std::size_t off) const { return { data, offset + off }; }

private:
  std::vector<bool>* data;
  std::size_t offset;
};


#endif
