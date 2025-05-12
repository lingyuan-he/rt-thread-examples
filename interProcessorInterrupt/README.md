# 处理器间中断（IPI）样例

本样例针对QEMU VExpress-A9虚拟平台演示对称多处理（SMP）系统中处理器间中断（IPI）的触发和处理。程序运行两个线程，每个线程都会每两秒向另一个处理器核发送IPI。IPI的中断处理例程会简单地打印哪个核接收到了IPI。为了IPI的触发和处理输出能够相互间隔开，两个线程启动间有1秒延迟。

编译步骤如下：
- 将`application/main.c`和`rtconfig.h`粘贴到纯净的`rt-thread/bsp/qemu-vexpress-a9`目录中；也可复制`qemu-vexpress-a9`文件夹后再进行粘贴，以保留源文件夹。
- 在目录中打开env环境，运行命令`scons -j4`。
- 编译正确会产生`rtthread.elf`和`rtthread.bin`文件。
- 文件夹中包括启动QEMU的脚本`qemu-nographic.sh`（Linux）和`qemu-nographic.bat`（Windows）。

关于编译和环境的更多说明见[QEMU VExpress A9板级支持包说明](https://github.com/RT-Thread/rt-thread/blob/c285c9806bfeeb72676ccb7f122bd2a207330585/bsp/qemu-vexpress-a9/README.md)。

执行对应平台的脚本开始运行，RT-Thread载入后样例开始运行，输出应如下所示：

Core 0 sending IPI to Core 1\
Core 1 received IPI\
Core 1 sending IPI to Core 0\
Core 0 received IPI\
Core 0 sending IPI to Core 1\
Core 1 received IPI\
Core 1 sending IPI to Core 0\
Core 0 received IPI\
（循环往复）

结束运行（Windows）：在msh提示输入的状态下，按下 **CTRL+C** ，有以下提示之后，按下 **y** ，即可退出 QEMU 。

```shell
hello rt-thread
msh />
msh />
msh />Terminate batch job (Y/N)? y
```

结束运行（Linux）：在msh提示输入的状态下，按下 **CTRL+A** ，再按下 **X** ，即可退出 QEMU 。

```shell
hello rt-thread
msh />
msh />
msh />QEMU: Terminated
```
