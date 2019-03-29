# Secondary-bootloader-for-a-Cortex-M0-chip
该程序为适用于Cortex M0+核芯片的BootLoader升级程序。

升级程序采用X-modem协议传输数据包，每包数据大小为1024字节，采用CRC16校验升级数据包的正确性。
源码编译完后小于3Kb，对于flash容量有效的芯片比较适用。

通过修改程序中APP_START_ADDR宏，实现app升级程序的首地址定位。
