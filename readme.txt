1. 使用说明
1、服务器
将server.c，cacert.pem，privkey.pem文件放在任意一个目录，然后通过此命令进行编译：gcc –Wall –lssl server.c –o server。会得到一个名为server的目标文件。
然后就可以运行服务器了，运行的格式为：./server+端口号+监听客户端的最大数量+本服务器使用的IP，如：./server 1234 1 192.168.1.101。也可以缺省输入，端口号的默认值是7838，监听客户端的最大数量默认为2，本服务器使用的IP默认为任意。服务器接收到的文件默认保存在 /newfile 目录下。
2、客户端
将client.c文件放在任意一个目录，然后通过此命令进行编译：gcc –Wall –lssl client.c –o client。会得到一个名为client的目标文件。
当服务器开始监听后，就可以通过运行client目标文件连接到服务器了，运行的格式为：./client+服务器IP+端口号，如：./client 192.168.1.101 1234。如果此时服务器还未和其他客户端建立连接，那么连接成功；如果服务器已经和其他客户端建立了连接，并且服务器监听的客户端未达到最大值，那么客户端会等待；否则返回错误。
当客户端成功连接到服务器后，可以看到中间使用的证书的各种信息，并提示输入需要上传到服务器的文件名及其具体路径，如输入：/wenmin/server.c。
2. 系统结构
服务器                                   客户端
SSL 库初始化                         SSL 库初始化
载入所有 SSL 算法                    载入所有 SSL 算法
载入所有 SSL 错误消息                载入所有 SSL 错误消息
创建本次会话连接所使用的协议          创建本次会话连接所使用的协议
申请SSL会话的环境 CTX              申请SSL会话的环境 CTX
载入用户的数字证书               创建一个 socket 用于 tcp 通信
载入用户私钥                   初始化服务器端的地址和端口信息
检查用户私钥是否正确 
开启一个 socket 监听 
等待客户端连上来                          连接服务器
基于 ctx 产生一个新的 SSL               基于 ctx 产生一个新的 SSL
将连接用户的 socket 加入到 SSL 
建立 SSL 连接                          建立 SSL 连接
接收数据并保存到/newfile                      发送数据
关闭 SSL 连接                          关闭 SSL 连接
释放 SSL                                 释放 SSL
关闭 socket 	                            关闭 socket
关闭监听的 socket 
释放 CTX                                 释放 CTX
3. 生成数字证书和密钥
1、生成RSA密钥的方法：openssl genrsa -des3 -out privkey.pem 2048
这个命令会生成一个2048位的密钥，同时有一个des3方法加密的密码，如果你不想要每次都输入密码，可以改成：openssl genrsa -out privkey.pem 2048建议用2048位密钥，少于此可能会不安全或很快将不安全。
2、生成证书：openssl req -new -key privkey.pem -out cert.csr
这个命令将会生成一个证书请求，当然，用到了前面生成的密钥privkey.pem文件。这里将生成一个新的文件cert.csr，即一个证书请求文件，你可以拿着这个文件去数字证书颁发机构（即CA）申请一个数字证书。CA会给你一个新的文件cacert.pem，那才是你的数字证书。
如果是自己做测试，那么证书的申请机构和颁发机构都是自己。就可以用下面这个命令来生成证书：openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095。这个命令将用上面生成的密钥privkey.pem生成一个数字证书cacert.pem
