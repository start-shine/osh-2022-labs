// IO
#include <iostream>
// std::string
#include <string>
// std::vector
#include <vector>
// std::string 转 int
#include <sstream>
// PATH_MAX 等常量
#include <climits>
// POSIX API
#include <unistd.h>
// wait
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h> // signal functions
std::vector<std::string> split(std::string s, const std::string &delimiter);
std::vector<std::string> cmd_history;
int arg = 0;                  //命令个数
char buf[1024];               //读入字符串
char *command[100];           //切分后的字符串数组
int pid;                      //设为全局变量，方便获得子进程的进程号
int flag[100][6];             //管道的输入输出重定向标记
char *file[100][6] = {0};     //对应两个重定向的文件
char *argv[100][100];         //参数
int ar = 0;                   //管道个数
char *f = (char *)"temp.txt"; //共享文件
int line = 0;                 //指令个数

void do_cmd(char **command);
void history_judge(std::string cmd)
{
  strcpy(buf, cmd.c_str());
  bool value;
  if (buf[0] == '!')
  {
    value = true;
    char buff[100];
    int num;
    if (buf[1] == '!')
    {
      // excute last instruction
      num = line - 2;
    }

    else
      num = strtol(&buf[1], NULL, 10);
    strcpy(buff, cmd_history[num].c_str());
    // std::cout << cmd_history[num];
    char *ptr = NULL;
    char *temp = strtok_r(buff, " ", &ptr);
    // std::cout << temp;
    int j = 0;
    char *cmd_tmp[100];
    while (temp)
    {

      cmd_tmp[j++] = temp;
      temp = strtok_r(NULL, " ", &ptr);
      // std::cout << cmd_tmp[j - 1];
    }
    cmd_tmp[j] = 0; //命令形式的字符串数组最后一位必须是NULL
    if (cmd_tmp[0][0] == '!')
      value = true;
    else
      value = false;
    if (value == true)
    {
      history_judge(cmd_tmp[0]);
      // std::cout << 1;
    }

    else
      do_cmd(cmd_tmp);
    // do_cmd(cmd_tmp);
  }
}

