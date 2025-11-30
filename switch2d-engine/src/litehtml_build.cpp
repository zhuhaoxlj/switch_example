// ============================================
// Litehtml 统一编译单元
// ============================================
// 这个文件将所有 litehtml 和 gumbo-parser 源文件
// 合并到一个编译单元中，以简化构建过程
// ============================================

// 定义必要的宏
#define LITEHTML_NO_THREADS

// 注意：Gumbo Parser 的 C 文件由 Makefile 单独编译
// 它们在 third_party/litehtml/src/gumbo/ 目录下

// 注意：litehtml 源文件编译比较复杂
// 我们在这里只包含主要的头文件，实际的源文件由 Makefile 单独编译

// 引入 litehtml 主头文件
#include "litehtml.h"

// 这个文件的作用是确保 litehtml 的符号被正确导出
// 实际的编译由独立的源文件列表处理
