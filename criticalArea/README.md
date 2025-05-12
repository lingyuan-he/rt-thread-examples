# 关键区域样例

本样例针对QEMU VExpress-A9虚拟平台演示对称多处理（SMP）系统中通过进入和离开关键区域来达到禁用抢占。
程序运行三个线程，线程1为较低优先级，在核0上绑定运行，会定期进入关键区域，运行数秒后离开关键区域。线程2和3为较高优先级，运行时打印所在的处理核后，调用时延函数停留1秒，然后放弃占用处理器以允许线程1运行。
线程2绑定到了核0上，线程3可以自由运行。

编译步骤如下：
- 将application/main.c和rtconfig.h粘贴到纯净的rt-thread/bsp/qemu-vexpress-a9目录中；也可复制qemu-vexpress-a9文件夹后再进行粘贴，以保留源文件夹。
- 在目录中打开env环境，运行命令`scons -j4`。
- 编译正确会产生rtthread.elf和rtthread.bin文件。
- 文件夹中包括启动QEMU的脚本qemu-nographic.sh（Linux）和qemu-nographic.bat（Windows）。

关于编译和环境的更多说明见[QEMU VExpress A9板级支持包说明](https://github.com/RT-Thread/rt-thread/blob/c285c9806bfeeb72676ccb7f122bd2a207330585/bsp/qemu-vexpress-a9/README.md)。

执行对应平台的脚本开始运行，RT-Thread载入后样例开始运行，输出应类似下面所示：

Low priority Thread1 is running on core 0, in critical area
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
Low priority Thread1 is running on core 0, leaving critical area
High priority Thread2 is running on core 0
High priority Thread3 is running on core 1
High priority Thread2 is running on core 0
High priority Thread3 is running on core 0
High priority Thread2 is running on core 0
High priority Thread3 is running on core 1
High priority Thread2 is running on core 0
Low priority Thread1 is running on core 0, in critical area
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
High priority Thread3 is running on core 1
Low priority Thread1 is running on core 0, leaving critical area
High priority Thread2 is running on core 0
High priority Thread3 is running on core 0
High priority Thread2 is running on core 0
High priority Thread3 is running on core 1
High priority Thread2 is running on core 0
High priority Thread3 is running on core 1
High priority Thread2 is running on core 0
（循环往复）

可以看到，关键区域中，高优先级但是绑定核0的线程2不能抢占线程1，因此不会运行（产生了优先级反转），但是不绑定的高优先级线程3可以转移到核1运行。关键区域外线程2和3都可以抢占运行，时常同时占用核0和1，但由于本例中代码只进行打印信息一件事，也会发生都在核0上运行的情况。

结束运行（Windows）：
在msh提示输入的状态下，按下 **CTRL+C** ，有以下提示之后，按下 **y** ，即可退出 QEMU 。

```shell
hello rt-thread
msh />
msh />
msh />Terminate batch job (Y/N)? y
```

结束运行（Linux）
Linux下退出QEMU的方式与Windows下有些不一样；在msh提示输入的状态下，按下 **CTRL+A** ，再按下 **X** ，即可退出 QEMU 。

```shell
hello rt-thread
msh />
msh />
msh />QEMU: Terminated
```
