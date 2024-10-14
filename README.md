K.D.L v1.0

自动从 FTP 页面下载文件，并进行 MD5 校验。

## 配置文件详解

（第一次启动会进行配置）：

- **Maximum single download tasks (int)**: 线程池中一次下载多少任务
- **MD5 verification failed. Should we download it again (Y/N)**: 当 MD5 校验失败后，是否重新下载
- **Output logs (Y/N)**: 是否输出日志
- **Default download address (string)**: 默认下载地址
- **How many rows are displayed on one page (int)**: 查看下载进度时一页显示多少行

## 命令详解

- **RSTCON**: 重新配置
- **DLFILE [String] [String]**: 下载文件，第一个参数为文件 URL（只能是 HTTPS 开头），第二个参数为保存地址，若无，则下载到默认地址。
- **SHOWCOMMAND**: 显示命令
- **SHOWCONFIG**: 显示设置

- **本开源代码仅用于教育和学习目的，作者不对使用该代码可能造成的任何损失或后果承担责任。**
