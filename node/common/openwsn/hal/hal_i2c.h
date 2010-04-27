i2c_init()
//设置 SCL 和 SDA 端口引脚，还初始化延时常数

i2c_start_tx()       // 通过发送 S（开始）信号初始化 I2C 的传送 
i2c_startw_tx()      // 通过发送 S 信号以及之后的延时信号对 I2C 的发送,  进行初始化 

i2c_write_char()      // 给从口发送 8 位数据 
i2c_wr_wait()         // 在从口相应之前一直重试字符的写操作 

i2c_check_ack()        // 监听应答信息 , 检查从口是否把数据拉低来响应时钟脉冲 

i2c_read_char()         // 从从口读取 8 位数据 

i2c_send_ack()         // 给从口发送一个 ACK 序列

i2c_send_nak()         // 给从口发送一个 NAK 序列 

i2c_stop_tx()         // 给从口发送一个 P(停止)信号 