# operation-system-lab-4
基于shell的文件复制程序，北京理工大学计算机学院大三操作系统课程实验。

## 实验内容
  在Linux操作系统下实现文件复制功能。实验步骤如下：
  
1. 在Linux系统下实现目录复制命令，新实现的命令命名为"mycopy"。
   
2. 使用mycopy命令能够支持多级目录（子目录）的复制，支持Linux下的soft link复制。
  
3. 要求使用命令行参数接受参与文件复制的源文件和目标文件，mycopy命令的使用方法为： mycopy src dest，其中src为源文件，dest为目标文件。

4. 在Linux系统下可使用mkdir、opendir、readdir、symlink、readlink等函数。

## 运行方式
  在Linux下编译main.cpp，并在终端中运行之。在同级目录或下级目录中新建几个测试文件，按实验内容里的要求输入指令即可复制。

## 运行效果

(1) 启动（激昂地）引用程序，在程序内按行输入指令：
![image](https://github.com/SharkHeadBalloon/operation-system-lab-4/assets/106319321/06d17c11-c82c-4208-b874-201a863b1e27)

(2) 查看目录发现成功了：
![image](https://github.com/SharkHeadBalloon/operation-system-lab-4/assets/106319321/cf7a0db3-4a59-4489-ad90-37e21d21fbd2)

## 参考资料
  《操作系统原理教程》
