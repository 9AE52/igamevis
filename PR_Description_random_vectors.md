# PR 描述：random_vectors 随机向量滤波器（模仿 ParaView Random Vectors）

## 功能概述
新增 `RandomVectorsFilter`：模仿 ParaView 的 **"Random Vectors"** 滤波器（对应 VTK `vtkBrownianPoints`）。
对输入网格的每个点生成随机向量 `BrownianVectors`，作为 `IG_VECTOR` 点属性输出。向量方向为随机单位向量，模长均匀分布在 [最小速度, 最大速度] 区间。

**不修改原模型**：过滤器深拷贝输入网格，随机向量只加到新网格上，并输出一个独立的新模型（命名 `原模型名_RandomVectors`）。

对任意网格输出一个点属性:
- `BrownianVectors` - 3 分量 float 向量（模长 ∈ [MinimumSpeed, MaximumSpeed]）

支持的网格类型：SurfaceMesh、VolumeMesh、UnstructuredMesh、StructuredMesh（及任何 PointSet）。

## 文件改动说明
Filter:
- 新增 `iGameCore/Filters/AttributeManipulation/iGameRandomVectorsFilter.cpp`
- 新增 `iGameCore/Filters/AttributeManipulation/iGameRandomVectorsFilter.h`
- `iGameCore/Filters/iGameFilterIncludes.h` 注册 `AttributeManipulation/iGameRandomVectorsFilter.h`

Example:
- 新增 `Examples/Filter/AttributeManipulation/TestRandomVectors.cpp`
- 命令行接收模型路径参数
- 校验输出为新模型、原模型不被修改

QT:
- 修改 `Qt/src/IQCore/igQtMainWindow.cpp`
- 在 UI 菜单：**滤镜 -> 数据属性操作 (Attribute Manipulation)** 中添加 **随机向量 (Random Vectors)**
- 执行成功后作为新模型加入模型树
- 未导入模型时输出提示

## UI 使用流程
1. 加载网格模型
2. 菜单 -> **滤镜 -> 数据属性操作 (Attribute Manipulation) -> 随机向量 (Random Vectors)**
3. 配置参数：最小速度 / 最大速度
4. 点击应用，模型树出现新模型（`xxx_RandomVectors`），展开可见 `BrownianVectors` 属性
5. 打开"矢量场"面板可查看每个点的向量；原模型保持不变

## 测试情况
`testRandomVectors`:

- 编译通过
- 运行结果（`Examples/Models/kit.vtk`）：
  - Points：11424
  - Dimension：3
  - Elements：11424
  - 向量模长范围：[0, 1]
  - 输出为新模型（非原模型），原模型无 `BrownianVectors`
  - `Result: PASS`

`igamevis`:

- 编译通过
- 运行结果：GUI 菜单入口可生成新模型并显示 `BrownianVectors` 属性
