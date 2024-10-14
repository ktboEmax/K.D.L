# K.D.L
 _  __  _____    _
| |/ / |  __ \  | |
| ' /  | |  | | | |
|  <   | |  | | | |
| . \ _| |__| |_| |____
|_|\_(_)______(_)______|v1.0

自动从ftp页面下载文件，并进行MD5校验
# 配置文件详解（第一次启动会进行配置）：
Maximum single download tasks(int): //线程池中一次下载多少任务
MD5 verification failed. Should we download it again(Y/N): //当MD5校验失败后，是否重新下载
Output logs(Y/N): //是否输出日志
Default download address(string): //默认下载地址
How many rows are displayed on one page(int): //查看下载进度时一页显示多少行

# 命令详解：
RSTCON//Reset configuration 重新配置

DLFILE [String] [String]//Download the file, the first parameter is the file URL, and the second parameter is the local address where the file should be stored,if not filled in, the default download address will be used 下载文件，第一个参数为页面地址（只能是https开头），第二个参数为保存地址，若无，则下载到默认地址

SHOWCOMMAND//Display Command 显示命令

SHOWCONFIG//Display configuration 显示设置
