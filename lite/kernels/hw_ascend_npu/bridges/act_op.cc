// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <all_ops.h>
#include "lite/kernels/hw_ascend_npu/bridges/graph.h"
#include "lite/kernels/hw_ascend_npu/bridges/utility.h"
#include "lite/kernels/npu/bridges/registry.h"

namespace paddle {
namespace lite {
namespace subgraph {
namespace hw_ascend_npu {

template <typename ActType>
int ActConverter(void* ctx, OpLite* op, KernelBase* kernel) {
  CHECK(ctx != nullptr);
  CHECK(op != nullptr);
  auto graph = static_cast<Graph*>(ctx);
  auto op_info = op->op_info();
  auto op_type = op_info->Type();
  auto scope = op->scope();
  LOG(INFO) << "[HWAscendNPU] Converting " + op_type + "...";

  // Get input and output vars and op attributes
  auto x_name = op_info->Input("X").front();
  auto x = scope->FindTensor(x_name);

  auto out_name = op_info->Output("Out").front();

  // X node
  std::shared_ptr<Node> x_node = nullptr;
  if (graph->Has(x_name)) {
    x_node = graph->Get(x_name);
  } else {
    x_node = graph->Add(x_name, *x);
  }

  // Act node
  auto act_node = graph->template Add<ActType>(out_name);
  auto act_op = act_node->template data<ActType>();
  act_op->set_input_x(*x_node->data());

  return SUCCESS;
}

template <>
int ActConverter<ge::op::Activation>(void* ctx,
                                     OpLite* op,
                                     KernelBase* kernel) {
  CHECK(ctx != nullptr);
  CHECK(op != nullptr);
  auto graph = static_cast<Graph*>(ctx);
  auto op_info = op->op_info();
  auto op_type = op_info->Type();
  auto scope = op->scope();
  LOG(INFO) << "[HWAscendNPU] Converting " + op_type + "...";

  // Get input and output vars and op attributes
  auto x_name = op_info->Input("X").front();
  auto x = scope->FindMutableTensor(x_name);
  auto x_dims = x->dims();
  auto out_name = op_info->Output("Out").front();
  LOG(INFO) << "[HWAscendNPU] xname: " << x_name << ", dims: " << x_dims;

  // X node
  std::shared_ptr<Node> x_node = nullptr;
  if (graph->Has(x_name)) {
    LOG(INFO) << "[HWAscendNPU] graph has node: " << x_name;
    x_node = graph->Get(x_name);
  } else {
    LOG(INFO) << "[HWAscendNPU] graph does no have node: " << x_name;
    x_node = graph->Add(x_name, *x);
  }
  LOG(INFO) << "[HWAscendNPU] out name: " << out_name;

#if 0
  // Act node
  auto act_node = graph->template Add<ge::op::Activation>(out_name);
  auto act_op = act_node->template data<ge::op::Activation>();
  act_op->set_input_x(*x_node->data());
  // TODO(hong19860320) set the coef value for act Ops, such as leaky_relu,
  // clipped_relu etc.
  LOG(INFO) << "[HWAscendNPU] activation mode: " << op_type
      << ", type: " << CvtActMode(op_type);
  act_op->set_attr_mode(CvtActMode(op_type));
  if (op_type == "relu_clipped") {
    auto Relu_clipped_coef = op_info->GetAttr<float>("Relu_clipped_coef");
    act_op->set_attr_coef(Relu_clipped_coef);
  } else if (op_type == "relu6") {
    float Relu_clipped_coef = 6.f;
    act_op->set_attr_coef(Relu_clipped_coef);
  }
#else
  // Act node
  auto act_node = graph->template Add<ge::op::Relu>(out_name);
  auto act_op = act_node->template data<ge::op::Relu>();
  act_op->set_input_x(*x_node->data());
#endif
  return SUCCESS;
}

}  // namespace hw_ascend_npu
}  // namespace subgraph
}  // namespace lite
}  // namespace paddle

REGISTER_SUBGRAPH_BRIDGE(
    sigmoid,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    relu,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    tanh,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    relu_clipped,
    kNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    relu6,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    leaky_relu,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    abs,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    softsign,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);
REGISTER_SUBGRAPH_BRIDGE(
    softplus,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Activation>);

REGISTER_SUBGRAPH_BRIDGE(
    log,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Log>);
REGISTER_SUBGRAPH_BRIDGE(
    square,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Square>);
REGISTER_SUBGRAPH_BRIDGE(
    sqrt,
    kHWAscendNPU,
    paddle::lite::subgraph::hw_ascend_npu::ActConverter<ge::op::Sqrt>);