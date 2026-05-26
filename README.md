# SmartCar — 智能车物料搬运竞赛项目

全国大学生智能汽车竞赛 第20届智能视觉组参赛项目，基于 NXP RT1064 平台的三轮全向底盘智能车。

## 硬件平台

| 项目 | 详情 |
|------|------|
| 主控芯片 | NXP MIMXRT1064DVL6A (Cortex-M7 @ 600MHz) |
| 摄像头 | MT9V03X 灰度全局快门摄像头 (FlexIO 接口) |
| IMU | IMU660RA 六轴惯性测量单元 |
| 底盘 | 三轮全向底盘 (减速比 4.4:1, 轮径 28mm) |
| 显示 | IPS200 彩色显示屏 |
| 通信 | 6×LPUART + 无线调试模块 |

另有 MCX Vision Board (NXP MCXN947) 实验分支，搭载 SCC8660 彩色摄像头与 TensorFlow Lite 模型推理。

## 功能特性

- **自主巡线导航** — 基于摄像头图像处理的赛道巡线算法 (OTSU 二值化、Sobel 边缘检测、自适应阈值)
- **物料抓取分拣** — 摄像头识别卡片信息，机械臂抓取并分类入库
- **复杂路况处理** — 十字路口、环岛、S 弯、斑马线等赛道元素识别与应对
- **PID 运动控制** — 三电机速度/位置双环 PID + 陀螺仪航向角 PID
- **卡尔曼滤波** — IMU 姿态解算与角度估计
- **状态机架构** — 发车 → 巡线 → 散卡抓取 → 十字回环 → 环岛 → 卸货

## 目录结构

```
├── Debug_SmartCar_main/          # RT1064 主项目
│   ├── libraries/                # SDK / CMSIS 驱动 / 第三方外设库
│   └── project/
│       ├── code/                 # 用户控制逻辑 (28 个 .c/.h 文件)
│       ├── iar/                  # IAR Embedded Workbench 工程
│       ├── mdk/                  # Keil MDK 工程
│       └── user/                 # main.c / isr.c
├── Debug_SmartCar_mcx/           # MCX Vision Board 实验分支
│   ├── libraries/                # MCUXpresso SDK + TensorFlow Lite
│   └── project/
│       ├── mdk/                  # Keil MDK 工程
│       └── user/                 # color_tracer / TFLite 模型推理
└── README.md
```

## 构建方式

- **Keil MDK** (uVision 5.33+)：打开 `Debug_SmartCar_main/project/mdk/debug_smart-car_main.uvprojx`
- **IAR Embedded Workbench**：打开 `Debug_SmartCar_main/project/iar/program/rt1064.ewp`

## 代码结构

| 模块 | 文件 | 功能 |
|------|------|------|
| 图像处理 | `ImageProcess.c` | 图像压缩、二值化、边界跟踪、特征检测 |
| 运动控制 | `Control.c` | 巡线控制、全向速度分解、航向锁定旋转 |
| PID | `PID.c` | 三电机速度/位置 PID + 角度 PID |
| 电机驱动 | `Motor.c` | PWM 输出、正交编码器读取、里程测量 |
| 状态机 | `FSM.c` | 顶级任务状态机 (发车→巡线→抓取→卸货) |
| 通信 | `Uart.c` / `Wireless.c` | 串口协议解码、无线调试 |
| 姿态解算 | `KalmanAngle.c` | 卡尔曼滤波角度估计 |
| 数学库 | `AngleMath.c` | 三角函数与快速反平方根 |
| 界面 | `Menus.c` / `IPSshow.c` | IPS 显示屏菜单系统 |

## 开源协议

本项目使用逐飞科技 (SEEKFREE) 开源库，详见 `libraries/LICENSE`。
