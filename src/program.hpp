#ifndef CANNON_PROGRAM_HPP
#define CANNON_PROGRAM_HPP

namespace cannon {

class program {
  public:
    friend std::ostream &operator<<(std::ostream &os, const program &program) {
      return os;
    }
};

}

#endif // CANNON_PROGRAM_HPP
