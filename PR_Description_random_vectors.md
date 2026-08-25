# PR 描述：random_vectors 随机向量滤波器（模仿 ParaView Random Vectors）

## 功能概述
新增 `RandomVectorsFilter`：模仿 ParaView 的 **"Random Vectors"** 滤波器（对应 VTK `vtkBrownianPoints`）。
对输入网格的每个点生成随机向量 `BrownianVectors`，作为 `IG_VECTOR` 点属性输出。向量方向为随机单位向量，模长均匀分布在 [最小速度, 最大速度] 区间。

对任意网格输出一个点属性:
- `BrownianVectors` - 3 分量 float 向量（模长 ∈ [MinimumSpeed, MaximumSpeed]）

支持的网格类型：SurfaceMesh、VolumeMesh、UnstructuredMesh、StructuredMesh（及任何 PointSet）。

## 文件改动说明
Filter:
- 新增 `iGameCore/Filters/Sources/iGameRandomVectorsFilter.cpp`
- 新增 `iGameCore/Filters/Sources/iGameRandomVectorsFilter.h`
- `iGameCore/Filters/iGameFilterIncludes.h` 注册 `Sources/iGameRandomVectorsFilter.h`

Example:
- 新增 `Examples/Filter/Sources/TestRandomVectors.cpp`
- 命令行接收模型路径参数

QT:
- 修改 `Qt/src/IQCore/igQtMainWindow.cpp`
- 在 UI 菜单：**滤镜 -> 数据处理 (Data Processing)** 中添加 **随机向量 (Random Vectors)**
- 未导入模型时输出提示

## UI 使用流程
1. 加载网格模型
2. 菜单 -> **滤镜 -> 数据处理 (Data Processing) -> 随机向量 (Random Vectors)**
3. 配置参数：最小速度 / 最大速度
4. 点击应用，模型树出现 `BrownianVectors` 属性
5. 打开"矢量场"面板可查看每个点的向量

## 测试情况
`testRandomVectors`:

- 编译通过
- 运行结果（`Examples/Models/kit.vtk`）：
  - Points：11424
  - Dimension：3
  - Elements：11424
  - 向量模长范围：[0, 1]
  - `Result: PASS`

`igamevis`:

- 编译通过
- 运行结果：GUI 菜单入口可正常为模型生成 `BrownianVectors` 属性
