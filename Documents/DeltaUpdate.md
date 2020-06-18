# 4399单机SDK增量升级说明

## 升级方式

- 全自动升级：所有升级处理逻辑及界面显示，都由sdk完成。
- 自定义界面升级：游戏方可以拿到升级信息及下载进度，自定义升级界面。

## 全自动升级
接入方在后台提交新版游戏时自动制作差分包，更新时用户只需下载APK文件中新旧版本有差异的部分。更新内容和版本提交相关事宜，请联系4399相关运营对接人员。  
当游戏初始化时，SDK将检查后台是否有新版本游戏上线，如果有，则显示更新内容，并提示用户升级。全自动更新无需配置和代码接入，SDK在初始化时自动检查更新。

## 自定义升级界面
如果游戏需要自定义更新界面，需告知4399运营人员，由后台关闭全自动更新开关。由游戏方自主接入更新接口  

如需了解相关的接口、类或设计细节，可以向开发者咨询接口详情等，或者查看[4399 Single SDK-v2.0.0+31]( http://common:kCcy8iper6@sdkftp.4399doc.com/internal/single/2.0/javadoc/index.html
)

### 1.1 检查新版本
```java
/*
 * @param MainActivity.this Activity上下文
 * @param OnCheckFinishedListener 检查更新回调，返回检查更新的结果
 */
SingleOperateCenter.doCheck(MainActivity.this, new UpgradeProgress<UpgradeInfo>() {
	@Override
	public void onStart() {}
	
	@Override
	public void onProgress(long... ps) {}
	
	@Override
	public void onFinished(int code, String message, UpgradeInfo info) {
	    // 根据 info 对象内容展示更新对话框
	    // 需要区分几种情况：
	    // 1 检查更新成功
	    //     1.1 有新版本
	    //         1.1.1 本地有下载好的文件，展示安装对话框，后续调用doInstall接口
	    //         1.1.2 本地没有下载好的文件，展示下载对话框，后续调用doInstall接口
	    //     1.2 无新版本，游戏方更加需要决定是否需要提升
	    // 2 检查更新失败，游戏方根据需要提供重试按钮，后续调用doCheck接口
	    showUpgradeInfo();
        }
 });
```

### 1.2 显示检查结果
```java
private void showUpdateInfo(final UpgradeInfo info) {
	int code = info.code();

	if (code == UpgradeInfo.STATUS_UP_TO_DATE) {
		// 没有更新，这里可以选择不提示
	}  else if (code == UpgradeInfo.STATUS_NEED_UPGRADE) {
		// 有更新内容
		if (info.havePreparedFile()) {
			// 本地已有下载好的更新包，显示“立即安装”, 即开始安装
		} else {
			// 本地没有下载好的更新包
			if (info.isCompel()) {
				// 强制更新
			} else {
				// 非强制更新
			}
		}
	
	} else {
		// 检查更新失败，根据需要决定是否显示失败信息
	}
}
```

### 1.3 下载更新包
#### 接口
```java
/**
 * @param MainActivity.this Actvity上下文
 * @param OnDownloadListener 下载监听器，返回下载过程各个阶段的信息
 */
SingleOperateCenter.doDownload(info, new UpgradeProgress<Void>() {
	@Override
	public void onStart() {
		// 开始下载
	}
	
	@Override
	public void onProgress(long... ps) {
		long written = ps[0]; // 已完成下载的字节
                long total = ps[1];   // 需要下载的字节
		// 正在下载，展示进度
	}
	
	@Override
	public void onFinished(int code, String message, Void aVoid) {
		if (code == SUCCESS || code == SUCCESS_ALREADY) { // 下载成功, 可以提
			doInstall(info);
		} else { // 下载失败，提醒重试
			
		}
	}
```
### 1.4 安装更新包
```java
/*
 * 下载成功后，或者本地已有下载好的更新包时，可以调用此函数
 * SDK会根据是增量更新还是全量更新，采取不同的安装方式
 */
SingleOperateCenter.doInstall(info, new UpgradeProgress<Void>() {
	@Override
	public void onStart() {
		// 开始安装，SDK 可能会做一些安装准备，如合并包
	}
	
	@Override
        public void onProgress(long... ps) {
		if (ps[0] == UpgradeProgress.PROGRESS_INSTALLING) { // 内部开始安装流程
		}
	}
	
	@Override
        public void onFinished(int code, String message, Void aVoid) {
		if (code != UpgradeProgress.SUCCESS) { 
			// 安装失败，可以提醒重试
		} 
		// 安装成功，应用进程将自动退出
	}
}
```