void do_cmd(char **command)
{

  // 外部命令
  // pid_t pid = fork();

  // std::vector<std::string> 转 char **
  // char *arg_ptrs[args.size() + 1];
  // std::cout << args.size();
  // for (auto i = 0; i < args.size(); i++)
  // {
  //   arg_ptrs[i] = &args[i][0];
  //   // std::cout << arg_ptrs[i] << std::endl;
  // }

  // exec p 系列的 argv 需要以 nullptr 结尾
  // arg_ptrs[args.size()] = nullptr;
  ar = 0;
  for (int i = 0; i < 100; i++)
  {
    flag[i][0] = flag[i][1] = flag[i][2] = flag[i][3] = flag[i][4] = flag[i][5] = 0;
    file[i][0] = file[i][1] = file[i][2] = file[i][3] = file[i][4] = file[i][5] = 0;
    for (int j = 0; j < 100; j++)
    {
      argv[i][j] = 0;
    }
  }
  for (int i = 0; i < arg; i++)
    argv[0][i] = command[i]; //初始化第一个参数
  argv[0][arg] = NULL;
  int a = 0; //当前命令参数的序号
  for (int i = 0; i < arg; i++)
  {
    //判断是否存在管道
    if (strcmp(command[i], "|") == 0)
    { // c语言中字符串比较只能用strcmp函数
      // printf("遇到 | 符号\n");
      argv[ar][a++] = NULL;
      ar++;
      a = 0;
    }
    else if (strcmp(command[i], "history") == 0)
    { // history
      flag[ar][3] = 1;
      file[ar][3] = command[i + 1];
      argv[ar][a++] = NULL;
    }
    else if (command[i][0] == '!')
    { //没有管道时的输出重定向
      flag[ar][4] = 1;
      file[ar][4] = command[i + 1];
      // std::cout << &command[i][1];
      argv[ar][a++] = NULL; //考虑有咩有输入重定向的情况
    }
    else if (strcmp(command[i], ">>") == 0)
    { //存在输出重定向
      flag[ar][2] = 1;
      file[ar][2] = command[i + 1];
      argv[ar][a++] = NULL;
    }
    else if (strcmp(command[i], "<") == 0)
    { //存在输入重定向
      flag[ar][0] = 1;
      file[ar][0] = command[i + 1];
      argv[ar][a++] = NULL;
    }

    else if (strcmp(command[i], ">") == 0)
    { //没有管道时的输出重定向
      flag[ar][1] = 1;
      file[ar][1] = command[i + 1];
      argv[ar][a++] = NULL; //考虑有咩有输入重定向的情况
    }

    else
      argv[ar][a++] = command[i];
  }

  pid = fork(); //创建的子进程
  // std::cout << "ar4" << ar;
  if (pid < 0)
  {
    perror("fork error\n");
    exit(0);
  }
  // std::cout << "pid" << pid << std::endl;
  //  std::cout << "ar5" << ar;
  //先判断是否存在管道，如果有管道，则需要用多个命令参数，并且创建新的子进程。否则一个命令即可
  if (pid == 0)
  {
    // std::cout << 111;
    //  std::cout << "pid" << pid << std::endl;
    //  std::cout << "ar2" << ar;
    if (!ar)
    { //没有管道
      // std::cout << flag[0][4];
      //  std::cout << flag[0][0];
      //  std::cout << file[0][0];
      if (flag[0][0])
      { //判断有无输入重定向
        close(0);
        int fd = open(file[0][0], O_RDONLY, 0644);
      }
      if (flag[0][1])
      { //判断有无输出重定向
        close(1);
        int fd2 = open(file[0][1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
      }
      if (flag[0][2])
      {
        // std::cout << 1 << std::endl;
        close(1);
        int fd3 = open(file[0][2], O_WRONLY | O_CREAT | O_APPEND, 0666);
      }
      if (flag[0][3])
      {
        int num = strtol(file[0][3], NULL, 10);
        for (int j = line - num - 1; j < line - 1; j++)
        {
          // std::string cmdh = cmd_history.back(); //取最后一个元素
          // cmd_history.pop_back();                //删除最后一个元素
          std::cout << j << " " << cmd_history[j] << std::endl;

          // do_cmd(cmd_history[j][0].c_str());
        }
      }
      // std::cout << flag[0][4];
      // if (flag[0][4])
      // {
      //   std::cout << 1;
      //   std::cout << cmd_history[0];
      //   // std::cout << 4 << std::endl;
      //   char *his_command[100];
      //   // char *ptr = NULL;

      //   int num = strtol(file[0][4], NULL, 10);
      //   std::cout << cmd_history[1];
      //   // std::cout << num << std::endl;
      //   std::cout << cmd_history[num];
      //   char buff[10];

      //   strcpy(buff, cmd_history[num].c_str());
      //   std::cout << buff[0];
      //   int i = 0, j = 0, k = 0;
      //   while (buff[i] != '\0')
      //   {
      //     if (buff[i] != ' ')
      //     {
      //       his_command[j][k++] = buff[i++];
      //     }
      //     else
      //     {
      //       std::cout << his_command[j];
      //       j++;
      //       k = 0;
      //     }
      //   }
      // std::cout << "buff" << buff << std::endl;
      // std::vector<std::string> argss = split(cmd_history[num], " ");
      // std::cout << argss.size();
      // char *tempp = strtok_r(buff, " ", &ptr);
      // std::cout << "ptr" << ptr << std::endl;
      // if (tempp != NULL)
      //   std::cout << "tempp" << tempp;
      // else
      //   std::cout << "tempp" << 0 << std::endl;
      // int j = 0;

      // while (j < argss.size())
      // {
      //   std::cout << j;
      //   strcpy(his_command[j++], argss[j].c_str());
      //   std::cout << his_command[j - 1];
      // }

      // while (tempp)
      // {
      //   his_command[j++] = tempp;
      //   tempp = strtok_r(NULL, " ", &ptr);
      //   std::cout << command[j - 1];
      // }
      // arg = j;
      // his_command[j] = 0; //命令形式的字符串数组最后一位必须是NULL
      // do_cmd(his_command);
      //}
      execvp(argv[0][0], argv[0]);
    }
    else
    {         //有管道
      int tt; //记录当前遍历到第几个命令
      for (tt = 0; tt < ar; tt++)
      {
        int pid2 = fork();
        if (pid2 < 0)
        {
          perror("fork error\n");
          exit(0);
        }
        else if (pid2 == 0)
        {
          if (tt)
          { //如果不是第一个命令，则需要从共享文件读取数据
            close(0);
            int fd = open(f, O_RDONLY); //输入重定向
          }
          if (flag[tt][0])
          {
            close(0);
            int fd = open(file[tt][0], O_RDONLY);
          }
          if (flag[tt][1])
          {
            close(1);
            int fd = open(file[tt][1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
          }
          if (flag[tt][1])
          {
            close(1);
            int fd = open(file[tt][2], O_WRONLY | O_CREAT | O_APPEND, 0666);
          }
          close(1);
          remove(f); //由于当前f文件正在open中，会等到解引用后才删除文件
          int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0666);
          if (execvp(argv[tt][0], argv[tt]) == -1)
          {
            perror("execvp error!\n");
            exit(0);
          }
        }
        else
        { //管道后的命令需要使用管道前命令的结果，因此需要等待
          waitpid(pid2, NULL, 0);
        }
      }
      //接下来需要执行管道的最后一条命令
      close(0);
      int fd = open(f, O_RDONLY); //输入重定向
      if (flag[tt][1])
      {
        close(1);
        int fd = open(file[tt][1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
      }
      if (flag[tt][2])
      {
        close(1);
        int fd = open(file[tt][2], O_WRONLY | O_CREAT | O_APPEND, 0666);
      }
      execvp(argv[tt][0], argv[tt]);
    }
  }
  // father
  else
  {

    waitpid(pid, NULL, 0);
  }
  // 这里只有子进程才会进入
  // execvp 会完全更换子进程接下来的代码，所以正常情况下 execvp 之后这里的代码就没意义了
  // 如果 execvp 之后的代码被运行了，那就是 execvp 出问题了

  // execvp(args[0].c_str(), arg_ptrs);

  // 所以这里直接报错
  // exit(255);
}

// 这里只有父进程（原进程）才会进入
volatile sig_atomic_t ctrl = 0;
static void my_handler(int sig)
{ // can be called asynchronously
  // ctrl = 1; // set flag
  printf("\n");
  printf("#");
}
int main()
{
  // signal(SIGINT, SIG_IGN);
  signal(SIGINT, my_handler);
  //  不同步 iostream 和 cstdio 的 buffer
  // std::ios::sync_with_stdio(false);

  // 用来存储读入的一行命令
  std::string cmd;
  while (1)
  {
    // if (ctrl)
    //{ // my action when signal set it 1
    // printf("\n Signal caught!\n");
    // printf("\n default action it not termination!\n");
    // ctrl = 0;
    //}
    // 打印提示符
    std::cout << "# ";

    // 读入一行。std::getline 结果不包含换行符。
    std::getline(std::cin, cmd);
    cmd_history.push_back(cmd);
    // 按空格分割命令为单词
    std::vector<std::string> args = split(cmd, " ");
    line++;
    // 没有可处理的命令
    if (args.empty())
    {
      continue;
    }

    // 更改工作目录为目标目录
    if (args[0] == "cd")
    {
      if (args.size() <= 1)
      {
        // 输出的信息尽量为英文，非英文输出（其实是非 ASCII 输出）在没有特别配置的情况下（特别是 Windows 下）会乱码
        // 如感兴趣可以自行搜索 GBK Unicode UTF-8 Codepage UTF-16 等进行学习
        std::cout << "Insufficient arguments\n";
        // 不要用 std::endl，std::endl = "\n" + fflush(stdout)
        continue;
      }

      // 调用系统 API
      int ret = chdir(args[1].c_str());
      if (ret < 0)
      {
        std::cout << "cd failed\n";
      }
      continue;
    }

    // 显示当前工作目录
    // if (args[0] == "pwd")
    // {
    //   std::string cwd;

    //   // 预先分配好空间
    //   cwd.resize(PATH_MAX);

    //   // std::string to char *: &s[0]（C++17 以上可以用 s.data()）
    //   // std::string 保证其内存是连续的
    //   const char *ret = getcwd(&cwd[0], PATH_MAX);
    //   if (ret == nullptr)
    //   {
    //     std::cout << "cwd failed\n";
    //   }
    //   else
    //   {
    //     std::cout << ret << "\n";
    //   }
    //   continue;
    // }

    // 设置环境变量
    if (args[0] == "export")
    {
      for (auto i = ++args.begin(); i != args.end(); i++)
      {
        std::string key = *i;

        // std::string 默认为空
        std::string value;

        // std::string::npos = std::string end
        // std::string 不是 nullptr 结尾的，但确实会有一个结尾字符 npos
        size_t pos;
        if ((pos = i->find('=')) != std::string::npos)
        {
          key = i->substr(0, pos);
          value = i->substr(pos + 1);
        }

        int ret = setenv(key.c_str(), value.c_str(), 1);
        if (ret < 0)
        {
          std::cout << "export failed\n";
        }
      }
    }

    // 退出
    if (args[0] == "exit")
    {
      if (args.size() <= 1)
      {
        return 0;
      }

      // std::string 转 int
      std::stringstream code_stream(args[1]);
      int code = 0;
      code_stream >> code;

      // 转换失败
      if (!code_stream.eof() || code_stream.fail())
      {
        std::cout << "Invalid exit code\n";
        continue;
      }

      return code;
    }
    // if (buf[0] == '!')
    //{
    history_judge(cmd);
    // continue;
    //}
    char *ptr = NULL;
    char *temp = strtok_r(buf, " ", &ptr);
    // std::cout << temp;
    int j = 0;

    while (temp)
    {
      command[j++] = temp;
      temp = strtok_r(NULL, " ", &ptr);
      // std::cout << command[j - 1];
    }
    arg = j;
    command[j] = 0; //命令形式的字符串数组最后一位必须是NULL
    do_cmd(command);
  }
}

// 经典的 cpp string split 实现
// https://stackoverflow.com/a/14266139/11691878
std::vector<std::string> split(std::string s, const std::string &delimiter)
{
  std::vector<std::string> res;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    res.push_back(token);
    s = s.substr(pos + delimiter.length());
  }
  res.push_back(s);
  return res;
}
