libipc接口说明：

libipc使用共享内存在两个进程之间实现进程间函数调用。

server的实现参考simple_server。
1. 在server程序和client库函数共用的头文件(参考simple_service.h)中定义IPC_SERVICES，
   I. 包含(1)服务名称，(2)服务提供的接口名。 
   II. simple_service.h中还包含了client库函数提供的接口(add(), hello_string(),...)，这些接口可以单独为一个头文件提供给用户。

2. 在client库中使用提供给用户的接口，
   I. 使用默认形式的接口需要依照：
        IPC_INTERFACE(1.I中定义的接口名，参数的个数，[函数中使用的参数名...]，[各个参数占用的内存大小])；
   II. 默认接口要求传入的“参数的实际内容”在共享内存中存在一份拷贝，因此在IPC_INTERFACE中需要给出“参数占用的内存大小”。
   III. 使用IPC_CALL(接口名)()调用IPC_INTERFACE中生成的消息发送函数，IPC_CALL(接口名)()中只接受指针参数（需传入IPC_INTERFACE中定义的参数名），指针指向“存储实际参数”的地址。
        如hello_string()函数，IPC_CALL(Ipc_hello_string)传入(buf, &len)，指针buf指向缓存数组的真正位置，指针&len指向存储(int)len的真正位置。
   IV. hello_string_nowait()给出了消息传输的真正逻辑：
        (1) 在Session中创建Message;
        (2) 使用ipc_msg_get_nth_arg_buf()和ipc_msg_set_nth_arg()为参数赋值。
        (3) 设置Message的操作码（此处为Ipc_hello_string）;
        (4) 调用ipc_msg_send_nowait()或ipc_msg_send()发送消息。

3. 定义server程序：
   I. 在server的c文件（此处为simple_server.c）中定义IPC_DO_SERVE_FUNC(接口名)()中实现函数逻辑。（传入参数均为指针）；
   II. 为server定义在message中解析接口的函数，使用宏IPC_SERVER_INTERFACE(接口名，参数个数)；
   III. 向server处理表注册接口，使用IPC_SERVER_REGISTER_INTERFACES(服务名，接口个数，[接口名, ...]) ；
   IV. 定义main函数：
        (1) 初始化server所需的数据结构；
        (2) 向操作系统注册服务：register_server(“用来访问服务的名称”)；
        (3) 调用ipc_server_loop()：该函数单线程得处理传入到server的调用请求。

注：创建session时传入的大小必须大于后续在 使用session发起的调用中 参数实际所占的大小。