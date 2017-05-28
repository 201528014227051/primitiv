#include <config.h>

#include <primitiv/constant_initializer.h>
#include <primitiv/tensor.h>

namespace primitiv {

void ConstantInitializer::apply(Tensor &x) const {
  x.reset(k_);
}

}  // namespace primitiv