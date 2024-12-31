# 更新进度

2024/12/28   21: 47
添加 epoll类 ： Reactor_05_add_epoll_class

2024/12/28   23: 43
channel 登场 ： Reactor_06_add_channel_1


2024/12/29   22: 00
channel 登场 3： Reactor_07_add_channel_3

2024/12/29   22: 00
第一次回调： Reactor_08_callback


2024/12/29   23: 12
增加EventLoop类： Reactor_09_add_EventLoop_class_1


2024/12/30   00: 59
优化 EventLoop类： Reactor_10_add_EventLoop_class_2  （待完成）



2024/12/30   01: 02
TcpServer 类： Reactor_11_add_TcpServer_class



2024/12/31   09: 54
Acceptor 类： Reactor_12_add_Acceptor_class
    把 TcpServer 中 创建的 链接 封装为 Acceptor


2024/12/31   15: 13
Connection 类： Reactor_13_add_Connection_class
     封装 Connection 放到  Channel 中 替换 ： 为新客户端连接准备读事件


2024/12/31   21: 57
优化 Acceptor ： Reactor_14_Optimize_Accept_1
     优化 Acceptor , 把 Channel 的 newConnection 回调函数 放到 Acceptor 中， 老师大概的意思是 因为， 在 Acceptor 中  创建服务端用于监听的listenfd。
                     在传统的网络服务 链接中 由 监听的 fd 产生 客户端 连接的 fd ； Acceptor 封装了监听的fd 监听的 Channel， Connection类的对象由 Acceptor 类 创建， 也就是说 创建。Connection的代码应该 写在 Acceptor 类中， 而不是在 TcpServer 中。









# server
Reactor server 

高并发服务器 test





==================================

# 创建 密钥
ssh-keygen -t rsa -b 4096 -C "373734199@qq.com"

SHA256:i12YdJPa7XY1OTf8Nx59f0UPSPcG85j0wwnrZvrMW6c


# 添加 SSH 密钥到 SSH 代理
eval "$(ssh-agent -s)"

ssh-add ~/.ssh/id_rsa


# 获取公钥内容
cat ~/.ssh/id_rsa.pub  得到

ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDMqNvlbGXKLuDGt6T6lnC1nmPHR5D9lJ4vwObwrSyR0sGx3BylH8M8bOrU52IG7T4m7AbjRoFj3QWuBLyHRJ9fxJmlTc+0phGRYsiHIYcBeCUqiSD9ZKb/1sZGCxJtHGnfz529gTIEAANs859a1yYs73U/zU0Vw11gpsImkgspwlM8zqUpiKieiJGz5A2ONThkkQwWtD7gqHoeyrGRXYl67dj+cyWlRYF9AmXdKp7VCKb5gkoGdjHwCtRBMh+xBWTGxYgbVvLw4l6Byfp46jXLwKCQ3N90UKBRbW9eey3OR83n7zU+Mp13i0qS3DDKxhz9q5KsVtikj9JEALMpFq89ToSTKTwp2v/cGWIBIbRS6KTWdzFdw9iqCDYvPDzuUbogo27a90wKdJlM2JUxX6O5ztOFS7wDTNABIoKAOuG7S37P4PA8Dh97CPpijHZ5ouAA+fyvhmtR6uhIEGsX0hnryTR26KoSxknmybfqeVZrFdPXnnl3ha0cxlIthH7ckJW6T2YZ2kcue7rcBDPijxzOWycvRinNmupbQPG8MKaebTYO1W4AYW8fXRh3NQ+fvEkTaojnlVsXqOnYHdysxCxdzmpbAlBVrSFxrSeKxIe3d/XlvtKlCadWdpLIJcjXw+DJvVck6OkzrOsO9cpMkWOYfla/pouxGbD9n78AlPQZzw== 373734199@qq.com


# 测试

ssh -T git@github.com




# 拉代码
git clone git@github.com:kingnan-Guo/server.git

git clone git@github.com:kingnan-Guo/server.git ReactorClient

# 检查分支 
git branch -r

# 切换分支 到 sylar
git checkout sylar

git checkout main

git checkout origin/Reactor







# 提交代码
git add .
git commit -m "first commit"
git push -u origin master
git push origin sylar






 # 上传前要登录
 
git config --global user.name "kingnan-Guo"
git config --global user.email "373734199@qq.com"

<!-- git remote add origin git@github.com:zhangyue/leetcode.git

git push -u origin master -->