#  一、项目资源下载
1. [基于C++的网盘系统项目源码CSDN下载地址](https://download.csdn.net/download/IronmanJay/87798821)
2. [基于C++的网盘系统项目源码GitHub下载地址](https://github.com/IronmanJay/NetworkDiskSystemProjectBasedOnCPlusPlus)
<hr style=" border:solid; width:100px; height:1px;" color=#000000 size=1">

# 二、项目简介
&nbsp;&nbsp;本项目基于C++开发，整个项目采用C/S架构，使用Sqlite3数据库存储用户信息，本地磁盘存储用户文件，使用Socket进行客户端和服务器之间的数据传输。完成了网盘中关于用户的几乎所有功能，包括：用户注册、用户登录、用户退出、查看在线好友、搜索好友、添加好友、刷新在线好友、删除好友、私聊、群聊等等；关于文件夹和文件的所有功能也几乎完成了，包括：创建文件夹、查看所有文件、删除文件夹、重命名文件夹、进入文件夹、返回上一级、上传文件、删除文件、下载文件、分享文件、移动文件等等。本项目的完成度非常高，包括了这么多功能，自然代码数也非常多，代码多不是问题，因为我写的非常详细，详细到每一步我都有解释，各位读者一定可以看懂。另外，为了方便读者学习，且本着开源精神，我已经将我写好的源码分享在上面了。此项目作为本科毕设或者写在简历上也不失为一个好的选择。此项目花费了我大量精力和时间，前前后后写了两个多月，希望读者可以收获满满，下面就开始学习吧！
<hr style=" border:solid; width:100px; height:1px;" color=#000000 size=1">

# 三、项目开发软件环境
- Windows 11
- Qt Creator 4.11.1 (Community)
- C++ 98
- Gcc 3.4.5
<hr style=" border:solid; width:100px; height:1px;" color=#000000 size=1">

# 四、项目开发硬件环境
- CPU：Intel® Core™ i7-8750H CPU @ 2.20GHz 2.20 GHz
- RAM：24GB
- GPU：NVIDIA GeForce GTX 1060
<hr style=" border:solid; width:100px; height:1px;" color=#000000 size=1">

# 五、项目演示

## 5.1 用户注册

1. 当我们启动两个项目后，输入用户名和密码最后点击“注册”，发现提示我们注册成功了：
![请添加图片描述](https://img-blog.csdnimg.cn/3408bf340afc4e139cf01393eae3dc41.png#pic_center)

2. 然后我们来到数据库查看数据，发现新的注册信息已经保存到数据库中了：
![请添加图片描述](https://img-blog.csdnimg.cn/3a9dcff782b642e5b4eaa3b184ee1999.png#pic_center)

3. 如果此时我们还是以同样的用户名去注册，就会提示我们注册失败，因为数据库中已经有同名的用户名了，而我们设置用户名字段唯一，所以注册会失败：
![请添加图片描述](https://img-blog.csdnimg.cn/f49cfc8afb13418c92ec445bb4dd66fb.png#pic_center)

## 5.2 用户登录

1. 启动两个项目，当我们使用已经在数据库中存在的用户，并且此用户的“online”字段为0的时候，是可以成功登陆的：
![请添加图片描述](https://img-blog.csdnimg.cn/5b59cd8d2d5149308e29354176a4f110.png#pic_center)

2. 当我们尝试使用数据库中不存在的用户登陆时，是不可以成功登录的：
![请添加图片描述](https://img-blog.csdnimg.cn/0ccc5d8e89f64aed89262ddd2830c4e5.png#pic_center)

## 5.3 用户退出

1. 启动两个项目，使用某一个用户登录，然后查询数据库，发现此用户的“online”字段为1，说明已经成功登陆了：
![请添加图片描述](https://img-blog.csdnimg.cn/17580e78c39540cc9f613b05abe59489.png#pic_center)

2. 然后点击关闭按钮，模仿用户退出行为：
![请添加图片描述](https://img-blog.csdnimg.cn/4b0d933385954c3ab6f5653f2b805e72.png#pic_center)

3. 此时再来数据库查询，就发现对应用户的online字段已经变为了0，说明此用户已经成功退出了：
![请添加图片描述](https://img-blog.csdnimg.cn/94a668fc067c40899cca11829d824597.png#pic_center)

## 5.4 查看在线好友

1. 同时启动多个客户端，然后分别登陆进去，点击“显示在线用户”按钮，可以发现在线的用户的用户名已经成功显示了：
![请添加图片描述](https://img-blog.csdnimg.cn/2794f85fda7c497c8866e54cf52d631f.png#pic_center)

## 5.5 搜索好友

1. 启动两个项目后，当我们搜索用户“abc”的时候，发现可以成功展示其用户名和状态：
![请添加图片描述](https://img-blog.csdnimg.cn/14b88cc9d626446b8b771d6d72798816.png#pic_center)

## 5.6 添加好友

1. 打开两个客户端和一个服务器后，发现被加用户可以正常弹出窗口：
![请添加图片描述](https://img-blog.csdnimg.cn/ed451b0754df4887a0280b60787f9abc.png#pic_center)

2. 当我们点击“Yes”后，发现可以已经成功添加好友了，并且数据库中也已经保存了两个用户的好友关系：
![请添加图片描述](https://img-blog.csdnimg.cn/1a036472133e4c8ab9c696b0f753ebb4.png#pic_center)

## 5.7 刷新在线好友

1. 分别启动服务器和客户端并登陆后，我们点击“刷新好友”按钮，发现此时可以将最新在线的好友列表显示出来了：
![请添加图片描述](https://img-blog.csdnimg.cn/a4264195a98049878ade7a5f5d4f7186.png#pic_center)

## 5.8 删除好友

1. 分别启动服务器和有好友关系的两个客户端，点击“删除好友”按钮后就可以成功删除两个用户之间的好友关系了，并且可以显示提示：
![请添加图片描述](https://img-blog.csdnimg.cn/058ca7a16a6143dfbb0afc9345b0230d.png#pic_center)

## 5.9 私聊

1. 启动服务器和两个客户端进行测试，发现可以正常进行聊天消息的传递：
![请添加图片描述](https://img-blog.csdnimg.cn/9033c10616084b38b5a09ce772832dca.png#pic_center)

## 5.10 群聊

1. 启动项目后，发现可以正常发送群聊信息：
![请添加图片描述](https://img-blog.csdnimg.cn/cb6e87bc2cf94c0cad2b875ac1c74bd7.png#pic_center)

## 5.11 创建文件夹

1. 可以成功在用户名同名的目录下新建文件夹
![请添加图片描述](https://img-blog.csdnimg.cn/4de0a3e784d84221ae880de5db0c5443.png#pic_center)

## 5.12 查看所有文件

1. 当我们点击“图书”界面的“刷新文件”，就可以显示此文件夹内所有的文件了：
![请添加图片描述](https://img-blog.csdnimg.cn/19a98a26cee94ae78fbbc0172081c482.png#pic_center)

## 5.13 删除文件夹

1. 当我们选中文件夹后点击“删除文件夹”后，就会弹出删除成功的提示，然后当我们点击“刷新文件”后，就会发现原来的被删除文件夹已经不存在了：
![请添加图片描述](https://img-blog.csdnimg.cn/cb6832a0cee64a8b97611fdeb9ddd724.png#pic_center)

## 5.14 重命名文件夹

1. 选中某个待重命名的文件，然后输入重命名后的文件名称：
![请添加图片描述](https://img-blog.csdnimg.cn/b05ec4deb64c41b7a848ad9e1a6fd1ab.png#pic_center)

2. 当我们点击“刷新文件”后，可以发现已经成功重命名文件了：
![请添加图片描述](https://img-blog.csdnimg.cn/d8196ba8b5ea47489ffc04cb1944f25c.png#pic_center)

## 5.15 进入文件夹

1. 当我们双击某个文件夹的时候就可以进入到此文件夹：
![请添加图片描述](https://img-blog.csdnimg.cn/a83dd506a5b24d03b57f561cf133f9af.png#pic_center)

## 5.16 返回上一级

1. 当我们点击“返回”的时候，可以从子目录返回到主目录：
![请添加图片描述](https://img-blog.csdnimg.cn/5668efed1b1e441e9b801e7ad7db7f7d.png#pic_center)

## 5.17 上传文件

1. 选中本地某个文件进行上传：
![请添加图片描述](https://img-blog.csdnimg.cn/1f58b327013b45868a2172cbc4495441.png#pic_center)

2. 可以发现，文件已经成功上传了：
![请添加图片描述](https://img-blog.csdnimg.cn/0558c9a224a241b9bbe9f650cab7fd6c.png#pic_center)

## 5.18 删除文件

1. 我们可以选中某个文件，然后点击“删除文件”：
![请添加图片描述](https://img-blog.csdnimg.cn/2fcd05ebadca42aaa7525a0596d202d5.png#pic_center)

2. 当我们再次“刷新文件”后，就会发现选中的文件已经被我们删除了：
![请添加图片描述](https://img-blog.csdnimg.cn/c6e5110d0e1342d28b6f591dbea676fa.png#pic_center)

## 5.19 下载文件

1. 选中服务器的某个文件进行下载：
![请添加图片描述](https://img-blog.csdnimg.cn/c1d79da52c29403697d3fdb896989fe5.png#pic_center)

2. 可以发现，已经下载成功了：
![请添加图片描述](https://img-blog.csdnimg.cn/9dc47b40f42d4b1481d645fa944e30ea.png#pic_center)

## 5.20 分享文件

1. 首先启动一个服务端和两个客户端，在两个客户端上点击“刷新好友”，要先刷新好友，才能对好友进行分享文件的操作：
![请添加图片描述](https://img-blog.csdnimg.cn/7717a80a1ea14f9698ab8a1a59cb2f49.png#pic_center)

2. 然后将“rose”用户下的“hello”文件夹进行分享：
![请添加图片描述](https://img-blog.csdnimg.cn/6f5fe68ed16d47a29c80aaccf775eac9.png#pic_center)

3. 选择“lucy”后，点击“确定”：
![请添加图片描述](https://img-blog.csdnimg.cn/cdcfd3d0213a47cf8c43d6a08871cea5.png#pic_center)

4. 此时两个客户端都有相应的提示了，我们只需要点击接收端的“Yes”：
![请添加图片描述](https://img-blog.csdnimg.cn/8ccdba3568b54072a1a0d5ba139c763d.png#pic_center)

5. 然后点击“lucy”用户客户端的“刷新文件”，可以看到“rose”用户的“hello”文件夹及其文件夹中的内容已经成功拷贝到“lucy”用户的文件目录中了：
![请添加图片描述](https://img-blog.csdnimg.cn/9e45c354a05444f289d858a3738c54f1.png#pic_center)

## 5.21 移动文件

1. 首先选择要移动的文件，然后点击“移动文件”：
![请添加图片描述](https://img-blog.csdnimg.cn/4b77d9d38ecd49b4bf1b9af770f9d7d7.png#pic_center)

2. 然后选择要移动文件的目标目录后，点击“目标目录”：
![请添加图片描述](https://img-blog.csdnimg.cn/002a0e75cee046d9b1dcc77ca3d60517.png#pic_center)

3. 当我们进入移动文件的目标目录查看后，发现文件已经移动到目标目录了：
![请添加图片描述](https://img-blog.csdnimg.cn/98b772e5e8ef45a48f82aaba78af9012.png#pic_center)

# 六、更多细节
&nbsp;&nbsp;更多细节请见：[基于C++的网盘系统项目开发教程](https://blog.csdn.net/IronmanJay/article/details/130718228)

