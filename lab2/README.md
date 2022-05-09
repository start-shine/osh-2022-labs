## shell 部分
- 支持管道：前一条指令的结果写入tmp.txt,后一条指令的结果从中取
- 支持基本的文件重定向
- 处理Ctrl-C
- 支持History：每一次关闭shell时将指令写到根目录下的history_shell.txt,打开shell时从中读取以往指令。指令的编号从0开始
- 处理Ctrl-D
- 更多功能：echo ~root , echo $SHELL

补充：管道只考虑了管道单独出现，以及管道和重定向一起出现的情况，没有考虑更多。

## strace 部分
实现基础部分
