# BatchCharsetEncodingConvert

批量字符集编码检测与转换工具。基于 Qt 5 构建，集成 universalchardet 引擎实现编码自动识别，支持多线程并行处理。

## 功能特性

- **文件扫描**：支持选择单个或多个文件，也可选择文件夹并按文件类型（23+ 种源码/文本格式）递归筛选
- **编码检测**：基于 universalchardet（Mozilla 通用字符集检测引擎）自动识别文件编码，支持 UTF-8、GBK、Big5、Shift_JIS、EUC-KR、ISO-8859 系列等主流编码
- **批量转换**：将文件从源编码批量转换为目标编码，转换前可选择创建 `.bak` 备份文件
- **多线程架构**：文件扫描、编码检测、文件转换三个阶段均在 `QThreadPool` 中并行执行，UI 不阻塞
- **灵活的编码设置**：
  - 逐行编辑：点击表格单元格直接修改单个文件的源编码或输出编码
  - 批量设置：右键表头「Out Encode」列可统一设置所有文件的输出编码
  - 多选设置：选中多行后右键可批量设置源编码或输出编码
  - 未知编码处理：转换时遇到无法识别的编码，弹窗让用户手动指定，可勾选「应用到全部」
- **进度与取消**：每个阶段均显示进度对话框，支持随时取消
- **右键菜单**：打开文件所在目录、使用记事本打开、移除选中行

## 支持的编码

UTF-8、UTF-8 BOM、UTF-16BE、UTF-16LE、UTF-32BE、UTF-32LE、ISO-2022-CN、BIG5、EUC-TW、GB18030、HZ-GB-2312、GBK、GB2312、ISO-8859-1 ~ ISO-8859-15、Shift_JIS、EUC-JP、EUC-KR、KOI8-R、KOI8-U、Windows-1250 ~ Windows-1258、TIS-620 等。

## 支持的文件类型

选择文件夹扫描时，可按以下类型筛选：

| 分类 | 扩展名 |
|------|--------|
| C/C++ | c, cpp, cc, cxx, h, hpp, hxx |
| HTML | html, htm |
| XML | xml |
| CSS | css |
| JavaScript | js, jsx |
| JSON | json |
| Python | py, pyw |
| Java | java |
| C# | cs |
| PHP | php |
| Ruby | rb |
| Go | go |
| Rust | rs |
| SQL | sql |
| Text | txt, log |
| Markdown | md, markdown |
| Config | ini, cfg, conf, properties |
| Shell | sh, bash, bat, cmd |
| YAML | yaml, yml |
| Lua | lua |
| Perl | pl, pm |
| Swift | swift |
| Kotlin | kt, kts |

## 项目结构

```
BatchCharsetEncodingConvert/
├── BatchCharsetConvert.pro      # Qt 项目文件
├── app.ico                      # 应用程序图标
├── app.rc                       # Windows 资源文件
├── code/
│   ├── main.cpp                 # 程序入口，设置 Fusion 样式
│   ├── Widget.hpp/cpp           # 主窗口，UI 布局与业务调度
│   ├── FileScanWorker.hpp/cpp   # 文件扫描工作线程（QRunnable）
│   ├── EncodingDetectWorker.hpp/cpp  # 编码检测工作线程（QRunnable）
│   ├── FileConvertWorker.hpp/cpp     # 文件转换工作线程（QRunnable）
│   ├── CharsetDetector.hpp/cpp  # universalchardet 封装，继承 nsUniversalDetector
│   └── ComboxDelegate.hpp/cpp   # 表格编码下拉框代理
└── universalchardet/            # universalchardet 字符集检测库源码
    ├── uchardet.h/cpp           # 库入口 API
    ├── nsUniversalDetector.h/cpp # 通用检测器基类
    ├── ns*Prober.h/cpp          # 各编码探测器
    └── LangModels/              # 30 个语言模型
```

## 架构设计

程序采用三阶段流水线，每个阶段通过 `QThreadPool` + `QRunnable` 实现多线程并行：

```
┌──────────────┐     ┌──────────────────┐     ┌──────────────────┐
│  文件扫描     │     │   编码检测        │     │   文件转换        │
│ FileScan     │────>│ EncodingDetect   │────>│ FileConvert      │
│ Worker       │     │ Worker (x N)     │     │ Worker (x N)     │
└──────────────┘     └──────────────────┘     └──────────────────┘
     单线程               多线程并行               多线程并行
     递归遍历             每文件一个任务           每文件一个任务
     按扩展名过滤         CharsetDetector         QTextStream 读写
                          (uchardet)              支持备份 .bak
```

### 线程通信

- Worker 通过 Qt 信号（`progressUpdated`、`scanCompleted`、`resultReady` 等）向主线程回报进度
- 主线程通过 `QAtomicInt m_cancelFlag` 实现线程安全的取消控制
- `QProgressDialog` 以 `Qt::WindowModal` 模式显示，支持取消操作

### 编码检测

`CharsetDetector` 继承 `nsUniversalDetector`，封装 universalchardet 的检测能力：
- `detectFile(QFile&)` — 读取文件内容并返回编码名称字符串
- `detectData(const QByteArray&)` — 对内存数据执行检测

## 构建

### 环境要求

- Qt 5.15+
- C++ 编译器（MSVC / MinGW / GCC）
- qmake

### 编译步骤

```bash
qmake BatchCharsetConvert.pro
make          # Linux / macOS
mingw32-make  # Windows MinGW
nmake         # Windows MSVC
```

或在 Qt Creator 中直接打开 `BatchCharsetConvert.pro` 构建。

## 使用说明

1. **添加文件**：点击「选择文件...」添加单个或多个文件，或点击「选择文件夹...」按类型递归扫描目录
2. **查看编码**：扫描完成后，表格自动显示每个文件的文件名、大小、检测到的编码
3. **调整编码**：
   - 双击「Encode」列修改源编码（自动检测不准确时）
   - 双击「Out Encode」列修改目标编码
   - 右键表头「Out Encode」列 → 「统一设置输出编码」批量设置
4. **执行转换**：点击「转换编码...」开始批量转换，转换前会询问是否创建备份

## 许可证

本项目使用的 universalchardet 库遵循 MPL 1.1 / GPL 2.0 / LGPL 2.1 三重许可证。
