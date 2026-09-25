[README.md](https://github.com/user-attachments/files/32652818/README.md)
# 地铁查询售票系统

> 基于 Qt/C++ 和 Microsoft SQL Server 的地铁线路查询与售票桌面应用

## 项目简介

本系统是一个地铁查询售票系统的课程设计项目，模拟真实地铁运营场景，提供线路查询、购票、用户管理等核心功能。系统采用 C/S（客户端/服务器）架构，前端使用 Qt 框架开发图形界面，后端使用 Microsoft SQL Server 存储数据，通过 ODBC 驱动进行数据访问。

## 技术栈

| 层级 | 技术 | 说明 |
|------|------|------|
| 前端 | Qt (C++) | 跨平台 GUI 框架，使用 QtWidgets |
| 数据库 | Microsoft SQL Server | 关系型数据库，通过 ODBC 连接 |
| 数据建模 | PowerDesigner 15.1 | 用于设计 CDM（概念模型）和 PDM（物理模型） |
| 数据库连接 | QODBC (Qt SQL Module) | Qt 提供的 ODBC 数据库驱动 |

## 目录结构

```
地铁查询售票系统/
├── 模型文件（CDM和PDM）/
│   ├── Conceptual Data_1.cdm       # PowerDesigner 概念数据模型
│   └── Conceptual Data_1.pdm       # PowerDesigner 物理数据模型
├── 数据库文件(MDF和LDF)/
│   ├── subway.mdf                   # SQL Server 主数据文件（6.0 MB）
│   └── subway.ldf                   # SQL Server 事务日志文件（2.3 MB）
├── 源代码(数据库和应用程序源代码)/
│   ├── database.sql                 # 完整数据库建表脚本（含表、视图、约束、外键）
│   ├── main.cpp                     # 应用程序入口
│   ├── loginwindow.cpp              # 登录窗口实现
│   ├── mainwindow.cpp               # 主窗口实现（含数据库连接逻辑）
│   └── linequerywindow.cpp          # 线路查询窗口实现
├── 赵康博_地铁查询售票系统.docx      # 项目设计文档（Word 版）
├── 赵康博_地铁查询售票系统.pdf       # 项目设计文档（PDF 版）
└── README.md                         # 本文件
```

## 数据库设计

### ER 模型概览

系统概念模型包含以下核心实体及其关系：

```
  ┌──────────┐         ┌──────────┐
  │   User    │         │   Admin   │
  │  用户     │         │  管理员   │
  └────┬─────┘         └─────┬────┘
       │                     │
       │ 1:N                 │ 1:N
       │                     │
       ▼                     ▼
  ┌──────────┐    ┌──────────────┐    ┌──────────┐
  │  Ticket  │───▶│    Path      │───▶│  Station  │
  │  车票    │    │   路径       │    │  车站    │
  └────┬─────┘    └──────────────┘    └─────┬────┘
       │                                     │
       │ N:1                          M:N    │
       ▼                               │     ▼
  ┌──────────────┐            ┌──────────────────┐
  │  FareRule    │            │   LineStation     │
  │  票价规则   │            │  线路-车站关联表  │
  └──────────────┘            └────────┬──────────┘
                                        │ M:N
                                        ▼
                                 ┌──────────────┐
                                 │  SubwayLine  │
                                 │   地铁线路   │
                                 └──────────────┘
```

### 数据表说明

| 表名 | 说明 | 主要字段 |
|------|------|----------|
| **SubwayLines** | 地铁线路信息 | lineID, lineName, startStation, endStation, firstTrainTime, lastTrainTime, intervalTime, status |
| **Stations** | 车站信息 | stationID, stationName, location(地理坐标), isTransfer(是否换乘站), facilities, openTime, closeTime |
| **LineStations** | 线路-车站关联（多对多） | id, lineID, stationID, sequence(站序) |
| **Users** | 用户信息 | userID, username, password, phone, email, balance(余额), registerTime, lastLoginTime |
| **Admins** | 管理员信息 | adminID, username, password, role, department, lastLoginTime |
| **Paths** | 出行路径 | pathID, startStationID, endStationID, totalDistance, estimatedTime, transferTimes, pathDetail |
| **Tickets** | 车票信息 | ticketID, userID, pathID, ruleID, purchaseTime, expireTime, status, QRcode, price |
| **FareRules** | 票价规则 | ruleID, baseFare, distanceFare, discountPolicy, effectiveDate, expirationDate, adminID |

### 数据库视图

| 视图名 | 说明 |
|--------|------|
| **vw_LineDetails** | 线路详情视图 — 汇总每条线路的车站数量、最早开放时间、最晚关闭时间 |
| **vw_UserTickets** | 用户车票视图 — 关联用户、车票、路径和车站信息，展示出发站和到达站 |
| **vw_MultiUserTickets** | 多用户购票统计视图 — 按用户汇总购票数量和总消费金额 |

### 数据完整性约束

- **CHECK 约束**：
  - `CHK_IntervalTime`：线路发车间隔必须大于 0
  - `CHK_Status`：车票状态限定为 `未使用` / `已使用` / `已过期`
  - `CHK_Balance`：用户余额不能为负数
- **UNIQUE 约束**：
  - `UQ_LineSequence`：同一线路上站序唯一
  - `UQ_LineStation`：同一线路上车站唯一
  - Users 和 Admins 的 username 字段唯一
- **外键约束**：共 8 条外键，覆盖线路-车站、路径-车站、车票-用户/路径/票价规则、票价规则-管理员等关系

## 应用程序架构

### 窗口流程

```
程序启动
  │
  ▼
┌──────────────┐
│  LoginWindow  │  登录窗口
│  (登录对话框)  │  用户输入用户名/密码 → 数据库验证
└──────┬───────┘
       │
       ├─ 登录成功 (QDialog::Accepted)
       │        │
       │        ▼
       │   ┌──────────────┐
       │   │  MainWindow   │  主窗口（传入 userID）
       │   │  (主功能界面)  │  连接数据库，提供核心功能
       │   └──────┬───────┘
       │          │
       │          ▼
       │   ┌──────────────────┐
       │   │ LineQueryWindow  │  线路查询窗口
       │   │ (线路查询功能)    │
       │   └──────────────────┘
       │
       └─ 登录失败 → 显示错误信息，留在登录窗口
```

### 源代码文件说明

#### `main.cpp` — 程序入口
- 创建 `QApplication` 实例
- 弹出 `LoginWindow` 登录对话框
- 登录成功后创建 `MainWindow`（传入已登录用户 ID）并显示主界面

#### `loginwindow.cpp` — 登录窗口
- 使用 ODBC 连接 SQL Server 数据库（`Trusted_Connection=Yes`）
- 从 UI 获取用户名和密码，通过参数化查询验证 `Users` 表
- 登录成功后调用 `accept()` 关闭对话框并返回 `currentUserId`
- 包含输入非空校验和错误提示

#### `mainwindow.cpp` — 主窗口
- 封装 `connectToDatabase()` 方法，配置 ODBC DSN 连接字符串
- 连接成功输出调试日志，失败弹出错误对话框
- 实际业务功能基于此类扩展

#### `linequerywindow.cpp` — 线路查询窗口
- 继承自 `QMainWindow`
- 当前为框架结构，预留线路查询功能扩展

## 快速开始

### 环境要求

- **Qt**：5.x 或 6.x（需包含 Qt SQL 模块和 ODBC 驱动）
- **Microsoft SQL Server**：2016 及以上版本（或 SQL Server Express）
- **ODBC 驱动**：SQL Server ODBC Driver
- **PowerDesigner**（可选）：15.1 及以上版本，用于查看/编辑 CDM 和 PDM 模型文件

### 数据库部署

#### 方式一：附加 MDF/LDF 文件

1. 将 `数据库文件(MDF和LDF)/subway.mdf` 和 `subway.ldf` 复制到 SQL Server 数据目录
2. 在 SSMS 中右键「数据库」→「附加」→ 选择 `subway.mdf`

#### 方式二：执行 SQL 脚本

1. 在 SSMS 中新建查询
2. 打开 `源代码(数据库和应用程序源代码)/database.sql`
3. 执行脚本创建所有表、视图、约束和外键

### 配置数据库连接

在 `loginwindow.cpp` 和 `mainwindow.cpp` 中修改 ODBC 连接字符串：

```cpp
// 方式一：Windows 身份验证（当前用户）
db.setDatabaseName("DRIVER={SQL Server};SERVER=localhost;DATABASE=SubwaySystem;Trusted_Connection=Yes;");

// 方式二：SQL Server 身份验证
QString dsn = "DRIVER={SQL Server};SERVER=你的服务器地址;DATABASE=SubwaySystem;UID=用户名;PWD=密码;";
```

### 编译运行

1. 使用 Qt Creator 打开项目 `.pro` 文件
2. 在 `.pro` 中添加 SQL 模块支持：
   ```pro
   QT += core gui sql widgets
   ```
3. 构建并运行

## 功能模块

| 模块 | 状态 | 说明 |
|------|------|------|
| 用户登录 | 已实现 | 用户名/密码验证，基于 `Users` 表查询 |
| 数据库连接 | 已实现 | ODBC 方式连接 SQL Server |
| 线路查询 | 框架已搭建 | `LineQueryWindow` 类已定义，功能待扩展 |
| 购票 | 数据层已就绪 | `Tickets` 表及关联表已建，UI 待扩展 |
| 票价计算 | 数据层已就绪 | `FareRules` 表已建，含基础票价和距离计费规则 |
| 路径规划 | 数据层已就绪 | `Paths` 表记录起讫站、距离、预估时间、换乘次数 |
| 用户管理 | 数据层已就绪 | `Users` 表含余额、注册时间、最后登录时间 |
| 管理员管理 | 数据层已就绪 | `Admins` 表含角色、部门信息 |

## 数据模型文件

- **`Conceptual Data_1.cdm`**：PowerDesigner 概念数据模型，定义实体（User、Admin、SubwayLine、Station、Ticket、FareRule、Transaction、Path、Line_Station）及实体间关系
- **`Conceptual Data_1.pdm`**：PowerDesigner 物理数据模型，将概念模型映射为具体数据库表结构（目标数据库：Sybase SQL Anywhere 11），包含主键、外键、索引定义

> 使用 PowerDesigner 15.1+ 打开 `.cdm` 和 `.pdm` 文件可查看完整的 ER 图和数据模型设计。
