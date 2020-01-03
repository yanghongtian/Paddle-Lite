---
layout: post
title: 模型转化方法
---

Lite架构在预测过程中表现出来的高性能得益于其丰富的优化组件，其中包括量化、子图融合、混合调度、Kernel优选等等策略。为了使优化过程更加方便易用，我们提供了**model_optimize_tool**来自动完成优化步骤，输出一个轻量的、最优的可执行模型。具体使用方法介绍如下：

## 准备model_optimize_tool
当前获得model_optimize_tool方法有三种：
1. 我们提供当前develop分支编译结果下载：[model_optimize_tool](http://paddlelite-data.bj.bcebos.com/model_optimize_tool/model_optimize_tool?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A07%3A12Z%2F300%2Fhost%2F68a2ff1c1ef11d366dc958ee6c6dbd00f3df61b94a8c4b18a4e2f6b0ef7af07b)、[model_optimize_tool_mac](http://paddlelite-data.bj.bcebos.com/model_optimize_tool/model_optimize_tool_mac?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A07%3A23Z%2F300%2Fhost%2F906a0716a02d422922577342d0bd9cf04ec2c7acca78ec358d835b7e254be3e8)

2. 可以进入Paddle-Lite Github仓库的[release界面](https://github.com/PaddlePaddle/Paddle-Lite/releases)，选择release版本下载对应的model_optimize_tool

3. 可以下载Paddle-Lite源码，从源码编译出model_optimize_tool工具
```bash
git clone https://github.com/PaddlePaddle/Paddle-Lite.git
cd Paddle-Lite
git checkout <release-version-tag>
./lite/tools/build.sh build_optimize_tool
```
编译结果位于`Paddle-Lite/build.model_optimize_tool/lite/api/model_optimize_tool`
**注意**：从源码编译model_optimize_tool前需要先[安装Paddle-Lite的开发环境](../source_compile)。

## 使用model_optimize_tool

model_optimize_tool是x86平台上的可执行文件，需要在PC端运行：包括Linux终端和Mac终端。

### 帮助信息
 执行model_optimize_tool时不加入任何输入选项，会输出帮助信息，提示当前支持的选项：
```bash
 ./model_optimize_tool
```
![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_help.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A11%3A08Z%2F300%2Fhost%2F57179eb47be8edaf766ea42bad18682b3e1b3faca8040997726206ba5f38770c)
### 功能一：转化模型为Paddle-Lite格式
model_optimize_tool可以将PaddlePaddle支持的模型转化为Paddle-Lite支持的模型格式，期间执行的操作包括：将protobuf格式的模型文件转化为naive_buffer格式的模型文件，有效降低模型体积；执行“量化、子图融合、混合调度、Kernel优选”等图优化操作，提升其在Paddle-Lite上的运行速度、内存占用等性能指标。

模型优化过程：

（1）准备待优化的PaddlePaddle模型

PaddlePaddle模型有两种保存格式：
   Combined Param：所有参数信息保存在单个文件`params`中，模型的拓扑信息保存在`__model__`文件中。

![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_combined_model.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A19%3A28Z%2F300%2Fhost%2F179ff9de443fc37c1954756946999f3df4f7bb1cbf5e8c5a70845b44c3fd4fcf)

   Seperated Param：参数信息分开保存在多个参数文件中，模型的拓扑信息保存在`__model__`文件中。
![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_seperated_model.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A19%3A42Z%2F300%2Fhost%2F0cbe5dde746a09d9209ab572ab849a415bf05fc0bf9ab9c07995c9f92fd6d90b)

(2) 终端中执行`model_optimize_tool`优化模型
**使用示例**：转化`mobilenet_v1`模型
```
./model_optimize_tool --model_dir=./mobilenet_v1 --valid_targets=arm --optimize_out_type=naive_buffer --optimize_out=mobilenet_v1_opt
```
以上命令可以将`mobilenet_v1`模型转化为arm硬件平台、naive_buffer格式的Paddle_Lite支持模型，优化后的模型文件位于`mobilenet_v1_opt`，转化结果如下图所示：

![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_resulted_model.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A21%3A39Z%2F300%2Fhost%2F8c5580deec3f4c2ff67145100db2ce9efb307c0603d1f19e45bc0db2708309a3)


(3) **更详尽的转化命令**总结：

```shell
./model_optimize_tool \
    --model_dir=<model_param_dir> \
    --model_file=<model_path> \
    --param_file=<param_path> \
    --optimize_out_type=(protobuf|naive_buffer) \
    --optimize_out=<output_optimize_model_dir> \
    --valid_targets=(arm|opencl|x86|npu|xpu) \
    --prefer_int8_kernel=(true|false) \
    --record_tailoring_info =(true|false)
```

| 选项         | 说明 |
| ------------------- | ------------------------------------------------------------ |
| --model_dir         | 待优化的PaddlePaddle模型（非combined形式）的路径 |
| --model_file        | 待优化的PaddlePaddle模型（combined形式）的网络结构文件路径。 |
| --param_file        | 待优化的PaddlePaddle模型（combined形式）的权重文件路径。 |
| --optimize_out_type | 输出模型类型，目前支持两种类型：protobuf和naive_buffer，其中naive_buffer是一种更轻量级的序列化/反序列化实现。若您需要在mobile端执行模型预测，请将此选项设置为naive_buffer。默认为protobuf。 |
| --optimize_out      | 优化模型的输出路径。                                         |
| --valid_targets     | 指定模型可执行的backend，默认为arm。目前可支持x86、arm、opencl、npu、xpu，可以同时指定多个backend(以空格分隔)，Model Optimize Tool将会自动选择最佳方式。如果需要支持华为NPU（Kirin 810/990 Soc搭载的达芬奇架构NPU），应当设置为npu, arm。 |
| --prefer_int8_kernel | 若待优化模型为int8量化模型（如量化训练得到的量化模型），则设置该选项为true以使用int8内核函数进行推理加速，默认为false。                          |
| --record_tailoring_info | 当使用[根据模型裁剪库文件](../library_tailoring)功能时，则设置该选项为true，以记录优化后模型含有的kernel和OP信息，默认为false。 |

* 如果待优化的fluid模型是非combined形式，请设置`--model_dir`，忽略`--model_file`和`--param_file`。
* 如果待优化的fluid模型是combined形式，请设置`--model_file`和`--param_file`，忽略`--model_dir`。
* 优化后的模型包括__model__.nb和param.nb文件。

### 功能二：统计模型算子信息、判断是否支持

model_optimize_tool可以统计并打印出model中的算子信息、判断Paddle-Lite是否支持该模型。并可以打印出当前Paddle-Lite的算子支持情况。

（1）使用model_optimize_tool统计模型中算子信息

下面命令可以打印出mobilenet_v1模型中包含的所有算子，并判断在硬件平台`valid_targets`下Paddle-Lite是否支持该模型

`./model_optimize_tool --print_model_ops=true  --model_dir=mobilenet_v1 --valid_targets=arm`

![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_print_modelops.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A22%3A49Z%2F300%2Fhost%2F9dd8708cce6c24630d702e71abdac48bb75ec1219285704b742b420de38b0f02)

（2）使用model_optimize_tool打印当前Paddle-Lite支持的算子信息

`./model_optimize_tool --print_all_ops=true`

以上命令可以打印出当前Paddle-Lite支持的所有算子信息，包括OP的数量和每个OP支持哪些硬件平台：

![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_print_allops.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A22%3A07Z%2F300%2Fhost%2Fc6438abfd267edaacae741e990c35f95f63a7ea9a803a0ea9659edcf4c1170df)

`./model_optimize_tool ----print_supported_ops=true  --valid_targets=arm`

以上命令可以打印出当`valid_targets=arm`时Paddle-Lite支持的所有OP：

![](http://paddlelite-data.bj.bcebos.com/doc_img/model_optimize_tool/opt_print_supportedops.png?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A23%3A46Z%2F300%2Fhost%2F849bd65807abec2cc2a0560006397952b25f3331fe39d13815b4f91f63a7532f)

## 其他功能：合并x2paddle和model_optimize_tool的一键脚本

**背景**：如果想用Paddle-Lite运行第三方来源（tensorflow、caffe、onnx）模型，一般需要经过两次转化。即使用x2paddle工具将第三方模型转化为PaddlePaddle格式，再使用model_optimize_tool将PaddlePaddle模型转化为Padde-Lite可支持格式。
为了简化这一过程，我们提供一键脚本，将x2paddle转化和model_optimize_tool转化合并：

**一键转化脚本**：[auto_transform.sh](http://paddlelite-data.bj.bcebos.com/model_optimize_tool/auto_transform.sh?authorization=bce-auth-v1%2Fda8cb47e87b14fdbbf696cae71997a31%2F2020-01-03T08%3A35%3A30Z%2F300%2Fhost%2Fccde5ef285cb627fccf72c20079a8fa615990d53a56dee377b8a90942cde999c)

**环境要求**：使用`auto_transform.sh`脚本转化第三方模型时，需要先安装x2paddle环境，请参考[x2paddle环境安装方法]([https://github.com/PaddlePaddle/X2Paddle#%E7%8E%AF%E5%A2%83%E4%BE%9D%E8%B5%96](https://github.com/PaddlePaddle/X2Paddle#环境依赖))安装x2paddle和其环境依赖项。

**使用方法**：

（1）打印帮助帮助信息：` ./auto_transform.sh`

（2）转化模型方法

```bash
USAGE:
    auto_transform.sh combines the function of x2paddle and model_optimize_tool, it can 
    tranform model from tensorflow/caffe/onnx form into paddle-lite naive-buffer form.
----------------------------------------
example:
    ./auto_transform.sh --framework=tensorflow --model=tf_model.pb --optimize_out=opt_model_result
----------------------------------------
Arguments about x2paddle:
    --framework=(tensorflow|caffe|onnx);
    --model='model file for tensorflow or onnx';
    --prototxt='proto file for caffe' --weight='weight file for caffe'
 For TensorFlow:
   --framework=tensorflow --model=tf_model.pb

 For Caffe:
   --framework=caffe --prototxt=deploy.prototxt --weight=deploy.caffemodel

 For ONNX
   --framework=onnx --model=onnx_model.onnx

Arguments about model_optimize_tool:
    --valid_targets=(arm|opencl|x86|npu|xpu); valid targets on Paddle-Lite.
    --fluid_save_dir='path to outputed model after x2paddle'
    --optimize_out='path to outputed Paddle-Lite model'
----------------------------------------
```