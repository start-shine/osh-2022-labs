# lab1 

PB20000072 王铖潇



## Linux 编译

- 提交编译好的内核文件 `bzImage`，保证其能够完成后续实验
- `bzImage` 文件大小小于 4MiB



## 创建初始内存盘

提交编译好的初始内存盘 `initrd.cpio.gz`，保证其能够显示 "Hello, Linux!"



## 添加一个自定义的 Linux syscall

- 提交编译好的内核 `bzImage`，保证能够在 buffer 长度充足时完成 syscall——2 分
- 编译好的内核 `bzImage` 也能保证在 buffer 长度不充足时完成 syscall——2 分
- 提交测试 syscall 的 initrd 源代码文件——2 分