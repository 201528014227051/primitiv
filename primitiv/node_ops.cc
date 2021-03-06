#include <config.h>

#include <vector>
#include <primitiv/error.h>
#include <primitiv/function_impl.h>
#include <primitiv/graph.h>
#include <primitiv/node_ops.h>
#include <primitiv/shape.h>
#include <primitiv/parameter.h>

namespace F = primitiv::functions;

#define REG(x) (x).graph().add_function

namespace primitiv {

Node operator+(const Node &x) { return REG(x)(new F::Positive(), {x}); }
Node operator-(const Node &x) { return REG(x)(new F::Negative(), {x}); }
Node operator+(const Node &x, float k) { return REG(x)(new F::AddConst(k), {x}); }
Node operator+(float k, const Node &x) { return REG(x)(new F::AddConst(k), {x}); }

Node operator+(const Node &a, const Node &b) {
  if (a.shape().is_scalar()) return REG(a)(new F::AddScalar(), {b, a});
  else if (b.shape().is_scalar()) return REG(a)(new F::AddScalar(), {a, b});
  else return REG(a)(new F::Add(), {a, b});
}

Node operator-(const Node &x, float k) { return REG(x)(new F::SubtractConstR(k), {x}); }
Node operator-(float k, const Node &x) { return REG(x)(new F::SubtractConstL(k), {x}); }

Node operator-(const Node &a, const Node &b) {
  if (a.shape().is_scalar()) return REG(a)(new F::SubtractScalarL(), {b, a});
  else if (b.shape().is_scalar()) return REG(a)(new F::SubtractScalarR(), {a, b});
  else return REG(a)(new F::Subtract(), {a, b});
}

Node operator*(const Node &x, float k) { return REG(x)(new F::MultiplyConst(k), {x}); }
Node operator*(float k, const Node &x) { return REG(x)(new F::MultiplyConst(k), {x}); }

Node operator*(const Node &a, const Node &b) {
  if (a.shape().is_scalar()) return REG(a)(new F::MultiplyScalar(), {b, a});
  else if (b.shape().is_scalar()) return REG(a)(new F::MultiplyScalar(), {a, b});
  else return REG(a)(new F::Multiply(), {a, b});
}


Node operator/(const Node &x, float k) { return REG(x)(new F::DivideConstR(k), {x}); }
Node operator/(float k, const Node &x) { return REG(x)(new F::DivideConstL(k), {x}); }

Node operator/(const Node &a, const Node &b) {
  if (a.shape().is_scalar()) return REG(a)(new F::DivideScalarL(), {b, a});
  else if (b.shape().is_scalar()) return REG(a)(new F::DivideScalarR(), {a, b});
  else return REG(a)(new F::Divide(), {a, b});
}

namespace node_ops {

Node input(const Shape &shape, const std::vector<float> &data, Device &dev, Graph &g) {
  return g.add_function(new F::Input(shape, data, dev), {});
}

Node input(Parameter &param, Graph &g) {
  return g.add_function(new F::ParameterInput(param), {});
}

Node copy(const Node &x, Device &dev) {
  return REG(x)(new F::Copy(dev), {x});
}

Node pick(const Node &x, unsigned dim, const std::vector<unsigned> &ids) {
  return REG(x)(new F::Pick(dim, ids), {x});
}

Node slice(const Node &x, unsigned dim, unsigned lower, unsigned upper) {
  return REG(x)(new F::Slice(dim, lower, upper), {x});
}

Node concat(const std::vector<Node> &xs, unsigned dim) {
  if (xs.empty()) THROW_ERROR("No nodes to concat.");
  return REG(xs[0])(new F::Concat(dim), xs);
}

Node reshape(const Node &x, const Shape &shape) {
  return REG(x)(new F::Reshape(shape), {x});
}

Node flatten(const Node &x) {
  return REG(x)(new F::Flatten(), {x});
}

Node transpose(const Node &x) {
  return REG(x)(new F::Transpose(), {x});
}

Node matmul(const Node &a, const Node &b) {
  return REG(a)(new F::MatrixMultiply(), {a, b});
}

Node sqrt(const Node &x) {
  return REG(x)(new F::Sqrt(), {x});
}

Node exp(const Node &x) {
  return REG(x)(new F::Exp(), {x});
}

Node tanh(const Node &x) {
  return REG(x)(new F::Tanh(), {x});
}

Node sigmoid(const Node &x) {
  return REG(x)(new F::Sigmoid(), {x});
}

Node softplus(const Node &x) {
  return REG(x)(new F::Softplus(), {x});
}

Node sin(const Node &x) {
  return REG(x)(new F::Sin(), {x});
}

Node cos(const Node &x) {
  return REG(x)(new F::Cos(), {x});
}

Node tan(const Node &x) {
  return REG(x)(new F::Tan(), {x});
}

Node relu(const Node &x) {
  return REG(x)(new F::ReLU(), {x});
}

Node lrelu(const Node &x) {
  return REG(x)(new F::LReLU(), {x});
}

Node prelu(const Node &x, float a) {
  return REG(x)(new F::PReLU(a), {x});
}

Node elu(const Node &x, float a) {
  return REG(x)(new F::ELU(a), {x});
}

Node selu(const Node &x, float a, float s) {
  return s * elu(x, a);
}

Node sum(const Node &x, unsigned dim) {
  return REG(x)(new F::Sum(dim), {x});
}

Node sum(const std::vector<Node> &xs) {
  if (xs.empty()) THROW_ERROR("No nodes to sum.");
  Node ret = xs[0];
  for (unsigned i = 1; i < xs.size(); ++i) ret = ret + xs[i];
  return ret;
}

Node mean(const Node &x, unsigned dim) {
  return (1. / x.shape()[dim]) * sum(x, dim);
}

Node mean(const std::vector<Node> &xs) {
  return sum(xs) / xs.size();
}

Node logsumexp(const Node &x, unsigned dim) {
  return REG(x)(new F::LogSumExp(dim), {x});
}

Node log_softmax(const Node &x, unsigned dim) {
  return x - broadcast(logsumexp(x, dim), dim, x.shape()[dim]);
}

Node softmax(const Node &x, unsigned dim) {
  return exp(log_softmax(x, dim));
}

Node broadcast(const Node &x, unsigned dim, unsigned size) {
  return REG(x)(new F::Broadcast(dim, size), {x});
}

Node softmax_cross_entropy(const Node &x, const Node &t, unsigned dim) {
  return REG(x)(new F::SoftmaxCrossEntropy(dim), {x, t});
}

Node softmax_cross_entropy(const Node &x, unsigned dim, const std::vector<unsigned> &ids) {
  return REG(x)(new F::SparseSoftmaxCrossEntropy(dim, ids), {x});
}

Node dropout(const Node &x, float rate, bool enabled) {
  if (!enabled) return x;
  if (rate == 1.) return 0. * x;
  const float p = 1. - rate;
  return (1. / p) * x * random::bernoulli(x.shape(), p, x.device(), x.graph());
}

namespace batch {

Node sum(const Node &x) {
  return REG(x)(new F::BatchSum(), {x});
}

Node mean(const Node &x) {
  return (1. / x.shape().batch()) * sum(x);
}

Node normalize(const Node &x) {
  if (!x.shape().has_batch()) return x;  // No meaning of normalization.
  const unsigned b = x.shape().batch();
  const float scale = b / (b - 1.);
  const Node m = mean(x);
  const Node v = scale * (mean(x * x) - m * m);
  return (x - m) / sqrt(v + 1e-8);
}

}  // namespace batch

Node zeros(const Shape &shape, Device &dev, Graph &g) {
  return g.add_function(new F::Constant(shape, 0, dev), {});
}

Node ones(const Shape &shape, Device &dev, Graph &g) {
  return g.add_function(new F::Constant(shape, 1, dev), {});
}

Node constant(const Shape &shape, float k, Device &dev, Graph &g) {
  return g.add_function(new F::Constant(shape, k, dev), {});
}

namespace random {

Node bernoulli(const Shape &shape, float p, Device &dev, Graph &g) {
  return g.add_function(new F::RandomBernoulli(shape, p, dev), {});
}

Node uniform(const Shape &shape, float lower, float upper, Device &dev, Graph &g) {
  return g.add_function(new F::RandomUniform(shape, lower, upper, dev), {});
}

Node normal(const Shape &shape, float mean, float sd, Device &dev, Graph &g) {
  return g.add_function(new F::RandomNormal(shape, mean, sd, dev), {});
}

Node log_normal(const Shape &shape, float mean, float sd, Device &dev, Graph &g) {
  return g.add_function(new F::RandomLogNormal(shape, mean, sd, dev), {});
}

}  // namespace random

}  // namespace node_ops
}  // namespace primitiv
