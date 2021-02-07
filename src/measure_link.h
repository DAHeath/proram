#ifndef MEASURE_LINK_H__
#define MEASURE_LINK_H__


#include "link.h"


struct MeasureLink : public Link {
  public:
    MeasureLink() { }
    MeasureLink(Link* base) : base(base) { }

    void send(std::span<const std::byte> s) {
      n += s.size();
      base->send(s);
    }

    void recv(std::span<std::byte> s) {
      n += s.size();
      base->recv(s);
    }

    void flush() { base->flush(); }


    std::size_t traffic() const { return n; }

  private:
    std::size_t n = 0;
    Link* base;
};


#endif
