# 处理器核绑定样例

本样例针对QEMU VExpress-A9虚拟平台演示对称多处理（SMP）系统中线程的处理器核绑定。
程序运行三个线程，每个线程都会每秒打印在哪个处理器核上运行。
线程1绑定在核0上运行，线程2绑定在核1上运行，线程3可以自由运行。

编译步骤如下：
- 将application/main.c和rtconfig.h粘贴到纯净的rt-thread/bsp/qemu-vexpress-a9目录中；也可复制qemu-vexpress-a9文件夹后再进行粘贴，以保留源文件夹。
- 在目录中打开env环境，运行命令`scons -j4`。
- 编译正确会产生rtthread.elf和rtthread.bin文件。
- 文件夹中包括启动QEMU的脚本qemu-nographic.sh（Linux）和qemu-nographic.bat（Windows）。

关于编译和环境的更多说明见[QEMU VExpress A9板级支持包说明](https://github.com/RT-Thread/rt-thread/blob/c285c9806bfeeb72676ccb7f122bd2a207330585/bsp/qemu-vexpress-a9/README.md)。

执行对应平台的脚本开始运行，RT-Thread载入后样例开始运行，输出应类似下面所示：

Thread1 is running on core 0, count: 0
Thread3 is running on core 0, count: 0
Thread2 is running on core 1, count: 0
Thread1 is running on core 0, count: 1
Thread2 is running on core 1, count: 1
Thread3 is running on core 0, count: 1
Thread1 is running on core 0, count: 2
Thread2 is running on core 1, count: 2
Thread3 is running on core 0, count: 2
Thread1 is running on core 0, count: 3
Thread2 is running on core 1, count: 3
Thread3 is running on core 0, count: 3
Thread2 is running on core 1, count: 4
Thread1 is running on core 0, count: 4
Thread3 is running on core 1, count: 4
Thread1 is running on core 0, count: 5
Thread2 is running on core 1, count: 5
Thread3 is running on core 0, count: 5
（循环往复）

由于核0一般负载不高，线程优先级也一致，没有绑定处理器核的线程3多数情况于核0运行，但是偶尔会出现在核1上。

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
