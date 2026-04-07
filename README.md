# UE5 爱心Widget项目

这是一个在UE5中使用C++创建的Widget，用于绘制唯美的爱心并添加粒子效果。

## 项目结构

- `Source/DearHu/HeartWidget.h` - C++ Widget类头文件
- `Source/DearHu/HeartWidget.cpp` - C++ Widget类实现文件

## 使用方法

1. 打开UE5编辑器，加载项目
2. 编辑器会自动编译C++代码
3. 在内容浏览器中，右键点击`Content`文件夹，选择"新建" -> "Widget Blueprint"
4. 命名为`HeartWidget`，然后在蓝图编辑器中添加`HeartWidget`组件
5. 拖拽`HeartWidget`蓝图到关卡中，或在其他UI蓝图中使用
6. 运行游戏，查看爱心和粒子效果

## 效果说明

- 爱心会以设定的速度跳动
- 从爱心边缘会发射出红色粒子
- 粒子会受到重力影响，逐渐下落并消失
- 爱心大小会随心跳动画变化
- 粒子会从爱心的整个边缘发射，形成环绕效果

## 技术实现

- **C++ Widget**：使用`UUserWidget`基类创建自定义Widget
- **爱心绘制**：使用参数方程 `x = 16sin³t, y = 13cost - 5cos2t - 2cos3t - cos4t` 生成爱心形状
- **粒子系统**：自定义粒子系统，实现粒子的生成、更新和渲染
- **动画效果**：使用正弦函数实现心跳动画
- **Slate绘制**：使用Slate的绘制API绘制爱心和粒子
- **日志系统**：添加了详细的日志，便于调试和问题定位

## 自定义参数

在蓝图编辑器中，选择`HeartWidget`组件，可以调整以下参数：

### Heart Settings
- `HeartSize`：爱心大小，默认值为100.0
- `HeartColor`：爱心颜色，默认值为红色
- `HeartBeatSpeed`：心跳速度，默认值为1.0

### Particle Settings
- `EmitPointCount`：粒子发射点的数量，默认值为5
- `ParticlesPerEmit`：每个发射点发射的粒子数量，默认值为5

## 注意事项

- 项目需要UE5.7或更高版本
- 首次打开项目时，UE5编辑器会自动编译C++代码
- Widget可以在任何UI场景中使用，不仅限于3D关卡
- 粒子效果可以通过调整参数来获得不同的视觉效果
- 详细的日志会输出到UE5的输出日志窗口，帮助调试